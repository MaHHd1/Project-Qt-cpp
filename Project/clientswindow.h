#ifndef CLIENTSWINDOW_H
#define CLIENTSWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>

#include "clients.h"

class ClientsWindow : public QDialog
{
    // No Q_OBJECT macro for .pro qmake without slots

public:
    enum Mode {
        AddMode,
        EditMode,
        ViewMode
    };

    explicit ClientsWindow(QWidget *parent = nullptr, Mode mode = AddMode);
    explicit ClientsWindow(const Client& client, QWidget *parent = nullptr, Mode mode = EditMode);
    ~ClientsWindow();

    // Get the client data
    Client getClient() const;
    void setClient(const Client& client);

    // Set window mode
    void setMode(Mode mode);
    Mode getMode() const { return currentMode; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    // Regular methods (not slots since no Q_OBJECT)
    void onSaveClicked();
    void onCancelClicked();
    void onClearClicked();
    void validateInput();
    void animateValidationError(QWidget *widget);

    // Helper methods
    void setupUI();
    void setupValidators();
    void setupConnections();
    void applyModernStyling();
    void updateWindowTitle();
    void populateFields();
    void clearFields();
    bool validateAllFields();
    void showValidationError(const QString &message);
    void animateShow();

    // Email validation helper
    bool isValidEmail(const QString& email);

    // UI Components
    QVBoxLayout *mainLayout;
    QFrame *headerFrame;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QFrame *formFrame;
    QFormLayout *formLayout;

    QLineEdit *nameEdit;
    QLineEdit *emailEdit;
    QLineEdit *cityEdit;
    QLineEdit *postalEdit;
    QTextEdit *addressEdit;

    QFrame *buttonFrame;
    QHBoxLayout *buttonLayout;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QPushButton *clearButton;

    QLabel *validationLabel;

    // Data and state
    Client currentClient;
    Mode currentMode;
    ClientManager *clientManager;

    // Animation
    QPropertyAnimation *fadeAnimation;
    QTimer *validationTimer;

    // Validation
    QRegularExpressionValidator *emailValidator;
    QRegularExpressionValidator *postalValidator;
    QRegularExpressionValidator *nameValidator;

    // Style constants
    static const QString VALID_STYLE;
    static const QString INVALID_STYLE;
    static const QString BUTTON_PRIMARY_STYLE;
    static const QString BUTTON_SECONDARY_STYLE;
    static const QString BUTTON_DANGER_STYLE;
};

#endif // CLIENTSWINDOW_H
