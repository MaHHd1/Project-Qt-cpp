// clients.cpp
#include "clients.h"
#include "connection.h"
#include <QRegularExpression>
#include <QSqlRecord>

// ClientDAO Implementation
ClientDAO::ClientDAO(QObject *parent) : QObject(parent), tableModel(nullptr)
{
    // Initialize table model
    Connection& conn = Connection::getInstance();
    if (conn.isConnected()) {
        tableModel = new QSqlTableModel(this, conn.getDatabase());
        tableModel->setTable("CLIENTS");
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        // Set column headers
        tableModel->setHeaderData(0, Qt::Horizontal, "ID");
        tableModel->setHeaderData(1, Qt::Horizontal, "Name");
        tableModel->setHeaderData(2, Qt::Horizontal, "Email");
        tableModel->setHeaderData(3, Qt::Horizontal, "City");
        tableModel->setHeaderData(4, Qt::Horizontal, "Postal Code");
        tableModel->setHeaderData(5, Qt::Horizontal, "Address");

        // Load data
        refreshTableModel();

        // Connect signals
        connect(tableModel, &QSqlTableModel::dataChanged,
                this, &ClientDAO::onModelDataChanged);
    } else {
        qDebug() << "Warning: Database not connected when creating ClientDAO";
    }
}

ClientDAO::~ClientDAO()
{
    if (tableModel) {
        tableModel->submitAll();
    }
}

bool ClientDAO::createClient(const Client& client)
{
    if (!client.isValid()) {
        emit errorOccurred("Invalid client data - name and email are required");
        return false;
    }

    // Check if email already exists
    if (emailExists(client.email)) {
        emit errorOccurred("Email already exists: " + client.email);
        return false;
    }

    Connection& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        emit errorOccurred("Database not connected");
        return false;
    }

    QSqlQuery query(conn.getDatabase());
    query.prepare("INSERT INTO CLIENTS (NAME, EMAIL, CITY, POSTAL, ADDRESS) "
                  "VALUES (:name, :email, :city, :postal, :address)");

    query.bindValue(":name", client.name.trimmed());
    query.bindValue(":email", client.email.trimmed().toLower());
    query.bindValue(":city", client.city.trimmed());
    query.bindValue(":postal", client.postal.trimmed());
    query.bindValue(":address", client.address.trimmed());

    if (!executeQuery(query, "Create Client")) {
        return false;
    }

    // Get the new client ID
    QSqlQuery lastIdQuery(conn.getDatabase());
    if (lastIdQuery.exec("SELECT CLIENTS_SEQ.CURRVAL FROM DUAL")) {
        if (lastIdQuery.next()) {
            Client newClient = client;
            newClient.id = lastIdQuery.value(0).toInt();
            emit clientCreated(newClient);
            qDebug() << "✓ Client created successfully:" << newClient.toString();
        }
    }

    refreshTableModel();
    return true;
}

Client ClientDAO::readClient(int id)
{
    Client client;

    if (id <= 0) {
        return client;
    }

    Connection& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        emit errorOccurred("Database not connected");
        return client;
    }

    QSqlQuery query(conn.getDatabase());
    query.prepare("SELECT ID, NAME, EMAIL, CITY, POSTAL, ADDRESS FROM CLIENTS WHERE ID = :id");
    query.bindValue(":id", id);

    if (executeQuery(query, "Read Client") && query.next()) {
        client = createClientFromQuery(query);
        qDebug() << "✓ Client read successfully:" << client.toString();
    } else {
        qDebug() << "Client not found with ID:" << id;
    }

    return client;
}

QList<Client> ClientDAO::readAllClients()
{
    QList<Client> clients;

    Connection& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        emit errorOccurred("Database not connected");
        return clients;
    }

    QSqlQuery query = conn.executeSelectQuery("SELECT ID, NAME, EMAIL, CITY, POSTAL, ADDRESS FROM CLIENTS ORDER BY NAME");

    while (query.next()) {
        Client client = createClientFromQuery(query);
        clients.append(client);
    }

    qDebug() << "✓ Read" << clients.size() << "clients from database";
    return clients;
}

bool ClientDAO::updateClient(const Client& client)
{
    if (client.id <= 0) {
        emit errorOccurred("Invalid client ID for update");
        return false;
    }

    if (!client.isValid()) {
        emit errorOccurred("Invalid client data - name and email are required");
        return false;
    }

    // Check if email already exists for another client
    if (emailExists(client.email, client.id)) {
        emit errorOccurred("Email already exists for another client: " + client.email);
        return false;
    }

    Connection& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        emit errorOccurred("Database not connected");
        return false;
    }

    QSqlQuery query(conn.getDatabase());
    query.prepare("UPDATE CLIENTS SET NAME = :name, EMAIL = :email, CITY = :city, "
                  "POSTAL = :postal, ADDRESS = :address WHERE ID = :id");

    query.bindValue(":id", client.id);
    query.bindValue(":name", client.name.trimmed());
    query.bindValue(":email", client.email.trimmed().toLower());
    query.bindValue(":city", client.city.trimmed());
    query.bindValue(":postal", client.postal.trimmed());
    query.bindValue(":address", client.address.trimmed());

    if (!executeQuery(query, "Update Client")) {
        return false;
    }

    emit clientUpdated(client);
    qDebug() << "✓ Client updated successfully:" << client.toString();

    refreshTableModel();
    return true;
}

bool ClientDAO::deleteClient(int id)
{
    if (id <= 0) {
        emit errorOccurred("Invalid client ID for deletion");
        return false;
    }

    if (!clientExists(id)) {
        emit errorOccurred("Client not found with ID: " + QString::number(id));
        return false;
    }

    Connection& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        emit errorOccurred("Database not connected");
        return false;
    }

    // First check if client has any commands
    QSqlQuery checkQuery(conn.getDatabase());
    checkQuery.prepare("SELECT COUNT(*) FROM COMMANDES WHERE CLIENT_ID = :id");
    checkQuery.bindValue(":id", id);

    if (executeQuery(checkQuery, "Check Client Commands") && checkQuery.next()) {
        int commandCount = checkQuery.value(0).toInt();
        if (commandCount > 0) {
            emit errorOccurred(QString("Cannot delete client - has %1 associated commands").arg(commandCount));
            return false;
        }
    }

    // Delete the client
    QSqlQuery query(conn.getDatabase());
    query.prepare("DELETE FROM CLIENTS WHERE ID = :id");
    query.bindValue(":id", id);

    if (!executeQuery(query, "Delete Client")) {
        return false;
    }

    emit clientDeleted(id);
    qDebug() << "✓ Client deleted successfully, ID:" << id;

    refreshTableModel();
    return true;
}

QList<Client> ClientDAO::searchClientsByName(const QString& name)
{
    QList<Client> clients;

    if (name.trimmed().isEmpty()) {
        return clients;
    }

    Connection& conn = Connection::getInstance();
    QSqlQuery query(conn.getDatabase());
    query.prepare("SELECT ID, NAME, EMAIL, CITY, POSTAL, ADDRESS FROM CLIENTS "
                  "WHERE UPPER(NAME) LIKE UPPER(:name) ORDER BY NAME");
    query.bindValue(":name", "%" + name.trimmed() + "%");

    if (executeQuery(query, "Search Clients by Name")) {
        while (query.next()) {
            clients.append(createClientFromQuery(query));
        }
    }

    qDebug() << "Found" << clients.size() << "clients matching name:" << name;
    return clients;
}

QList<Client> ClientDAO::searchClientsByEmail(const QString& email)
{
    QList<Client> clients;

    if (email.trimmed().isEmpty()) {
        return clients;
    }

    Connection& conn = Connection::getInstance();
    QSqlQuery query(conn.getDatabase());
    query.prepare("SELECT ID, NAME, EMAIL, CITY, POSTAL, ADDRESS FROM CLIENTS "
                  "WHERE UPPER(EMAIL) LIKE UPPER(:email) ORDER BY NAME");
    query.bindValue(":email", "%" + email.trimmed() + "%");

    if (executeQuery(query, "Search Clients by Email")) {
        while (query.next()) {
            clients.append(createClientFromQuery(query));
        }
    }

    qDebug() << "Found" << clients.size() << "clients matching email:" << email;
    return clients;
}

QList<Client> ClientDAO::searchClientsByCity(const QString& city)
{
    QList<Client> clients;

    if (city.trimmed().isEmpty()) {
        return clients;
    }

    Connection& conn = Connection::getInstance();
    QSqlQuery query(conn.getDatabase());
    query.prepare("SELECT ID, NAME, EMAIL, CITY, POSTAL, ADDRESS FROM CLIENTS "
                  "WHERE UPPER(CITY) LIKE UPPER(:city) ORDER BY NAME");
    query.bindValue(":city", "%" + city.trimmed() + "%");

    if (executeQuery(query, "Search Clients by City")) {
        while (query.next()) {
            clients.append(createClientFromQuery(query));
        }
    }

    qDebug() << "Found" << clients.size() << "clients in city:" << city;
    return clients;
}

bool ClientDAO::clientExists(int id)
{
    Connection& conn = Connection::getInstance();
    QSqlQuery query(conn.getDatabase());
    query.prepare("SELECT COUNT(*) FROM CLIENTS WHERE ID = :id");
    query.bindValue(":id", id);

    if (executeQuery(query, "Check Client Exists") && query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

bool ClientDAO::emailExists(const QString& email, int excludeId)
{
    Connection& conn = Connection::getInstance();
    QSqlQuery query(conn.getDatabase());

    if (excludeId > 0) {
        query.prepare("SELECT COUNT(*) FROM CLIENTS WHERE UPPER(EMAIL) = UPPER(:email) AND ID != :excludeId");
        query.bindValue(":excludeId", excludeId);
    } else {
        query.prepare("SELECT COUNT(*) FROM CLIENTS WHERE UPPER(EMAIL) = UPPER(:email)");
    }
    query.bindValue(":email", email.trimmed());

    if (executeQuery(query, "Check Email Exists") && query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

int ClientDAO::getClientCount()
{
    Connection& conn = Connection::getInstance();
    QSqlQuery query = conn.executeSelectQuery("SELECT COUNT(*) FROM CLIENTS");

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

Client ClientDAO::getClientByEmail(const QString& email)
{
    Client client;

    Connection& conn = Connection::getInstance();
    QSqlQuery query(conn.getDatabase());
    query.prepare("SELECT ID, NAME, EMAIL, CITY, POSTAL, ADDRESS FROM CLIENTS "
                  "WHERE UPPER(EMAIL) = UPPER(:email)");
    query.bindValue(":email", email.trimmed());

    if (executeQuery(query, "Get Client by Email") && query.next()) {
        client = createClientFromQuery(query);
    }

    return client;
}

QSqlTableModel* ClientDAO::getTableModel()
{
    return tableModel;
}

void ClientDAO::refreshTableModel()
{
    if (tableModel) {
        tableModel->select();
        qDebug() << "Table model refreshed";
    }
}

void ClientDAO::onModelDataChanged()
{
    qDebug() << "Client table model data changed";
}

bool ClientDAO::executeQuery(QSqlQuery& query, const QString& operation)
{
    if (!query.exec()) {
        logError(operation, query.lastError());
        return false;
    }
    return true;
}

void ClientDAO::logError(const QString& operation, const QSqlError& error)
{
    QString errorMessage = QString("%1 failed: %2").arg(operation, error.text());
    qDebug() << "Database error:" << errorMessage;
    emit errorOccurred(errorMessage);
}

Client ClientDAO::createClientFromQuery(const QSqlQuery& query)
{
    return Client(
        query.value("ID").toInt(),
        query.value("NAME").toString(),
        query.value("EMAIL").toString(),
        query.value("CITY").toString(),
        query.value("POSTAL").toString(),
        query.value("ADDRESS").toString()
        );
}

// ClientManager Implementation
ClientManager::ClientManager(QObject *parent) : QObject(parent)
{
    dao = new ClientDAO(this);

    // Connect DAO signals to manager signals
    connect(dao, &ClientDAO::clientCreated, this, &ClientManager::clientAdded);
    connect(dao, &ClientDAO::clientUpdated, this, &ClientManager::clientModified);
    connect(dao, &ClientDAO::clientDeleted, this, &ClientManager::clientRemoved);
    connect(dao, &ClientDAO::errorOccurred, this, &ClientManager::validationError);
}

bool ClientManager::addNewClient(const Client& client)
{
    QString errorMessage;
    if (!validateClient(client, errorMessage)) {
        emit validationError(errorMessage);
        return false;
    }

    Client sanitizedClient(
        sanitizeInput(client.name),
        sanitizeInput(client.email),
        sanitizeInput(client.city),
        sanitizeInput(client.postal),
        sanitizeInput(client.address)
        );

    return dao->createClient(sanitizedClient);
}

bool ClientManager::modifyClient(const Client& client)
{
    QString errorMessage;
    if (!validateClient(client, errorMessage)) {
        emit validationError(errorMessage);
        return false;
    }

    Client sanitizedClient = client;
    sanitizedClient.name = sanitizeInput(client.name);
    sanitizedClient.email = sanitizeInput(client.email);
    sanitizedClient.city = sanitizeInput(client.city);
    sanitizedClient.postal = sanitizeInput(client.postal);
    sanitizedClient.address = sanitizeInput(client.address);

    return dao->updateClient(sanitizedClient);
}

bool ClientManager::removeClient(int id)
{
    if (id <= 0) {
        emit validationError("Invalid client ID");
        return false;
    }

    return dao->deleteClient(id);
}

Client ClientManager::getClient(int id)
{
    return dao->readClient(id);
}

QList<Client> ClientManager::getAllClients()
{
    return dao->readAllClients();
}

bool ClientManager::validateClient(const Client& client, QString& errorMessage)
{
    if (!validateName(client.name)) {
        errorMessage = "Name is required and must be at least 2 characters long";
        return false;
    }

    if (!validateEmail(client.email)) {
        errorMessage = "Valid email address is required";
        return false;
    }

    return true;
}

bool ClientManager::validateEmail(const QString& email)
{
    return isValidEmail(email);
}

bool ClientManager::validateName(const QString& name)
{
    return isValidName(name);
}

ClientDAO* ClientManager::getDAO() const
{
    return dao;
}

bool ClientManager::isValidEmail(const QString& email)
{
    if (email.trimmed().isEmpty() || email.length() > 255) {
        return false;
    }

    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return emailRegex.match(email.trimmed()).hasMatch();
}

bool ClientManager::isValidName(const QString& name)
{
    QString trimmedName = name.trimmed();
    return !trimmedName.isEmpty() && trimmedName.length() >= 2 && trimmedName.length() <= 100;
}

QString ClientManager::sanitizeInput(const QString& input)
{
    return input.trimmed();
}


