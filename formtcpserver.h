#ifndef FORMTCPSERVER_H
#define FORMTCPSERVER_H


//提供tcp服务器界面及逻辑,tcp协议调试模块的文件接口声明
/*ui初始化与样式
 * 服务器控制
 * 连接管理
 * 消息收发
 * 日志展示与持久化
 * 校验与提示*/
#include <QWidget>

#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>  //网络接口信息
#include <QDateTime>
#include <QMessageBox>
#include <QFile>
#include <QTextBlock>
#include <QListWidget>
#include <QColor>
#include <QCloseEvent>

#include <QSettings>
#include <QStandardPaths>
#include <QDir>  //目录操作
#include "errorhandler.h"  //统一错误处理的与弹窗

namespace Ui {
class FormTcpServer;
}

class FormTcpServer : public QWidget
{
    Q_OBJECT

public:
    explicit FormTcpServer(QWidget *parent = nullptr);
    ~FormTcpServer();

private slots:
    void on_pushButton_TCPServerStart_clicked();
    void on_pushButton_TCPServerStop_clicked();
    void on_pushButton_TCPServerClose_clicked();
    void on_pushButton_TCPServerSendMsg_clicked();
private:
    Ui::FormTcpServer *ui;

    QTcpServer *tcpServer = nullptr ; //服务器监听实例
    QTcpServer *tcpServerSocket =nullptr ; //最近一次建立的socket,用于简单模式
    QList<QTcpSocket*> tcpServerSocketList; //活跃客户端列表
    bool serverRunning = false; //当前是否在监听
    int receivedMessageCount = 0; //已接收消息计数(累加)

    //功能:在日志列表追加彩色行
    void appendColorLog(const QString &text,const QColor &color);

    //裁剪日志行数,防止控件过大
    void trimLog(int keepRows= 1000, int trimStep= 200);

public:
    //检验ip字符串是否合法
    bool CheckIPAddressValid(QString strIpAddress);

    //将列表控件内部保存到文件(追加或覆盖由实现决定)
    void saveListWidgetToFile(QListWidget* listwidget);

    //保存日志
    void saveLog();


    //窗口关闭事件,保存日志,清理资源
    void closeEvent(QCloseEvent *event) override;
    // 是Qt框架中的一个特殊宏，用于在类中声明槽函数（slot）。它是Qt信号与槽机制的重要组成部分。
public Q_SLOTS:   // Q_SLOTS告诉Qt的元对象系统，后面的函数是槽函数，public表示这些槽函数可以从外部调用

//处理新客户链接,记录并更新列表
    void TcpServerConnectedFunc();

//功能:处理客户端断开,清理列表与ui状态
    void CientDisconnectedFunc();

//功能:读取所有可用数据并计数,展示日志
    void ReadAllDataFunc();
};

#endif // FORMTCPSERVER_H
