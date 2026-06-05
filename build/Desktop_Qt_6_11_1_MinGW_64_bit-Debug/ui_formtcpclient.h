/********************************************************************************
** Form generated from reading UI file 'formtcpclient.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMTCPCLIENT_H
#define UI_FORMTCPCLIENT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FormTcpClient
{
public:
    QGroupBox *groupBox;
    QListWidget *listWidget_TCPClientMsg;
    QGroupBox *groupBox_2;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *spinBox_TCPClientPort;
    QPushButton *pushButton_TCPClientConnect;
    QPushButton *pushButton_TCPClientDisconnect;
    QPushButton *pushButton_TCPClientQuit;
    QComboBox *comboBox_TCPClientIP;
    QGroupBox *groupBox_3;
    QPlainTextEdit *plainTextEdit_TCPClientSendData;
    QPushButton *pushButton_TCPClientSendMsg;
    QCheckBox *checkBox_TCPClientAutoText;

    void setupUi(QWidget *FormTcpClient)
    {
        if (FormTcpClient->objectName().isEmpty())
            FormTcpClient->setObjectName("FormTcpClient");
        FormTcpClient->resize(860, 560);
        groupBox = new QGroupBox(FormTcpClient);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(0, 10, 611, 541));
        QFont font;
        font.setFamilies({QString::fromUtf8("\344\273\277\345\256\213")});
        font.setPointSize(11);
        groupBox->setFont(font);
        listWidget_TCPClientMsg = new QListWidget(groupBox);
        listWidget_TCPClientMsg->setObjectName("listWidget_TCPClientMsg");
        listWidget_TCPClientMsg->setGeometry(QRect(10, 20, 591, 511));
        listWidget_TCPClientMsg->setFont(font);
        groupBox_2 = new QGroupBox(FormTcpClient);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(620, 10, 211, 341));
        groupBox_2->setFont(font);
        label = new QLabel(groupBox_2);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 121, 19));
        label->setFont(font);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 90, 121, 21));
        label_2->setFont(font);
        spinBox_TCPClientPort = new QSpinBox(groupBox_2);
        spinBox_TCPClientPort->setObjectName("spinBox_TCPClientPort");
        spinBox_TCPClientPort->setGeometry(QRect(20, 120, 161, 31));
        spinBox_TCPClientPort->setFont(font);
        spinBox_TCPClientPort->setMaximum(65535);
        spinBox_TCPClientPort->setValue(12345);
        pushButton_TCPClientConnect = new QPushButton(groupBox_2);
        pushButton_TCPClientConnect->setObjectName("pushButton_TCPClientConnect");
        pushButton_TCPClientConnect->setGeometry(QRect(20, 160, 161, 51));
        pushButton_TCPClientConnect->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/images/connect.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPClientConnect->setIcon(icon);
        pushButton_TCPClientDisconnect = new QPushButton(groupBox_2);
        pushButton_TCPClientDisconnect->setObjectName("pushButton_TCPClientDisconnect");
        pushButton_TCPClientDisconnect->setGeometry(QRect(20, 220, 161, 51));
        pushButton_TCPClientDisconnect->setFont(font);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/prefix1/images/disconnect.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPClientDisconnect->setIcon(icon1);
        pushButton_TCPClientQuit = new QPushButton(groupBox_2);
        pushButton_TCPClientQuit->setObjectName("pushButton_TCPClientQuit");
        pushButton_TCPClientQuit->setGeometry(QRect(20, 280, 161, 51));
        pushButton_TCPClientQuit->setFont(font);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/prefix1/images/exit.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPClientQuit->setIcon(icon2);
        comboBox_TCPClientIP = new QComboBox(groupBox_2);
        comboBox_TCPClientIP->addItem(QString());
        comboBox_TCPClientIP->setObjectName("comboBox_TCPClientIP");
        comboBox_TCPClientIP->setGeometry(QRect(20, 50, 161, 31));
        comboBox_TCPClientIP->setFont(font);
        groupBox_3 = new QGroupBox(FormTcpClient);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(620, 360, 211, 191));
        groupBox_3->setFont(font);
        plainTextEdit_TCPClientSendData = new QPlainTextEdit(groupBox_3);
        plainTextEdit_TCPClientSendData->setObjectName("plainTextEdit_TCPClientSendData");
        plainTextEdit_TCPClientSendData->setGeometry(QRect(10, 20, 191, 91));
        plainTextEdit_TCPClientSendData->setFont(font);
        pushButton_TCPClientSendMsg = new QPushButton(groupBox_3);
        pushButton_TCPClientSendMsg->setObjectName("pushButton_TCPClientSendMsg");
        pushButton_TCPClientSendMsg->setGeometry(QRect(10, 120, 191, 31));
        pushButton_TCPClientSendMsg->setFont(font);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/new/prefix1/images/sendmsg2.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPClientSendMsg->setIcon(icon3);
        checkBox_TCPClientAutoText = new QCheckBox(groupBox_3);
        checkBox_TCPClientAutoText->setObjectName("checkBox_TCPClientAutoText");
        checkBox_TCPClientAutoText->setGeometry(QRect(10, 160, 191, 23));
        checkBox_TCPClientAutoText->setFont(font);

        retranslateUi(FormTcpClient);

        QMetaObject::connectSlotsByName(FormTcpClient);
    } // setupUi

    void retranslateUi(QWidget *FormTcpClient)
    {
        FormTcpClient->setWindowTitle(QCoreApplication::translate("FormTcpClient", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("FormTcpClient", "TCP\345\215\217\350\256\256\345\256\242\346\210\267\347\253\257\346\266\210\346\201\257\345\210\227\350\241\250", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("FormTcpClient", "\347\275\221\347\273\234\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("FormTcpClient", "\350\277\234\347\250\213\344\270\273\346\234\272\345\234\260\345\235\200:", nullptr));
        label_2->setText(QCoreApplication::translate("FormTcpClient", "\350\277\234\347\250\213\344\270\273\346\234\272\347\253\257\345\217\243:", nullptr));
        pushButton_TCPClientConnect->setText(QCoreApplication::translate("FormTcpClient", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        pushButton_TCPClientDisconnect->setText(QCoreApplication::translate("FormTcpClient", "\346\226\255\345\274\200\346\234\215\345\212\241\345\231\250", nullptr));
        pushButton_TCPClientQuit->setText(QCoreApplication::translate("FormTcpClient", "\351\200\200\345\207\272\347\250\213\345\272\217", nullptr));
        comboBox_TCPClientIP->setItemText(0, QCoreApplication::translate("FormTcpClient", "127.0.0.1", nullptr));

        groupBox_3->setTitle(QCoreApplication::translate("FormTcpClient", "\345\256\242\346\210\267\347\253\257\346\265\213\350\257\225\346\266\210\346\201\257", nullptr));
        pushButton_TCPClientSendMsg->setText(QCoreApplication::translate("FormTcpClient", "\345\217\221\351\200\201\346\266\210\346\201\257", nullptr));
        checkBox_TCPClientAutoText->setText(QCoreApplication::translate("FormTcpClient", "\350\207\252\345\212\250\345\214\226\346\265\213\350\257\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FormTcpClient: public Ui_FormTcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMTCPCLIENT_H
