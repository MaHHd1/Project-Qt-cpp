#include "commands.h"
#include "connection.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QRegularExpression>
#include <QApplication>

// CommandDAO Implementation
CommandDAO::CommandDAO(QObject *parent)
    : QObject(parent), tableModel(nullptr), joinedModel(nullptr)
{
    // Initialize table model
    Connection& conn = Connection::getInstance();
    if (conn.isConnected()) {
        // Get fresh database reference each time
        QSqlDatabase db = conn.getDatabase();
        tableModel = new QSqlTableModel(this, db);
        tableModel->setTable("COMMANDS");
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        // Set column headers
        tableModel->setHeaderData(0, Qt::Horizontal, "COMMAND_ID");
        tableModel->setHeaderData(1, Qt::Horizontal, "CLIENT_ID");
        tableModel->setHeaderData(2, Qt::Horizontal, "COMMAND_DATE");
        tableModel->setHeaderData(3, Qt::Horizontal, "TOTAL");
        tableModel->setHeaderData(4, Qt::Horizontal, "PAYMENT_METHOD");
        tableModel->setHeaderData(5, Qt::Horizontal, "DELIVERY_ADDRESS");

        // Initialize joined model
        joinedModel = new QSqlTableModel(this, db);

        // Load data
        refreshTableModel();
    } else {
        qDebug() << "Warning: Database not connected when creating CommandDAO";
    }
}

CommandDAO::~CommandDAO()
{
    if (tableModel) {
        delete tableModel;
    }
    if (joinedModel) {
        delete joinedModel;
    }
}

bool CommandDAO::createCommand(const Command& command) {
    if (!command.isValid()) {
        emit errorOccurred("Invalid command data");
        return false;
    }

    Connection& conn = Connection::getInstance();
    if (!conn.ensureConnected()) {
        emit errorOccurred("Database connection error");
        return false;
    }

    QSqlDatabase db = conn.getDatabase();
    db.transaction();

    try {
        QSqlQuery seqQuery(db);
        if (!seqQuery.exec("SELECT LAKHOUA.COMMANDS_SEQ.NEXTVAL FROM DUAL") || !seqQuery.next()) {
            throw std::runtime_error("Sequence error: " + seqQuery.lastError().text().toStdString());
        }
        int newId = seqQuery.value(0).toInt();

        QSqlQuery query(db);
        query.prepare(
            "INSERT INTO LAKHOUA.COMMANDS (COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS) "
            "VALUES (:commandId, :clientId, TO_DATE(:commandDate, 'YYYY-MM-DD HH24:MI:SS'), :total, :paymentMethod, :deliveryAddress)"
            );

        query.bindValue(":commandId", newId);
        query.bindValue(":clientId", command.clientId);
        query.bindValue(":commandDate", command.commandDate.toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":total", command.total);
        query.bindValue(":paymentMethod", command.paymentMethod);
        query.bindValue(":deliveryAddress", command.deliveryAddress);

        if (!query.exec()) {
            throw std::runtime_error("Insert failed: " + query.lastError().text().toStdString());
        }

        if (!db.commit()) {
            throw std::runtime_error("Commit failed");
        }

        Command newCommand = command;
        newCommand.commandId = newId;
        emit commandCreated(newCommand);
        refreshTableModel();
        return true;

    } catch (const std::exception& e) {
        db.rollback();
        emit errorOccurred(QString::fromStdString(e.what()));
        return false;
    }
}

Command CommandDAO::readCommand(int commandId) {
    Connection& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        qDebug() << "Database not connected in readCommand";
        return Command();
    }

    QSqlDatabase db = conn.getDatabase();
    QSqlQuery query(db);  // Specify the database for the query

    query.prepare("SELECT COMMAND_ID, CLIENT_ID, TO_CHAR(COMMAND_DATE, 'YYYY-MM-DD HH24:MI:SS') as COMMAND_DATE, "
                  "TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDS WHERE COMMAND_ID = :commandId");
    query.bindValue(":commandId", commandId);

    if (!query.exec()) {
        logError("Read Command", query.lastError());
        return Command();
    }

    if (query.next()) {
        Command command;
        command.commandId = query.value("COMMAND_ID").toInt();
        command.clientId = query.value("CLIENT_ID").toInt();

        // Handle date parsing more robustly
        QString dateString = query.value("COMMAND_DATE").toString();
        QDateTime dateTime = QDateTime::fromString(dateString, "yyyy-MM-dd hh:mm:ss");
        if (!dateTime.isValid()) {
            qWarning() << "Invalid date format from database:" << dateString;
            dateTime = QDateTime::currentDateTime();
        }
        command.commandDate = dateTime;

        command.total = query.value("TOTAL").toString();
        command.paymentMethod = query.value("PAYMENT_METHOD").toString();
        command.deliveryAddress = query.value("DELIVERY_ADDRESS").toString();
        return command;
    }

    return Command();
}

QList<Command> CommandDAO::readAllCommands() {
    QList<Command> commands;

    // Get the database connection and ensure it's connected
    Connection& conn = Connection::getInstance();
    if (!conn.ensureConnected()) {
        qDebug() << "Database not connected in readAllCommands";
        return commands;
    }

    // Get the database instance using the connection name
    QSqlDatabase db = conn.getDatabase();
    QSqlQuery query(db);

    // Use explicit column names and proper Oracle date formatting
    QString queryString = "SELECT COMMAND_ID, CLIENT_ID, "
                          "TO_CHAR(COMMAND_DATE, 'YYYY-MM-DD HH24:MI:SS') as COMMAND_DATE_STR, "
                          "TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                          "FROM COMMANDS "
                          "ORDER BY COMMAND_DATE DESC";

    if (!query.exec(queryString)) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        qDebug() << "Database open status:" << db.isOpen();
        qDebug() << "Database valid status:" << db.isValid();
        qDebug() << "Connection status:" << conn.isConnected();
        return commands;
    }

    while (query.next()) {
        Command cmd;
        cmd.commandId = query.value("COMMAND_ID").toInt();
        cmd.clientId = query.value("CLIENT_ID").toInt();

        // Parse the Oracle date string
        QString dateStr = query.value("COMMAND_DATE_STR").toString();
        cmd.commandDate = QDateTime::fromString(dateStr, "yyyy-MM-dd hh:mm:ss");
        if (!cmd.commandDate.isValid()) {
            qDebug() << "Invalid date format:" << dateStr;
            cmd.commandDate = QDateTime::currentDateTime();
        }

        cmd.total = query.value("TOTAL").toString();
        cmd.paymentMethod = query.value("PAYMENT_METHOD").toString();
        cmd.deliveryAddress = query.value("DELIVERY_ADDRESS").toString();

        commands.append(cmd);
    }

    qDebug() << "Retrieved" << commands.size() << "commands from database";
    return commands;
}

QList<Command> CommandDAO::readCommandsByClient(int clientId)
{
    QList<Command> commands;
    QSqlQuery query;
    query.prepare("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDS WHERE CLIENT_ID = ? ORDER BY COMMAND_DATE DESC");
    query.addBindValue(clientId);

    if (executeQuery(query, "Read Commands By Client")) {
        while (query.next()) {
            commands.append(createCommandFromQuery(query));
        }
    }

    return commands;
}

bool CommandDAO::updateCommand(const Command& command)
{
    if (!command.isValid() || command.commandId <= 0) {
        emit errorOccurred("Invalid command data for update");
        return false;
    }

    if (!validateClientExists(command.clientId)) {
        emit errorOccurred("Client does not exist");
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE COMMANDS SET "
                  "CLIENT_ID = :clientId, "
                  "COMMAND_DATE = TO_DATE(:commandDate, 'YYYY-MM-DD HH24:MI:SS'), "
                  "TOTAL = :total, "
                  "PAYMENT_METHOD = :paymentMethod, "
                  "DELIVERY_ADDRESS = :deliveryAddress "
                  "WHERE COMMAND_ID = :commandId");

    query.bindValue(":clientId", command.clientId);
    query.bindValue(":commandDate", command.commandDate.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":total", command.total);
    query.bindValue(":paymentMethod", command.paymentMethod);
    query.bindValue(":deliveryAddress", command.deliveryAddress);
    query.bindValue(":commandId", command.commandId);

    if (!query.exec()) {
        logError("Update Command", query.lastError());
        return false;
    }

    refreshTableModel();
    emit commandUpdated(command);
    return true;
}

bool CommandDAO::deleteCommand(int commandId) {
    Connection& conn = Connection::getInstance();
    if (!conn.ensureConnected()) {
        qDebug() << "Database connection failed in deleteCommand";
        emit errorOccurred("Database connection error");
        return false;
    }

    QSqlDatabase db = conn.getDatabase();
    QSqlQuery query(db);

    query.prepare("DELETE FROM COMMANDS WHERE COMMAND_ID = :commandId");
    query.bindValue(":commandId", commandId);

    if (!query.exec()) {
        qDebug() << "Delete failed:" << query.lastError().text();
        emit errorOccurred(QString("Delete failed: %1").arg(query.lastError().text()));
        return false;
    }

    // Check if any row was actually deleted
    if (query.numRowsAffected() == 0) {
        qDebug() << "No command found with ID:" << commandId;
        emit errorOccurred(QString("No command found with ID: %1").arg(commandId));
        return false;
    }

    refreshTableModel();
    emit commandDeleted(commandId);
    return true;
}

QList<Command> CommandDAO::searchCommandsByDate(const QDate& startDate, const QDate& endDate)
{
    QList<Command> commands;
    QSqlQuery query;
    query.prepare("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDS WHERE DATE(COMMAND_DATE) BETWEEN ? AND ? ORDER BY COMMAND_DATE DESC");
    query.addBindValue(startDate);
    query.addBindValue(endDate);

    if (executeQuery(query, "Search Commands By Date")) {
        while (query.next()) {
            commands.append(createCommandFromQuery(query));
        }
    }

    return commands;
}

QList<Command> CommandDAO::searchCommandsByPaymentMethod(const QString& paymentMethod)
{
    QList<Command> commands;
    QSqlQuery query;
    query.prepare("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDS WHERE UPPER(PAYMENT_METHOD) LIKE UPPER(?) ORDER BY COMMAND_DATE DESC");
    query.addBindValue("%" + paymentMethod + "%");

    if (executeQuery(query, "Search Commands By Payment Method")) {
        while (query.next()) {
            commands.append(createCommandFromQuery(query));
        }
    }

    return commands;
}

QList<Command> CommandDAO::searchCommandsByTotalRange(double minTotal, double maxTotal)
{
    QList<Command> commands;
    QSqlQuery query;
    query.prepare("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDS WHERE CAST(TOTAL AS DECIMAL) BETWEEN ? AND ? ORDER BY TOTAL DESC");
    query.addBindValue(minTotal);
    query.addBindValue(maxTotal);

    if (executeQuery(query, "Search Commands By Total Range")) {
        while (query.next()) {
            commands.append(createCommandFromQuery(query));
        }
    }

    return commands;
}

QList<Command> CommandDAO::searchCommandsByClient(const QString& clientName)
{
    QList<Command> commands;
    QSqlQuery query;
    query.prepare("SELECT c.COMMAND_ID, c.CLIENT_ID, c.COMMAND_DATE, c.TOTAL, c.PAYMENT_METHOD, c.DELIVERY_ADDRESS, "
                  "cl.NAME, cl.EMAIL FROM COMMANDS c "
                  "JOIN CLIENTS cl ON c.CLIENT_ID = cl.ID "
                  "WHERE UPPER(cl.NAME) LIKE UPPER(?) ORDER BY c.COMMAND_DATE DESC");
    query.addBindValue("%" + clientName + "%");

    if (executeQuery(query, "Search Commands By Client")) {
        while (query.next()) {
            commands.append(createCommandFromQuery(query, true));
        }
    }

    return commands;
}

bool CommandDAO::commandExists(int commandId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM COMMANDS WHERE COMMAND_ID = ?");
    query.addBindValue(commandId);

    if (executeQuery(query, "Check Command Exists")) {
        if (query.next()) {
            return query.value(0).toInt() > 0;
        }
    }

    return false;
}

int CommandDAO::getCommandCount()
{
    Connection& conn = Connection::getInstance();
    if (!conn.ensureConnected()) {
        qDebug() << "Database not connected in getCommandCount";
        return 0;
    }

    QSqlDatabase db = conn.getDatabase();
    QSqlQuery query(db);

    if (!query.exec("SELECT COUNT(*) FROM COMMANDS")) {
        qDebug() << "Get Command Count failed:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "Command count:" << count;
        return count;
    }

    return 0;
}

int CommandDAO::getCommandCountByClient(int clientId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM COMMANDS WHERE CLIENT_ID = ?");
    query.addBindValue(clientId);

    if (executeQuery(query, "Get Command Count By Client")) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    }

    return 0;
}

double CommandDAO::getTotalSales()
{
    Connection& conn = Connection::getInstance();
    if (!conn.ensureConnected()) {
        qDebug() << "Database not connected in getTotalSales";
        return 0.0;
    }

    QSqlDatabase db = conn.getDatabase();
    QSqlQuery query(db);

    if (!query.exec("SELECT SUM(CAST(TOTAL AS DECIMAL)) FROM COMMANDS")) {
        qDebug() << "Get Total Sales failed:" << query.lastError().text();
        return 0.0;
    }

    if (query.next()) {
        double total = query.value(0).toDouble();
        qDebug() << "Total sales:" << total;
        return total;
    }

    return 0.0;
}

double CommandDAO::getTotalSalesByClient(int clientId)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(CAST(TOTAL AS DECIMAL)) FROM COMMANDS WHERE CLIENT_ID = ?");
    query.addBindValue(clientId);

    if (executeQuery(query, "Get Total Sales By Client")) {
        if (query.next()) {
            return query.value(0).toDouble();
        }
    }

    return 0.0;
}

QStringList CommandDAO::getPaymentMethods()
{
    QStringList methods;
    QSqlQuery query("SELECT DISTINCT PAYMENT_METHOD FROM COMMANDS WHERE PAYMENT_METHOD IS NOT NULL ORDER BY PAYMENT_METHOD");

    if (executeQuery(query, "Get Payment Methods")) {
        while (query.next()) {
            QString method = query.value(0).toString();
            if (!method.isEmpty()) {
                methods.append(method);
            }
        }
    }

    return methods;
}

QList<Command> CommandDAO::getCommandsWithClientInfo()
{
    QList<Command> commands;
    QSqlQuery query("SELECT c.COMMAND_ID, c.CLIENT_ID, c.COMMAND_DATE, c.TOTAL, c.PAYMENT_METHOD, c.DELIVERY_ADDRESS, "
                    "cl.NAME, cl.EMAIL FROM COMMANDS c "
                    "LEFT JOIN CLIENTS cl ON c.CLIENT_ID = cl.ID "
                    "ORDER BY c.COMMAND_DATE DESC");

    if (executeQuery(query, "Get Commands With Client Info")) {
        while (query.next()) {
            commands.append(createCommandFromQuery(query, true));
        }
    }

    return commands;
}

Command CommandDAO::getCommandWithClientInfo(int commandId)
{
    QSqlQuery query;
    query.prepare("SELECT c.COMMAND_ID, c.CLIENT_ID, c.COMMAND_DATE, c.TOTAL, c.PAYMENT_METHOD, c.DELIVERY_ADDRESS, "
                  "cl.NAME, cl.EMAIL FROM COMMANDS c "
                  "LEFT JOIN CLIENTS cl ON c.CLIENT_ID = cl.ID "
                  "WHERE c.COMMAND_ID = ?");
    query.addBindValue(commandId);

    if (executeQuery(query, "Get Command With Client Info")) {
        if (query.next()) {
            return createCommandFromQuery(query, true);
        }
    }

    return Command();
}

QSqlTableModel* CommandDAO::getTableModel()
{
    return tableModel;
}

QSqlTableModel* CommandDAO::getCommandsWithClientsModel()
{
    if (joinedModel) {
        // Since QSqlTableModel doesn't handle JOINs well, we'll use a custom approach
        // This is a simplified version - you might want to use QSqlQueryModel instead
        joinedModel->setQuery("SELECT c.COMMAND_ID, c.CLIENT_ID, c.COMMAND_DATE, c.TOTAL, "
                              "c.PAYMENT_METHOD, c.DELIVERY_ADDRESS, cl.NAME as CLIENT_NAME, cl.EMAIL as CLIENT_EMAIL "
                              "FROM COMMANDS c LEFT JOIN CLIENTS cl ON c.CLIENT_ID = cl.ID "
                              "ORDER BY c.COMMAND_DATE DESC");
    }
    return joinedModel;
}

void CommandDAO::refreshTableModel()
{
    if (tableModel) {
        tableModel->select();
    }
}

void CommandDAO::onModelDataChanged()
{
    // Handle model data changes if needed
    qDebug() << "Command table model data changed";
}

bool CommandDAO::executeQuery(QSqlQuery& query, const QString& operation)
{
    if (!query.exec()) {
        logError(operation, query.lastError());
        return false;
    }
    return true;
}


void CommandDAO::logError(const QString& operation, const QSqlError& error)
{
    QString errorMsg = QString("%1 failed: %2").arg(operation, error.text());
    qDebug() << errorMsg;
    emit errorOccurred(errorMsg);
}

Command CommandDAO::createCommandFromQuery(const QSqlQuery& query, bool includeClientInfo)
{
    Command command;
    command.commandId = query.value("COMMAND_ID").toInt();
    command.clientId = query.value("CLIENT_ID").toInt();

    // FIXED: Replace deprecated type() call
    QVariant dateValue = query.value("COMMAND_DATE");

    // Use typeId() instead of deprecated type()
    if (dateValue.typeId() == QMetaType::QDateTime) {
        command.commandDate = dateValue.toDateTime();
    } else {
        // Fallback for string dates
        QString dateStr = dateValue.toString();
        command.commandDate = QDateTime::fromString(dateStr, "yyyy-MM-dd hh:mm:ss");
        if (!command.commandDate.isValid()) {
            command.commandDate = QDateTime::currentDateTime();
        }
    }

    command.total = query.value("TOTAL").toString();
    command.paymentMethod = query.value("PAYMENT_METHOD").toString();
    command.deliveryAddress = query.value("DELIVERY_ADDRESS").toString();

    if (includeClientInfo) {
        if (query.record().contains("NAME")) {
            command.clientName = query.value("NAME").toString();
        }
        if (query.record().contains("EMAIL")) {
            command.clientEmail = query.value("EMAIL").toString();
        }
    }

    return command;
}

bool CommandDAO::validateClientExists(int clientId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM CLIENTS WHERE ID = ?");
    query.addBindValue(clientId);

    if (executeQuery(query, "Validate Client Exists")) {
        if (query.next()) {
            return query.value(0).toInt() > 0;
        }
    }

    return false;
}

// CommandManager Implementation
CommandManager::CommandManager(QObject *parent)
    : QObject(parent)
{
    dao = new CommandDAO(this);

    // Connect DAO signals to manager signals
    connect(dao, &CommandDAO::commandCreated, this, &CommandManager::commandAdded);
    connect(dao, &CommandDAO::commandUpdated, this, &CommandManager::commandModified);
    connect(dao, &CommandDAO::commandDeleted, this, &CommandManager::commandRemoved);
    connect(dao, &CommandDAO::errorOccurred, this, &CommandManager::validationError);
}

bool CommandManager::addNewCommand(const Command& command)
{
    QString errorMessage;
    if (!validateCommand(command, errorMessage)) {
        emit validationError(errorMessage);
        return false;
    }

    return dao->createCommand(command);
}

bool CommandManager::modifyCommand(const Command& command)
{
    QString errorMessage;
    if (!validateCommand(command, errorMessage)) {
        emit validationError(errorMessage);
        return false;
    }

    if (command.commandId <= 0) {
        emit validationError("Invalid command ID for update");
        return false;
    }

    return dao->updateCommand(command);
}

bool CommandManager::removeCommand(int commandId)
{
    if (commandId <= 0) {
        qDebug() << "Invalid command ID:" << commandId;
        emit validationError("Invalid command ID");
        return false;
    }

    qDebug() << "Attempting to remove command ID:" << commandId;
    bool result = dao->deleteCommand(commandId);
    qDebug() << "Remove command result:" << result;
    return result;
}

Command CommandManager::getCommand(int commandId)
{
    return dao->readCommand(commandId);
}

QList<Command> CommandManager::getAllCommands()
{
    return dao->readAllCommands();
}

QList<Command> CommandManager::getClientCommands(int clientId)
{
    return dao->readCommandsByClient(clientId);
}

double CommandManager::calculateClientTotal(int clientId)
{
    return dao->getTotalSalesByClient(clientId);
}

int CommandManager::getClientCommandCount(int clientId)
{
    return dao->getCommandCountByClient(clientId);
}

QList<Command> CommandManager::getRecentCommands(int days)
{
    QDate startDate = QDate::currentDate().addDays(-days);
    QDate endDate = QDate::currentDate();
    return dao->searchCommandsByDate(startDate, endDate);
}

QList<Command> CommandManager::getTopCommands(int limit)
{
    // Get all commands and sort by total
    QList<Command> allCommands = dao->readAllCommands();

    // Sort by total amount descending
    std::sort(allCommands.begin(), allCommands.end(), [](const Command& a, const Command& b) {
        return a.getTotalAmount() > b.getTotalAmount();
    });

    // Return top commands up to limit
    if (allCommands.size() > limit) {
        return allCommands.mid(0, limit);
    }

    return allCommands;
}

bool CommandManager::validateCommand(const Command& command, QString& errorMessage) {
    if (command.clientId <= 0) {
        errorMessage = "Invalid client ID";
        return false;
    }

    if (command.total.isEmpty() || !isValidTotal(command.total)) {
        errorMessage = "Invalid total amount";
        return false;
    }

    if (command.paymentMethod.trimmed().isEmpty()) {
        errorMessage = "Payment method cannot be empty";
        return false;
    }

    if (command.deliveryAddress.trimmed().isEmpty()) {
        errorMessage = "Delivery address cannot be empty";
        return false;
    }

    if (!command.commandDate.isValid()) {
        errorMessage = "Invalid command date";
        return false;
    }

    return true;
}

bool CommandManager::validateTotal(const QString& total)
{
    return isValidTotal(total);
}

bool CommandManager::validateClientId(int clientId)
{
    return clientId > 0 && dao->validateClientExists(clientId);
}

bool CommandManager::validatePaymentMethod(const QString& paymentMethod)
{
    return isValidPaymentMethod(paymentMethod);
}

CommandDAO* CommandManager::getDAO() const
{
    return dao;
}

bool CommandManager::isValidTotal(const QString& total)
{
    if (total.trimmed().isEmpty()) {
        return false;
    }

    bool ok;
    double amount = total.toDouble(&ok);
    return ok && amount >= 0;
}

bool CommandManager::isValidPaymentMethod(const QString& paymentMethod)
{
    QString trimmed = paymentMethod.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }

    QStringList validMethods = getValidPaymentMethods();
    return validMethods.contains(trimmed, Qt::CaseInsensitive) || validMethods.isEmpty();
}

QString CommandManager::sanitizeInput(const QString& input)
{
    QString sanitized = input.trimmed();
    // Remove potentially dangerous characters
    sanitized.remove(QRegularExpression("[<>\"'%;()&+]"));
    return sanitized;
}

QStringList CommandManager::getValidPaymentMethods() const
{
    // Define valid payment methods - you can modify this list as needed
    return QStringList() << "Cash" << "Credit Card" << "Debit Card" << "PayPal"
                         << "Bank Transfer" << "Check" << "Mobile Payment";
}

// CommandStatistics Implementation
CommandStatistics::CommandStatistics(CommandDAO* dao, QObject *parent)
    : QObject(parent), commandDAO(dao)
{
}

CommandStatistics::Statistics CommandStatistics::getOverallStatistics()
{
    Statistics stats;

    stats.totalCommands = commandDAO->getCommandCount();
    stats.totalSales = commandDAO->getTotalSales();

    if (stats.totalCommands > 0) {
        stats.averageOrderValue = stats.totalSales / stats.totalCommands;
    } else {
        stats.averageOrderValue = 0.0;
    }

    // Get top client by total sales
    auto topClients = getTopClientsByTotal(1);
    if (!topClients.isEmpty()) {
        stats.topClientId = topClients.first().first;

        // Get client name
        QSqlQuery query;
        query.prepare("SELECT NAME FROM CLIENTS WHERE ID = ?");
        query.addBindValue(stats.topClientId);
        if (query.exec() && query.next()) {
            stats.topClientName = query.value(0).toString();
        }
    }

    // Get most used payment method
    auto paymentStats = getPaymentMethodStats();
    if (!paymentStats.isEmpty()) {
        auto it = std::max_element(paymentStats.begin(), paymentStats.end(),
                                   [](const auto& a, const auto& b) { return a < b; });
        stats.mostUsedPaymentMethod = it.key();
    }

    // Get date range
    QSqlQuery dateQuery("SELECT MIN(COMMAND_DATE), MAX(COMMAND_DATE) FROM COMMANDS");
    if (dateQuery.exec() && dateQuery.next()) {
        stats.firstOrderDate = dateQuery.value(0).toDate();
        stats.lastOrderDate = dateQuery.value(1).toDate();
    }

    return stats;
}

CommandStatistics::Statistics CommandStatistics::getClientStatistics(int clientId)
{
    Statistics stats;

    stats.totalCommands = commandDAO->getCommandCountByClient(clientId);
    stats.totalSales = commandDAO->getTotalSalesByClient(clientId);

    if (stats.totalCommands > 0) {
        stats.averageOrderValue = stats.totalSales / stats.totalCommands;
    } else {
        stats.averageOrderValue = 0.0;
    }

    stats.topClientId = clientId;

    // Get client name
    QSqlQuery query;
    query.prepare("SELECT NAME FROM CLIENTS WHERE ID = ?");
    query.addBindValue(clientId);
    if (query.exec() && query.next()) {
        stats.topClientName = query.value(0).toString();
    }

    // Get date range for this client
    QSqlQuery dateQuery;
    dateQuery.prepare("SELECT MIN(COMMAND_DATE), MAX(COMMAND_DATE) FROM COMMANDS WHERE CLIENT_ID = ?");
    dateQuery.addBindValue(clientId);
    if (dateQuery.exec() && dateQuery.next()) {
        stats.firstOrderDate = dateQuery.value(0).toDate();
        stats.lastOrderDate = dateQuery.value(1).toDate();
    }

    return stats;
}

QMap<QString, int> CommandStatistics::getPaymentMethodStats()
{
    QMap<QString, int> stats;

    QSqlQuery query("SELECT PAYMENT_METHOD, COUNT(*) FROM COMMANDS "
                    "WHERE PAYMENT_METHOD IS NOT NULL "
                    "GROUP BY PAYMENT_METHOD");

    while (query.next()) {
        QString method = query.value(0).toString();
        int count = query.value(1).toInt();
        stats[method] = count;
    }

    return stats;
}

QMap<QDate, double> CommandStatistics::getDailySales(const QDate& startDate, const QDate& endDate)
{
    QMap<QDate, double> sales;

    QSqlQuery query;
    query.prepare("SELECT DATE(COMMAND_DATE), SUM(CAST(TOTAL AS DECIMAL)) "
                  "FROM COMMANDS "
                  "WHERE DATE(COMMAND_DATE) BETWEEN ? AND ? "
                  "GROUP BY DATE(COMMAND_DATE) "
                  "ORDER BY DATE(COMMAND_DATE)");
    query.addBindValue(startDate);
    query.addBindValue(endDate);

    if (query.exec()) {
        while (query.next()) {
            QDate date = query.value(0).toDate();
            double total = query.value(1).toDouble();
            sales[date] = total;
        }
    }

    return sales;
}

QList<QPair<int, double>> CommandStatistics::getTopClientsByTotal(int limit)
{
    QList<QPair<int, double>> topClients;

    QSqlQuery query;
    query.prepare("SELECT CLIENT_ID, SUM(CAST(TOTAL AS DECIMAL)) as TOTAL_SALES "
                  "FROM COMMANDS "
                  "GROUP BY CLIENT_ID "
                  "ORDER BY TOTAL_SALES DESC "
                  "LIMIT ?");
    query.addBindValue(limit);

    if (query.exec()) {
        while (query.next()) {
            int clientId = query.value(0).toInt();
            double totalSales = query.value(1).toDouble();
            topClients.append(qMakePair(clientId, totalSales));
        }
    }

    return topClients;
}
QSqlQuery CommandDAO::createConnectedQuery()
{
    Connection& conn = Connection::getInstance();
    if (!conn.ensureConnected()) {
        qDebug() << "Database connection failed";
        return QSqlQuery();
    }
    return QSqlQuery(conn.getDatabase());
}


