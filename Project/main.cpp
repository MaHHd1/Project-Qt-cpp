#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QSqlDatabase>
#include <QDir>

#include "mainwindow.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    // Create the Qt application
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("Client Management System");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Your Company");
    app.setOrganizationDomain("yourcompany.com");

    qDebug() << "=== Starting Client Management System ===";
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Working Directory:" << QDir::currentPath();

    // Check available SQL drivers
    qDebug() << "Available SQL drivers:" << QSqlDatabase::drivers();

    // Initialize database connection
    qDebug() << "Initializing database connection...";
    Connection& conn = Connection::getInstance();

    if (!conn.createConnection()) {
        qDebug() << "Failed to establish database connection!";

        // Show error message to user
        QMessageBox::critical(nullptr,
                              "Database Connection Error",
                              "Failed to connect to the lakhoua database.\n\n"
                              "Please ensure that:\n"
                              "• Oracle service is running\n"
                              "• TNS Listener is active on port 1521\n"
                              "• Database 'lakhoua' is accessible\n"
                              "• Username 'lakhoua' and password are correct\n\n"
                              "Application will now exit.");

        return -1; // Exit with error code
    }

    qDebug() << "✓ Database connection established successfully!";

    // Test database tables
    qDebug() << "Testing database tables...";
    QStringList tables = conn.getDatabase().tables();
    qDebug() << "Available tables:" << tables;

    // Check for required tables
    bool clientsTableExists = false;
    bool commandsTableExists = false;

    for (const QString& table : tables) {
        if (table.toUpper() == "CLIENTS") {
            clientsTableExists = true;
            qDebug() << "✓ CLIENTS table found";
        }
        if (table.toUpper() == "COMMANDES") {
            commandsTableExists = true;
            qDebug() << "✓ COMMANDES table found";
        }
    }

    // Warn if tables don't exist
    if (!clientsTableExists) {
        qDebug() << "⚠ WARNING: CLIENTS table not found";
        QMessageBox::warning(nullptr,
                             "Table Warning",
                             "CLIENTS table not found in database.\n"
                             "Some features may not work correctly.");
    }

    if (!commandsTableExists) {
        qDebug() << "⚠ WARNING: COMMANDES table not found";
        QMessageBox::warning(nullptr,
                             "Table Warning",
                             "COMMANDES table not found in database.\n"
                             "Some features may not work correctly.");
    }

    // Test database with simple queries
    if (clientsTableExists) {
        QSqlQuery clientCount = conn.executeSelectQuery("SELECT COUNT(*) FROM CLIENTS");
        if (clientCount.next()) {
            int count = clientCount.value(0).toInt();
            qDebug() << "Number of clients in database:" << count;
        }
    }

    if (commandsTableExists) {
        QSqlQuery commandCount = conn.executeSelectQuery("SELECT COUNT(*) FROM COMMANDES");
        if (commandCount.next()) {
            int count = commandCount.value(0).toInt();
            qDebug() << "Number of commands in database:" << count;
        }
    }

    // Create and show the main window
    qDebug() << "Creating main window...";
    MainWindow window;

    // Set window properties
    window.setWindowTitle("Client Management System - Connected to lakhoua");
    window.setWindowIcon(QIcon(":/resources/app_icon.png")); // If you have an icon

    // Show the main window
    window.show();
    qDebug() << "✓ Main window displayed";

    // Show success message
    QMessageBox::information(&window,
                             "Connection Successful",
                             QString("Successfully connected to database 'lakhoua'!\n\n"
                                     "Tables found:\n"
                                     "• CLIENTS: %1\n"
                                     "• COMMANDES: %2")
                                 .arg(clientsTableExists ? "✓ Available" : "✗ Missing")
                                 .arg(commandsTableExists ? "✓ Available" : "✗ Missing"));

    qDebug() << "=== Application started successfully ===";

    // Start the Qt event loop
    int result = app.exec();

    // Cleanup when application exits
    qDebug() << "Application shutting down...";
    conn.closeConnection();
    qDebug() << "Database connection closed.";
    qDebug() << "Application exit code:" << result;

    return result;
}
