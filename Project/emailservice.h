#ifndef EMAILSERVICE_H
#define EMAILSERVICE_H

#include <QObject>
#include <QTcpSocket>
#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QStringList>
#include <QSslSocket>
#include <QDebug>

class EmailService : public QObject
{
    Q_OBJECT

public:
    struct EmailConfig {
        QString smtpServer;
        int port;
        QString username;
        QString password;
        bool useSSL;
        bool useTLS;
    };

    struct EmailMessage {
        QString to;
        QString subject;
        QString body;
        QString from;
        QString fromName;
        bool isHtml = false;
    };

    explicit EmailService(QObject *parent = nullptr);
    ~EmailService();

    void setConfig(const EmailConfig &config);
    void sendEmail(const EmailMessage &message);

signals:
    void emailSent(bool success, const QString &message);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QSslSocket *socket;
    EmailConfig config;
    EmailMessage currentMessage;

    enum State {
        Init,
        HandShake,
        Auth,
        User,
        Pass,
        Mail,
        Rcpt,
        Data,
        Body,
        Quit,
        Close
    };

    State state;
    QString response;
    QTimer *timeout;

    void sendCommand(const QString &command);
    void processResponse();
    QString encodeBase64(const QString &text);
};

// Implementation
inline EmailService::EmailService(QObject *parent)
    : QObject(parent), socket(nullptr), state(Init)
{
    socket = new QSslSocket(this);
    timeout = new QTimer(this);
    timeout->setSingleShot(true);
    timeout->setInterval(30000); // 30 seconds timeout

    connect(socket, &QSslSocket::connected, this, &EmailService::onConnected);
    connect(socket, &QSslSocket::readyRead, this, &EmailService::onReadyRead);
    connect(socket, &QSslSocket::disconnected, this, &EmailService::onDisconnected);

    // FIXED: Use proper signal connection syntax for Qt 6
    connect(socket, &QSslSocket::errorOccurred, this, &EmailService::onError);

    connect(timeout, &QTimer::timeout, [this]() {
        emit emailSent(false, "Connection timeout");
        socket->abort();
    });
}

inline EmailService::~EmailService()
{
    if (socket && socket->state() != QAbstractSocket::UnconnectedState) {
        socket->abort();
    }
}

inline void EmailService::setConfig(const EmailConfig &config)
{
    this->config = config;
}

inline void EmailService::sendEmail(const EmailMessage &message)
{
    if (config.smtpServer.isEmpty()) {
        emit emailSent(false, "SMTP server not configured");
        return;
    }

    currentMessage = message;
    state = Init;
    response.clear();

    timeout->start();

    if (config.useSSL) {
        socket->connectToHostEncrypted(config.smtpServer, config.port);
    } else {
        socket->connectToHost(config.smtpServer, config.port);
    }
}

inline void EmailService::onConnected()
{
    qDebug() << "Connected to SMTP server";
    state = HandShake;
}

inline void EmailService::onReadyRead()
{
    QString responseLine = socket->readAll();
    response += responseLine;

    if (response.endsWith("\r\n")) {
        processResponse();
        response.clear();
    }
}

inline void EmailService::processResponse()
{
    qDebug() << "Server response:" << response.trimmed();

    switch (state) {
    case HandShake:
        if (response.startsWith("220")) {
            sendCommand("EHLO localhost\r\n");
            state = Auth;
        } else {
            emit emailSent(false, "Server rejected connection");
            socket->disconnectFromHost();
        }
        break;

    case Auth:
        if (response.startsWith("250")) {
            if (config.useTLS && !socket->isEncrypted()) {
                sendCommand("STARTTLS\r\n");
            } else {
                sendCommand("AUTH LOGIN\r\n");
                state = User;
            }
        } else if (response.startsWith("220") && response.contains("TLS")) {
            socket->startClientEncryption();
            sendCommand("EHLO localhost\r\n");
        }
        break;

    case User:
        if (response.startsWith("334")) {
            sendCommand(encodeBase64(config.username) + "\r\n");
            state = Pass;
        }
        break;

    case Pass:
        if (response.startsWith("334")) {
            sendCommand(encodeBase64(config.password) + "\r\n");
            state = Mail;
        } else if (response.startsWith("235")) {
            // Authentication successful
            sendCommand(QString("MAIL FROM:<%1>\r\n").arg(currentMessage.from));
            state = Mail;
        }
        break;

    case Mail:
        if (response.startsWith("250") || response.startsWith("235")) {
            if (response.startsWith("235")) {
                // Just authenticated, send MAIL FROM
                sendCommand(QString("MAIL FROM:<%1>\r\n").arg(currentMessage.from));
            } else {
                // MAIL FROM accepted, send RCPT TO
                sendCommand(QString("RCPT TO:<%1>\r\n").arg(currentMessage.to));
                state = Rcpt;
            }
        }
        break;

    case Rcpt:
        if (response.startsWith("250")) {
            sendCommand("DATA\r\n");
            state = Data;
        }
        break;

    case Data:
        if (response.startsWith("354")) {
            // Send email headers and body
            QString emailContent;
            emailContent += QString("From: %1 <%2>\r\n")
                                .arg(currentMessage.fromName.isEmpty() ? currentMessage.from : currentMessage.fromName)
                                .arg(currentMessage.from);
            emailContent += QString("To: %1\r\n").arg(currentMessage.to);
            emailContent += QString("Subject: %1\r\n").arg(currentMessage.subject);
            emailContent += QString("Content-Type: %1; charset=UTF-8\r\n")
                                .arg(currentMessage.isHtml ? "text/html" : "text/plain");
            emailContent += "\r\n";
            emailContent += currentMessage.body;
            emailContent += "\r\n.\r\n";

            socket->write(emailContent.toUtf8());
            state = Body;
        }
        break;

    case Body:
        if (response.startsWith("250")) {
            sendCommand("QUIT\r\n");
            state = Quit;
            emit emailSent(true, "Email sent successfully");
        } else {
            emit emailSent(false, "Failed to send email: " + response);
            state = Quit;
        }
        break;

    case Quit:
        socket->disconnectFromHost();
        break;

    default:
        break;
    }
}

inline void EmailService::sendCommand(const QString &command)
{
    qDebug() << "Sending command:" << command.trimmed();
    socket->write(command.toUtf8());
}

inline QString EmailService::encodeBase64(const QString &text)
{
    return text.toUtf8().toBase64();
}

inline void EmailService::onDisconnected()
{
    timeout->stop();
    qDebug() << "Disconnected from SMTP server";
}

inline void EmailService::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError) // FIXED: Properly handle unused parameter
    timeout->stop();
    QString errorString = socket->errorString();
    qDebug() << "Socket error:" << errorString;
    emit emailSent(false, "Connection error: " + errorString);
}

#endif // EMAILSERVICE_H
