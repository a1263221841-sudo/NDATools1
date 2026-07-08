#include "formudpserver.h"
#include "ui_formudpserver.h"
#include <QApplication>

FormUDPServer::FormUDPServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormUDPServer)
{
    ui->setupUi(this);

    //设置下拉框可编辑'
    ui->comboBox_UDPServerIp->setEditable(true);
    //获取本机所有网络接口的ip地址
    QList<QHostAddress> addressList = QNetworkInterface::allAddresses();

    //创建ip地址字符串列表
    QStringList ipList;

    //预分配列表容量
    ipList.reserve(addressList.size());

    //遍历地址列表
    for(const QHostAddress &address:addressList){
        if(address.protocol()= QAbstractSocket::IPv4Protocol){
            ipList.append(address.toString());
    }
    }


    //批量添加ip地址到下拉框控件
    ui->comboBox_UDPServerIp->setUpdatesEnabled(false);

    //批量添加到所有地址
    ui->comboBox_UDPServerIp->addItem(ipList);
    ui->comboBox_UDPServerIp->setUpdatesEnabled(true);
}



FormUDPServer::~FormUDPServer()
{
    delete ui;
}


// 启动/停止服务器按钮点击事件：切换UDP服务器的监听状态
void FormUDPServer::on_pushButton_UDPServerStart_clicked(){

}

// 关闭按钮点击事件：保存日志并退出应用程序
void FormUDPServer::on_pushButton_UDPServerClose_clicked()
{

}

// 发送测试消息按钮点击事件：向最近连接的客户端发送测试消息
void FormUDPServer::on_pushButton_UDPServerSendMsg_clicked()
{

}

// 读取并处理收到的数据报：当UDP套接字有数据可读时自动调用
void FormUDPServer::udpSeverReadPendingDatagrams()
{

}

// 追加日志项到列表：将消息或状态信息追加到日志列表控件
void FormUDPServer::udpServerAppendStrItem(int type,const QString &strData,bool clear)
{

}