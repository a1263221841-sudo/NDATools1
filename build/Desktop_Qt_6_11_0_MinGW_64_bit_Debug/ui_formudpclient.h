/********************************************************************************
** Form generated from reading UI file 'formudpclient.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMUDPCLIENT_H
#define UI_FORMUDPCLIENT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FormUDPClient
{
public:
    QGroupBox *groupBox_3;
    QPlainTextEdit *plainTextEdit_UDPClientSendData;
    QPushButton *pushButton_UDPClientSendMsg;
    QGroupBox *groupBox_2;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *spinBox_UDPClientPort;
    QComboBox *comboBox_UDPClientIp;
    QGroupBox *groupBox;
    QPlainTextEdit *plainTextEdit_UDPClientMsgList;

    void setupUi(QWidget *FormUDPClient)
    {
        if (FormUDPClient->objectName().isEmpty())
            FormUDPClient->setObjectName("FormUDPClient");
        FormUDPClient->resize(860, 560);
        groupBox_3 = new QGroupBox(FormUDPClient);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(630, 190, 211, 361));
        QFont font;
        font.setFamilies({QString::fromUtf8("\344\273\277\345\256\213")});
        font.setPointSize(11);
        groupBox_3->setFont(font);
        plainTextEdit_UDPClientSendData = new QPlainTextEdit(groupBox_3);
        plainTextEdit_UDPClientSendData->setObjectName("plainTextEdit_UDPClientSendData");
        plainTextEdit_UDPClientSendData->setGeometry(QRect(10, 20, 191, 281));
        plainTextEdit_UDPClientSendData->setFont(font);
        pushButton_UDPClientSendMsg = new QPushButton(groupBox_3);
        pushButton_UDPClientSendMsg->setObjectName("pushButton_UDPClientSendMsg");
        pushButton_UDPClientSendMsg->setGeometry(QRect(10, 310, 191, 31));
        pushButton_UDPClientSendMsg->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/images/sendmsg2.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_UDPClientSendMsg->setIcon(icon);
        groupBox_2 = new QGroupBox(FormUDPClient);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(630, 10, 211, 171));
        groupBox_2->setFont(font);
        label = new QLabel(groupBox_2);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 121, 19));
        label->setFont(font);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 90, 121, 21));
        label_2->setFont(font);
        spinBox_UDPClientPort = new QSpinBox(groupBox_2);
        spinBox_UDPClientPort->setObjectName("spinBox_UDPClientPort");
        spinBox_UDPClientPort->setGeometry(QRect(20, 120, 161, 31));
        spinBox_UDPClientPort->setFont(font);
        spinBox_UDPClientPort->setMaximum(65535);
        spinBox_UDPClientPort->setValue(12345);
        comboBox_UDPClientIp = new QComboBox(groupBox_2);
        comboBox_UDPClientIp->addItem(QString());
        comboBox_UDPClientIp->setObjectName("comboBox_UDPClientIp");
        comboBox_UDPClientIp->setGeometry(QRect(20, 50, 161, 31));
        comboBox_UDPClientIp->setFont(font);
        groupBox = new QGroupBox(FormUDPClient);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 10, 611, 541));
        groupBox->setFont(font);
        plainTextEdit_UDPClientMsgList = new QPlainTextEdit(groupBox);
        plainTextEdit_UDPClientMsgList->setObjectName("plainTextEdit_UDPClientMsgList");
        plainTextEdit_UDPClientMsgList->setGeometry(QRect(13, 24, 581, 501));

        retranslateUi(FormUDPClient);

        QMetaObject::connectSlotsByName(FormUDPClient);
    } // setupUi

    void retranslateUi(QWidget *FormUDPClient)
    {
        FormUDPClient->setWindowTitle(QCoreApplication::translate("FormUDPClient", "Form", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("FormUDPClient", "\345\256\242\346\210\267\347\253\257\346\265\213\350\257\225\346\266\210\346\201\257", nullptr));
        pushButton_UDPClientSendMsg->setText(QCoreApplication::translate("FormUDPClient", "\345\217\221\351\200\201\346\266\210\346\201\257", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("FormUDPClient", "\347\275\221\347\273\234\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("FormUDPClient", "\350\277\234\347\250\213\344\270\273\346\234\272\345\234\260\345\235\200:", nullptr));
        label_2->setText(QCoreApplication::translate("FormUDPClient", "\350\277\234\347\250\213\344\270\273\346\234\272\347\253\257\345\217\243:", nullptr));
        comboBox_UDPClientIp->setItemText(0, QCoreApplication::translate("FormUDPClient", "127.0.0.1", nullptr));

        groupBox->setTitle(QCoreApplication::translate("FormUDPClient", "UDP\345\215\217\350\256\256\345\256\242\346\210\267\347\253\257\346\266\210\346\201\257\345\210\227\350\241\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FormUDPClient: public Ui_FormUDPClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMUDPCLIENT_H
