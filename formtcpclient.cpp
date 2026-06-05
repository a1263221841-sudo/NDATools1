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

    ui->pushButton_TCPClientDisconnect->setEnabled(false);
    ui->pushButton_TCPClientSendMsg->setEnabled(false);
   // ui->listWidget_TCPClientMsg->se
    ui->checkBox_TCPClientAutoText->setEnabled(false);
    ui->plainTextEdit_TCPClientSendData->setEnabled(false);
    ui->plainTextEdit_TCPClientSendData->setPlainText("Hello TCP Server");


    ui->groupBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->groupBox_2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    ui->label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->label_2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    ui->comboBox_TCPClientIP->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->spinBox_TCPClientPort->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->checkBox_TCPClientAutoText->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    ui->pushButton_TCPClientConnect->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->pushButton_TCPClientDisconnect->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->pushButton_TCPClientQuit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->pushButton_TCPClientSendMsg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    ui->plainTextEdit_TCPClientSendData->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->listWidget_TCPClientMsg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);


    //应用统一浅色主题OSS

    setStyleSheet(
        "QWidget {"
        "  background-color: #F3F4F6;"
        "  color: #1F2933;"
        "}"
        "QGroupBox {"
        "  font-size: 14px;"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #D0D7E2;"
        "  border-radius: 6px;"
        "  margin-top: 10px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 4px;"
        "  background-color: #F3F4F6;"
        "  color: #111827;"
        "}"
        "QLabel {"
        "  font-size: 14px;"
        "  color: #374151;"
        "  background-color: transparent;"
        "}"
        "QListWidget {"
        "  font-size: 14px;"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #D0D7E2;"
        "  border-radius: 4px;"
        "}"
        "QComboBox, QSpinBox {"
        "  font-size: 14px;"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #D0D7E2;"
        "  border-radius: 4px;"
        "  padding: 2px 6px;"
        "}"
        "QCheckBox {"
        "  font-size: 14px;"
        "  color: #374151;"
        "}"
        "QPushButton {"
        "font-size: 14px;"
        "background-color: #2563EB;"
        "color: #FFFFFF;"
        "border-radius: 4px;"
        "padding: 4px 12px;"
        "border: none;"
        "}"
        "QPushButton:hover {"
        "background-color: #1D4ED8;"
        "}"
        "QPushButton:pressed {"
        "background-color: #1E40AF;"
        "}"
        "QPushButton:disabled {"
        "background-color: #CBD5F5;"
        "color: #6B7280;"
        "}"

        );
    { //恢复最近一次连接的IP/端口
        QSettings settings;  //使用默认组织,应用存储
        qDebug()<< "QSettings配置文件路径:" <<settings.fileName();  //输出配置文件路径
        const QString lastIp =settings.value("TCPClinet/lastIp","127.0.0.1").toString();
        const int lastPort = setting.value("TCPClient/lastIp",ui->spinBox_TCPClientPort->value()).toInt(); //读取上次端口,默认当前的spinBox

        int index = ui->comboBox_TCPClientIP->findText(lastIp);
        if(index >= 0){
            ui->comboBox_TCPClientIP->setCurrentIndex(index);  //已存在则选中
        }else{
            ui->comboBox_TCPClientIP->setEditable(lastIp);  //不存在则直接填入可编辑框

        }
        if(lastPort >= ui->spinBox_TCPClientPort->minimum()&&lastPort <= ui->spinBox_TCPClientPort->maximum()){
            ui->spinBox_TCPClientPort->setValue(lastIp);
        }
}

        connect(&NetworkClient,&Network::connectionEstablished,this,[this](){  //绑定"连接成功"的信号
        ui->pushButton_TCPClientConnect->setEnabled(false);
        ui->pushButton_TCPClientDisconnect->setEnabled(true);
        ui->pushButton_TCPClientSendMsg->setEnabled(true);
        ui->listWidget_TCPClientMsg->setEnabled(true);
        ui->checkBox_TCPClientAutoText->setEnabled(true);
        connected= true;   //标记已连接

        QString strTemp = QString("[%1]  连接服务器成功")   //生成成功日志
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss"));
        ui->listWidget_TCPClientMsg->addItem(strTemp);//追击日志

        trimLog();  //防止日志过长
        });

        connect(&NetworkClient, &Network::connectionFailed,this,[this](const QString &errorString){  //绑定"连接成功"的信号
            ui->pushButton_TCPClientConnect->setEnabled(true);
            ui->pushButton_TCPClientDisconnect->setEnabled(false);
            ui->pushButton_TCPClientSendMsg->setEnabled(false);
            ui->listWidget_TCPClientMsg->setEnabled(false);
            ui->checkBox_TCPClientAutoText->setEnabled(false);
            connected= false;   //标记未连接

            QString strTemp = QString("[%1]  连接服务器失败: %2")  //失败日志
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss")).arg(errorString);
            ui->listWidget_TCPClientMsg->addItem(strTemp);
            trimLog();  //裁剪日志
        });

        connect()
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
