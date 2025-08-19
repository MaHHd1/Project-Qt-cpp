#ifndef COMMANDSWINDOW_H
#define COMMANDSWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QGroupBox>
#include <QFrame>
#include <QMessageBox>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QCompleter>
#include <QStringListModel>

#include "commands.h"
#include "clients.h"

class CommandsWindow : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        AddMode,
        EditMode,
        ViewMode
    };

    explicit CommandsWindow(CommandManager* commandManager,
                            ClientManager* clientManager,
                            QWidget *parent = nullptr);
    explicit CommandsWindow(CommandManager* commandManager,
                            ClientManager* clientManager,
                            const Command& command,
                            Mode mode = EditMode,
                            QWidget *parent = nullptr);
    ~CommandsWindow();

    Command getCurrentCommand() const;
    void setCommand(const Command& command);
    void setMode(Mode mode);
    Mode getMode() const { return currentMode; }

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onClientChanged();
    void onTotalChanged();
    void onPaymentMethodChanged();
    void onValidationTimer();
    void onClientSearch();
    void onSelectClientClicked();
    void onNewClientClicked();
    void onCalculateTotal();

private:
    // UI Setup
    void setupUI();
    void setupFormLayout();
    void setupButtonLayout();
    void setupClientSelection();
    void applyModernStyling();
    void connectSignals();

    // Data management
    void loadClientData();
    void loadPaymentMethods();
    void populateForm();
    void clearForm();
    void updateClientInfo();

    // Validation
    bool validateForm();
    void showValidationError(const QString& message);
    void clearValidationErrors();
    void setFieldError(QWidget* field, bool hasError);

    // Animation and effects
    void animateShow();
    void animateFieldError(QWidget* field);

    // Utility
    void updateWindowTitle();
    void updateButtonStates();
    void setFieldsReadOnly(bool readOnly);
    QString formatCurrency(double amount) const;
    double parseCurrency(const QString& text) const;

    // UI Components
    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QHBoxLayout *buttonLayout;
    QHBoxLayout *clientSelectionLayout;

    // Form fields
    QGroupBox *commandInfoGroup;
    QGroupBox *clientInfoGroup;
    QGroupBox *deliveryInfoGroup;

    QLabel *commandIdLabel;
    QLineEdit *commandIdEdit;

    QLabel *clientLabel;
    QComboBox *clientCombo;
    QPushButton *selectClientBtn;
    QPushButton *newClientBtn;
    QLineEdit *clientSearchEdit;

    QLabel *clientNameDisplay;
    QLabel *clientEmailDisplay;
    QLabel *clientAddressDisplay;

    QLabel *commandDateLabel;
    QDateTimeEdit *commandDateEdit;

    QLabel *totalLabel;
    QLineEdit *totalEdit;
    QPushButton *calculateBtn;

    QLabel *paymentMethodLabel;
    QComboBox *paymentMethodCombo;

    QLabel *deliveryAddressLabel;
    QTextEdit *deliveryAddressEdit;

    QLabel *notesLabel;
    QTextEdit *notesEdit;

    // Buttons
    QPushButton *saveBtn;
    QPushButton *cancelBtn;
    QPushButton *clearBtn;

    // Status and validation
    QLabel *statusLabel;
    QFrame *validationFrame;
    QLabel *validationLabel;

    // Data managers
    CommandManager *commandManager;
    ClientManager *clientManager;

    // Current state
    Command currentCommand;
    Mode currentMode;
    bool isModified;

    // Validation
    QTimer *validationTimer;
    QStringList validationErrors;

    // Client data cache
    QList<Client> availableClients;
    QCompleter *clientCompleter;
    QStringListModel *clientModel;

    // Animation
    QPropertyAnimation *showAnimation;
    QGraphicsOpacityEffect *opacityEffect;

    // Constants
    static const int WINDOW_WIDTH = 600;
    static const int WINDOW_HEIGHT = 700;
    static const int VALIDATION_DELAY = 500; // ms
};

#endif // COMMANDSWINDOW_H
