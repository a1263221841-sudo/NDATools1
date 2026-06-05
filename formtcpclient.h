#ifndef FORMTCPCLIENT_H
#define FORMTCPCLIENT_H


//TCP协议客户端界面类声明
//主要职责:发起/断开TCP连接发送消息,自动话测试切换,日志显示与裁剪
#include <QWidget>


#include <QMessageBox>
#include <QRegularExpression>  //正则效验
#include <QString>
#include <QDateTime>
//#include <QSetting>    //持久化最近配置
#include <QTextCursor> //文本光标
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>

#include <QStandardPaths>  //获取配置文件目录
#include <QDir>   //目录操作

#include "network.h"  //自定义网络通信封装(连接/发放)
#include "error.h"  //错误统一处理
#include "inputvalidator.h"  //输入校验工具,(ip端口)

namespace Ui {
class FormTcpClient;
}

class FormTcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit FormTcpClient(QWidget *parent = nullptr);
    ~FormTcpClient();

private slots:
    //发送连接,验证ip/端口,保存最近配置并请示连接
    void on_pushButton_TCPClientConnect_clicked();

    //断开连接 关闭socket并重置ui
    void on_pushButton_TCPClientDisconnect_clicked();

    //关闭窗口
    void on_pushButton_TCPClientClose_clicked();

    //发送消息 需要已经连接且输入非空
    void on_pushButton_TCPClientSendMsg_clicked();

    void on_checkBox_TCPClientAutoTesting_clicked();


private:
    Ui::FormTcpClient *ui;


    Network NetworkClient;  //网络通信核心对象,封装socket行为
    bool connected=false;  //当前连接状态标记

    void trimLog(int keepBlocks = 1000,int trimStep = 200);  //日志裁剪,保留行数/超限删除步长
    bool CheckIPAddrIsValid(QString strIpAddress);  //IP合法性校验(保留旧接口)
    void enableCommunicationContorols(bool enable);  //批量控制与连接相关的UI
    void closeEvent(QCloseEvent *event) override;  //窗口关闭事件,保存日志并接受关闭

public:
    //保存日志文本到文件
    void savePlainTextEditToFile(QPlainTextEdit* plianTextEdit);
     //保存日志
    void saveLog();
};

#endif // FORMTCPCLIENT_H
