// chatbotdialog.h - Enhanced Version
#ifndef CHATBOTDIALOG_H
#define CHATBOTDIALOG_H

#include <QDialog>
#include <QStringListModel>
#include <QTimer>
#include "clients.h"         // This contains ClientManager class
#include "commands.h"        // This contains CommandManager class

// Forward declarations
class QCompleter;
class QPropertyAnimation;
class QGraphicsOpacityEffect;

namespace Ui {
class ChatbotDialog;
}

class ChatbotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatbotDialog(QWidget *parent = nullptr);
    ~ChatbotDialog();

    // Manager setup - CHANGED TO USE ClientManager AND CommandManager CLASSES
    void setManagers(ClientManager* clientManager, CommandManager* commandManager);

    // Utility methods
    bool hasClientManager() const;
    bool hasCommandManager() const;

private slots:
    void on_sendButton_clicked();
    void on_inputLineEdit_returnPressed();
    void onInputChanged(const QString &text);
    void onTypingFinished();

private:
    Ui::ChatbotDialog *ui;
    // CHANGED TO USE ClientManager AND CommandManager CLASSES
    ClientManager *m_clientManager;
    CommandManager *m_commandManager;
    QStringListModel *m_suggestionModel;
    QTimer *m_typingTimer;

    // UI Setup and Enhancement
    void setupSuggestions();
    void setupAutoCompleter();
    void showWelcomeMessage();
    void showQuickStats();

    // Core Processing
    void processQuery(const QString &query);
    void handleUnknownCommand(const QString &query);

    // Message Display
    void addMessage(const QString &message, bool isUser = false);
    void addMessageWithAnimation(const QString &message, bool isUser = false);
    void showInputPreview(const QString &text);
    void showTypingIndicator();
    void hideTypingIndicator();

    // Enhanced Commands
    void showEnhancedHelp();
    void clearChat();
    void showDetailedStatistics();

    // Enhanced Client Operations
    void showAllClientsEnhanced();
    void searchClientByNameEnhanced(const QString &name);
    void searchClientByEmailEnhanced(const QString &email);
    void searchClientByCityEnhanced(const QString &city);
    void showClientCount();

    // Enhanced Order Operations
    void showRecentOrders();
    void searchCommandsByClientEnhanced(const QString &clientInfo);
    void searchCommandsByDateEnhanced(const QString &date);
    void showCommandCount();

    // Enhanced Analytics
    void showTotalSalesEnhanced();
    void showAverageOrderValue();
    void showTopClients();
    void showMonthlyRevenue();
    void showSalesByCity();

    // Helper Display Methods
    void showSampleClients();
    void showSampleEmails();
    void showAvailableCities();

    // Query Parsing Helpers
    QString extractNameFromQuery(const QString &query, const QStringList &prefixes);
    QString extractEmailFromQuery(const QString &query);
    QString extractCityFromQuery(const QString &query);
    QString extractClientFromQuery(const QString &query);
    QString extractDateFromQuery(const QString &query);

    // Legacy Methods (for backward compatibility)
    void showAllClients() { showAllClientsEnhanced(); }
    void showTotalSales() { showTotalSalesEnhanced(); }
    void showHelp() { showEnhancedHelp(); }
    void searchClientByName(const QString &name) { searchClientByNameEnhanced(name); }
    void searchClientByEmail(const QString &email) { searchClientByEmailEnhanced(email); }
    void searchClientByCity(const QString &city) { searchClientByCityEnhanced(city); }
    void searchCommandsByClient(const QString &clientInfo) { searchCommandsByClientEnhanced(clientInfo); }
    void searchCommandsByDate(const QString &date) { searchCommandsByDateEnhanced(date); }
    void showClientCommands(int clientId);
};

#endif // CHATBOTDIALOG_H
