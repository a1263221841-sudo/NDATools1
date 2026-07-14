/********************************************************************************
** Form generated from reading UI file 'formudpserver.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMUDPSERVER_H
#define UI_FORMUDPSERVER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FormUDPServer
{
public:
    QGroupBox *groupBox;
    QListWidget *listWidget_UDPServerMsg;
    QGroupBox *groupBox_2;
    QLabel *label_5;
    QLabel *label_6;
    QSpinBox *spinBox_UDPServerPort;
    QPushButton *pushButton_UDPServerStartListen;
    QPushButton *pushButton_UDPServerQuit;
    QPushButton *pushButton_UDPServerSendMsg;
    QLabel *label_7;
    QComboBox *comboBox_UDPServerIp;
    QPlainTextEdit *plainTextEdit_UDPServerSendData;

    void setupUi(QWidget *FormUDPServer)
    {
        if (FormUDPServer->objectName().isEmpty())
            FormUDPServer->setObjectName("FormUDPServer");
        FormUDPServer->resize(860, 560);
        groupBox = new QGroupBox(FormUDPServer);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 10, 601, 531));
        QFont font;
        font.setFamilies({QString::fromUtf8("\344\273\277\345\256\213")});
        font.setPointSize(11);
        groupBox->setFont(font);
        listWidget_UDPServerMsg = new QListWidget(groupBox);
        listWidget_UDPServerMsg->setObjectName("listWidget_UDPServerMsg");
        listWidget_UDPServerMsg->setGeometry(QRect(10, 20, 581, 501));
        groupBox_2 = new QGroupBox(FormUDPServer);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(630, 10, 201, 531));
        groupBox_2->setFont(font);
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 20, 111, 19));
        label_5->setFont(font);
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 90, 121, 21));
        label_6->setFont(font);
        spinBox_UDPServerPort = new QSpinBox(groupBox_2);
        spinBox_UDPServerPort->setObjectName("spinBox_UDPServerPort");
        spinBox_UDPServerPort->setGeometry(QRect(20, 120, 161, 31));
        spinBox_UDPServerPort->setFont(font);
        spinBox_UDPServerPort->setMaximum(65535);
        spinBox_UDPServerPort->setValue(12345);
        pushButton_UDPServerStartListen = new QPushButton(groupBox_2);
        pushButton_UDPServerStartListen->setObjectName("pushButton_UDPServerStartListen");
        pushButton_UDPServerStartListen->setGeometry(QRect(20, 170, 161, 51));
        pushButton_UDPServerStartListen->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/images/start.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_UDPServerStartListen->setIcon(icon);
        pushButton_UDPServerQuit = new QPushButton(groupBox_2);
        pushButton_UDPServerQuit->setObjectName("pushButton_UDPServerQuit");
        pushButton_UDPServerQuit->setGeometry(QRect(20, 260, 161, 51));
        pushButton_UDPServerQuit->setFont(font);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/prefix1/images/exit.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_UDPServerQuit->setIcon(icon1);
        pushButton_UDPServerSendMsg = new QPushButton(groupBox_2);
        pushButton_UDPServerSendMsg->setObjectName("pushButton_UDPServerSendMsg");
        pushButton_UDPServerSendMsg->setGeometry(QRect(20, 490, 161, 31));
        pushButton_UDPServerSendMsg->setFont(font);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/prefix1/images/sendmsg2.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_UDPServerSendMsg->setIcon(icon2);
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 370, 151, 21));
        label_7->setFont(font);
        comboBox_UDPServerIp = new QComboBox(groupBox_2);
        comboBox_UDPServerIp->addItem(QString());
        comboBox_UDPServerIp->setObjectName("comboBox_UDPServerIp");
        comboBox_UDPServerIp->setGeometry(QRect(20, 50, 161, 31));
        comboBox_UDPServerIp->setFont(font);
        plainTextEdit_UDPServerSendData = new QPlainTextEdit(groupBox_2);
        plainTextEdit_UDPServerSendData->setObjectName("plainTextEdit_UDPServerSendData");
        plainTextEdit_UDPServerSendData->setGeometry(QRect(20, 400, 161, 81));

        retranslateUi(FormUDPServer);

        QMetaObject::connectSlotsByName(FormUDPServer);
    } // setupUi

    void retranslateUi(QWidget *FormUDPServer)
    {
        FormUDPServer->setWindowTitle(QCoreApplication::translate("FormUDPServer", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("FormUDPServer", "UDP\345\215\217\350\256\256\346\234\215\345\212\241\345\231\250\346\266\210\346\201\257\345\210\227\350\241\250", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("FormUDPServer", "\345\217\202\346\225\260\350\256\276\347\275\256:", nullptr));
        label_5->setText(QCoreApplication::translate("FormUDPServer", "\346\234\254\346\234\272\344\270\273\346\234\272\345\234\260\345\235\200:", nullptr));
        label_6->setText(QCoreApplication::translate("FormUDPServer", "\346\234\254\345\234\260\344\270\273\346\234\272\347\253\257\345\217\243:", nullptr));
        pushButton_UDPServerStartListen->setText(QCoreApplication::translate("FormUDPServer", "\345\220\257\345\212\250\347\233\221\345\220\254", nullptr));
        pushButton_UDPServerQuit->setText(QCoreApplication::translate("FormUDPServer", "\351\200\200\345\207\272\347\250\213\345\272\217", nullptr));
        pushButton_UDPServerSendMsg->setText(QCoreApplication::translate("FormUDPServer", "\345\217\221\351\200\201\346\265\213\350\257\225\346\266\210\346\201\257", nullptr));
        label_7->setText(QCoreApplication::translate("FormUDPServer", "\346\234\215\345\212\241\345\231\250\346\265\213\350\257\225\345\217\221\351\200\201:", nullptr));
        comboBox_UDPServerIp->setItemText(0, QCoreApplication::translate("FormUDPServer", "127.0.0.1", nullptr));

    } // retranslateUi

};

namespace Ui {
    class FormUDPServer: public Ui_FormUDPServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMUDPSERVER_H
