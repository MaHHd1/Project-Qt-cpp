/********************************************************************************
** Form generated from reading UI file 'chatbotdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATBOTDIALOG_H
#define UI_CHATBOTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ChatbotDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTextEdit *chatHistory;
    QLabel *suggestionLabel;
    QListView *suggestionListView;
    QHBoxLayout *inputLayout;
    QLineEdit *inputLineEdit;
    QPushButton *sendButton;

    void setupUi(QDialog *ChatbotDialog)
    {
        if (ChatbotDialog->objectName().isEmpty())
            ChatbotDialog->setObjectName("ChatbotDialog");
        ChatbotDialog->resize(600, 700);
        ChatbotDialog->setStyleSheet(QString::fromUtf8("QDialog {\n"
"    background: #f8fafc;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"QTextEdit {\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 8px;\n"
"    padding: 10px;\n"
"    background: white;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QLineEdit {\n"
"    padding: 10px 14px;\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 8px;\n"
"    font-size: 14px;\n"
"    background: white;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border-color: #667eea;\n"
"}\n"
"\n"
"QPushButton {\n"
"    padding: 10px 16px;\n"
"    border-radius: 8px;\n"
"    font-weight: 500;\n"
"    background: #667eea;\n"
"    color: white;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: #5a67d8;\n"
"}\n"
"\n"
"QListView {\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 8px;\n"
"    background: white;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QListView::item {\n"
"    padding: 8px;\n"
"    border-bottom: 1px solid #f1f1f1;\n"
"}\n"
"\n"
"QListView::item:hover {\n"
"    b"
                        "ackground: #edf2f7;\n"
"}\n"
"\n"
"QListView::item:selected {\n"
"    background: #667eea;\n"
"    color: white;\n"
"}"));
        verticalLayout = new QVBoxLayout(ChatbotDialog);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        chatHistory = new QTextEdit(ChatbotDialog);
        chatHistory->setObjectName("chatHistory");
        chatHistory->setMaximumSize(QSize(16777215, 400));
        chatHistory->setReadOnly(true);

        verticalLayout->addWidget(chatHistory);

        suggestionLabel = new QLabel(ChatbotDialog);
        suggestionLabel->setObjectName("suggestionLabel");
        suggestionLabel->setStyleSheet(QString::fromUtf8("font-weight: bold;\n"
"color: #4a5568;"));

        verticalLayout->addWidget(suggestionLabel);

        suggestionListView = new QListView(ChatbotDialog);
        suggestionListView->setObjectName("suggestionListView");
        suggestionListView->setMaximumSize(QSize(16777215, 100));

        verticalLayout->addWidget(suggestionListView);

        inputLayout = new QHBoxLayout();
        inputLayout->setSpacing(10);
        inputLayout->setObjectName("inputLayout");
        inputLineEdit = new QLineEdit(ChatbotDialog);
        inputLineEdit->setObjectName("inputLineEdit");

        inputLayout->addWidget(inputLineEdit);

        sendButton = new QPushButton(ChatbotDialog);
        sendButton->setObjectName("sendButton");

        inputLayout->addWidget(sendButton);


        verticalLayout->addLayout(inputLayout);


        retranslateUi(ChatbotDialog);

        QMetaObject::connectSlotsByName(ChatbotDialog);
    } // setupUi

    void retranslateUi(QDialog *ChatbotDialog)
    {
        ChatbotDialog->setWindowTitle(QCoreApplication::translate("ChatbotDialog", "Client Management Chatbot", nullptr));
        suggestionLabel->setText(QCoreApplication::translate("ChatbotDialog", "Suggestions:", nullptr));
        inputLineEdit->setPlaceholderText(QCoreApplication::translate("ChatbotDialog", "Type your question here...", nullptr));
        sendButton->setText(QCoreApplication::translate("ChatbotDialog", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatbotDialog: public Ui_ChatbotDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATBOTDIALOG_H
