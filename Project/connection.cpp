#include "connection.h"

// Database configuration constants (Updated for ODBC)
const QString Connection::DB_HOSTNAME = "localhost";
const QString Connection::DB_NAME = "lakhoua";
const QString Connection::DB_USERNAME = "lakhoua";
const QString Connection::DB_PASSWORD = "505";
const int Connection::DB_PORT = 1521;
const QString Connection::DB_DRIVER = "QODBC";  // Changed from QOCI to QODBC

Connection::Connection() : connected(false)
{
    // Constructor - connection will be made when createConnection() is called
}

Connection::~Connection()
{
    // Don't automatically close connection in destructor for singleton
    // closeConnection();
}

// Fixed getInstance method - don't recreate connection automatically
Connection& Connection::getInstance() {
    static Connection instance;
    return instance;
}

bool Connection::createConnection()
{
    // Close existing connection if any
    if (connected && db.isOpen()) {
        closeConnection();
    }

    // Check if QODBC driver is available
    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        QString errorMsg = QString("QODBC driver not available!\n"
                                   "Available Qt drivers: %1")
                               .arg(QSqlDatabase::drivers().join(", "));
        qDebug() << errorMsg;
        QMessageBox::critical(nullptr, "Driver Error", errorMsg);
        return false;
    }

    // Remove existing connection if it exists
    if (QSqlDatabase::contains("OracleConnection")) {
        QSqlDatabase::removeDatabase("OracleConnection");
    }

    // Add database with ODBC driver
    db = QSqlDatabase::addDatabase("QODBC", "OracleConnection");

    // Configure connection string for Oracle XE
    QString dsn = "Driver={Oracle in XE};DBQ=XE;UID=lakhoua;PWD=505;";
    db.setDatabaseName(dsn);

    qDebug() << "=== Oracle XE Connection Attempt ===";
    qDebug() << "Using DSN:" << dsn;
    qDebug() << "Available ODBC Drivers:" << QSqlDatabase::drivers();

    // Try to open the connection
    if (!db.open()) {
        QString errorDetails = QString(
                                   "Failed to connect to Oracle XE!\n\n"
                                   "Connection Details:\n"
                                   "Driver: {Oracle in XE}\n"
                                   "SID: XE\n"
                                   "Username: lakhoua\n\n"
                                   "Error: %1\n\n"
                                   "Immediate checks:\n"
                                   "1. Open Services (services.msc) and verify:\n"
                                   "   - OracleServiceXE is running\n"
                                   "   - OracleXETNSListener is running\n"
                                   "2. Test basic connectivity with:\n"
                                   "   sqlplus lakhoua/505@XE\n"
                                   "3. Verify ODBC driver in:\n"
                                   "   ODBC Data Source Administrator (64-bit)")
                                   .arg(db.lastError().text());

        qDebug() << errorDetails;
        QMessageBox::critical(nullptr, "Connection Failed", errorDetails);
        connected = false;
        return false;
    }

    connected = true;
    qDebug() << "✓ Successfully connected to Oracle XE!";

    // Verify connection with a simple query
    if (!testConnection()) {
        qDebug() << "Initial connection test failed!";
        closeConnection();
        return false;
    }

    return true;
}

void Connection::closeConnection()
{
    if (connected && db.isOpen()) {
        db.close();
        qDebug() << "Database connection closed.";
    }
    connected = false;

    // Only remove the database connection when explicitly closing
    // Don't remove it automatically to prevent issues with ongoing queries
}

bool Connection::isConnected() const
{
    return connected && db.isOpen() && db.isValid();
}

QSqlDatabase Connection::getDatabase() const
{
    // Always return the same database instance
    return QSqlDatabase::database("OracleConnection");
}

// Add a method to ensure connection is alive
bool Connection::ensureConnected()
{
    if (!isConnected()) {
        qDebug() << "Connection lost, attempting to reconnect...";
        return createConnection();
    }
    return true;
}

bool Connection::testConnection()
{
    if (!connected || !db.isOpen()) {
        qDebug() << "Cannot test connection - database not connected";
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT 1 FROM DUAL")) {
        QString errorMsg = QString("Connection test failed!\nError: %1")
        .arg(query.lastError().text());
        qDebug() << errorMsg;
        QMessageBox::warning(nullptr, "Connection Test Failed", errorMsg);
        return false;
    }

    qDebug() << "✓ Connection test successful!";
    return true;
}

bool Connection::executeQuery(const QString& queryString)
{
    if (!ensureConnected()) {
        QString errorMsg = "Database not connected! Cannot execute query.";
        qDebug() << errorMsg;
        QMessageBox::warning(nullptr, "Database Error", errorMsg);
        return false;
    }

    QSqlQuery query(getDatabase());
    if (!query.exec(queryString)) {
        QString errorMsg = QString("Query execution failed!\n\n"
                                   "Query: %1\n\n"
                                   "Error: %2")
                               .arg(queryString)
                               .arg(query.lastError().text());
        qDebug() << errorMsg;
        QMessageBox::warning(nullptr, "Query Error", errorMsg);
        return false;
    }

    qDebug() << "✓ Query executed successfully:" << queryString;
    return true;
}

QSqlQuery Connection::executeSelectQuery(const QString& queryString)
{
    QSqlQuery query(getDatabase());

    if (!ensureConnected()) {
        qDebug() << "Database not connected! Cannot execute select query.";
        return query;
    }

    if (!query.exec(queryString)) {
        QString errorMsg = QString("Select query execution failed!\n\n"
                                   "Query: %1\n\n"
                                   "Error: %2")
                               .arg(queryString)
                               .arg(query.lastError().text());
        qDebug() << errorMsg;
        QMessageBox::warning(nullptr, "Query Error", errorMsg);
    } else {
        qDebug() << "✓ Select query executed successfully:" << queryString;
    }

    return query;
}
