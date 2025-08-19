#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include <QMessageBox>

class Connection
{
public:
    Connection();
    ~Connection();

    // Static method to get database instance
    static Connection& getInstance();

    // Database connection methods
    bool createConnection();
    void closeConnection();
    bool isConnected() const;
    bool ensureConnected(); // New method to ensure connection is alive

    // Get database reference
    QSqlDatabase getDatabase() const;

    // Test connection
    bool testConnection();

    // Execute query methods
    bool executeQuery(const QString& queryString);
    QSqlQuery executeSelectQuery(const QString& queryString);

private:
    QSqlDatabase db;
    bool connected;

    // Database configuration for lakhoua
    static const QString DB_HOSTNAME;
    static const QString DB_NAME;
    static const QString DB_USERNAME;
    static const QString DB_PASSWORD;
    static const int DB_PORT;
    static const QString DB_DRIVER;
};

#endif // CONNECTION_H
