/********************************************************************************
** Form generated from reading UI file 'formtcpserver.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMTCPSERVER_H
#define UI_FORMTCPSERVER_H

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

class Ui_FormTcpServer
{
public:
    QGroupBox *groupBox;
    QListWidget *listWidget_TCPServerMsg;
    QGroupBox *groupBox_2;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *spinBox_TCPServerPort;
    QPushButton *pushButton_TCPServerStartListen;
    QPushButton *pushButton_TCPServerCloseListen;
    QPushButton *pushButton_TCPServerQuit;
    QPushButton *pushButton_TCPServerSendMsg;
    QLabel *label_3;
    QComboBox *comboBox_TCPServerIp;
    QPlainTextEdit *plainTextEdit_TCPServerSendData;

    void setupUi(QWidget *FormTcpServer)
    {
        if (FormTcpServer->objectName().isEmpty())
            FormTcpServer->setObjectName("FormTcpServer");
        FormTcpServer->resize(860, 560);
        groupBox = new QGroupBox(FormTcpServer);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 20, 601, 531));
        QFont font;
        font.setFamilies({QString::fromUtf8("\344\273\277\345\256\213")});
        font.setPointSize(11);
        groupBox->setFont(font);
        listWidget_TCPServerMsg = new QListWidget(groupBox);
        listWidget_TCPServerMsg->setObjectName("listWidget_TCPServerMsg");
        listWidget_TCPServerMsg->setGeometry(QRect(10, 20, 581, 501));
        groupBox_2 = new QGroupBox(FormTcpServer);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(630, 20, 201, 531));
        groupBox_2->setFont(font);
        label = new QLabel(groupBox_2);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 111, 19));
        label->setFont(font);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 90, 121, 21));
        label_2->setFont(font);
        spinBox_TCPServerPort = new QSpinBox(groupBox_2);
        spinBox_TCPServerPort->setObjectName("spinBox_TCPServerPort");
        spinBox_TCPServerPort->setGeometry(QRect(20, 120, 161, 31));
        spinBox_TCPServerPort->setFont(font);
        spinBox_TCPServerPort->setMaximum(65535);
        spinBox_TCPServerPort->setValue(12345);
        pushButton_TCPServerStartListen = new QPushButton(groupBox_2);
        pushButton_TCPServerStartListen->setObjectName("pushButton_TCPServerStartListen");
        pushButton_TCPServerStartListen->setGeometry(QRect(20, 170, 161, 51));
        pushButton_TCPServerStartListen->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/images/start.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPServerStartListen->setIcon(icon);
        pushButton_TCPServerCloseListen = new QPushButton(groupBox_2);
        pushButton_TCPServerCloseListen->setObjectName("pushButton_TCPServerCloseListen");
        pushButton_TCPServerCloseListen->setGeometry(QRect(20, 240, 161, 51));
        pushButton_TCPServerCloseListen->setFont(font);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/prefix1/images/stop.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPServerCloseListen->setIcon(icon1);
        pushButton_TCPServerQuit = new QPushButton(groupBox_2);
        pushButton_TCPServerQuit->setObjectName("pushButton_TCPServerQuit");
        pushButton_TCPServerQuit->setGeometry(QRect(20, 310, 161, 51));
        pushButton_TCPServerQuit->setFont(font);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/prefix1/images/exit.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPServerQuit->setIcon(icon2);
        pushButton_TCPServerSendMsg = new QPushButton(groupBox_2);
        pushButton_TCPServerSendMsg->setObjectName("pushButton_TCPServerSendMsg");
        pushButton_TCPServerSendMsg->setGeometry(QRect(20, 490, 161, 31));
        pushButton_TCPServerSendMsg->setFont(font);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/new/prefix1/images/sendmsg2.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_TCPServerSendMsg->setIcon(icon3);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 370, 151, 21));
        label_3->setFont(font);
        comboBox_TCPServerIp = new QComboBox(groupBox_2);
        comboBox_TCPServerIp->addItem(QString());
        comboBox_TCPServerIp->setObjectName("comboBox_TCPServerIp");
        comboBox_TCPServerIp->setGeometry(QRect(20, 50, 161, 31));
        comboBox_TCPServerIp->setFont(font);
        plainTextEdit_TCPServerSendData = new QPlainTextEdit(groupBox_2);
        plainTextEdit_TCPServerSendData->setObjectName("plainTextEdit_TCPServerSendData");
        plainTextEdit_TCPServerSendData->setGeometry(QRect(20, 400, 161, 81));

        retranslateUi(FormTcpServer);

        QMetaObject::connectSlotsByName(FormTcpServer);
    } // setupUi

    void retranslateUi(QWidget *FormTcpServer)
    {
        FormTcpServer->setWindowTitle(QCoreApplication::translate("FormTcpServer", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("FormTcpServer", "TCP\345\215\217\350\256\256\346\234\215\345\212\241\345\231\250\346\266\210\346\201\257\345\210\227\350\241\250", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("FormTcpServer", "\347\275\221\347\273\234\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("FormTcpServer", "\346\234\254\346\234\272\344\270\273\346\234\272\345\234\260\345\235\200:", nullptr));
        label_2->setText(QCoreApplication::translate("FormTcpServer", "\346\234\254\345\234\260\344\270\273\346\234\272\347\253\257\345\217\243:", nullptr));
        pushButton_TCPServerStartListen->setText(QCoreApplication::translate("FormTcpServer", "\345\220\257\345\212\250\347\233\221\345\220\254", nullptr));
        pushButton_TCPServerCloseListen->setText(QCoreApplication::translate("FormTcpServer", "\345\205\263\351\227\255\347\233\221\345\220\254", nullptr));
        pushButton_TCPServerQuit->setText(QCoreApplication::translate("FormTcpServer", "\351\200\200\345\207\272\347\250\213\345\272\217", nullptr));
        pushButton_TCPServerSendMsg->setText(QCoreApplication::translate("FormTcpServer", "\345\217\221\351\200\201\346\265\213\350\257\225\346\266\210\346\201\257", nullptr));
        label_3->setText(QCoreApplication::translate("FormTcpServer", "\346\234\215\345\212\241\345\231\250\346\265\213\350\257\225\345\217\221\351\200\201:", nullptr));
        comboBox_TCPServerIp->setItemText(0, QCoreApplication::translate("FormTcpServer", "127.0.0.1", nullptr));

    } // retranslateUi

};

namespace Ui {
    class FormTcpServer: public Ui_FormTcpServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMTCPSERVER_H
