#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QSpacerItem>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , totalClientsLabel(nullptr)
    , newClientsLabel(nullptr)
    , activeCitiesLabel(nullptr)
    , totalOrdersLabel(nullptr)
    , monthlyRevenueLabel(nullptr)
    , pendingOrdersLabel(nullptr)
    , avgOrderValueLabel(nullptr)
    , fadeAnimation(nullptr)
    , refreshTimer(nullptr)
{
    ui->setupUi(this);

    // Set window properties
    setWindowTitle("🏢 Client Management System - Administrator Dashboard");
    if (QIcon(":/resources/app_icon.ico").isNull()) {
        setWindowIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    } else {
        setWindowIcon(QIcon(":/resources/app_icon.ico"));
    }
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // Initialize managers
    clientManager = new ClientManager(this);
    commandManager = new CommandManager(this);

    // Setup UI components
    setupUI();
    applyModernStyling();
    connectSignals();

    // Load initial data
    loadClientsData();
    loadCommandsData();

    // Setup refresh timer
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(30000); // Refresh every 30 seconds
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::refreshStatistics);
    refreshTimer->start();

    // Show with animation
    QTimer::singleShot(100, this, &MainWindow::animateStatCards);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // Get references to UI elements
    mainTabWidget = ui->mainTabWidget;
    clientsTab = ui->clientsTab;
    commandsTab = ui->commandsTab;
    clientsStatsFrame = ui->clientsStatsFrame;
    commandsStatsFrame = ui->commandsStatsFrame;
    addClientBtn = ui->addClientBtn;
    exportClientsBtn = ui->exportClientsBtn;
    clientSearchEdit = ui->clientSearchEdit;
    clientsTable = ui->clientsTable;
    addCommandBtn = ui->addCommandBtn;
    salesReportBtn = ui->salesReportBtn;
    commandSearchEdit = ui->commandSearchEdit;
    commandsTable = ui->commandsTable;

    // Setup tables and statistics
    setupTables();
    setupStatisticsFrames();
}

void MainWindow::setupStatisticsFrames()
{
    // Setup clients statistics
    QGridLayout *clientStatsLayout = new QGridLayout(clientsStatsFrame);
    clientStatsLayout->setSpacing(20);
    clientStatsLayout->setContentsMargins(20, 15, 20, 15);

    // Create stat cards for clients
    QFrame *totalClientsCard = createStatCard("Total Clients", "0", "👥");
    QFrame *newClientsCard = createStatCard("New This Month", "0", "🆕");
    QFrame *activeCitiesCard = createStatCard("Active Cities", "0", "🏙️");

    clientStatsLayout->addWidget(totalClientsCard, 0, 0);
    clientStatsLayout->addWidget(newClientsCard, 0, 1);
    clientStatsLayout->addWidget(activeCitiesCard, 0, 2);
    clientStatsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 3);

    // Store labels for updates
    totalClientsLabel = totalClientsCard->findChild<QLabel*>("valueLabel");
    newClientsLabel = newClientsCard->findChild<QLabel*>("valueLabel");
    activeCitiesLabel = activeCitiesCard->findChild<QLabel*>("valueLabel");

    // Setup commands statistics
    QGridLayout *commandStatsLayout = new QGridLayout(commandsStatsFrame);
    commandStatsLayout->setSpacing(20);
    commandStatsLayout->setContentsMargins(20, 15, 20, 15);

    // Create stat cards for commands
    QFrame *totalOrdersCard = createStatCard("Total Orders", "0", "📦");
    QFrame *revenueCard = createStatCard("Monthly Revenue", "$0", "💰");
    QFrame *pendingCard = createStatCard("Pending Orders", "0", "⏳");
    QFrame *avgValueCard = createStatCard("Avg Order Value", "$0", "📊");

    commandStatsLayout->addWidget(totalOrdersCard, 0, 0);
    commandStatsLayout->addWidget(revenueCard, 0, 1);
    commandStatsLayout->addWidget(pendingCard, 0, 2);
    commandStatsLayout->addWidget(avgValueCard, 0, 3);

    // Store labels for updates
    totalOrdersLabel = totalOrdersCard->findChild<QLabel*>("valueLabel");
    monthlyRevenueLabel = revenueCard->findChild<QLabel*>("valueLabel");
    pendingOrdersLabel = pendingCard->findChild<QLabel*>("valueLabel");
    avgOrderValueLabel = avgValueCard->findChild<QLabel*>("valueLabel");
}

void MainWindow::setupTables()
{
    // Setup clients table with actions column
    clientsTable->setColumnCount(7);
    QStringList clientHeaders = {"ID", "Name", "Email", "City", "Postal", "Address", "Actions"};
    clientsTable->setHorizontalHeaderLabels(clientHeaders);
    clientsTable->horizontalHeader()->setStretchLastSection(false);
    clientsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    clientsTable->setColumnWidth(6, 120); // Wider column for buttons
    clientsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientsTable->setAlternatingRowColors(true);
    clientsTable->setSortingEnabled(true);

    // Setup commands table
    commandsTable->setColumnCount(7);
    QStringList commandHeaders = {"ID", "Client", "Date", "Total", "Payment", "Address", "Actions"};
    commandsTable->setHorizontalHeaderLabels(commandHeaders);
    commandsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    commandsTable->setColumnWidth(0, 60);   // ID
    commandsTable->setColumnWidth(1, 150);  // Client
    commandsTable->setColumnWidth(2, 120);  // Date
    commandsTable->setColumnWidth(3, 80);   // Total
    commandsTable->setColumnWidth(4, 100);  // Payment
    commandsTable->setColumnWidth(5, 200);  // Address
    commandsTable->setColumnWidth(6, 100);  // Actions
    commandsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    commandsTable->setAlternatingRowColors(true);
    commandsTable->setSortingEnabled(true);
}

void MainWindow::applyModernStyling()
{
    setStyleSheet(R"(
        /* Main Window */
        QMainWindow {
            background: #f8fafc;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        /* ===== STAT CARDS ===== */
        QFrame#statCard {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 12px;
            padding: 15px;
        }

        QFrame#statCard:hover {
            background: #ffffff;
            border-color: #cbd5e0;
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.05);
        }

        QFrame#clientsStatsFrame, QFrame#commandsStatsFrame {
            background: white;
            border: 1px solid #e2e8f0;
            border-radius: 12px;
            padding: 15px;
        }

        /* ===== INPUT FIELDS ===== */
        QLineEdit, QTextEdit, QComboBox, QSpinBox, QDateTimeEdit {
            padding: 10px 14px;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            font-size: 14px;
            background: white;
            color: #2d3748;
            min-height: 36px;
        }

        QLineEdit:focus, QTextEdit:focus, QComboBox:focus,
        QSpinBox:focus, QDateTimeEdit:focus {
            border-color: #667eea;
            outline: none;
        }

        QLineEdit::placeholder, QTextEdit::placeholder {
            color: #a0aec0;
            font-style: italic;
        }

        /* ===== BUTTONS ===== */
        QPushButton {
            padding: 8px 16px;
            border-radius: 8px;
            font-weight: 500;
            background: #edf2f7;
            color: #2d3748;
            border: 1px solid #e2e8f0;
            min-width: 80px;
        }

        QPushButton:hover {
            background: #e2e8f0;
        }

        QPushButton:pressed {
            background: #cbd5e0;
        }

        QPushButton:disabled {
            background: #f8fafc;
            color: #a0aec0;
        }

        /* Primary Action Button */
        QPushButton.primary {
            background: #667eea;
            color: white;
            border-color: #5a67d8;
        }

        QPushButton.primary:hover {
            background: #5a67d8;
        }

        /* Danger Button */
        QPushButton.danger {
            background: #fc8181;
            color: white;
            border-color: #f56565;
        }

        QPushButton.danger:hover {
            background: #f56565;
        }

        /* Table Action Buttons */
        QPushButton.table-action-btn {
            border: none;
            padding: 5px;
            border-radius: 5px;
            min-width: 30px;
            max-width: 30px;
            background: transparent;
        }

        #editBtn {
            color: #667eea;
        }

        #editBtn:hover {
            background: rgba(102, 126, 234, 0.1);
        }

        #deleteBtn {
            color: #f56565;
        }

        #deleteBtn:hover {
            background: rgba(245, 101, 101, 0.1);
        }

        /* ===== TABLES ===== */
        QTableWidget {
            background: white;
            alternate-background-color: #f8fafc;
            gridline-color: #e2e8f0;
            border-radius: 8px;
            border: 1px solid #e2e8f0;
        }

        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #e2e8f0;
        }

        QTableWidget::item:selected {
            background: rgba(102, 126, 234, 0.2);
            color: #2d3748;
        }

        QHeaderView {
            background: transparent;
        }

        QHeaderView::section {
            background-color: #edf2f7;
            color: #2d3748;
            font-weight: 600;
            padding: 10px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
        }

        /* ===== MENUS & TOOLBARS ===== */
        QMenuBar {
            background: transparent;
            padding: 4px;
        }

        QMenuBar::item {
            padding: 6px 12px;
            border-radius: 4px;
        }

        QMenuBar::item:selected {
            background: rgba(102, 126, 234, 0.1);
            color: #667eea;
        }

        QMenu {
            border: 1px solid #e2e8f0;
            border-radius: 6px;
            padding: 4px;
            background: white;
        }

        QMenu::item {
            padding: 6px 24px;
            border-radius: 4px;
        }

        QMenu::item:selected {
            background: rgba(102, 126, 234, 0.1);
            color: #667eea;
        }

        /* ===== STATUS BAR ===== */
        QStatusBar {
            background: rgba(255, 255, 255, 0.9);
            border-top: 1px solid #e2e8f0;
            color: #4a5568;
            font-size: 12px;
            padding: 8px 20px;
        }

        /* ===== GROUP BOXES ===== */
        QGroupBox {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 20px;
            font-weight: 600;
            color: #4a5568;
            background: white;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px;
        }

        /* ===== DARK THEME VARIANTS ===== */
        .dark-theme {
            background: #2d3748;
            color: #e2e8f0;
        }

        .dark-theme QTableWidget,
        .dark-theme QGroupBox {
            background: #1a202c;
            border-color: #4a5568;
        }

        .dark-theme QHeaderView::section {
            background-color: #2d3748;
            color: #e2e8f0;
            border-color: #4a5568;
        }
    )");

    // Connection Status Indicator
    QLabel *connectionStatus = new QLabel(this);
    connectionStatus->setObjectName("connectionStatus");
    connectionStatus->setProperty("connected", true);
    connectionStatus->setText(" ✓ Connected");
    connectionStatus->setStyleSheet(R"(
        #connectionStatus {
            padding: 4px 12px;
            border-radius: 12px;
            font-weight: 500;
            font-size: 12px;
        }
        #connectionStatus[connected="true"] {
            background: rgba(72, 187, 120, 0.15);
            color: #48bb78;
        }
        #connectionStatus[connected="false"] {
            background: rgba(245, 101, 101, 0.15);
            color: #f56565;
        }
    )");
    statusBar()->addPermanentWidget(connectionStatus);

    // Version Info
    QLabel *versionLabel = new QLabel(this);
    versionLabel->setObjectName("versionLabel");
    versionLabel->setText("v1.0.0");
    versionLabel->setStyleSheet(R"(
        #versionLabel {
            color: #718096;
            font-size: 11px;
            padding: 0 8px;
        }
        .dark-theme #versionLabel {
            color: #a0aec0;
        }
    )");
    statusBar()->addPermanentWidget(versionLabel);
}

void MainWindow::connectSignals()
{
    // Tab change signal
    connect(mainTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // Button signals
    connect(addClientBtn, &QPushButton::clicked, this, &MainWindow::onAddClientClicked);
    connect(addCommandBtn, &QPushButton::clicked, this, &MainWindow::onAddCommandClicked);
    connect(exportClientsBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(salesReportBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);

    // Search signals
    connect(clientSearchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchClients);
    connect(commandSearchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchCommands);

    // Menu actions
    if (ui->actionRefresh) {
        connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::onRefreshClicked);
    }
    if (ui->actionNew_Client) {
        connect(ui->actionNew_Client, &QAction::triggered, this, &MainWindow::onAddClientClicked);
    }
    if (ui->actionNew_Command) {
        connect(ui->actionNew_Command, &QAction::triggered, this, &MainWindow::onAddCommandClicked);
    }

    // Connect client manager signals
    connect(clientManager, &ClientManager::clientAdded, this, &MainWindow::loadClientsData);
    connect(clientManager, &ClientManager::clientModified, this, &MainWindow::loadClientsData);
    connect(clientManager, &ClientManager::clientRemoved, this, &MainWindow::loadClientsData);
}

QFrame* MainWindow::createStatCard(const QString &title, const QString &value, const QString &icon)
{
    QFrame *card = new QFrame();
    card->setObjectName("statCard");
    card->setFixedSize(200, 80);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 10, 15, 10);

    // Icon and title
    QHBoxLayout *titleLayout = new QHBoxLayout();
    if (!icon.isEmpty()) {
        QLabel *iconLabel = new QLabel(icon);
        iconLabel->setStyleSheet("font-size: 16px;");
        titleLayout->addWidget(iconLabel);
    }

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: #718096; font-size: 12px; font-weight: 500;");
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    layout->addLayout(titleLayout);

    // Value
    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("valueLabel");
    valueLabel->setStyleSheet("color: #667eea; font-size: 22px; font-weight: 700;");
    layout->addWidget(valueLabel);

    setCardHoverEffect(card);
    return card;
}

void MainWindow::setCardHoverEffect(QFrame *card)
{
    card->installEventFilter(this);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    QFrame *card = qobject_cast<QFrame*>(watched);
    if (card && card->objectName() == "statCard") {
        if (event->type() == QEvent::Enter) {
            // Add hover effect
            QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
            card->setGraphicsEffect(effect);

            QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(200);
            animation->setStartValue(1.0);
            animation->setEndValue(0.8);
            animation->start(QPropertyAnimation::DeleteWhenStopped);

            return true;
        } else if (event->type() == QEvent::Leave) {
            // Remove hover effect
            QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
            card->setGraphicsEffect(effect);

            QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(200);
            animation->setStartValue(0.8);
            animation->setEndValue(1.0);
            animation->start(QPropertyAnimation::DeleteWhenStopped);

            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::loadClientsData()
{
    if (!clientManager) return;

    populateClientsTable();
    updateClientStatistics();
}

void MainWindow::loadCommandsData() {
    try {
        qDebug() << "Loading commands data...";
        populateCommandsTable();
        updateCommandStatistics();
    } catch (const std::exception &e) {
        qCritical() << "Error loading commands:" << e.what();
        QMessageBox::critical(this, "Error",
                              QString("Failed to load commands:\n%1").arg(e.what()));
    }
}

void MainWindow::populateClientsTable()
{
    QList<Client> clients = clientManager->getAllClients();
    clientsTable->setRowCount(clients.size());

    for (int row = 0; row < clients.size(); ++row) {
        const Client &client = clients.at(row);

        // Add regular data cells
        clientsTable->setItem(row, 0, new QTableWidgetItem(QString::number(client.id)));
        clientsTable->setItem(row, 1, new QTableWidgetItem(client.name));
        clientsTable->setItem(row, 2, new QTableWidgetItem(client.email));
        clientsTable->setItem(row, 3, new QTableWidgetItem(client.city));
        clientsTable->setItem(row, 4, new QTableWidgetItem(client.postal));
        clientsTable->setItem(row, 5, new QTableWidgetItem(client.address));

        // Add action buttons
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(actionWidget);
        layout->setContentsMargins(5, 2, 5, 2);
        layout->setSpacing(5);

        // Edit button
        QPushButton *editBtn = new QPushButton("✏️");
        editBtn->setObjectName("editBtn");
        editBtn->setProperty("class", "table-action-btn");
        editBtn->setToolTip("Edit client");
        connect(editBtn, &QPushButton::clicked, this, [this, row]() {
            editClient(row);
        });

        // Delete button
        QPushButton *deleteBtn = new QPushButton("🗑️");
        deleteBtn->setObjectName("deleteBtn");
        deleteBtn->setProperty("class", "table-action-btn");
        deleteBtn->setToolTip("Delete client");
        connect(deleteBtn, &QPushButton::clicked, this, [this, row]() {
            deleteClient(row);
        });

        layout->addWidget(editBtn);
        layout->addWidget(deleteBtn);
        layout->addStretch();
        actionWidget->setLayout(layout);

        clientsTable->setCellWidget(row, 6, actionWidget);
    }
}
void MainWindow::editClient(int row)
{
    int clientId = clientsTable->item(row, 0)->text().toInt();
    Client client = clientManager->getClient(clientId);

    ClientsWindow *dialog = new ClientsWindow(client, this, ClientsWindow::EditMode);
    connect(dialog, &ClientsWindow::finished, this, [this](int result) {
        if (result == QDialog::Accepted) {
            loadClientsData();
        }
    });
    dialog->exec();
}

void MainWindow::deleteClient(int row)
{
    int clientId = clientsTable->item(row, 0)->text().toInt();
    QString clientName = clientsTable->item(row, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Deletion",
        QString("Are you sure you want to delete client:\n%1 (ID: %2)?").arg(clientName).arg(clientId),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (clientManager->removeClient(clientId)) {
            loadClientsData();
            QMessageBox::information(this, "Success", "Client deleted successfully");
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete client");
        }
    }
}

void MainWindow::populateCommandsTable() {
    commandsTable->setRowCount(0);
    commandsTable->setSortingEnabled(false);

    qDebug() << "Fetching commands from database...";
    QList<Command> commands = commandManager->getAllCommands();

    if (commands.isEmpty()) {
        qDebug() << "No commands found or error occurred";
        QTableWidgetItem *noDataItem = new QTableWidgetItem("No commands found");
        noDataItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignCenter));
        commandsTable->setRowCount(1);
        commandsTable->setSpan(0, 0, 1, commandsTable->columnCount());
        commandsTable->setItem(0, 0, noDataItem);
        commandsTable->setSortingEnabled(true);
        return;
    }

    qDebug() << "Populating table with" << commands.size() << "commands";
    commandsTable->setRowCount(commands.size());

    // Pre-cache client names to avoid repeated queries
    QHash<int, QString> clientNames;

    try {
        for (int row = 0; row < commands.size(); ++row) {
            const Command &command = commands.at(row);

            // Get client name (with caching)
            QString clientName;
            if (clientNames.contains(command.clientId)) {
                clientName = clientNames.value(command.clientId);
            } else {
                Client client = clientManager->getClient(command.clientId);
                clientName = (client.id > 0) ? client.name : "Unknown";
                clientNames.insert(command.clientId, clientName);
            }

            // Create and populate items
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(command.commandId));
            QTableWidgetItem *clientItem = new QTableWidgetItem(clientName);
            QTableWidgetItem *dateItem = new QTableWidgetItem(command.commandDate.toString("yyyy-MM-dd hh:mm"));
            QTableWidgetItem *totalItem = new QTableWidgetItem(QString("$%1").arg(command.total));
            QTableWidgetItem *paymentItem = new QTableWidgetItem(command.paymentMethod);

            // Set data alignment
            idItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
            totalItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));

            // Shorten delivery address for display
            QString shortAddress = command.deliveryAddress;
            if (shortAddress.length() > 50) {
                shortAddress = shortAddress.left(47) + "...";
            }
            QTableWidgetItem *addressItem = new QTableWidgetItem(shortAddress);

            // Add items to table
            commandsTable->setItem(row, 0, idItem);
            commandsTable->setItem(row, 1, clientItem);
            commandsTable->setItem(row, 2, dateItem);
            commandsTable->setItem(row, 3, totalItem);
            commandsTable->setItem(row, 4, paymentItem);
            commandsTable->setItem(row, 5, addressItem);

            // Create action buttons widget
            QWidget *actionWidget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(actionWidget);
            layout->setContentsMargins(5, 2, 5, 2);
            layout->setSpacing(5);

            // Edit button - FIXED: Store command ID properly
            QPushButton *editBtn = new QPushButton("✏️");
            editBtn->setObjectName("editBtn");
            editBtn->setProperty("class", "table-action-btn");
            editBtn->setToolTip("Edit command");
            editBtn->setCursor(Qt::PointingHandCursor);

            // CRITICAL FIX: Use QSignalMapper or store the command ID directly
            editBtn->setProperty("commandId", command.commandId);

            // Connect using a more reliable approach
            connect(editBtn, &QPushButton::clicked, this, [this, editBtn]() {
                int commandId = editBtn->property("commandId").toInt();
                qDebug() << "Edit button clicked for command ID:" << commandId;
                this->editCommandById(commandId);
            });

            // Delete button - FIXED: Same approach
            QPushButton *deleteBtn = new QPushButton("🗑️");
            deleteBtn->setObjectName("deleteBtn");
            deleteBtn->setProperty("class", "table-action-btn");
            deleteBtn->setToolTip("Delete command");
            deleteBtn->setCursor(Qt::PointingHandCursor);

            // Store command ID in button property
            deleteBtn->setProperty("commandId", command.commandId);

            // Connect using property-based approach
            connect(deleteBtn, &QPushButton::clicked, this, [this, deleteBtn]() {
                int commandId = deleteBtn->property("commandId").toInt();
                qDebug() << "Delete button clicked for command ID:" << commandId;
                this->deleteCommandById(commandId);
            });

            layout->addWidget(editBtn);
            layout->addWidget(deleteBtn);
            layout->addStretch();
            actionWidget->setLayout(layout);

            commandsTable->setCellWidget(row, 6, actionWidget);
        }
    } catch (const std::exception &e) {
        qCritical() << "Error populating commands table:" << e.what();
        QMessageBox::critical(this, "Error",
                              QString("Failed to populate commands table:\n%1").arg(e.what()));
    }

    // Resize columns to content
    commandsTable->resizeColumnsToContents();

    // Enable sorting after population is complete
    commandsTable->setSortingEnabled(true);

    // Set alternating row colors
    commandsTable->setAlternatingRowColors(true);

    qDebug() << "Commands table populated successfully";
}

void MainWindow::updateClientStatistics()
{
    if (!clientManager || !totalClientsLabel) return;

    int totalClients = clientManager->getDAO()->getClientCount();
    totalClientsLabel->setText(QString::number(totalClients));

    // Mock data for now - you can implement actual calculations
    if (newClientsLabel) newClientsLabel->setText("32");
    if (activeCitiesLabel) activeCitiesLabel->setText("15");
}

void MainWindow::updateCommandStatistics()
{
    if (!commandManager) return;

    int totalOrders = commandManager->getDAO()->getCommandCount();
    double totalSales = commandManager->getDAO()->getTotalSales();

    if (totalOrdersLabel) totalOrdersLabel->setText(QString::number(totalOrders));
    if (monthlyRevenueLabel) monthlyRevenueLabel->setText(formatCurrency(totalSales));

    // Mock data for pending orders - implement actual calculation
    if (pendingOrdersLabel) pendingOrdersLabel->setText("89");

    // Calculate average order value
    double avgValue = (totalOrders > 0) ? (totalSales / totalOrders) : 0.0;
    if (avgOrderValueLabel) avgOrderValueLabel->setText(formatCurrency(avgValue));
}

QString MainWindow::formatCurrency(double amount)
{
    return QString("$%1").arg(amount, 0, 'f', 2);
}

void MainWindow::animateStatCards()
{
    QList<QFrame*> statCards = findChildren<QFrame*>("statCard");

    for (int i = 0; i < statCards.size(); ++i) {
        QFrame *card = statCards.at(i);
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(card);
        card->setGraphicsEffect(effect);
        effect->setOpacity(0.0);

        QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity", card);
        animation->setDuration(600);
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);
        animation->setEasingCurve(QEasingCurve::OutCubic);

        QTimer::singleShot(i * 100, this, [animation]() {
            animation->start();
        });
    }
}

void MainWindow::animateWidget(QWidget *widget, int duration)
{
    if (!widget) return;

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    widget->setGraphicsEffect(effect);

    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

// Slot implementations
void MainWindow::onTabChanged(int index)
{
    // Refresh data when switching tabs
    if (index == 0) {
        loadClientsData();
    } else if (index == 1) {
        loadCommandsData();
    }
}

void MainWindow::onRefreshClicked()
{
    loadClientsData();
    loadCommandsData();
    QMessageBox::information(this, "Refresh", "Data refreshed successfully!");
}

void MainWindow::onAddClientClicked()
{
    ClientsWindow *dialog = new ClientsWindow(this, ClientsWindow::AddMode);
    connect(dialog, &ClientsWindow::finished, this, [this](int result) {
        if (result == QDialog::Accepted) {
            loadClientsData();
        }
    });
    dialog->exec();
}

void MainWindow::onAddCommandClicked() {
    // Get the first client as default (or handle empty case)
    QList<Client> clients = clientManager->getAllClients();
    if (clients.isEmpty()) {
        QMessageBox::warning(this, "No Clients", "Please add clients first before creating commands.");
        return;
    }

    Command newCommand;
    newCommand.clientId = clients.first().id; // Default to first client
    newCommand.commandDate = QDateTime::currentDateTime();
    newCommand.total = "0.00"; // Default total
    newCommand.paymentMethod = "Cash"; // Default payment method

    CommandsWindow *dialog = new CommandsWindow(commandManager, clientManager, newCommand, CommandsWindow::AddMode, this);
    connect(dialog, &CommandsWindow::finished, this, [this](int result) {
        if (result == QDialog::Accepted) {
            loadCommandsData();
        }
    });
    dialog->exec();
}

void MainWindow::onSearchClients()
{
    QString searchText = clientSearchEdit->text();
    if (searchText.isEmpty()) {
        populateClientsTable();
        return;
    }

    // Simple search implementation
    for (int row = 0; row < clientsTable->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < clientsTable->columnCount(); ++col) {
            QTableWidgetItem *item = clientsTable->item(row, col);
            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        clientsTable->setRowHidden(row, !match);
    }
}

void MainWindow::onSearchCommands()
{
    QString searchText = commandSearchEdit->text();
    if (searchText.isEmpty()) {
        populateCommandsTable();
        return;
    }

    // Simple search implementation
    for (int row = 0; row < commandsTable->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < commandsTable->columnCount(); ++col) {
            QTableWidgetItem *item = commandsTable->item(row, col);
            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        commandsTable->setRowHidden(row, !match);
    }
}

void MainWindow::refreshStatistics()
{
    updateClientStatistics();
    updateCommandStatistics();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // Add any custom resize handling here if needed
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    // Trigger initial animations when window is first shown
    if (!event->spontaneous()) {
        QTimer::singleShot(100, this, &MainWindow::animateStatCards);
    }
}
void MainWindow::editCommand(int row)
{
    if (row < 0 || row >= commandsTable->rowCount()) {
        qDebug() << "Invalid row for edit:" << row;
        return;
    }

    QTableWidgetItem* idItem = commandsTable->item(row, 0);
    if (!idItem) {
        qDebug() << "No ID item at row:" << row;
        return;
    }

    int commandId = idItem->text().toInt();
    editCommandById(commandId);
}


void MainWindow::editCommandById(int commandId)
{
    qDebug() << "Editing command ID:" << commandId;

    Command command = commandManager->getCommand(commandId);

    // Add debug output to check if command is loaded correctly
    qDebug() << "Loaded command:" << command.toString();

    if (command.commandId > 0) {
        CommandsWindow *dialog = new CommandsWindow(commandManager, clientManager,
                                                    command, CommandsWindow::EditMode, this);
        connect(dialog, &CommandsWindow::finished, this, [this](int result) {
            if (result == QDialog::Accepted) {
                qDebug() << "Command edited, refreshing data...";
                loadCommandsData();
            }
        });
        dialog->exec();
    } else {
        qDebug() << "Command not found or invalid ID:" << commandId;
        QMessageBox::warning(this, "Error", "Could not load command details");
    }
}

void MainWindow::deleteCommandById(int commandId)
{
    qDebug() << "Deleting command ID:" << commandId;

    // Get client name for confirmation message
    Command command = commandManager->getCommand(commandId);
    QString clientName = "Unknown";

    if (command.commandId > 0) {
        Client client = clientManager->getClient(command.clientId);
        clientName = client.name;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Deletion",
        QString("Are you sure you want to delete command #%1 for client %2?").arg(commandId).arg(clientName),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (commandManager->removeCommand(commandId)) {
            qDebug() << "Delete successful";
            loadCommandsData();
            QMessageBox::information(this, "Success", "Command deleted successfully");
        } else {
            qDebug() << "Delete failed";
            QMessageBox::warning(this, "Error", "Failed to delete command");
        }
    }
}


