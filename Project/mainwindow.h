#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QFileDialog>
#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QSplitter>
#include <QGroupBox>
#include <QProgressBar>
#include <QProgressDialog>       // FIXED: Added missing include
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTableWidgetItem>
#include <QIcon>
#include <QInputDialog>          // Added for QInputDialog
#include <QDesktopServices>      // Added for QDesktopServices
#include <QUrl>                  // Added for QUrl
#include <QRegularExpression>    // FIXED: Added missing include
#include <QMessageBox>           // FIXED: Added missing include

#include "chatbotdialog.h"
#include "clients.h"
#include "commands.h"
#include "clientswindow.h"       // Add this include
#include "commandswindow.h"      // Add this include
#include <QDesktopServices>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onTabChanged(int index);
    void onRefreshClicked();
    void onAddClientClicked();
    void onAddCommandClicked();
    void onSearchClients();
    void onSearchCommands();
    void refreshStatistics();
    void animateStatCards();
    void editClient(int row);
    void deleteClient(int row);
    void editCommand(int row);
    void onChatbotClicked();

private:
    Ui::MainWindow *ui;

    // Main components
    QTabWidget *mainTabWidget;

    // Client tab components
    QWidget *clientsTab;
    QFrame *clientsStatsFrame;
    QLabel *totalClientsLabel;
    QLabel *newClientsLabel;
    QLabel *activeCitiesLabel;
    QPushButton *addClientBtn;
    QPushButton *exportClientsBtn;
    QLineEdit *clientSearchEdit;
    QTableWidget *clientsTable;

    QPushButton *sendMailBtn;

    // Commands tab components
    QWidget *commandsTab;
    QFrame *commandsStatsFrame;
    QLabel *totalOrdersLabel;
    QLabel *monthlyRevenueLabel;
    QLabel *pendingOrdersLabel;
    QLabel *avgOrderValueLabel;
    QPushButton *addCommandBtn;
    QPushButton *salesReportBtn;
    QLineEdit *commandSearchEdit;
    QTableWidget *commandsTable;

    // Data managers
    ClientManager *clientManager;
    CommandManager *commandManager;

    // Animation effects
    QPropertyAnimation *fadeAnimation;
    QTimer *refreshTimer;

    // UI setup methods
    void setupUI();
    void setupStatisticsFrames();
    void setupTables();
    void applyModernStyling();
    void connectSignals();

    // Data methods
    void loadClientsData();
    void loadCommandsData();
    void updateClientStatistics();
    void updateCommandStatistics();
    void populateClientsTable();
    void populateCommandsTable();

    // Utility methods
    QFrame* createStatCard(const QString &title, const QString &value, const QString &icon = "");
    void animateWidget(QWidget *widget, int duration = 300);
    void setCardHoverEffect(QFrame *card);
    QString formatCurrency(double amount);

    // Command edit/delete methods - MOVED TO PRIVATE SECTION
    void editCommandById(int commandId);
    void deleteCommandById(int commandId);


    void setupEmailService();
    void sendEmailViaClient(const QString &email, const QString &subject, const QString &body);

    void generateClientsCommandsPDF();
    void onSendMailClicked();
    void sendClientCommandsEmail(int clientId);  // Keep this for backward compatibility
    QString createEmailContent(const Client &client, const QList<Command> &commands);

    // NEW METHODS - Add these:
    Client findClientByEmail(const QString &email);
    void sendClientCommandsEmailByAddress(const Client &client, const QList<Command> &commands);
    double calculateTotalValue(const QList<Command> &commands);  // ADDED: Helper method
    ChatbotDialog *chatbotDialog;
};

#endif // MAINWINDOW_H
