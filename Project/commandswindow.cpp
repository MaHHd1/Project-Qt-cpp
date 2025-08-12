#include "commandswindow.h"
#include <QCloseEvent>
#include <QShowEvent>
#include <QApplication>
// Remove QDesktopWidget include as it's deprecated
#include <QScreen>
#include <QHeaderView>
#include <QSplitter>
#include <QSpacerItem>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QInputDialog>

CommandsWindow::CommandsWindow(CommandManager* commandManager,
                               ClientManager* clientManager,
                               QWidget *parent)
    : QDialog(parent)
    , mainLayout(nullptr)
    , formLayout(nullptr)
    , buttonLayout(nullptr)
    , clientSelectionLayout(nullptr)
    , commandManager(commandManager)
    , clientManager(clientManager)
    , currentMode(AddMode)
    , isModified(false)
    , validationTimer(nullptr)
    , clientCompleter(nullptr)
    , clientModel(nullptr)
    , showAnimation(nullptr)
    , opacityEffect(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    setupUI();
    loadClientData();
    loadPaymentMethods();
    clearForm();

    // Set default values for new command
    commandDateEdit->setDateTime(QDateTime::currentDateTime());
    currentCommand = Command();

    updateWindowTitle();
    updateButtonStates();
}

CommandsWindow::CommandsWindow(CommandManager* commandManager,
                               ClientManager* clientManager,
                               const Command& command,
                               Mode mode,
                               QWidget *parent)
    : QDialog(parent)
    , mainLayout(nullptr)
    , formLayout(nullptr)
    , buttonLayout(nullptr)
    , clientSelectionLayout(nullptr)
    , commandManager(commandManager)
    , clientManager(clientManager)
    , currentCommand(command)
    , currentMode(mode)
    , isModified(false)
    , validationTimer(nullptr)
    , clientCompleter(nullptr)
    , clientModel(nullptr)
    , showAnimation(nullptr)
    , opacityEffect(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    setupUI();
    loadClientData();
    loadPaymentMethods();
    setCommand(command);
    setMode(mode);

    updateWindowTitle();
    updateButtonStates();
}

CommandsWindow::~CommandsWindow()
{
    if (validationTimer) {
        validationTimer->stop();
        delete validationTimer;
    }

    if (showAnimation) {
        delete showAnimation;
    }

    if (opacityEffect) {
        delete opacityEffect;
    }

    if (clientCompleter) {
        delete clientCompleter;
    }

    if (clientModel) {
        delete clientModel;
    }
}

void CommandsWindow::setupUI()
{
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Center on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    setupFormLayout();
    setupButtonLayout();
    applyModernStyling();
    connectSignals();

    // Setup validation timer
    validationTimer = new QTimer(this);
    validationTimer->setSingleShot(true);
    validationTimer->setInterval(VALIDATION_DELAY);

    // Setup animations
    opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0.0);
}


void CommandsWindow::setupFormLayout()
{
    // Command Information Group
    commandInfoGroup = new QGroupBox("Command Information");
    commandInfoGroup->setObjectName("formGroup");
    QFormLayout *commandFormLayout = new QFormLayout(commandInfoGroup);
    commandFormLayout->setSpacing(15);
    commandFormLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // Command ID (read-only for edit mode)
    commandIdLabel = new QLabel("Command ID:");
    commandIdEdit = new QLineEdit();
    commandIdEdit->setReadOnly(true);
    commandIdEdit->setObjectName("readOnlyField");
    commandFormLayout->addRow(commandIdLabel, commandIdEdit);

    // Command Date
    commandDateLabel = new QLabel("Command Date:");
    commandDateEdit = new QDateTimeEdit();
    commandDateEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    commandDateEdit->setCalendarPopup(true);
    commandDateEdit->setObjectName("inputField");
    commandFormLayout->addRow(commandDateLabel, commandDateEdit);

    mainLayout->addWidget(commandInfoGroup);

    // Client Information Group
    clientInfoGroup = new QGroupBox("Client Information");
    clientInfoGroup->setObjectName("formGroup");
    QVBoxLayout *clientGroupLayout = new QVBoxLayout(clientInfoGroup);
    clientGroupLayout->setSpacing(10);

    // Client selection layout
    clientSelectionLayout = new QHBoxLayout();

    clientLabel = new QLabel("Client:");
    clientCombo = new QComboBox();
    clientCombo->setObjectName("inputField");
    clientCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    selectClientBtn = new QPushButton("Select");
    selectClientBtn->setObjectName("secondaryBtn");
    selectClientBtn->setFixedWidth(80);

    newClientBtn = new QPushButton("New");
    newClientBtn->setObjectName("secondaryBtn");
    newClientBtn->setFixedWidth(70);

    clientSelectionLayout->addWidget(clientLabel);
    clientSelectionLayout->addWidget(clientCombo, 1);
    clientSelectionLayout->addWidget(selectClientBtn);
    clientSelectionLayout->addWidget(newClientBtn);

    clientGroupLayout->addLayout(clientSelectionLayout);

    // Client search
    clientSearchEdit = new QLineEdit();
    clientSearchEdit->setPlaceholderText("Search clients by name or email...");
    clientSearchEdit->setObjectName("searchField");
    clientGroupLayout->addWidget(clientSearchEdit);

    // Client display info
    QFrame *clientDisplayFrame = new QFrame();
    clientDisplayFrame->setObjectName("clientDisplayFrame");
    clientDisplayFrame->setFrameStyle(QFrame::StyledPanel);
    QFormLayout *clientDisplayLayout = new QFormLayout(clientDisplayFrame);

    clientNameDisplay = new QLabel("Not selected");
    clientNameDisplay->setObjectName("displayLabel");
    clientEmailDisplay = new QLabel("Not selected");
    clientEmailDisplay->setObjectName("displayLabel");
    clientAddressDisplay = new QLabel("Not selected");
    clientAddressDisplay->setObjectName("displayLabel");
    clientAddressDisplay->setWordWrap(true);

    clientDisplayLayout->addRow("Name:", clientNameDisplay);
    clientDisplayLayout->addRow("Email:", clientEmailDisplay);
    clientDisplayLayout->addRow("Address:", clientAddressDisplay);

    clientGroupLayout->addWidget(clientDisplayFrame);
    mainLayout->addWidget(clientInfoGroup);

    // Order Details Group
    QGroupBox *orderDetailsGroup = new QGroupBox("Order Details");
    orderDetailsGroup->setObjectName("formGroup");
    QFormLayout *orderFormLayout = new QFormLayout(orderDetailsGroup);
    orderFormLayout->setSpacing(15);

    // Total amount
    QHBoxLayout *totalLayout = new QHBoxLayout();
    totalLabel = new QLabel("Total Amount:");
    totalEdit = new QLineEdit();
    totalEdit->setObjectName("inputField");
    totalEdit->setPlaceholderText("0.00");

    // Add currency validator
    QRegularExpression totalRegex("^\\d+(\\.\\d{0,2})?$");
    QRegularExpressionValidator *totalValidator = new QRegularExpressionValidator(totalRegex, this);
    totalEdit->setValidator(totalValidator);

    calculateBtn = new QPushButton("Calculate");
    calculateBtn->setObjectName("secondaryBtn");
    calculateBtn->setFixedWidth(90);

    totalLayout->addWidget(totalEdit, 1);
    totalLayout->addWidget(calculateBtn);
    orderFormLayout->addRow(totalLabel, totalLayout);

    // Payment Method
    paymentMethodLabel = new QLabel("Payment Method:");
    paymentMethodCombo = new QComboBox();
    paymentMethodCombo->setObjectName("inputField");
    paymentMethodCombo->setEditable(true);
    orderFormLayout->addRow(paymentMethodLabel, paymentMethodCombo);

    mainLayout->addWidget(orderDetailsGroup);

    // Delivery Information Group
    deliveryInfoGroup = new QGroupBox("Delivery Information");
    deliveryInfoGroup->setObjectName("formGroup");
    QFormLayout *deliveryFormLayout = new QFormLayout(deliveryInfoGroup);
    deliveryFormLayout->setSpacing(15);

    // Delivery Address
    deliveryAddressLabel = new QLabel("Delivery Address:");
    deliveryAddressEdit = new QTextEdit();
    deliveryAddressEdit->setObjectName("inputField");
    deliveryAddressEdit->setMaximumHeight(80);
    deliveryAddressEdit->setPlaceholderText("Enter full delivery address...");
    deliveryFormLayout->addRow(deliveryAddressLabel, deliveryAddressEdit);

    // Notes (optional)
    notesLabel = new QLabel("Notes (Optional):");
    notesEdit = new QTextEdit();
    notesEdit->setObjectName("inputField");
    notesEdit->setMaximumHeight(60);
    notesEdit->setPlaceholderText("Additional notes or comments...");
    deliveryFormLayout->addRow(notesLabel, notesEdit);

    mainLayout->addWidget(deliveryInfoGroup);

    // Validation frame
    validationFrame = new QFrame();
    validationFrame->setObjectName("validationFrame");
    validationFrame->setVisible(false);
    QHBoxLayout *validationLayout = new QHBoxLayout(validationFrame);

    validationLabel = new QLabel();
    validationLabel->setObjectName("validationLabel");
    validationLabel->setWordWrap(true);
    validationLayout->addWidget(validationLabel);

    mainLayout->addWidget(validationFrame);

    // Status label
    statusLabel = new QLabel("Ready");
    statusLabel->setObjectName("statusLabel");
    mainLayout->addWidget(statusLabel);
}

void CommandsWindow::setupButtonLayout()
{
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    // Add spacer to push buttons to the right
    buttonLayout->addStretch();

    // Clear button
    clearBtn = new QPushButton("Clear");
    clearBtn->setObjectName("clearBtn");
    clearBtn->setFixedSize(100, 40);
    buttonLayout->addWidget(clearBtn);

    // Cancel button
    cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("cancelBtn");
    cancelBtn->setFixedSize(100, 40);
    buttonLayout->addWidget(cancelBtn);

    // Save button
    saveBtn = new QPushButton("Save");
    saveBtn->setObjectName("saveBtn");
    saveBtn->setFixedSize(100, 40);
    saveBtn->setDefault(true);
    buttonLayout->addWidget(saveBtn);

    mainLayout->addLayout(buttonLayout);
}

void CommandsWindow::applyModernStyling()
{
    setStyleSheet(R"(
        QDialog {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(240, 242, 247, 1), stop:1 rgba(255, 255, 255, 1));
        }

        QGroupBox#formGroup {
            font-weight: 600;
            font-size: 14px;
            color: #2d3748;
            border: 2px solid #e2e8f0;
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
            background: rgba(255, 255, 255, 0.8);
        }

        QGroupBox#formGroup::title {
            subcontrol-origin: margin;
            left: 20px;
            padding: 5px 10px;
            background: white;
            border-radius: 5px;
            color: #667eea;
        }

        QLineEdit#inputField, QComboBox#inputField, QTextEdit#inputField {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
            background: white;
            selection-background-color: rgba(102, 126, 234, 0.3);
        }

        QLineEdit#inputField:focus, QComboBox#inputField:focus, QTextEdit#inputField:focus {
            border-color: #667eea;
            background: #fafbff;
        }

        QLineEdit#searchField {
            border: 2px solid #cbd5e0;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
            background: #f7fafc;
            font-style: italic;
        }

        QLineEdit#searchField:focus {
            border-color: #667eea;
            background: white;
            font-style: normal;
        }

        QLineEdit#readOnlyField {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
            background: #f8f9fa;
            color: #718096;
        }

        QFrame#clientDisplayFrame {
            background: #f8f9fa;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 10px;
        }

        QLabel#displayLabel {
            color: #4a5568;
            font-size: 13px;
            padding: 2px 0;
        }

        QPushButton#saveBtn {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(72, 187, 120, 1), stop:1 rgba(56, 178, 172, 1));
            color: white;
            border: none;
            border-radius: 8px;
            font-weight: 600;
            font-size: 13px;
        }

        QPushButton#saveBtn:hover {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(64, 170, 108, 1), stop:1 rgba(50, 160, 155, 1));
        }

        QPushButton#saveBtn:pressed {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(56, 150, 95, 1), stop:1 rgba(44, 140, 140, 1));
        }

        QPushButton#cancelBtn {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(245, 101, 101, 1), stop:1 rgba(237, 137, 54, 1));
            color: white;
            border: none;
            border-radius: 8px;
            font-weight: 600;
            font-size: 13px;
        }

        QPushButton#cancelBtn:hover {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(220, 90, 90, 1), stop:1 rgba(215, 120, 45, 1));
        }

        QPushButton#clearBtn {
            background: transparent;
            color: #718096;
            border: 2px solid #cbd5e0;
            border-radius: 8px;
            font-weight: 600;
            font-size: 13px;
        }

        QPushButton#clearBtn:hover {
            background: #f7fafc;
            border-color: #a0aec0;
            color: #4a5568;
        }

        QPushButton#secondaryBtn {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(102, 126, 234, 1), stop:1 rgba(118, 75, 162, 1));
            color: white;
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            font-weight: 500;
            font-size: 12px;
        }

        QPushButton#secondaryBtn:hover {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                       stop:0 rgba(90, 110, 220, 1), stop:1 rgba(100, 65, 150, 1));
        }

        QLabel#statusLabel {
            color: #718096;
            font-size: 12px;
            font-style: italic;
            padding: 5px 0;
        }

        QFrame#validationFrame {
            background: rgba(245, 101, 101, 0.1);
            border: 1px solid rgba(245, 101, 101, 0.3);
            border-radius: 8px;
            padding: 10px;
        }

        QLabel#validationLabel {
            color: #e53e3e;
            font-size: 13px;
            font-weight: 500;
        }

        QDateTimeEdit {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
            background: white;
        }

        QDateTimeEdit:focus {
            border-color: #667eea;
            background: #fafbff;
        }

        QComboBox::drop-down {
            border: none;
            width: 20px;
        }

        QComboBox::down-arrow {
            image: none;
            border: none;
            width: 12px;
            height: 12px;
            background: #cbd5e0;
            border-radius: 6px;
        }
    )");
}

void CommandsWindow::connectSignals()
{
    // Form field signals
    connect(clientCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandsWindow::onClientChanged);
    connect(totalEdit, &QLineEdit::textChanged, this, &CommandsWindow::onTotalChanged);
    connect(paymentMethodCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &CommandsWindow::onPaymentMethodChanged);

    // Button signals
    connect(saveBtn, &QPushButton::clicked, this, &CommandsWindow::onSaveClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &CommandsWindow::onCancelClicked);
    connect(clearBtn, &QPushButton::clicked, this, &CommandsWindow::clearForm);
    connect(selectClientBtn, &QPushButton::clicked, this, &CommandsWindow::onSelectClientClicked);
    connect(newClientBtn, &QPushButton::clicked, this, &CommandsWindow::onNewClientClicked);
    connect(calculateBtn, &QPushButton::clicked, this, &CommandsWindow::onCalculateTotal);

    // Search and validation signals
    connect(clientSearchEdit, &QLineEdit::textChanged, this, &CommandsWindow::onClientSearch);
    connect(validationTimer, &QTimer::timeout, this, &CommandsWindow::onValidationTimer);

    // Field modification tracking
    connect(commandDateEdit, &QDateTimeEdit::dateTimeChanged, [this]() { isModified = true; });
    connect(totalEdit, &QLineEdit::textChanged, [this]() { isModified = true; });
    connect(deliveryAddressEdit, &QTextEdit::textChanged, [this]() { isModified = true; });
    connect(notesEdit, &QTextEdit::textChanged, [this]() { isModified = true; });
}

void CommandsWindow::loadClientData()
{
    if (!clientManager) return;

    availableClients = clientManager->getAllClients();

    clientCombo->clear();
    clientCombo->addItem("-- Select Client --", -1);

    QStringList clientNames;
    for (const Client& client : availableClients) {
        QString displayText = QString("%1 - %2").arg(client.name, client.email);
        clientCombo->addItem(displayText, client.id);
        clientNames << displayText;
    }

    // Setup completer for client search
    if (!clientModel) {
        clientModel = new QStringListModel(this);
    }
    clientModel->setStringList(clientNames);

    if (!clientCompleter) {
        clientCompleter = new QCompleter(this);
        clientSearchEdit->setCompleter(clientCompleter);
    }
    clientCompleter->setModel(clientModel);
    clientCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    clientCompleter->setFilterMode(Qt::MatchContains);
}

void CommandsWindow::loadPaymentMethods()
{
    if (!commandManager) return;

    paymentMethodCombo->clear();

    // Add standard payment methods (assuming DAO method doesn't exist or fails)
    QStringList standardMethods = {"Cash", "Credit Card", "Debit Card", "PayPal",
                                   "Bank Transfer", "Check", "Mobile Payment"};

    paymentMethodCombo->addItems(standardMethods);
    paymentMethodCombo->setCurrentText("Cash");
}

void CommandsWindow::populateForm()
{
    if (currentCommand.commandId > 0) {
        commandIdEdit->setText(QString::number(currentCommand.commandId));
    } else {
        commandIdEdit->setText("New Command");
    }

    commandDateEdit->setDateTime(currentCommand.commandDate);
    totalEdit->setText(currentCommand.total);
    paymentMethodCombo->setCurrentText(currentCommand.paymentMethod);
    deliveryAddressEdit->setPlainText(currentCommand.deliveryAddress);

    // Handle notes field - comment out if Command doesn't have notes field
    // if (notesEdit) {
    //     notesEdit->setPlainText(currentCommand.notes);
    // }

    // Set client selection
    if (currentCommand.clientId > 0) {
        for (int i = 0; i < clientCombo->count(); ++i) {
            if (clientCombo->itemData(i).toInt() == currentCommand.clientId) {
                clientCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    updateClientInfo();
    isModified = false;
}

void CommandsWindow::clearForm()
{
    commandIdEdit->clear();
    commandDateEdit->setDateTime(QDateTime::currentDateTime());
    totalEdit->clear();
    paymentMethodCombo->setCurrentIndex(0);
    deliveryAddressEdit->clear();
    notesEdit->clear();
    clientCombo->setCurrentIndex(0);
    clientSearchEdit->clear();

    updateClientInfo();
    clearValidationErrors();

    isModified = false;
    statusLabel->setText("Form cleared");
}

void CommandsWindow::updateClientInfo()
{
    int clientId = clientCombo->currentData().toInt();

    if (clientId <= 0) {
        clientNameDisplay->setText("Not selected");
        clientEmailDisplay->setText("Not selected");
        clientAddressDisplay->setText("Not selected");
        return;
    }

    // Find client in available clients
    for (const Client& client : availableClients) {
        if (client.id == clientId) {
            clientNameDisplay->setText(client.name);
            clientEmailDisplay->setText(client.email);
            clientAddressDisplay->setText(QString("%1, %2 %3").arg(client.address, client.city, client.postal));

            // Auto-fill delivery address if empty
            if (deliveryAddressEdit->toPlainText().isEmpty()) {
                deliveryAddressEdit->setPlainText(QString("%1\n%2, %3 %4").arg(
                    client.name, client.address, client.city, client.postal));
                isModified = true;
            }
            break;
        }
    }
}

bool CommandsWindow::validateForm()
{
    validationErrors.clear();

    // Validate client selection
    int clientId = clientCombo->currentData().toInt();
    if (clientId <= 0) {
        validationErrors << "Please select a client";
        setFieldError(clientCombo, true);
    } else {
        setFieldError(clientCombo, false);
    }

    // Validate total amount
    QString totalText = totalEdit->text().trimmed();
    if (totalText.isEmpty()) {
        validationErrors << "Please enter a total amount";
        setFieldError(totalEdit, true);
    } else {
        bool ok;
        double total = totalText.toDouble(&ok);
        if (!ok || total <= 0) {
            validationErrors << "Please enter a valid total amount greater than 0";
            setFieldError(totalEdit, true);
        } else {
            setFieldError(totalEdit, false);
        }
    }

    // Validate payment method
    QString paymentMethod = paymentMethodCombo->currentText().trimmed();
    if (paymentMethod.isEmpty()) {
        validationErrors << "Please select a payment method";
        setFieldError(paymentMethodCombo, true);
    } else {
        setFieldError(paymentMethodCombo, false);
    }

    // Validate delivery address
    QString deliveryAddress = deliveryAddressEdit->toPlainText().trimmed();
    if (deliveryAddress.isEmpty()) {
        validationErrors << "Please enter a delivery address";
        setFieldError(deliveryAddressEdit, true);
    } else {
        setFieldError(deliveryAddressEdit, false);
    }

    if (!validationErrors.isEmpty()) {
        showValidationError(validationErrors.join("\n"));
        return false;
    }

    clearValidationErrors();
    return true;
}

void CommandsWindow::showValidationError(const QString& message)
{
    validationLabel->setText(message);
    validationFrame->setVisible(true);
}

void CommandsWindow::clearValidationErrors()
{
    validationFrame->setVisible(false);
    validationErrors.clear();

    // Clear field error states
    setFieldError(clientCombo, false);
    setFieldError(totalEdit, false);
    setFieldError(paymentMethodCombo, false);
    setFieldError(deliveryAddressEdit, false);
}

void CommandsWindow::setFieldError(QWidget* field, bool hasError)
{
    if (!field) return;

    if (hasError) {
        field->setProperty("hasError", true);
        field->setStyleSheet(field->styleSheet() +
                             "QWidget { border-color: #e53e3e !important; background-color: #fed7d7 !important; }");
    } else {
        field->setProperty("hasError", false);
        // Reset to original style by removing error styling
        QString currentStyle = field->styleSheet();
        currentStyle.remove("QWidget { border-color: #e53e3e !important; background-color: #fed7d7 !important; }");
        field->setStyleSheet(currentStyle);
    }
}

void CommandsWindow::animateShow()
{
    if (!showAnimation) {
        showAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
        showAnimation->setDuration(300);
    }

    showAnimation->setStartValue(0.0);
    showAnimation->setEndValue(1.0);
    showAnimation->start();
}

void CommandsWindow::animateFieldError(QWidget* field)
{
    if (!field) return;

    // Simple error indication without complex geometry animation
    field->setFocus();
}

void CommandsWindow::updateWindowTitle()
{
    QString title;
    switch (currentMode) {
    case AddMode:
        title = "New Command";
        break;
    case EditMode:
        title = QString("Edit Command #%1").arg(currentCommand.commandId);
        break;
    case ViewMode:
        title = QString("View Command #%1").arg(currentCommand.commandId);
        break;
    }

    if (isModified) {
        title += " *";
    }

    setWindowTitle(title);
}

void CommandsWindow::updateButtonStates()
{
    switch (currentMode) {
    case AddMode:
        saveBtn->setText("Save");
        saveBtn->setEnabled(true);
        clearBtn->setVisible(true);
        setFieldsReadOnly(false);
        break;
    case EditMode:
        saveBtn->setText("Update");
        saveBtn->setEnabled(true);
        clearBtn->setVisible(true);
        setFieldsReadOnly(false);
        break;
    case ViewMode:
        saveBtn->setText("Edit");
        saveBtn->setEnabled(true);
        clearBtn->setVisible(false);
        // Make all fields read-only in view mode
        setFieldsReadOnly(true);
        break;
    }
}

void CommandsWindow::setFieldsReadOnly(bool readOnly)
{
    commandDateEdit->setReadOnly(readOnly);
    clientCombo->setEnabled(!readOnly);
    totalEdit->setReadOnly(readOnly);
    paymentMethodCombo->setEnabled(!readOnly);
    deliveryAddressEdit->setReadOnly(readOnly);
    notesEdit->setReadOnly(readOnly);
    clientSearchEdit->setEnabled(!readOnly);
    selectClientBtn->setEnabled(!readOnly);
    newClientBtn->setEnabled(!readOnly);
    calculateBtn->setEnabled(!readOnly);
}

QString CommandsWindow::formatCurrency(double amount) const
{
    return QString::number(amount, 'f', 2);
}

double CommandsWindow::parseCurrency(const QString& text) const
{
    QString cleaned = text;
    cleaned.remove(QRegularExpression("[^0-9.]"));
    return cleaned.toDouble();
}

Command CommandsWindow::getCurrentCommand() const
{
    Command command = currentCommand;

    if (currentMode == AddMode) {
        command.commandId = 0; // Will be set by database
    }

    command.commandDate = commandDateEdit->dateTime();
    command.clientId = clientCombo->currentData().toInt();
    command.total = totalEdit->text();
    command.paymentMethod = paymentMethodCombo->currentText();
    command.deliveryAddress = deliveryAddressEdit->toPlainText();
    // command.notes = notesEdit->toPlainText(); // Uncomment if Command has notes field

    return command;
}

void CommandsWindow::setCommand(const Command& command)
{
    currentCommand = command;
    populateForm();
    isModified = false;
    updateWindowTitle();
}

void CommandsWindow::setMode(Mode mode)
{
    currentMode = mode;
    updateWindowTitle();
    updateButtonStates();
}

void CommandsWindow::closeEvent(QCloseEvent *event)
{
    if (isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Unsaved Changes",
            "You have unsaved changes. Do you want to save them before closing?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        switch (reply) {
        case QMessageBox::Save:
            if (validateForm()) {
                accept();
            } else {
                event->ignore();
                return;
            }
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            event->ignore();
            return;
        default:
            break;
        }
    }

    QDialog::closeEvent(event);
}

void CommandsWindow::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    animateShow();
}

void CommandsWindow::accept()
{
    if (currentMode == ViewMode) {
        setMode(EditMode);
        return;
    }

    if (!validateForm()) {
        QMessageBox::warning(this, "Validation Error",
                             "Please correct the following errors:\n\n" +
                                 validationErrors.join("\n"));
        return;
    }

    statusLabel->setText("Saving...");
    QApplication::processEvents();

    Command command = getCurrentCommand();
    bool success = false;

    try {
        if (currentMode == AddMode) {
            success = commandManager->addNewCommand(command);
            if (success) {
                statusLabel->setText("Command saved successfully");
                QMessageBox::information(this, "Success", "Command saved successfully");
            }
        } else if (currentMode == EditMode) {
            success = commandManager->modifyCommand(command);
            if (success) {
                statusLabel->setText("Command updated successfully");
                QMessageBox::information(this, "Success", "Command updated successfully");
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Failed to save command: %1").arg(e.what()));
        statusLabel->setText("Save failed");
        return;
    } catch (...) {
        QMessageBox::critical(this, "Error", "An unknown error occurred while saving the command");
        statusLabel->setText("Save failed");
        return;
    }

    if (success) {
        isModified = false;
        QDialog::accept();
    } else {
        QMessageBox::warning(this, "Warning",
                             "Failed to save command. Please try again.");
        statusLabel->setText("Save failed");
    }
}


void CommandsWindow::reject()
{
    if (isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Unsaved Changes",
            "You have unsaved changes. Are you sure you want to cancel?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    QDialog::reject();
}

void CommandsWindow::onSaveClicked()
{
    accept();
}

void CommandsWindow::onCancelClicked()
{
    reject();
}

void CommandsWindow::onClientChanged()
{
    updateClientInfo();
    isModified = true;

    // Start validation timer
    if (validationTimer) {
        validationTimer->start();
    }
}

void CommandsWindow::onTotalChanged()
{
    isModified = true;

    // Format the input as currency
    QString text = totalEdit->text();
    if (!text.isEmpty()) {
        double value = parseCurrency(text);
        if (value > 0) {
            statusLabel->setText(QString("Total: %1").arg(formatCurrency(value)));
        }
    }

    // Start validation timer
    if (validationTimer) {
        validationTimer->start();
    }
}

void CommandsWindow::onPaymentMethodChanged()
{
    isModified = true;

    // Start validation timer
    if (validationTimer) {
        validationTimer->start();
    }
}

void CommandsWindow::onValidationTimer()
{
    // Perform live validation without showing errors
    clearValidationErrors();

    // Just check if form is valid for status updates
    bool isValid = true;

    // Check client
    if (clientCombo->currentData().toInt() <= 0) {
        isValid = false;
    }

    // Check total
    QString totalText = totalEdit->text().trimmed();
    if (totalText.isEmpty() || parseCurrency(totalText) <= 0) {
        isValid = false;
    }

    // Check payment method
    if (paymentMethodCombo->currentText().trimmed().isEmpty()) {
        isValid = false;
    }

    // Check delivery address
    if (deliveryAddressEdit->toPlainText().trimmed().isEmpty()) {
        isValid = false;
    }

    if (isValid) {
        statusLabel->setText("Form is complete");
        saveBtn->setEnabled(true);
    } else {
        statusLabel->setText("Please complete all required fields");
        saveBtn->setEnabled(currentMode == ViewMode); // Always enabled in view mode
    }
}

void CommandsWindow::onClientSearch()
{
    QString searchText = clientSearchEdit->text();
    if (searchText.isEmpty()) {
        return;
    }

    // Filter the combo box based on search
    clientCombo->clear();
    clientCombo->addItem("-- Select Client --", -1);

    for (const Client& client : availableClients) {
        QString displayText = QString("%1 - %2").arg(client.name, client.email);
        if (displayText.contains(searchText, Qt::CaseInsensitive)) {
            clientCombo->addItem(displayText, client.id);
        }
    }

    // If only one match found (plus the default item), auto-select it
    if (clientCombo->count() == 2) {
        clientCombo->setCurrentIndex(1);
    }
}

void CommandsWindow::onSelectClientClicked()
{
    // This would open a client selection dialog
    // For now, just show a message
    QMessageBox::information(this, "Client Selection",
                             "Client selection dialog would open here.\n"
                             "This feature requires the client selection dialog to be implemented.");
}

void CommandsWindow::onNewClientClicked()
{
    // This would open a new client dialog
    // For now, just show a message
    QMessageBox::information(this, "New Client",
                             "New client dialog would open here.\n"
                             "This feature requires the client management dialog to be implemented.");
}

void CommandsWindow::onCalculateTotal()
{
    // This would open a calculation dialog or show order items
    // For now, just show a simple input dialog
    bool ok;
    QString currentTotal = totalEdit->text().isEmpty() ? "0.00" : totalEdit->text();
    double total = QInputDialog::getDouble(this, "Calculate Total",
                                           "Enter total amount:",
                                           parseCurrency(currentTotal),
                                           0.01, 999999.99, 2, &ok);
    if (ok) {
        totalEdit->setText(formatCurrency(total));
        isModified = true;
    }
}
