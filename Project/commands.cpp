#include "commands.h"
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
    tableModel = new QSqlTableModel(this);
    tableModel->setTable("COMMANDES");
    tableModel->select();

    // Set up joined model for commands with client info
    joinedModel = new QSqlTableModel(this);

    // Connect model signals
    connect(tableModel, &QSqlTableModel::dataChanged, this, &CommandDAO::onModelDataChanged);
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

bool CommandDAO::createCommand(const Command& command)
{
    if (!command.isValid()) {
        emit errorOccurred("Invalid command data");
        return false;
    }

    if (!validateClientExists(command.clientId)) {
        emit errorOccurred("Client does not exist");
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO COMMANDES (CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS) "
                  "VALUES (?, ?, ?, ?, ?)");

    query.addBindValue(command.clientId);
    query.addBindValue(command.commandDate);
    query.addBindValue(command.total);
    query.addBindValue(command.paymentMethod);
    query.addBindValue(command.deliveryAddress);

    if (executeQuery(query, "Create Command")) {
        refreshTableModel();
        emit commandCreated(command);
        return true;
    }

    return false;
}

Command CommandDAO::readCommand(int commandId)
{
    QSqlQuery query;
    query.prepare("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDES WHERE COMMAND_ID = ?");
    query.addBindValue(commandId);

    if (executeQuery(query, "Read Command")) {
        if (query.next()) {
            return createCommandFromQuery(query);
        }
    }

    return Command(); // Return empty command if not found
}

QList<Command> CommandDAO::readAllCommands()
{
    QList<Command> commands;
    QSqlQuery query("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                    "FROM COMMANDES ORDER BY COMMAND_DATE DESC");

    if (executeQuery(query, "Read All Commands")) {
        while (query.next()) {
            commands.append(createCommandFromQuery(query));
        }
    }

    return commands;
}

QList<Command> CommandDAO::readCommandsByClient(int clientId)
{
    QList<Command> commands;
    QSqlQuery query;
    query.prepare("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDES WHERE CLIENT_ID = ? ORDER BY COMMAND_DATE DESC");
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
    query.prepare("UPDATE COMMANDES SET CLIENT_ID = ?, COMMAND_DATE = ?, TOTAL = ?, "
                  "PAYMENT_METHOD = ?, DELIVERY_ADDRESS = ? WHERE COMMAND_ID = ?");

    query.addBindValue(command.clientId);
    query.addBindValue(command.commandDate);
    query.addBindValue(command.total);
    query.addBindValue(command.paymentMethod);
    query.addBindValue(command.deliveryAddress);
    query.addBindValue(command.commandId);

    if (executeQuery(query, "Update Command")) {
        refreshTableModel();
        emit commandUpdated(command);
        return true;
    }

    return false;
}

bool CommandDAO::deleteCommand(int commandId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM COMMANDES WHERE COMMAND_ID = ?");
    query.addBindValue(commandId);

    if (executeQuery(query, "Delete Command")) {
        refreshTableModel();
        emit commandDeleted(commandId);
        return true;
    }

    return false;
}

QList<Command> CommandDAO::searchCommandsByDate(const QDate& startDate, const QDate& endDate)
{
    QList<Command> commands;
    QSqlQuery query;
    query.prepare("SELECT COMMAND_ID, CLIENT_ID, COMMAND_DATE, TOTAL, PAYMENT_METHOD, DELIVERY_ADDRESS "
                  "FROM COMMANDES WHERE DATE(COMMAND_DATE) BETWEEN ? AND ? ORDER BY COMMAND_DATE DESC");
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
                  "FROM COMMANDES WHERE UPPER(PAYMENT_METHOD) LIKE UPPER(?) ORDER BY COMMAND_DATE DESC");
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
                  "FROM COMMANDES WHERE CAST(TOTAL AS DECIMAL) BETWEEN ? AND ? ORDER BY TOTAL DESC");
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
                  "cl.NAME, cl.EMAIL FROM COMMANDES c "
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
    query.prepare("SELECT COUNT(*) FROM COMMANDES WHERE COMMAND_ID = ?");
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
    QSqlQuery query("SELECT COUNT(*) FROM COMMANDES");

    if (executeQuery(query, "Get Command Count")) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    }

    return 0;
}

int CommandDAO::getCommandCountByClient(int clientId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM COMMANDES WHERE CLIENT_ID = ?");
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
    QSqlQuery query("SELECT SUM(CAST(TOTAL AS DECIMAL)) FROM COMMANDES");

    if (executeQuery(query, "Get Total Sales")) {
        if (query.next()) {
            return query.value(0).toDouble();
        }
    }

    return 0.0;
}

double CommandDAO::getTotalSalesByClient(int clientId)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(CAST(TOTAL AS DECIMAL)) FROM COMMANDES WHERE CLIENT_ID = ?");
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
    QSqlQuery query("SELECT DISTINCT PAYMENT_METHOD FROM COMMANDES WHERE PAYMENT_METHOD IS NOT NULL ORDER BY PAYMENT_METHOD");

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
                    "cl.NAME, cl.EMAIL FROM COMMANDES c "
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
                  "cl.NAME, cl.EMAIL FROM COMMANDES c "
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
                              "FROM COMMANDES c LEFT JOIN CLIENTS cl ON c.CLIENT_ID = cl.ID "
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
    command.commandDate = query.value("COMMAND_DATE").toDateTime();
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
        emit validationError("Invalid command ID");
        return false;
    }

    if (!dao->commandExists(commandId)) {
        emit validationError("Command does not exist");
        return false;
    }

    return dao->deleteCommand(commandId);
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

bool CommandManager::validateCommand(const Command& command, QString& errorMessage)
{
    if (!validateClientId(command.clientId)) {
        errorMessage = "Invalid client ID";
        return false;
    }

    if (!validateTotal(command.total)) {
        errorMessage = "Invalid total amount";
        return false;
    }

    if (!validatePaymentMethod(command.paymentMethod)) {
        errorMessage = "Invalid payment method";
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
    QSqlQuery dateQuery("SELECT MIN(COMMAND_DATE), MAX(COMMAND_DATE) FROM COMMANDES");
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
    dateQuery.prepare("SELECT MIN(COMMAND_DATE), MAX(COMMAND_DATE) FROM COMMANDES WHERE CLIENT_ID = ?");
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

    QSqlQuery query("SELECT PAYMENT_METHOD, COUNT(*) FROM COMMANDES "
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
                  "FROM COMMANDES "
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
                  "FROM COMMANDES "
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


