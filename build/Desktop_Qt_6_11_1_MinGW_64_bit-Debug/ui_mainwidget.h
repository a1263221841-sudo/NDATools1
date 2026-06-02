/********************************************************************************
** Form generated from reading UI file 'mainwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "formtcpclient.h"
#include "formtcpserver.h"
#include "formudpclient.h"
#include "formudpserver.h"

QT_BEGIN_NAMESPACE

class Ui_MainWidget
{
public:
    QTabWidget *tab_Widget;
    FormTcpServer *tab_TCPServer;
    FormTcpClient *tab_TCPClient;
    FormUDPServer *tab_UDPServer;
    FormUDPClient *tab_UDPClient;
    QWidget *tab_DataExchange;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName("MainWidget");
        MainWidget->resize(900, 600);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/images/NDALogo.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        MainWidget->setWindowIcon(icon);
        tab_Widget = new QTabWidget(MainWidget);
        tab_Widget->setObjectName("tab_Widget");
        tab_Widget->setGeometry(QRect(10, 10, 881, 581));
        tab_TCPServer = new FormTcpServer();
        tab_TCPServer->setObjectName("tab_TCPServer");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/prefix1/images/udpserver.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        tab_Widget->addTab(tab_TCPServer, icon1, QString());
        tab_TCPClient = new FormTcpClient();
        tab_TCPClient->setObjectName("tab_TCPClient");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/prefix1/images/tcpclient.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        tab_Widget->addTab(tab_TCPClient, icon2, QString());
        tab_UDPServer = new FormUDPServer();
        tab_UDPServer->setObjectName("tab_UDPServer");
        tab_Widget->addTab(tab_UDPServer, icon1, QString());
        tab_UDPClient = new FormUDPClient();
        tab_UDPClient->setObjectName("tab_UDPClient");
        tab_Widget->addTab(tab_UDPClient, icon2, QString());
        tab_DataExchange = new QWidget();
        tab_DataExchange->setObjectName("tab_DataExchange");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/new/prefix1/images/sendmsg.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        tab_Widget->addTab(tab_DataExchange, icon3, QString());

        retranslateUi(MainWidget);

        tab_Widget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QCoreApplication::translate("MainWidget", "TCP/UDP\347\275\221\350\267\257\350\260\203\350\257\225\345\212\251\346\211\213", nullptr));
        tab_Widget->setTabText(tab_Widget->indexOf(tab_TCPServer), QCoreApplication::translate("MainWidget", "TCP\346\234\215\345\212\241\345\231\250", nullptr));
        tab_Widget->setTabText(tab_Widget->indexOf(tab_TCPClient), QCoreApplication::translate("MainWidget", "TCP\345\256\242\346\210\267\347\253\257", nullptr));
        tab_Widget->setTabText(tab_Widget->indexOf(tab_UDPServer), QCoreApplication::translate("MainWidget", "UDP\346\234\215\345\212\241\345\231\250", nullptr));
        tab_Widget->setTabText(tab_Widget->indexOf(tab_UDPClient), QCoreApplication::translate("MainWidget", "UDP\345\256\242\346\210\267\347\253\257", nullptr));
        tab_Widget->setTabText(tab_Widget->indexOf(tab_DataExchange), QCoreApplication::translate("MainWidget", "\346\225\260\346\215\256\345\244\204\347\220\206\345\267\245\345\205\267", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H
