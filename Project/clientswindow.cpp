#include "clientswindow.h"
#include <QApplication>
#include <QScreen>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QSpacerItem>

// Style constants
const QString ClientsWindow::VALID_STYLE =
    "QLineEdit, QTextEdit { "
    "border: 2px solid #e2e8f0; "
    "border-radius: 8px; "
    "padding: 8px 12px; "
    "font-size: 14px; "
    "background: white; "
    "} "
    "QLineEdit:focus, QTextEdit:focus { "
    "border-color: #667eea; "
    "outline: none; "
    "}";

const QString ClientsWindow::INVALID_STYLE =
    "QLineEdit, QTextEdit { "
    "border: 2px solid #f56565; "
    "border-radius: 8px; "
    "padding: 8px 12px; "
    "font-size: 14px; "
    "background: #fed7d7; "
    "} "
    "QLineEdit:focus, QTextEdit:focus { "
    "border-color: #e53e3e; "
    "outline: none; "
    "}";

const QString ClientsWindow::BUTTON_PRIMARY_STYLE =
    "QPushButton { "
    "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
    "stop:0 rgba(102, 126, 234, 1), stop:1 rgba(118, 75, 162, 1)); "
    "color: white; "
    "border: none; "
    "border-radius: 8px; "
    "padding: 12px 24px; "
    "font-size: 14px; "
    "font-weight: 600; "
    "min-width: 100px; "
    "} "
    "QPushButton:hover { "
    "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
    "stop:0 rgba(90, 110, 220, 1), stop:1 rgba(100, 65, 150, 1)); "
    "} "
    "QPushButton:pressed { "
    "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
    "stop:0 rgba(80, 100, 200, 1), stop:1 rgba(90, 55, 140, 1)); "
    "}";

const QString ClientsWindow::BUTTON_SECONDARY_STYLE =
    "QPushButton { "
    "background: transparent; "
    "color: #667eea; "
    "border: 2px solid #667eea; "
    "border-radius: 8px; "
    "padding: 10px 22px; "
    "font-size: 14px; "
    "font-weight: 500; "
    "min-width: 100px; "
    "} "
    "QPushButton:hover { "
    "background: #667eea; "
    "color: white; "
    "} "
    "QPushButton:pressed { "
    "background: #5a67d8; "
    "}";

const QString ClientsWindow::BUTTON_DANGER_STYLE =
    "QPushButton { "
    "background: transparent; "
    "color: #f56565; "
    "border: 2px solid #f56565; "
    "border-radius: 8px; "
    "padding: 10px 22px; "
    "font-size: 14px; "
    "font-weight: 500; "
    "min-width: 100px; "
    "} "
    "QPushButton:hover { "
    "background: #f56565; "
    "color: white; "
    "} "
    "QPushButton:pressed { "
    "background: #e53e3e; "
    "}";

ClientsWindow::ClientsWindow(QWidget *parent, Mode mode)
    : QDialog(parent)
    , currentMode(mode)
    , clientManager(nullptr)
    , fadeAnimation(nullptr)
    , validationTimer(new QTimer(this))
{
    // Initialize client manager
    clientManager = new ClientManager(this);

    setupUI();
    setupValidators();
    setupConnections();
    applyModernStyling();
    updateWindowTitle();

    // Set initial state based on mode
    setMode(mode);
}

ClientsWindow::ClientsWindow(const Client& client, QWidget *parent, Mode mode)
    : ClientsWindow(parent, mode)
{
    currentClient = client;
    populateFields();
    updateWindowTitle();
}

ClientsWindow::~ClientsWindow()
{
    // Cleanup is handled by Qt's parent-child relationship
}

void ClientsWindow::setupUI()
{
    setModal(true);
    setFixedSize(500, 600);

    // Center on parent or screen
    if (parentWidget()) {
        move(parentWidget()->geometry().center() - rect().center());
    } else {
        QScreen *screen = QApplication::primaryScreen();
        if (screen) {
            move(screen->geometry().center() - rect().center());
        }
    }

    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Header frame
    headerFrame = new QFrame();
    headerFrame->setFixedHeight(80);
    headerFrame->setObjectName("headerFrame");

    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setContentsMargins(30, 15, 30, 15);

    titleLabel = new QLabel("Add New Client");
    titleLabel->setObjectName("titleLabel");

    subtitleLabel = new QLabel("Enter client information below");
    subtitleLabel->setObjectName("subtitleLabel");

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addWidget(headerFrame);

    // Form frame
    formFrame = new QFrame();
    formFrame->setObjectName("formFrame");

    QVBoxLayout *formContainer = new QVBoxLayout(formFrame);
    formContainer->setContentsMargins(30, 30, 30, 20);

    formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // Form fields
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Enter full name");
    nameEdit->installEventFilter(this);

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("Enter email address");
    emailEdit->installEventFilter(this);

    cityEdit = new QLineEdit();
    cityEdit->setPlaceholderText("Enter city");
    cityEdit->installEventFilter(this);

    postalEdit = new QLineEdit();
    postalEdit->setPlaceholderText("Enter postal code");
    postalEdit->installEventFilter(this);

    addressEdit = new QTextEdit();
    addressEdit->setPlaceholderText("Enter full address");
    addressEdit->setMaximumHeight(80);
    addressEdit->installEventFilter(this);

    // Add fields to form
    formLayout->addRow("📝 Full Name *", nameEdit);
    formLayout->addRow("📧 Email Address *", emailEdit);
    formLayout->addRow("🏙️ City", cityEdit);
    formLayout->addRow("📮 Postal Code", postalEdit);
    formLayout->addRow("🏠 Address", addressEdit);

    formContainer->addLayout(formLayout);

    // Validation label
    validationLabel = new QLabel();
    validationLabel->setObjectName("validationLabel");
    validationLabel->setWordWrap(true);
    validationLabel->hide();

    formContainer->addWidget(validationLabel);
    formContainer->addStretch();

    mainLayout->addWidget(formFrame);

    // Button frame
    buttonFrame = new QFrame();
    buttonFrame->setFixedHeight(80);
    buttonFrame->setObjectName("buttonFrame");

    buttonLayout = new QHBoxLayout(buttonFrame);
    buttonLayout->setContentsMargins(30, 20, 30, 20);
    buttonLayout->setSpacing(15);

    clearButton = new QPushButton("🗑️ Clear");
    cancelButton = new QPushButton("❌ Cancel");
    saveButton = new QPushButton("💾 Save");

    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);

    mainLayout->addWidget(buttonFrame);

    // Set tab order
    setTabOrder(nameEdit, emailEdit);
    setTabOrder(emailEdit, cityEdit);
    setTabOrder(cityEdit, postalEdit);
    setTabOrder(postalEdit, addressEdit);
    setTabOrder(addressEdit, saveButton);
    setTabOrder(saveButton, cancelButton);
    setTabOrder(cancelButton, clearButton);
}

void ClientsWindow::setupValidators()
{
    // Email validator
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    emailValidator = new QRegularExpressionValidator(emailRegex, this);
    emailEdit->setValidator(emailValidator);

    // Name validator (letters, spaces, some punctuation)
    QRegularExpression nameRegex("^[a-zA-Z\\s\\-\\.,']{2,100}$");
    nameValidator = new QRegularExpressionValidator(nameRegex, this);
    nameEdit->setValidator(nameValidator);

    // Postal validator (alphanumeric, spaces, dashes)
    QRegularExpression postalRegex("^[a-zA-Z0-9\\s\\-]{3,20}$");
    postalValidator = new QRegularExpressionValidator(postalRegex, this);
    postalEdit->setValidator(postalValidator);
}

void ClientsWindow::setupConnections()
{
    // Button connections - using lambdas since no Q_OBJECT
    connect(saveButton, &QPushButton::clicked, [this]() { onSaveClicked(); });
    connect(cancelButton, &QPushButton::clicked, [this]() { onCancelClicked(); });
    connect(clearButton, &QPushButton::clicked, [this]() { onClearClicked(); });

    // Validation connections - using lambdas
    connect(nameEdit, &QLineEdit::textChanged, [this]() { validateInput(); });
    connect(emailEdit, &QLineEdit::textChanged, [this]() { validateInput(); });
    connect(cityEdit, &QLineEdit::textChanged, [this]() { validateInput(); });
    connect(postalEdit, &QLineEdit::textChanged, [this]() { validateInput(); });
    connect(addressEdit, &QTextEdit::textChanged, [this]() { validateInput(); });

    // Validation timer for delayed validation
    validationTimer->setSingleShot(true);
    validationTimer->setInterval(500);
    connect(validationTimer, &QTimer::timeout, [this]() {
        validateAllFields();
    });
}

void ClientsWindow::applyModernStyling()
{
    setStyleSheet(
        "QDialog { "
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "stop:0 rgba(240, 242, 247, 1), stop:1 rgba(255, 255, 255, 1)); "
        "} "

        "QFrame#headerFrame { "
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "stop:0 rgba(102, 126, 234, 1), stop:1 rgba(118, 75, 162, 1)); "
        "border: none; "
        "} "

        "QLabel#titleLabel { "
        "color: white; "
        "font-size: 22px; "
        "font-weight: 700; "
        "} "

        "QLabel#subtitleLabel { "
        "color: rgba(255, 255, 255, 0.8); "
        "font-size: 14px; "
        "font-weight: 400; "
        "} "

        "QFrame#formFrame { "
        "background: white; "
        "border: none; "
        "} "

        "QFormLayout QLabel { "
        "color: #4a5568; "
        "font-size: 14px; "
        "font-weight: 600; "
        "} "

        "QFrame#buttonFrame { "
        "background: rgba(247, 250, 252, 0.8); "
        "border-top: 1px solid #e2e8f0; "
        "} "

        "QLabel#validationLabel { "
        "color: #f56565; "
        "font-size: 12px; "
        "font-weight: 500; "
        "background: #fed7d7; "
        "border: 1px solid #feb2b2; "
        "border-radius: 6px; "
        "padding: 8px 12px; "
        "margin: 10px 0px; "
        "}"
        );

    // Apply input styles
    QString inputStyle = VALID_STYLE;
    nameEdit->setStyleSheet(inputStyle);
    emailEdit->setStyleSheet(inputStyle);
    cityEdit->setStyleSheet(inputStyle);
    postalEdit->setStyleSheet(inputStyle);
    addressEdit->setStyleSheet(inputStyle);

    // Apply button styles
    saveButton->setStyleSheet(BUTTON_PRIMARY_STYLE);
    cancelButton->setStyleSheet(BUTTON_SECONDARY_STYLE);
    clearButton->setStyleSheet(BUTTON_DANGER_STYLE);
}

void ClientsWindow::updateWindowTitle()
{
    switch (currentMode) {
    case AddMode:
        setWindowTitle("Add New Client");
        titleLabel->setText("➕ Add New Client");
        subtitleLabel->setText("Enter client information below");
        break;
    case EditMode:
        setWindowTitle("Edit Client");
        titleLabel->setText("✏️ Edit Client");
        subtitleLabel->setText("Update client information");
        break;
    case ViewMode:
        setWindowTitle("View Client");
        titleLabel->setText("👁️ View Client");
        subtitleLabel->setText("Client information (read-only)");
        break;
    }
}

void ClientsWindow::setMode(Mode mode)
{
    currentMode = mode;
    updateWindowTitle();

    bool isReadOnly = (mode == ViewMode);
    bool showSave = (mode != ViewMode);

    nameEdit->setReadOnly(isReadOnly);
    emailEdit->setReadOnly(isReadOnly);
    cityEdit->setReadOnly(isReadOnly);
    postalEdit->setReadOnly(isReadOnly);
    addressEdit->setReadOnly(isReadOnly);

    saveButton->setVisible(showSave);
    clearButton->setVisible(showSave);

    if (isReadOnly) {
        saveButton->setText("📋 Close");
        saveButton->setVisible(true);
        clearButton->setVisible(false);
    } else {
        saveButton->setText((mode == EditMode) ? "💾 Update" : "💾 Save");
    }
}

void ClientsWindow::populateFields()
{
    nameEdit->setText(currentClient.name);
    emailEdit->setText(currentClient.email);
    cityEdit->setText(currentClient.city);
    postalEdit->setText(currentClient.postal);
    addressEdit->setPlainText(currentClient.address);
}

void ClientsWindow::clearFields()
{
    nameEdit->clear();
    emailEdit->clear();
    cityEdit->clear();
    postalEdit->clear();
    addressEdit->clear();

    // Reset validation state
    validationLabel->hide();
    nameEdit->setStyleSheet(VALID_STYLE);
    emailEdit->setStyleSheet(VALID_STYLE);
    cityEdit->setStyleSheet(VALID_STYLE);
    postalEdit->setStyleSheet(VALID_STYLE);
    addressEdit->setStyleSheet(VALID_STYLE);

    nameEdit->setFocus();
}

void ClientsWindow::setClient(const Client& client)
{
    currentClient = client;
    populateFields();
}

Client ClientsWindow::getClient() const
{
    Client client = currentClient;
    client.name = nameEdit->text().trimmed();
    client.email = emailEdit->text().trimmed().toLower();
    client.city = cityEdit->text().trimmed();
    client.postal = postalEdit->text().trimmed();
    client.address = addressEdit->toPlainText().trimmed();

    return client;
}

bool ClientsWindow::isValidEmail(const QString& email)
{
    int pos = 0;
    QString emailCopy = email;
    QValidator::State state = emailValidator->validate(emailCopy, pos);
    return (state == QValidator::Acceptable);
}

bool ClientsWindow::validateAllFields()
{
    bool isValid = true;
    QStringList errors;

    // Validate name
    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) {
        nameEdit->setStyleSheet(INVALID_STYLE);
        errors << "Name is required";
        isValid = false;
    } else if (name.length() < 2) {
        nameEdit->setStyleSheet(INVALID_STYLE);
        errors << "Name must be at least 2 characters";
        isValid = false;
    } else {
        nameEdit->setStyleSheet(VALID_STYLE);
    }

    // Validate email - FIXED VERSION
    QString email = emailEdit->text().trimmed();
    if (email.isEmpty()) {
        emailEdit->setStyleSheet(INVALID_STYLE);
        errors << "Email is required";
        isValid = false;
    } else if (!isValidEmail(email)) {
        emailEdit->setStyleSheet(INVALID_STYLE);
        errors << "Please enter a valid email address";
        isValid = false;
    } else {
        emailEdit->setStyleSheet(VALID_STYLE);
    }

    // Optional fields styling
    cityEdit->setStyleSheet(VALID_STYLE);
    postalEdit->setStyleSheet(VALID_STYLE);
    addressEdit->setStyleSheet(VALID_STYLE);

    // Show validation errors
    if (!errors.isEmpty()) {
        showValidationError(errors.join("\n"));
    } else {
        validationLabel->hide();
    }

    saveButton->setEnabled(isValid);
    return isValid;
}

void ClientsWindow::showValidationError(const QString &message)
{
    validationLabel->setText(message);
    validationLabel->show();

    // Animate validation label
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    validationLabel->setGraphicsEffect(effect);

    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(300);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void ClientsWindow::animateValidationError(QWidget *widget)
{
    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(500);

    QRect original = widget->geometry();
    QRect shakeLeft = original.translated(-5, 0);
    QRect shakeRight = original.translated(5, 0);

    animation->setKeyValueAt(0, original);
    animation->setKeyValueAt(0.2, shakeLeft);
    animation->setKeyValueAt(0.4, shakeRight);
    animation->setKeyValueAt(0.6, shakeLeft);
    animation->setKeyValueAt(0.8, shakeRight);
    animation->setKeyValueAt(1, original);

    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void ClientsWindow::validateInput()
{
    validationTimer->start(); // Restart the timer
}

void ClientsWindow::onSaveClicked()
{
    if (currentMode == ViewMode) {
        accept();
        return;
    }

    if (!validateAllFields()) {
        animateValidationError(formFrame);
        return;
    }

    Client client = getClient();
    QString errorMessage;

    bool success = false;
    if (currentMode == AddMode) {
        success = clientManager->addNewClient(client);
    } else if (currentMode == EditMode) {
        success = clientManager->modifyClient(client);
    }

    if (success) {
        QMessageBox::information(this, "Success",
                                 currentMode == AddMode ? "Client added successfully!" : "Client updated successfully!");
        currentClient = client;
        accept();
    } else {
        QMessageBox::critical(this, "Error",
                              QString("Failed to %1 client. Please try again.").arg(currentMode == AddMode ? "add" : "update"));
    }
}

void ClientsWindow::onCancelClicked()
{
    // Check if there are unsaved changes
    if (currentMode != ViewMode) {
        Client formClient = getClient();
        bool hasChanges = (formClient.name != currentClient.name ||
                           formClient.email != currentClient.email ||
                           formClient.city != currentClient.city ||
                           formClient.postal != currentClient.postal ||
                           formClient.address != currentClient.address);

        if (hasChanges) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Unsaved Changes",
                                          "You have unsaved changes. Are you sure you want to cancel?",
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply != QMessageBox::Yes) {
                return;
            }
        }
    }

    reject();
}

void ClientsWindow::onClearClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Form",
                                  "Are you sure you want to clear all fields?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        clearFields();
    }
}

void ClientsWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (event->modifiers() & Qt::ControlModifier) {
            onSaveClicked();
        } else {
            // Move to next field
            QWidget::keyPressEvent(event);
        }
    } else if (event->key() == Qt::Key_Escape) {
        onCancelClicked();
    } else {
        QDialog::keyPressEvent(event);
    }
}

void ClientsWindow::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (!event->spontaneous()) {
        animateShow();
        nameEdit->setFocus();
    }
}

void ClientsWindow::animateShow()
{
    // Fade in animation
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    setGraphicsEffect(effect);

    fadeAnimation = new QPropertyAnimation(effect, "opacity");
    fadeAnimation->setDuration(300);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    fadeAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}

bool ClientsWindow::eventFilter(QObject *watched, QEvent *event)
{
    // Handle focus events for better user experience
    if (event->type() == QEvent::FocusIn) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(watched);
        QTextEdit *textEdit = qobject_cast<QTextEdit*>(watched);

        if (lineEdit || textEdit) {
            // Clear validation error when user starts typing
            validationLabel->hide();
        }
    }

    return QDialog::eventFilter(watched, event);
}
