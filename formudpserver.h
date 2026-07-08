#ifndef FORMUDPSERVER_H
#define FORMUDPSERVER_H


//启动停止udp协议服务器监听,接收客户端数据,回发响应数据,记录日志并裁剪
//提供udp服务器功能,支持绑定指定ip和端口,接受客户端消息并自动回发响应

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QWidget>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>

//qt工具模块,提供日期时间,消息框,配置管理等功能
#include <QDateTime>
#include <QMessageBox>
#include <QSettings>
#include <QListWidget>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QStandardPaths>
#include <QDir>

// 项目自定义头文件：提供错误处理和输入验证功能
#include "errorhandler.h"
#include "inputvalidator.h"


namespace Ui {
class FormUDPServer;
}

class FormUDPServer : public QWidget
{
    Q_OBJECT

public:
    explicit FormUDPServer(QWidget *parent = nullptr);
    ~FormUDPServer();


private slots:
    // 启动/停止服务器按钮点击事件：切换UDP服务器的监听状态
    void on_pushButton_UDPServerStart_clicked();

    // 关闭按钮点击事件：保存日志并退出应用程序
    void on_pushButton_UDPServerClose_clicked();

    // 发送测试消息按钮点击事件：向最近连接的客户端发送测试消息
    void on_pushButton_UDPServerSendMsg_clicked();

    // 读取并处理收到的数据报：当UDP套接字有数据可读时自动调用
    void udpSeverReadPendingDatagrams();

    // 追加日志项到列表：将消息或状态信息追加到日志列表控件
    void udpServerAppendStrItem(int type,const QString &strData,bool clear);


private:
    Ui::FormUDPServer *ui;

    // UDP服务器套接字指针：指向用于UDP网络通信的QUdpSocket对象
    QUdpSocket *udpServerSocket;

    // 最近收到数据的客户端地址：存储最后一次接收数据的客户端IP地址
    QHostAddress m_lastClientAddress;

    // 最近收到数据的客户端端口：存储最后一次接收数据报的客户端端口
    quint16 m_lastClientPort=0;

    // 服务器运行状态标志：标识UDP服务器当前是否正监听
    bool serverRunning=false;

    // 日志裁剪函数：当日志列表项数超过阈值时，删除前部项以控制内存使用
    void trimLog(int keepRows=1000,int trimStep=200);

    // 窗口关闭事件处理函数：当窗口关闭时自动调用，保存日志并接受关闭
    void closeEvent(QCloseEvent *event) override;


public:
    // 保存日志列表到文本文件：将QListWidget中的所有项保存到文本文件
    void saveListWidgetToFile(QListWidget* listWidget);

    // 保存日志函数：保存当前界面的日志列表到文件（便捷掊）
    void saveLog();

};

#endif // FORMUDPSERVER_H
