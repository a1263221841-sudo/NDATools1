#include "formtcpclient.h"
#include "ui_formtcpclient.h"
#include <QApplication>


// 构造函数：初始化UI、状态、样式并绑定信号
FormTcpClient::FormTcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormTcpClient)
{
    ui->setupUi(this);


    //设置IP下拉框为可编辑,允许手动输入iP地址
    ui->comboBox_TCPClientIP->setEditable(true);

    ui->pushButton_TCPClientDisconnect->setEnabled(flase);
    ui->pushButton_TCPClientSendMsg->setEnabled(false);
    ui->listWidget_TCPClientMsg->
}

FormTcpClient::~FormTcpClient()
{
    delete ui;
}

//发送连接,验证ip/端口,保存最近配置并请示连接
void FormTcpClient::on_pushButton_TCPClientConnect_clicked()
{

}

//断开连接 关闭socket并重置ui
void FormTcpClient::on_pushButton_TCPClientDisconnect_clicked()
{

}

//关闭窗口
void FormTcpClient::on_pushButton_TCPClientClose_clicked()
{

}

//发送消息 需要已经连接且输入非空
void FormTcpClient::on_pushButton_TCPClientSendMsg_clicked()
{

}

void FormTcpClient::on_checkBox_TCPClientAutoTesting_clicked()
{

}

void trimLog(int keepBlocks = 1000,int trimStep = 200);  //日志裁剪,保留行数/超限删除步长
bool CheckIPAddrIsValid(QString strIpAddress);  //IP合法性校验(保留旧接口)
void enableCommunicationContorols(bool enable);  //批量控制与连接相关的UI
void closeEvent(QCloseEvent *event) override;  //窗口关闭事件,保存日志并接受关闭

//保存日志文本到文件
void FormTcpClient::savePlainTextEditToFile(QPlainTextEdit* plianTextEdit)
{

}
//保存日志
void FormTcpClient::saveLog()
{

}
