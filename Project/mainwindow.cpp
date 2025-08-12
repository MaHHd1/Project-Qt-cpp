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
    // Setup clients table
    clientsTable->setColumnCount(6);
    QStringList clientHeaders = {"ID", "Name", "Email", "City", "Postal", "Address"};
    clientsTable->setHorizontalHeaderLabels(clientHeaders);
    clientsTable->horizontalHeader()->setStretchLastSection(true);
    clientsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientsTable->setAlternatingRowColors(true);
    clientsTable->setSortingEnabled(true);

    // Setup commands table
    commandsTable->setColumnCount(6);
    QStringList commandHeaders = {"ID", "Client", "Date", "Total", "Payment", "Address"};
    commandsTable->setHorizontalHeaderLabels(commandHeaders);
    commandsTable->horizontalHeader()->setStretchLastSection(true);
    commandsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    commandsTable->setAlternatingRowColors(true);
    commandsTable->setSortingEnabled(true);
}

void MainWindow::applyModernStyling()
{
    // Additional styling beyond what's in the UI file
    setStyleSheet(styleSheet() + R"(
        /* Additional modern styling */
        QFrame#statCard {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(255, 255, 255, 0.9), stop:1 rgba(255, 255, 255, 0.7));
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 15px;
            padding: 15px;
        }

        QFrame#statCard:hover {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(255, 255, 255, 0.95), stop:1 rgba(255, 255, 255, 0.8));
            border: 1px solid rgba(255, 255, 255, 0.5);
        }
    )");
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

    // Menu actions (if they exist in the UI file)
    if (ui->actionRefresh) {
        connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::onRefreshClicked);
    }
    if (ui->actionNew_Client) {
        connect(ui->actionNew_Client, &QAction::triggered, this, &MainWindow::onAddClientClicked);
    }
    if (ui->actionNew_Command) {
        connect(ui->actionNew_Command, &QAction::triggered, this, &MainWindow::onAddCommandClicked);
    }
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

void MainWindow::loadCommandsData()
{
    if (!commandManager) return;

    populateCommandsTable();
    updateCommandStatistics();
}

void MainWindow::populateClientsTable()
{
    QList<Client> clients = clientManager->getAllClients();
    clientsTable->setRowCount(clients.size());

    for (int row = 0; row < clients.size(); ++row) {
        const Client &client = clients.at(row);

        clientsTable->setItem(row, 0, new QTableWidgetItem(QString::number(client.id)));
        clientsTable->setItem(row, 1, new QTableWidgetItem(client.name));
        clientsTable->setItem(row, 2, new QTableWidgetItem(client.email));
        clientsTable->setItem(row, 3, new QTableWidgetItem(client.city));
        clientsTable->setItem(row, 4, new QTableWidgetItem(client.postal));
        clientsTable->setItem(row, 5, new QTableWidgetItem(client.address));
    }
}

void MainWindow::populateCommandsTable()
{
    QList<Command> commands = commandManager->getAllCommands();
    commandsTable->setRowCount(commands.size());

    for (int row = 0; row < commands.size(); ++row) {
        const Command &command = commands.at(row);

        commandsTable->setItem(row, 0, new QTableWidgetItem(QString::number(command.commandId)));
        commandsTable->setItem(row, 1, new QTableWidgetItem(QString::number(command.clientId)));
        commandsTable->setItem(row, 2, new QTableWidgetItem(command.commandDate.toString("yyyy-MM-dd")));
        commandsTable->setItem(row, 3, new QTableWidgetItem(formatCurrency(command.getTotalAmount())));
        commandsTable->setItem(row, 4, new QTableWidgetItem(command.paymentMethod));
        commandsTable->setItem(row, 5, new QTableWidgetItem(command.deliveryAddress));
    }
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
    // TODO: Open client dialog
    QMessageBox::information(this, "Add Client", "Add Client dialog will open here");
}

void MainWindow::onAddCommandClicked()
{
    // TODO: Open command dialog
    QMessageBox::information(this, "Add Command", "Add Command dialog will open here");
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
