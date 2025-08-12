// commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>
#include <QString>
#include <QList>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>  // Add this include
#include <QSqlTableModel>
#include <QDebug>
#include <QMessageBox>
#include <QMap>
#include <QPair>
#include <QDate>

// Command data structure
struct Command {
    int commandId;
    int clientId;
    QDateTime commandDate;
    QString total;
    QString paymentMethod;
    QString deliveryAddress;

    // Additional fields for display
    QString clientName;  // For joining with clients table
    QString clientEmail; // For display purposes

    // Default constructor
    Command() : commandId(0), clientId(0), commandDate(QDateTime::currentDateTime()) {}

    // Constructor with parameters
    Command(int commandId, int clientId, const QDateTime& commandDate,
            const QString& total, const QString& paymentMethod, const QString& deliveryAddress)
        : commandId(commandId), clientId(clientId), commandDate(commandDate),
        total(total), paymentMethod(paymentMethod), deliveryAddress(deliveryAddress) {}

    // Constructor without command ID (for new commands)
    Command(int clientId, const QString& total, const QString& paymentMethod,
            const QString& deliveryAddress)
        : commandId(0), clientId(clientId), commandDate(QDateTime::currentDateTime()),
        total(total), paymentMethod(paymentMethod), deliveryAddress(deliveryAddress) {}

    // Check if command is valid
    bool isValid() const {
        return clientId > 0 && !total.isEmpty() && commandDate.isValid();
    }

    // Get total as double
    double getTotalAmount() const {
        return total.toDouble();
    }

    // Set total from double
    void setTotalAmount(double amount) {
        total = QString::number(amount, 'f', 2);
    }

    // Convert to string for debugging
    QString toString() const {
        return QString("Command[ID: %1, ClientID: %2, Date: %3, Total: %4, Payment: %5, Address: %6]")
        .arg(commandId).arg(clientId).arg(commandDate.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(total).arg(paymentMethod).arg(deliveryAddress);
    }
};

// Forward declaration
class CommandManager;

// Command DAO (Data Access Object) class
class CommandDAO : public QObject
{
    Q_OBJECT

public:
    explicit CommandDAO(QObject *parent = nullptr);
    ~CommandDAO();

    // CRUD Operations
    bool createCommand(const Command& command);
    Command readCommand(int commandId);
    QList<Command> readAllCommands();
    QList<Command> readCommandsByClient(int clientId);
    bool updateCommand(const Command& command);
    bool deleteCommand(int commandId);

    // Search operations
    QList<Command> searchCommandsByDate(const QDate& startDate, const QDate& endDate);
    QList<Command> searchCommandsByPaymentMethod(const QString& paymentMethod);
    QList<Command> searchCommandsByTotalRange(double minTotal, double maxTotal);
    QList<Command> searchCommandsByClient(const QString& clientName);

    // Utility methods
    bool commandExists(int commandId);
    int getCommandCount();
    int getCommandCountByClient(int clientId);
    double getTotalSales();
    double getTotalSalesByClient(int clientId);
    QStringList getPaymentMethods();
    bool validateClientExists(int clientId); // Make this public

    // Get commands with client information (JOIN query)
    QList<Command> getCommandsWithClientInfo();
    Command getCommandWithClientInfo(int commandId);

    // Table model for Qt views
    QSqlTableModel* getTableModel();
    QSqlTableModel* getCommandsWithClientsModel();
    void refreshTableModel();

signals:
    void commandCreated(const Command& command);
    void commandUpdated(const Command& command);
    void commandDeleted(int commandId);
    void errorOccurred(const QString& error);

private slots:
    void onModelDataChanged();

private:
    QSqlTableModel* tableModel;
    QSqlTableModel* joinedModel;

    // Helper methods
    bool executeQuery(QSqlQuery& query, const QString& operation);
    void logError(const QString& operation, const QSqlError& error);
    Command createCommandFromQuery(const QSqlQuery& query, bool includeClientInfo = false);
};

// Command Manager class for business logic
class CommandManager : public QObject
{
    Q_OBJECT

public:
    explicit CommandManager(QObject *parent = nullptr);

    // Business logic methods
    bool addNewCommand(const Command& command);
    bool modifyCommand(const Command& command);
    bool removeCommand(int commandId);
    Command getCommand(int commandId);
    QList<Command> getAllCommands();
    QList<Command> getClientCommands(int clientId);

    // Business calculations
    double calculateClientTotal(int clientId);
    int getClientCommandCount(int clientId);
    QList<Command> getRecentCommands(int days = 30);
    QList<Command> getTopCommands(int limit = 10);

    // Validation methods
    bool validateCommand(const Command& command, QString& errorMessage);
    bool validateTotal(const QString& total);
    bool validateClientId(int clientId);
    bool validatePaymentMethod(const QString& paymentMethod);

    // Get DAO reference
    CommandDAO* getDAO() const;

signals:
    void commandAdded(const Command& command);
    void commandModified(const Command& command);
    void commandRemoved(int commandId);
    void validationError(const QString& error);

private:
    CommandDAO* dao;

    // Validation helpers
    bool isValidTotal(const QString& total);
    bool isValidPaymentMethod(const QString& paymentMethod);
    QString sanitizeInput(const QString& input);
    QStringList getValidPaymentMethods() const;
};

// Command Statistics helper class
class CommandStatistics : public QObject
{
    Q_OBJECT

public:
    explicit CommandStatistics(CommandDAO* dao, QObject *parent = nullptr);

    struct Statistics {
        int totalCommands;
        double totalSales;
        double averageOrderValue;
        int topClientId;
        QString topClientName;
        QString mostUsedPaymentMethod;
        QDate firstOrderDate;
        QDate lastOrderDate;

        // Initialize with default values
        Statistics() : totalCommands(0), totalSales(0.0), averageOrderValue(0.0),
            topClientId(0) {}
    };

    Statistics getOverallStatistics();
    Statistics getClientStatistics(int clientId);
    QMap<QString, int> getPaymentMethodStats();
    QMap<QDate, double> getDailySales(const QDate& startDate, const QDate& endDate);
    QList<QPair<int, double>> getTopClientsByTotal(int limit = 10);

private:
    CommandDAO* commandDAO;
};

#endif // COMMANDS_H
