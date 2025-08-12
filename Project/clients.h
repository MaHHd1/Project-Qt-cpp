// clients.h
#ifndef CLIENTS_H
#define CLIENTS_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QDebug>
#include <QMessageBox>

// Client data structure
struct Client {
    int id;
    QString name;
    QString email;
    QString city;
    QString postal;
    QString address;

    // Default constructor
    Client() : id(0) {}

    // Constructor with parameters
    Client(int id, const QString& name, const QString& email,
           const QString& city, const QString& postal, const QString& address)
        : id(id), name(name), email(email), city(city), postal(postal), address(address) {}

    // Constructor without ID (for new clients)
    Client(const QString& name, const QString& email,
           const QString& city, const QString& postal, const QString& address)
        : id(0), name(name), email(email), city(city), postal(postal), address(address) {}

    // Check if client is valid
    bool isValid() const {
        return !name.isEmpty() && !email.isEmpty();
    }

    // Convert to string for debugging
    QString toString() const {
        return QString("Client[ID: %1, Name: %2, Email: %3, City: %4, Postal: %5, Address: %6]")
        .arg(id).arg(name).arg(email).arg(city).arg(postal).arg(address);
    }
};

// Client DAO (Data Access Object) class
class ClientDAO : public QObject
{
    Q_OBJECT

public:
    explicit ClientDAO(QObject *parent = nullptr);
    ~ClientDAO();

    // CRUD Operations
    bool createClient(const Client& client);
    Client readClient(int id);
    QList<Client> readAllClients();
    bool updateClient(const Client& client);
    bool deleteClient(int id);

    // Search operations
    QList<Client> searchClientsByName(const QString& name);
    QList<Client> searchClientsByEmail(const QString& email);
    QList<Client> searchClientsByCity(const QString& city);

    // Utility methods
    bool clientExists(int id);
    bool emailExists(const QString& email, int excludeId = -1);
    int getClientCount();
    Client getClientByEmail(const QString& email);

    // Table model for Qt views
    QSqlTableModel* getTableModel();
    void refreshTableModel();

signals:
    void clientCreated(const Client& client);
    void clientUpdated(const Client& client);
    void clientDeleted(int clientId);
    void errorOccurred(const QString& error);

private slots:
    void onModelDataChanged();

private:
    QSqlTableModel* tableModel;

    // Helper methods
    bool executeQuery(QSqlQuery& query, const QString& operation);
    void logError(const QString& operation, const QSqlError& error);
    Client createClientFromQuery(const QSqlQuery& query);
};

// Client Manager class for business logic
class ClientManager : public QObject
{
    Q_OBJECT

public:
    explicit ClientManager(QObject *parent = nullptr);

    // Business logic methods
    bool addNewClient(const Client& client);
    bool modifyClient(const Client& client);
    bool removeClient(int id);
    Client getClient(int id);
    QList<Client> getAllClients();

    // Validation methods
    bool validateClient(const Client& client, QString& errorMessage);
    bool validateEmail(const QString& email);
    bool validateName(const QString& name);

    // Get DAO reference
    ClientDAO* getDAO() const;

signals:
    void clientAdded(const Client& client);
    void clientModified(const Client& client);
    void clientRemoved(int clientId);
    void validationError(const QString& error);

private:
    ClientDAO* dao;

    // Validation helpers
    bool isValidEmail(const QString& email);
    bool isValidName(const QString& name);
    QString sanitizeInput(const QString& input);
};

#endif // CLIENTS_H
