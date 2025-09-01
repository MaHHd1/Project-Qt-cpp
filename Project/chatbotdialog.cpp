// chatbotdialog.cpp - Enhanced Version
#include "chatbotdialog.h"
#include "ui_chatbotdialog.h"
#include <QDateTime>
#include <QScrollBar>
#include <QStringList>
#include <QRegularExpression>
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QCompleter>
#include <QMessageBox>

ChatbotDialog::ChatbotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatbotDialog),
    m_clientManager(nullptr),
    m_commandManager(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("🤖 Enhanced Client Management Assistant");
    resize(700, 600);

    // Set up modern styling
    setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #f8f9fa, stop:1 #e9ecef);
        }
        QTextEdit {
            border: 1px solid #dee2e6;
            border-radius: 8px;
            background-color: white;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        QLineEdit {
            border: 2px solid #dee2e6;
            border-radius: 6px;
            padding: 8px;
            font-size: 14px;
            background-color: white;
        }
        QLineEdit:focus {
            border-color: #007bff;
        }
        QPushButton {
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QPushButton:pressed {
            background-color: #004085;
        }
    )");

    // Initialize typing timer
    m_typingTimer = new QTimer(this);
    m_typingTimer->setSingleShot(true);
    m_typingTimer->setInterval(500);
    connect(m_typingTimer, &QTimer::timeout, this, &ChatbotDialog::onTypingFinished);

    // Set up enhanced suggestions with categories
    setupSuggestions();

    // Set up auto-completer
    setupAutoCompleter();

    // Connect signals
    connect(ui->suggestionListView, &QListView::clicked, [this](const QModelIndex &index) {
        ui->inputLineEdit->setText(index.data().toString());
        ui->inputLineEdit->setFocus();
    });

    connect(ui->inputLineEdit, &QLineEdit::textChanged, this, &ChatbotDialog::onInputChanged);

    // Add animated welcome message
    showWelcomeMessage();
}

ChatbotDialog::~ChatbotDialog()
{
    delete ui;
}

void ChatbotDialog::setupSuggestions()
{
    QStringList suggestions;

    // Categorized suggestions
    suggestions << "=== CLIENT OPERATIONS ==="
                << "show all clients"
                << "find client John Doe"
                << "find client by email user@example.com"
                << "find clients in New York"
                << "count clients"
                << "client statistics"
                << ""
                << "=== ORDER OPERATIONS ==="
                << "show commands for client ID 1"
                << "show commands from 2024-01-01"
                << "recent orders"
                << "count commands"
                << "pending orders"
                << ""
                << "=== ANALYTICS ==="
                << "total sales"
                << "monthly revenue"
                << "top clients"
                << "sales by city"
                << "average order value"
                << ""
                << "=== UTILITIES ==="
                << "help"
                << "clear chat"
                << "export data";

    m_suggestionModel = new QStringListModel(suggestions, this);
    ui->suggestionListView->setModel(m_suggestionModel);
}

void ChatbotDialog::setupAutoCompleter()
{
    QStringList commands;
    commands << "show all clients" << "find client" << "find email" << "clients in"
             << "commands for" << "commands from" << "total sales" << "count clients"
             << "count commands" << "help" << "clear" << "export" << "statistics"
             << "recent orders" << "top clients" << "monthly revenue";

    QCompleter* completer = new QCompleter(commands, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->inputLineEdit->setCompleter(completer);
}

void ChatbotDialog::setManagers(ClientManager* clientManager, CommandManager* commandManager)
{
    m_clientManager = clientManager;
    m_commandManager = commandManager;

    if (m_clientManager && m_commandManager) {
        addMessageWithAnimation("✅ Database connection established successfully!");
        addMessageWithAnimation("🔄 System ready for real-time queries.");

        // Load quick stats
        QTimer::singleShot(1000, this, &ChatbotDialog::showQuickStats);
    } else {
        addMessageWithAnimation("⚠️ Limited functionality - database connection unavailable.");
    }
}

void ChatbotDialog::showWelcomeMessage()
{
    addMessageWithAnimation("🚀 Welcome to your Enhanced Client Management Assistant!");
    addMessageWithAnimation("I'm here to help you efficiently manage clients and analyze orders.");
    addMessageWithAnimation("");
    addMessageWithAnimation("🎯 <b>Quick Start Commands:</b>");
    addMessageWithAnimation("   • Type 'help' for full command list");
    addMessageWithAnimation("   • Try 'show all clients' to see your database");
    addMessageWithAnimation("   • Use 'total sales' for quick analytics");
    addMessageWithAnimation("");
    addMessageWithAnimation("💡 <b>Pro Tips:</b>");
    addMessageWithAnimation("   • Use partial names/emails for fuzzy search");
    addMessageWithAnimation("   • Commands are case-insensitive");
    addMessageWithAnimation("   • Click suggestions or use auto-complete");
}

void ChatbotDialog::showQuickStats()
{
    if (!hasClientManager() || !hasCommandManager()) return;

    int clientCount = m_clientManager->getDAO()->getClientCount();
    int commandCount = m_commandManager->getDAO()->getCommandCount();
    double totalSales = m_commandManager->getDAO()->getTotalSales();

    addMessageWithAnimation("📊 <b>Quick Statistics:</b>");
    addMessageWithAnimation(QString("   👥 %1 clients | 📦 %2 orders | 💰 $%3 total sales")
                                .arg(clientCount)
                                .arg(commandCount)
                                .arg(QString::number(totalSales, 'f', 2)));
}

void ChatbotDialog::onInputChanged(const QString &text)
{
    Q_UNUSED(text)
    m_typingTimer->start(); // Restart timer on each keystroke
}

void ChatbotDialog::onTypingFinished()
{
    QString text = ui->inputLineEdit->text().trimmed();
    if (text.length() >= 3) {
        // Show typing suggestions or preview
        showInputPreview(text);
    }
}

void ChatbotDialog::showInputPreview(const QString &text)
{
    // This could show a preview of what the command might return
    // For now, we'll keep it simple
    Q_UNUSED(text)
}

bool ChatbotDialog::hasClientManager() const
{
    return m_clientManager != nullptr;
}

bool ChatbotDialog::hasCommandManager() const
{
    return m_commandManager != nullptr;
}

void ChatbotDialog::on_sendButton_clicked()
{
    QString query = ui->inputLineEdit->text().trimmed();
    if (!query.isEmpty()) {
        addMessage(query, true);
        ui->inputLineEdit->clear();

        // Show typing indicator
        showTypingIndicator();

        // Process query after short delay for better UX
        QTimer::singleShot(800, [this, query]() {
            hideTypingIndicator();
            processQuery(query);
        });
    }
}

void ChatbotDialog::on_inputLineEdit_returnPressed()
{
    on_sendButton_clicked();
}

void ChatbotDialog::showTypingIndicator()
{
    addMessage("💭 Processing your request...", false);
}

void ChatbotDialog::hideTypingIndicator()
{
    // Remove the last message if it's the typing indicator
    // This is a simplified implementation
}

void ChatbotDialog::processQuery(const QString &query)
{
    QString lowerQuery = query.toLower().trimmed();

    // Enhanced command recognition with better error handling
    try {
        if (lowerQuery.contains("help") || lowerQuery == "?") {
            showEnhancedHelp();
        }
        else if (lowerQuery.contains("clear") || lowerQuery == "cls") {
            clearChat();
        }
        else if (lowerQuery.contains("statistics") || lowerQuery.contains("stats")) {
            showDetailedStatistics();
        }
        else if (lowerQuery.contains("show all clients") || lowerQuery == "all clients" || lowerQuery == "list clients") {
            showAllClientsEnhanced();
        }
        else if (lowerQuery.contains("recent orders") || lowerQuery.contains("latest orders")) {
            showRecentOrders();
        }
        else if (lowerQuery.contains("top clients") || lowerQuery.contains("best clients")) {
            showTopClients();
        }
        else if (lowerQuery.contains("monthly revenue") || lowerQuery.contains("monthly sales")) {
            showMonthlyRevenue();
        }
        else if (lowerQuery.contains("sales by city") || lowerQuery.contains("city sales")) {
            showSalesByCity();
        }
        else if (lowerQuery.contains("count clients") || lowerQuery == "how many clients") {
            showClientCount();
        }
        else if (lowerQuery.contains("count commands") || lowerQuery.contains("count orders") || lowerQuery == "how many orders") {
            showCommandCount();
        }
        else if (lowerQuery.contains("total sales") || lowerQuery.contains("revenue") || lowerQuery.contains("earnings")) {
            showTotalSalesEnhanced();
        }
        else if (lowerQuery.contains("average order") || lowerQuery.contains("avg order")) {
            showAverageOrderValue();
        }
        else if (lowerQuery.contains("find client") || lowerQuery.contains("search client")) {
            QString name = extractNameFromQuery(query, QStringList{"find client", "search client"});
            if (!name.isEmpty()) {
                searchClientByNameEnhanced(name);
            } else {
                addMessageWithAnimation("❓ Please specify a client name. Example: 'find client John Doe'");
                showSampleClients();
            }
        }
        else if (lowerQuery.contains("find email") || lowerQuery.contains("email")) {
            QString email = extractEmailFromQuery(query);
            if (!email.isEmpty()) {
                searchClientByEmailEnhanced(email);
            } else {
                addMessageWithAnimation("❓ Please specify an email. Example: 'find email user@example.com'");
                showSampleEmails();
            }
        }
        else if (lowerQuery.contains("clients in") || lowerQuery.contains("city")) {
            QString city = extractCityFromQuery(query);
            if (!city.isEmpty()) {
                searchClientByCityEnhanced(city);
            } else {
                addMessageWithAnimation("❓ Please specify a city. Example: 'clients in New York'");
                showAvailableCities();
            }
        }
        else if (lowerQuery.contains("commands for") || lowerQuery.contains("orders for")) {
            QString clientInfo = extractClientFromQuery(query);
            if (!clientInfo.isEmpty()) {
                searchCommandsByClientEnhanced(clientInfo);
            } else {
                addMessageWithAnimation("❓ Please specify a client. Example: 'commands for client 1' or 'orders for John'");
            }
        }
        else if (lowerQuery.contains("commands from") || lowerQuery.contains("orders from")) {
            QString date = extractDateFromQuery(query);
            if (!date.isEmpty()) {
                searchCommandsByDateEnhanced(date);
            } else {
                addMessageWithAnimation("❓ Please specify a date. Example: 'commands from 2024-01-01'");
            }
        }
        else {
            handleUnknownCommand(query);
        }
    }
    catch (const std::exception& e) {
        addMessageWithAnimation("❌ An error occurred while processing your request.");
        addMessageWithAnimation("🔧 Please try a different command or contact support.");
        qDebug() << "Error in processQuery:" << e.what();
    }
}

void ChatbotDialog::handleUnknownCommand(const QString &query)
{
    QString lowerQuery = query.toLower();

    // Smart suggestions based on input
    if (lowerQuery.length() < 3) {
        addMessageWithAnimation("❓ Please be more specific. Type 'help' to see available commands.");
    }
    else if (lowerQuery.contains("@")) {
        addMessageWithAnimation("🔍 Detected email format. Searching clients by email...");
        searchClientByEmailEnhanced(lowerQuery);
    }
    else if (lowerQuery.split(" ").length() >= 2 && !lowerQuery.contains("show") && !lowerQuery.contains("find")) {
        addMessageWithAnimation(QString("🔍 Searching for client name: '%1'").arg(query));
        searchClientByNameEnhanced(query);
    }
    else if (QRegularExpression("\\d{4}-\\d{2}-\\d{2}").match(lowerQuery).hasMatch()) {
        addMessageWithAnimation("🔍 Detected date format. Searching orders by date...");
        searchCommandsByDateEnhanced(extractDateFromQuery(query));
    }
    else {
        addMessageWithAnimation("🤔 I'm not sure how to help with that. Here are some suggestions:");
        addMessageWithAnimation("");
        addMessageWithAnimation("📋 <b>Try these popular commands:</b>");
        addMessageWithAnimation("   • 'show all clients' - View all clients");
        addMessageWithAnimation("   • 'total sales' - See revenue statistics");
        addMessageWithAnimation("   • 'recent orders' - Show latest orders");
        addMessageWithAnimation("   • 'top clients' - Best performing clients");
        addMessageWithAnimation("");
        addMessageWithAnimation("💡 Or type 'help' for the complete command list!");
    }
}

void ChatbotDialog::clearChat()
{
    ui->chatHistory->clear();
    addMessageWithAnimation("🧹 Chat cleared! How can I help you?");
}

void ChatbotDialog::showDetailedStatistics()
{
    if (!hasClientManager() || !hasCommandManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }

    addMessageWithAnimation("📊 <b>Detailed Business Statistics</b>");
    addMessageWithAnimation("═══════════════════════════════════");

    // Client stats
    int clientCount = m_clientManager->getDAO()->getClientCount();
    addMessageWithAnimation(QString("👥 <b>Clients:</b> %1 total").arg(clientCount));

    // Order stats
    int commandCount = m_commandManager->getDAO()->getCommandCount();
    double totalSales = m_commandManager->getDAO()->getTotalSales();

    addMessageWithAnimation(QString("📦 <b>Orders:</b> %1 total").arg(commandCount));
    addMessageWithAnimation(QString("💰 <b>Revenue:</b> $%1").arg(QString::number(totalSales, 'f', 2)));

    if (commandCount > 0) {
        double avgOrder = totalSales / commandCount;
        addMessageWithAnimation(QString("📈 <b>Average Order:</b> $%1").arg(QString::number(avgOrder, 'f', 2)));

        double avgOrdersPerClient = (double)commandCount / clientCount;
        addMessageWithAnimation(QString("🔄 <b>Orders per Client:</b> %1").arg(QString::number(avgOrdersPerClient, 'f', 1)));
    }
}

void ChatbotDialog::showEnhancedHelp()
{
    addMessageWithAnimation("🤖 <b>Enhanced Client Management Assistant - Command Reference</b>");
    addMessageWithAnimation("═══════════════════════════════════════════════════════════");
    addMessageWithAnimation("");

    addMessageWithAnimation("📋 <b>CLIENT MANAGEMENT</b>");
    addMessageWithAnimation("   • 'show all clients' - Display all clients with details");
    addMessageWithAnimation("   • 'find client [name]' - Search by name (fuzzy matching)");
    addMessageWithAnimation("   • 'find email [email]' - Search by email address");
    addMessageWithAnimation("   • 'clients in [city]' - Find clients in specific city");
    addMessageWithAnimation("   • 'count clients' - Total number of clients");
    addMessageWithAnimation("");

    addMessageWithAnimation("📦 <b>ORDER MANAGEMENT</b>");
    addMessageWithAnimation("   • 'commands for client [id/name]' - Client's order history");
    addMessageWithAnimation("   • 'commands from [YYYY-MM-DD]' - Orders from specific date");
    addMessageWithAnimation("   • 'recent orders' - Show latest orders");
    addMessageWithAnimation("   • 'count commands' - Total number of orders");
    addMessageWithAnimation("");

    addMessageWithAnimation("📊 <b>ANALYTICS & REPORTS</b>");
    addMessageWithAnimation("   • 'total sales' - Complete revenue overview");
    addMessageWithAnimation("   • 'statistics' - Detailed business metrics");
    addMessageWithAnimation("   • 'top clients' - Best performing clients");
    addMessageWithAnimation("   • 'monthly revenue' - Revenue by month");
    addMessageWithAnimation("   • 'sales by city' - Geographic sales breakdown");
    addMessageWithAnimation("   • 'average order' - Average order value");
    addMessageWithAnimation("");

    addMessageWithAnimation("🛠️ <b>UTILITIES</b>");
    addMessageWithAnimation("   • 'clear' - Clear chat history");
    addMessageWithAnimation("   • 'help' - Show this help message");
    addMessageWithAnimation("");

    addMessageWithAnimation("💡 <b>PRO TIPS</b>");
    addMessageWithAnimation("   • All searches support partial matching");
    addMessageWithAnimation("   • Commands are case-insensitive");
    addMessageWithAnimation("   • Use auto-complete for faster typing");
    addMessageWithAnimation("   • Click suggestions in the side panel");

    if (!hasClientManager() || !hasCommandManager()) {
        addMessageWithAnimation("");
        addMessageWithAnimation("⚠️ <b>Note:</b> Some features may be limited due to database connectivity.");
    }
}

// Enhanced versions of existing methods with better formatting and error handling
void ChatbotDialog::showAllClientsEnhanced()
{
    if (!hasClientManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }

    QList<Client> clients = m_clientManager->getAllClients();

    if (clients.isEmpty()) {
        addMessageWithAnimation("📋 No clients found in the database.");
        addMessageWithAnimation("💡 Try adding some clients first!");
        return;
    }

    addMessageWithAnimation(QString("📋 <b>Client Directory (%1 total)</b>").arg(clients.size()));
    addMessageWithAnimation("═══════════════════════════════════════");

    for (int i = 0; i < clients.size() && i < 15; ++i) {
        const Client &client = clients.at(i);
        addMessageWithAnimation(QString("👤 <b>%1</b> (ID: %2)")
                                    .arg(client.name)
                                    .arg(client.id));
        addMessageWithAnimation(QString("   📧 %1 | 🏙️ %2")
                                    .arg(client.email.isEmpty() ? "No email" : client.email)
                                    .arg(client.city.isEmpty() ? "No city" : client.city));

        // Show order count if available
        if (hasCommandManager()) {
            QList<Command> commands = m_commandManager->getClientCommands(client.id);
            if (!commands.isEmpty()) {
                double total = 0;
                for (const Command &cmd : commands) {
                    total += cmd.getTotalAmount();
                }
                addMessageWithAnimation(QString("   📦 %1 orders ($%2)")
                                            .arg(commands.size())
                                            .arg(QString::number(total, 'f', 2)));
            }
        }
        addMessageWithAnimation("");
    }

    if (clients.size() > 15) {
        addMessageWithAnimation(QString("... and %1 more clients").arg(clients.size() - 15));
        addMessageWithAnimation("💡 Use search commands to find specific clients!");
    }
}

void ChatbotDialog::showRecentOrders()
{
    if (!hasCommandManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }

    QList<Command> allCommands = m_commandManager->getAllCommands();

    if (allCommands.isEmpty()) {
        addMessageWithAnimation("📦 No orders found in the database.");
        return;
    }

    // Sort by date (most recent first) - simplified approach
    addMessageWithAnimation("📦 <b>Recent Orders (Last 10)</b>");
    addMessageWithAnimation("═══════════════════════════════");

    int shown = 0;
    for (const Command &command : allCommands) {
        if (shown >= 10) break;

        Client client = m_clientManager->getClient(command.clientId);
        QString clientName = (client.id > 0) ? client.name : "Unknown Client";

        addMessageWithAnimation(QString("🛒 Order #%1 - %2").arg(command.commandId).arg(clientName));
        addMessageWithAnimation(QString("   📅 %1 | 💰 $%2")
                                    .arg(command.commandDate.toString("yyyy-MM-dd hh:mm"))
                                    .arg(command.total));
        addMessageWithAnimation("");
        shown++;
    }
}

void ChatbotDialog::addMessage(const QString &message, bool isUser)
{
    QTextEdit *chatHistory = ui->chatHistory;
    QString formattedMessage;

    if (isUser) {
        formattedMessage = QString("<div style='margin:8px 5px; padding:12px; background: linear-gradient(135deg, #007bff, #0056b3); "
                                   "border-radius:18px 18px 5px 18px; text-align:right; color:white; box-shadow: 0 2px 5px rgba(0,123,255,0.3);'>"
                                   "<b>You:</b> %1<br><span style='color:#e3f2fd; font-size:11px; opacity:0.8;'>%2</span>"
                                   "</div>")
                               .arg(message.toHtmlEscaped())
                               .arg(QDateTime::currentDateTime().toString("hh:mm"));
    } else {
        formattedMessage = QString("<div style='margin:8px 5px; padding:12px; background: linear-gradient(135deg, #f8f9fa, #e9ecef); "
                                   "border-radius:18px 18px 18px 5px; border-left:4px solid #007bff; box-shadow: 0 2px 5px rgba(0,0,0,0.1);'>"
                                   "<b style='color:#007bff;'>🤖 Assistant:</b> %1<br><span style='color:#6c757d; font-size:11px;'>%2</span>"
                                   "</div>")
                               .arg(message)
                               .arg(QDateTime::currentDateTime().toString("hh:mm"));
    }

    chatHistory->append(formattedMessage);

    // Smooth scroll to bottom
    QScrollBar *scrollbar = chatHistory->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void ChatbotDialog::addMessageWithAnimation(const QString &message, bool isUser)
{
    // For now, just call addMessage - animation can be added later
    addMessage(message, isUser);
}

// Helper methods for extracting information from queries
QString ChatbotDialog::extractNameFromQuery(const QString &query, const QStringList &prefixes)
{
    QString lowerQuery = query.toLower();

    for (const QString &prefix : prefixes) {
        if (lowerQuery.contains(prefix)) {
            int startIndex = lowerQuery.indexOf(prefix) + prefix.length();
            return query.mid(startIndex).trimmed();
        }
    }
    return "";
}

QString ChatbotDialog::extractEmailFromQuery(const QString &query)
{
    QRegularExpression emailRegex("\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Z|a-z]{2,}\\b");
    QRegularExpressionMatch match = emailRegex.match(query);

    if (match.hasMatch()) {
        return match.captured(0);
    }

    QString lowerQuery = query.toLower();
    if (lowerQuery.contains("email")) {
        int startIndex = lowerQuery.indexOf("email") + 5;
        return query.mid(startIndex).trimmed();
    }
    return "";
}

QString ChatbotDialog::extractCityFromQuery(const QString &query)
{
    QString lowerQuery = query.toLower();

    if (lowerQuery.contains("clients in")) {
        int startIndex = lowerQuery.indexOf("clients in") + 10;
        return query.mid(startIndex).trimmed();
    } else if (lowerQuery.contains("in")) {
        int startIndex = lowerQuery.indexOf("in") + 2;
        return query.mid(startIndex).trimmed();
    }
    return "";
}

QString ChatbotDialog::extractClientFromQuery(const QString &query)
{
    QString lowerQuery = query.toLower();

    if (lowerQuery.contains("for client")) {
        int startIndex = lowerQuery.indexOf("for client") + 10;
        return query.mid(startIndex).trimmed();
    } else if (lowerQuery.contains("for")) {
        int startIndex = lowerQuery.indexOf("for") + 3;
        return query.mid(startIndex).trimmed();
    }
    return "";
}

QString ChatbotDialog::extractDateFromQuery(const QString &query)
{
    QRegularExpression dateRegex("\\b\\d{4}-\\d{2}-\\d{2}\\b");
    QRegularExpressionMatch match = dateRegex.match(query);

    if (match.hasMatch()) {
        return match.captured(0);
    }

    QString lowerQuery = query.toLower();
    if (lowerQuery.contains("from")) {
        int startIndex = lowerQuery.indexOf("from") + 4;
        return query.mid(startIndex).trimmed();
    }
    return "";
}

void ChatbotDialog::showClientCommands(int clientId)
{
    // Implementation for showing client commands
    if (!hasCommandManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }

    QList<Command> commands = m_commandManager->getClientCommands(clientId);

    if (commands.isEmpty()) {
        addMessageWithAnimation("📦 No orders found for this client.");
        return;
    }

    addMessageWithAnimation(QString("📦 <b>Orders for Client ID %1</b>").arg(clientId));
    addMessageWithAnimation("═══════════════════════════════");

    for (const Command &command : commands) {
        addMessageWithAnimation(QString("🛒 Order #%1").arg(command.commandId));
        addMessageWithAnimation(QString("   📅 %1 | 💰 $%2 | %3")
                                    .arg(command.commandDate.toString("yyyy-MM-dd"))
                                    .arg(command.total)
                                    .arg(command.paymentMethod));
        addMessageWithAnimation("");
    }
}

// Stub implementations for methods that need to be implemented
void ChatbotDialog::searchClientByNameEnhanced(const QString &name) {
    addMessageWithAnimation(QString("🔍 Searching for client: %1").arg(name));
    // Implementation needed
}

void ChatbotDialog::searchClientByEmailEnhanced(const QString &email) {
    addMessageWithAnimation(QString("🔍 Searching for email: %1").arg(email));
    // Implementation needed
}

void ChatbotDialog::searchClientByCityEnhanced(const QString &city) {
    addMessageWithAnimation(QString("🔍 Searching clients in city: %1").arg(city));
    // Implementation needed
}

void ChatbotDialog::showClientCount() {
    if (!hasClientManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }
    int count = m_clientManager->getDAO()->getClientCount();
    addMessageWithAnimation(QString("👥 Total clients: %1").arg(count));
}

void ChatbotDialog::showCommandCount() {
    if (!hasCommandManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }
    int count = m_commandManager->getDAO()->getCommandCount();
    addMessageWithAnimation(QString("📦 Total commands: %1").arg(count));
}

void ChatbotDialog::showTotalSalesEnhanced() {
    if (!hasCommandManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }
    double total = m_commandManager->getDAO()->getTotalSales();
    addMessageWithAnimation(QString("💰 Total sales: $%1").arg(QString::number(total, 'f', 2)));
}

void ChatbotDialog::showAverageOrderValue() {
    if (!hasCommandManager()) {
        addMessageWithAnimation("❌ Database connection not available.");
        return;
    }
    int count = m_commandManager->getDAO()->getCommandCount();
    double total = m_commandManager->getDAO()->getTotalSales();
    if (count > 0) {
        double avg = total / count;
        addMessageWithAnimation(QString("📊 Average order value: $%1").arg(QString::number(avg, 'f', 2)));
    } else {
        addMessageWithAnimation("📊 No orders found to calculate average.");
    }
}

void ChatbotDialog::showTopClients() {
    addMessageWithAnimation("🏆 Top clients feature coming soon!");
    // Implementation needed
}

void ChatbotDialog::showMonthlyRevenue() {
    addMessageWithAnimation("📈 Monthly revenue feature coming soon!");
    // Implementation needed
}

void ChatbotDialog::showSalesByCity() {
    addMessageWithAnimation("🏙️ Sales by city feature coming soon!");
    // Implementation needed
}

void ChatbotDialog::searchCommandsByClientEnhanced(const QString &clientInfo) {
    addMessageWithAnimation(QString("🔍 Searching commands for client: %1").arg(clientInfo));
    // Implementation needed
}

void ChatbotDialog::searchCommandsByDateEnhanced(const QString &date) {
    addMessageWithAnimation(QString("🔍 Searching commands from date: %1").arg(date));
    // Implementation needed
}

void ChatbotDialog::showSampleClients() {
    addMessageWithAnimation("💡 Try searching for clients like: 'John Doe', 'Jane Smith', etc.");
}

void ChatbotDialog::showSampleEmails() {
    addMessageWithAnimation("💡 Try searching for emails like: 'john@example.com', 'jane@company.com'");
}

void ChatbotDialog::showAvailableCities() {
    addMessageWithAnimation("💡 Try searching for cities like: 'New York', 'London', 'Paris'");
}
