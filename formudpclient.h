#ifndef FORMUDPCLIENT_H
#define FORMUDPCLIENT_H

#include <QWidget>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QFile>
#include <QCloseEvent>

#include <QDateTime>
#include <QSettings>
#include <QTextCursor>
#include <QStandardPaths>
#include <QDir>

//项目公共模块
#include "inputvalidator.h"
#include "errorhandler.h"

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif


//udp客户端界面类,FormUdpClient ,用于向udP服务器发送数据报
//接收服务器响应,并在界面中记录,裁剪收发日志,udP套接字采用懒创建策略
namespace Ui {
class FormUDPClient;
}

class FormUDPClient : public QWidget
{
    Q_OBJECT

public:
    explicit FormUDPClient(QWidget *parent = nullptr);
    ~FormUDPClient();

private  slots:
    void on_pushButton_UDPClientSendMsg_clicked();

private:
    bool socketReady= false;

    //日志裁剪
    void trimLog(int keepBlocks=1000,int trimStep=200);

    //窗口关闭时保存日志并接受关闭
    void closeEvent(QCloseEvent *event) override;

public:
    //将指定QPlainTextEdit的全部记录只在到应用数据目录下.追加模式,utf-8
    void savePlainTextEditToFile(QPlainTextEdit * plainTextEdit);

    void saveLog();

    //udp客户端套接字指针,初始化为nullptr
    QUdpSocket *UDPClientSocket=nullpt;

    //发送目标地址缓存,可选用途
    QHostAddress UDPClientHostAddress;

public Q_SLOTS:
    //读取服务器返回的数据并加载到日志框,由readyRead信号触发
    void ReadServerDatagramFunc();
private:
    Ui::FormUDPClient *ui;
};

#endif // FORMUDPCLIENT_H
