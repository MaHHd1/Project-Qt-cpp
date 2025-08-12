#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTableWidgetItem>
#include <QIcon>

#include "clientswindow.h"
#include "commandswindow.h"
#include "clients.h"
#include "commands.h"

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
};

#endif // MAINWINDOW_H
