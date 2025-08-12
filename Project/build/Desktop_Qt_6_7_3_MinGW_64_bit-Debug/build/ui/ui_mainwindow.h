/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Client;
    QAction *actionNew_Command;
    QAction *actionImport_Data;
    QAction *actionExport_Data;
    QAction *actionExit;
    QAction *actionRefresh;
    QAction *actionClients_Statistics;
    QAction *actionSales_Report;
    QAction *actionAbout;
    QAction *actionUser_Manual;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_main;
    QFrame *headerFrame;
    QVBoxLayout *headerLayout;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QTabWidget *mainTabWidget;
    QWidget *clientsTab;
    QVBoxLayout *clientsLayout;
    QLabel *clientsHeaderLabel;
    QLabel *clientsSubtitleLabel;
    QFrame *clientsStatsFrame;
    QHBoxLayout *clientActionsLayout;
    QPushButton *addClientBtn;
    QPushButton *clientStatsBtn;
    QPushButton *exportClientsBtn;
    QSpacerItem *clientActionsHSpacer;
    QHBoxLayout *clientSearchLayout;
    QLineEdit *clientSearchEdit;
    QPushButton *clientSearchBtn;
    QTableWidget *clientsTable;
    QWidget *commandsTab;
    QVBoxLayout *commandsLayout;
    QLabel *commandsHeaderLabel;
    QLabel *commandsSubtitleLabel;
    QFrame *commandsStatsFrame;
    QHBoxLayout *commandActionsLayout;
    QPushButton *addCommandBtn;
    QPushButton *salesReportBtn;
    QPushButton *deliveryStatusBtn;
    QSpacerItem *commandActionsHSpacer;
    QHBoxLayout *commandSearchLayout;
    QLineEdit *commandSearchEdit;
    QPushButton *commandSearchBtn;
    QTableWidget *commandsTable;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1400, 900);
        MainWindow->setMinimumSize(QSize(1200, 800));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/app_icon.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setStyleSheet(QString::fromUtf8("/* Background will be set programmatically */"));
        actionNew_Client = new QAction(MainWindow);
        actionNew_Client->setObjectName("actionNew_Client");
        actionNew_Command = new QAction(MainWindow);
        actionNew_Command->setObjectName("actionNew_Command");
        actionImport_Data = new QAction(MainWindow);
        actionImport_Data->setObjectName("actionImport_Data");
        actionExport_Data = new QAction(MainWindow);
        actionExport_Data->setObjectName("actionExport_Data");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionRefresh = new QAction(MainWindow);
        actionRefresh->setObjectName("actionRefresh");
        actionClients_Statistics = new QAction(MainWindow);
        actionClients_Statistics->setObjectName("actionClients_Statistics");
        actionSales_Report = new QAction(MainWindow);
        actionSales_Report->setObjectName("actionSales_Report");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionUser_Manual = new QAction(MainWindow);
        actionUser_Manual->setObjectName("actionUser_Manual");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setStyleSheet(QString::fromUtf8("QWidget#centralwidget {\n"
"    background: url(:/resources/background.png) center/cover;\n"
"}"));
        verticalLayout_main = new QVBoxLayout(centralwidget);
        verticalLayout_main->setSpacing(0);
        verticalLayout_main->setObjectName("verticalLayout_main");
        verticalLayout_main->setContentsMargins(0, 0, 0, 0);
        headerFrame = new QFrame(centralwidget);
        headerFrame->setObjectName("headerFrame");
        headerFrame->setMinimumSize(QSize(0, 80));
        headerFrame->setMaximumSize(QSize(16777215, 80));
        headerFrame->setStyleSheet(QString::fromUtf8("QFrame#headerFrame {\n"
"    background: rgba(255, 255, 255, 0.15);\n"
"    backdrop-filter: blur(20px);\n"
"    border-bottom: 1px solid rgba(255, 255, 255, 0.2);\n"
"}\n"
"\n"
"QLabel#titleLabel {\n"
"    color: white;\n"
"    font-size: 28px;\n"
"    font-weight: 600;\n"
"    text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);\n"
"}\n"
"\n"
"QLabel#subtitleLabel {\n"
"    color: rgba(255, 255, 255, 0.8);\n"
"    font-size: 14px;\n"
"    font-weight: 300;\n"
"}"));
        headerFrame->setFrameShape(QFrame::NoFrame);
        headerFrame->setFrameShadow(QFrame::Raised);
        headerLayout = new QVBoxLayout(headerFrame);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(30, 15, 30, 15);
        titleLabel = new QLabel(headerFrame);
        titleLabel->setObjectName("titleLabel");

        headerLayout->addWidget(titleLabel);

        subtitleLabel = new QLabel(headerFrame);
        subtitleLabel->setObjectName("subtitleLabel");

        headerLayout->addWidget(subtitleLabel);


        verticalLayout_main->addWidget(headerFrame);

        mainTabWidget = new QTabWidget(centralwidget);
        mainTabWidget->setObjectName("mainTabWidget");
        mainTabWidget->setStyleSheet(QString::fromUtf8("QTabWidget::pane {\n"
"    border: 1px solid rgba(255, 255, 255, 0.2);\n"
"    background: rgba(255, 255, 255, 0.95);\n"
"    border-radius: 10px;\n"
"    border-top-left-radius: 0px;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background: rgba(255, 255, 255, 0.1);\n"
"    color: rgba(255, 255, 255, 0.8);\n"
"    border: none;\n"
"    padding: 15px 25px;\n"
"    margin-right: 2px;\n"
"    border-top-left-radius: 10px;\n"
"    border-top-right-radius: 10px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 200px;\n"
"}\n"
"\n"
"QTabBar::tab:hover {\n"
"    background: rgba(255, 255, 255, 0.15);\n"
"    color: white;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background: rgba(255, 255, 255, 0.95);\n"
"    color: #2d3748;\n"
"    font-weight: 600;\n"
"}"));
        mainTabWidget->setDocumentMode(true);
        mainTabWidget->setTabsClosable(false);
        mainTabWidget->setMovable(false);
        mainTabWidget->setTabBarAutoHide(false);
        clientsTab = new QWidget();
        clientsTab->setObjectName("clientsTab");
        clientsLayout = new QVBoxLayout(clientsTab);
        clientsLayout->setSpacing(20);
        clientsLayout->setObjectName("clientsLayout");
        clientsLayout->setContentsMargins(30, 30, 30, 30);
        clientsHeaderLabel = new QLabel(clientsTab);
        clientsHeaderLabel->setObjectName("clientsHeaderLabel");
        clientsHeaderLabel->setStyleSheet(QString::fromUtf8("font-size: 24px;\n"
"font-weight: 600;\n"
"color: #2d3748;\n"
"margin-bottom: 8px;"));

        clientsLayout->addWidget(clientsHeaderLabel);

        clientsSubtitleLabel = new QLabel(clientsTab);
        clientsSubtitleLabel->setObjectName("clientsSubtitleLabel");
        clientsSubtitleLabel->setStyleSheet(QString::fromUtf8("font-size: 14px;\n"
"color: #718096;\n"
"margin-bottom: 20px;"));

        clientsLayout->addWidget(clientsSubtitleLabel);

        clientsStatsFrame = new QFrame(clientsTab);
        clientsStatsFrame->setObjectName("clientsStatsFrame");
        clientsStatsFrame->setMinimumSize(QSize(0, 120));
        clientsStatsFrame->setMaximumSize(QSize(16777215, 120));
        clientsStatsFrame->setStyleSheet(QString::fromUtf8("QFrame#clientsStatsFrame {\n"
"    background: rgba(255, 255, 255, 0.9);\n"
"    border: 1px solid rgba(255, 255, 255, 0.3);\n"
"    border-radius: 15px;\n"
"    margin-bottom: 10px;\n"
"}"));
        clientsStatsFrame->setFrameShape(QFrame::StyledPanel);
        clientsStatsFrame->setFrameShadow(QFrame::Raised);

        clientsLayout->addWidget(clientsStatsFrame);

        clientActionsLayout = new QHBoxLayout();
        clientActionsLayout->setSpacing(15);
        clientActionsLayout->setObjectName("clientActionsLayout");
        addClientBtn = new QPushButton(clientsTab);
        addClientBtn->setObjectName("addClientBtn");
        addClientBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(102, 126, 234, 1), stop:1 rgba(118, 75, 162, 1));\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 12px 24px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(90, 110, 220, 1), stop:1 rgba(100, 65, 150, 1));\n"
"}"));

        clientActionsLayout->addWidget(addClientBtn);

        clientStatsBtn = new QPushButton(clientsTab);
        clientStatsBtn->setObjectName("clientStatsBtn");
        clientStatsBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(0, 212, 170, 1), stop:1 rgba(0, 168, 255, 1));\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 12px 24px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(0, 190, 150, 1), stop:1 rgba(0, 150, 230, 1));\n"
"}"));

        clientActionsLayout->addWidget(clientStatsBtn);

        exportClientsBtn = new QPushButton(clientsTab);
        exportClientsBtn->setObjectName("exportClientsBtn");
        exportClientsBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: transparent;\n"
"    color: #667eea;\n"
"    border: 2px solid #667eea;\n"
"    border-radius: 10px;\n"
"    padding: 10px 22px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: #667eea;\n"
"    color: white;\n"
"}"));

        clientActionsLayout->addWidget(exportClientsBtn);

        clientActionsHSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        clientActionsLayout->addItem(clientActionsHSpacer);


        clientsLayout->addLayout(clientActionsLayout);

        clientSearchLayout = new QHBoxLayout();
        clientSearchLayout->setSpacing(15);
        clientSearchLayout->setObjectName("clientSearchLayout");
        clientSearchEdit = new QLineEdit(clientsTab);
        clientSearchEdit->setObjectName("clientSearchEdit");
        clientSearchEdit->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    padding: 12px 16px;\n"
"    border: 2px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    font-size: 14px;\n"
"    background: white;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border-color: #667eea;\n"
"}"));

        clientSearchLayout->addWidget(clientSearchEdit);

        clientSearchBtn = new QPushButton(clientsTab);
        clientSearchBtn->setObjectName("clientSearchBtn");
        clientSearchBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: transparent;\n"
"    color: #667eea;\n"
"    border: 2px solid #667eea;\n"
"    border-radius: 10px;\n"
"    padding: 10px 22px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 80px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: #667eea;\n"
"    color: white;\n"
"}"));

        clientSearchLayout->addWidget(clientSearchBtn);


        clientsLayout->addLayout(clientSearchLayout);

        clientsTable = new QTableWidget(clientsTab);
        clientsTable->setObjectName("clientsTable");
        clientsTable->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    background: white;\n"
"    border: 1px solid #f0f0f0;\n"
"    border-radius: 15px;\n"
"    gridline-color: #f0f0f0;\n"
"    selection-background-color: rgba(102, 126, 234, 0.2);\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 8px;\n"
"    border-bottom: 1px solid #f0f0f0;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background: rgba(102, 126, 234, 0.3);\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(248, 250, 252, 1), stop:1 rgba(226, 232, 240, 1));\n"
"    color: #2d3748;\n"
"    padding: 10px;\n"
"    border: none;\n"
"    border-bottom: 2px solid #e2e8f0;\n"
"    font-weight: 600;\n"
"}"));
        clientsTable->setAlternatingRowColors(true);
        clientsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        clientsTable->setSortingEnabled(true);

        clientsLayout->addWidget(clientsTable);

        mainTabWidget->addTab(clientsTab, QString());
        commandsTab = new QWidget();
        commandsTab->setObjectName("commandsTab");
        commandsLayout = new QVBoxLayout(commandsTab);
        commandsLayout->setSpacing(20);
        commandsLayout->setObjectName("commandsLayout");
        commandsLayout->setContentsMargins(30, 30, 30, 30);
        commandsHeaderLabel = new QLabel(commandsTab);
        commandsHeaderLabel->setObjectName("commandsHeaderLabel");
        commandsHeaderLabel->setStyleSheet(QString::fromUtf8("font-size: 24px;\n"
"font-weight: 600;\n"
"color: #2d3748;\n"
"margin-bottom: 8px;"));

        commandsLayout->addWidget(commandsHeaderLabel);

        commandsSubtitleLabel = new QLabel(commandsTab);
        commandsSubtitleLabel->setObjectName("commandsSubtitleLabel");
        commandsSubtitleLabel->setStyleSheet(QString::fromUtf8("font-size: 14px;\n"
"color: #718096;\n"
"margin-bottom: 20px;"));

        commandsLayout->addWidget(commandsSubtitleLabel);

        commandsStatsFrame = new QFrame(commandsTab);
        commandsStatsFrame->setObjectName("commandsStatsFrame");
        commandsStatsFrame->setMinimumSize(QSize(0, 120));
        commandsStatsFrame->setMaximumSize(QSize(16777215, 120));
        commandsStatsFrame->setStyleSheet(QString::fromUtf8("QFrame#commandsStatsFrame {\n"
"    background: rgba(255, 255, 255, 0.9);\n"
"    border: 1px solid rgba(255, 255, 255, 0.3);\n"
"    border-radius: 15px;\n"
"    margin-bottom: 10px;\n"
"}"));
        commandsStatsFrame->setFrameShape(QFrame::StyledPanel);
        commandsStatsFrame->setFrameShadow(QFrame::Raised);

        commandsLayout->addWidget(commandsStatsFrame);

        commandActionsLayout = new QHBoxLayout();
        commandActionsLayout->setSpacing(15);
        commandActionsLayout->setObjectName("commandActionsLayout");
        addCommandBtn = new QPushButton(commandsTab);
        addCommandBtn->setObjectName("addCommandBtn");
        addCommandBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(102, 126, 234, 1), stop:1 rgba(118, 75, 162, 1));\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 12px 24px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(90, 110, 220, 1), stop:1 rgba(100, 65, 150, 1));\n"
"}"));

        commandActionsLayout->addWidget(addCommandBtn);

        salesReportBtn = new QPushButton(commandsTab);
        salesReportBtn->setObjectName("salesReportBtn");
        salesReportBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(0, 212, 170, 1), stop:1 rgba(0, 168, 255, 1));\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 12px 24px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(0, 190, 150, 1), stop:1 rgba(0, 150, 230, 1));\n"
"}"));

        commandActionsLayout->addWidget(salesReportBtn);

        deliveryStatusBtn = new QPushButton(commandsTab);
        deliveryStatusBtn->setObjectName("deliveryStatusBtn");
        deliveryStatusBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: transparent;\n"
"    color: #667eea;\n"
"    border: 2px solid #667eea;\n"
"    border-radius: 10px;\n"
"    padding: 10px 22px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: #667eea;\n"
"    color: white;\n"
"}"));

        commandActionsLayout->addWidget(deliveryStatusBtn);

        commandActionsHSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        commandActionsLayout->addItem(commandActionsHSpacer);


        commandsLayout->addLayout(commandActionsLayout);

        commandSearchLayout = new QHBoxLayout();
        commandSearchLayout->setSpacing(15);
        commandSearchLayout->setObjectName("commandSearchLayout");
        commandSearchEdit = new QLineEdit(commandsTab);
        commandSearchEdit->setObjectName("commandSearchEdit");
        commandSearchEdit->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    padding: 12px 16px;\n"
"    border: 2px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    font-size: 14px;\n"
"    background: white;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border-color: #667eea;\n"
"}"));

        commandSearchLayout->addWidget(commandSearchEdit);

        commandSearchBtn = new QPushButton(commandsTab);
        commandSearchBtn->setObjectName("commandSearchBtn");
        commandSearchBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: transparent;\n"
"    color: #667eea;\n"
"    border: 2px solid #667eea;\n"
"    border-radius: 10px;\n"
"    padding: 10px 22px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-width: 80px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: #667eea;\n"
"    color: white;\n"
"}"));

        commandSearchLayout->addWidget(commandSearchBtn);


        commandsLayout->addLayout(commandSearchLayout);

        commandsTable = new QTableWidget(commandsTab);
        commandsTable->setObjectName("commandsTable");
        commandsTable->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    background: white;\n"
"    border: 1px solid #f0f0f0;\n"
"    border-radius: 15px;\n"
"    gridline-color: #f0f0f0;\n"
"    selection-background-color: rgba(102, 126, 234, 0.2);\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 8px;\n"
"    border-bottom: 1px solid #f0f0f0;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background: rgba(102, 126, 234, 0.3);\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,\n"
"               stop:0 rgba(248, 250, 252, 1), stop:1 rgba(226, 232, 240, 1));\n"
"    color: #2d3748;\n"
"    padding: 10px;\n"
"    border: none;\n"
"    border-bottom: 2px solid #e2e8f0;\n"
"    font-weight: 600;\n"
"}"));
        commandsTable->setAlternatingRowColors(true);
        commandsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        commandsTable->setSortingEnabled(true);

        commandsLayout->addWidget(commandsTable);

        mainTabWidget->addTab(commandsTab, QString());

        verticalLayout_main->addWidget(mainTabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1400, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuView = new QMenu(menubar);
        menuView->setObjectName("menuView");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew_Client);
        menuFile->addAction(actionNew_Command);
        menuFile->addSeparator();
        menuFile->addAction(actionImport_Data);
        menuFile->addAction(actionExport_Data);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuView->addAction(actionRefresh);
        menuView->addSeparator();
        menuView->addAction(actionClients_Statistics);
        menuView->addAction(actionSales_Report);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionUser_Manual);

        retranslateUi(MainWindow);

        mainTabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Client Management System - Administrator", nullptr));
        actionNew_Client->setText(QCoreApplication::translate("MainWindow", "New Client", nullptr));
#if QT_CONFIG(shortcut)
        actionNew_Client->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionNew_Command->setText(QCoreApplication::translate("MainWindow", "New Command", nullptr));
#if QT_CONFIG(shortcut)
        actionNew_Command->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionImport_Data->setText(QCoreApplication::translate("MainWindow", "Import Data", nullptr));
        actionExport_Data->setText(QCoreApplication::translate("MainWindow", "Export Data", nullptr));
#if QT_CONFIG(shortcut)
        actionExport_Data->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+E", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRefresh->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
#if QT_CONFIG(shortcut)
        actionRefresh->setShortcut(QCoreApplication::translate("MainWindow", "F5", nullptr));
#endif // QT_CONFIG(shortcut)
        actionClients_Statistics->setText(QCoreApplication::translate("MainWindow", "Clients Statistics", nullptr));
        actionSales_Report->setText(QCoreApplication::translate("MainWindow", "Sales Report", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        actionUser_Manual->setText(QCoreApplication::translate("MainWindow", "User Manual", nullptr));
#if QT_CONFIG(shortcut)
        actionUser_Manual->setShortcut(QCoreApplication::translate("MainWindow", "F1", nullptr));
#endif // QT_CONFIG(shortcut)
        titleLabel->setText(QCoreApplication::translate("MainWindow", "\360\237\217\242 Client Management System", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("MainWindow", "Administrator Dashboard - Manage clients and commands efficiently", nullptr));
        clientsHeaderLabel->setText(QCoreApplication::translate("MainWindow", "\360\237\221\245 Client Management", nullptr));
        clientsSubtitleLabel->setText(QCoreApplication::translate("MainWindow", "Manage customer information, add new clients, and view client details", nullptr));
        addClientBtn->setText(QCoreApplication::translate("MainWindow", "\342\236\225 Add New Client", nullptr));
        clientStatsBtn->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 View Statistics", nullptr));
        exportClientsBtn->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Export Data", nullptr));
        clientSearchEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\224\215 Search clients by name, email, or city...", nullptr));
        clientSearchBtn->setText(QCoreApplication::translate("MainWindow", "Search", nullptr));
        mainTabWidget->setTabText(mainTabWidget->indexOf(clientsTab), QCoreApplication::translate("MainWindow", "\360\237\221\245 Clients Management", nullptr));
        commandsHeaderLabel->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Commands Management", nullptr));
        commandsSubtitleLabel->setText(QCoreApplication::translate("MainWindow", "Track orders, manage deliveries, and monitor payment status", nullptr));
        addCommandBtn->setText(QCoreApplication::translate("MainWindow", "\342\236\225 New Command", nullptr));
        salesReportBtn->setText(QCoreApplication::translate("MainWindow", "\360\237\223\210 Sales Report", nullptr));
        deliveryStatusBtn->setText(QCoreApplication::translate("MainWindow", "\360\237\232\232 Delivery Status", nullptr));
        commandSearchEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\224\215 Search commands by ID, client, or payment method...", nullptr));
        commandSearchBtn->setText(QCoreApplication::translate("MainWindow", "Search", nullptr));
        mainTabWidget->setTabText(mainTabWidget->indexOf(commandsTab), QCoreApplication::translate("MainWindow", "\360\237\223\213 Commands Management", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
