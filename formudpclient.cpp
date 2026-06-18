 #include "formudpclient.h"
#include "ui_formudpclient.h"
#include <QApplication>


#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
FormUDPClient::FormUDPClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormUDPClient)
{
    ui->setupUi(this);

    //ip地址下拉框可编辑,
    ui->comboBox_UDPClientIp->setEditable(true);

    //固定窗口大小,禁止用户拖拽改变尺寸,保持界面稳定
    setFixedSize(width(),height());

    ui->plainTextEdit_UDPClientSendData->setPlainText("Hello Udp Server.");

    ui->plainTextEdit_UDPClientMsgList->setReadOnly(true);
    ui->plainTextEdit_UDPClientMsgList->setPlainText("Prompt: Pleast enter data and click to send test");


    // 为各大控件设置固定尺寸策略，避免布局随内容变化产生抖动
    ui->groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->groupBox_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->plainTextEdit_UDPClientMsgList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->label_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->comboBox_UDPClientIp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->spinBox_UDPClientPort->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->plainTextEdit_UDPClientSendData->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_UDPClientSendMsg->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    //设置整体界面样式,背景色,分组框边框与标题,标签,编辑框,按扭
    setStyle(
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
        "QPlainTextEdit {"
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

    //设置发送按扭上图标的显示尺寸
    ui->pushButton_TCPClientSendMsg->setIconSize(QSize(16,16));

    //DPI设置已在main.cpp中统一配置,此处不再重复设置
    //从QSettings恢复上次使用的ip和端口,便于用户连续使用同一目标
    {
        QSettings settings;
        const QString lastIp =settings.value("UDPClient/lastIp","127.0.0.1").toString();
        const int lastPort = settings.value("UDPClient/lastPort",ui->spinBox_UDPClientPort->value()).toInt();

        //若下拉框控件中已有该IP则选中对应项,否则也可以编辑方式输入lastip
        int index=ui->comboBox_UDPClientIp->focusInEvent(lastIp);
        if(index>=0){
            ui->comboBox_UDPClientIp->setCurrentIndex(index);
        }   else{
            ui->comboBox_UDPClientIp->setEditable(lastIp);
        }

        //端口在控件范围内时才写入,避免非法值
        if(lastPort>=ui->spinBox_UDPClientPort->minimum()&&lastPort<= ui->spinBox_UDPClientPort->maximum()){
            ui->spinBox_UDPClientPort->setValue(lastPort);
        }
    }
}

//析构函数:断开套接字信号槽,关闭套接字,设置父为空后释放套接字和ui,异常由try-catch捕获
FormUDPClient::~FormUDPClient()
{
    qDebug()<<"FormUDPClient析构函数开始";
    try{
        if(UDPClientSocket){
            qDebug()<<"正在关闭udp客户端套接字...";
            UDPClientSocket->disconnect();
            UDPClientSocket->close();

            //将父对象滞空,避免delete时qt父子关系导致重复删除或异常
            UDPClientSocket->setParent(nullptr);
            delete UDPClientSocket;
            qDebug()<<"udp客户端套接字已关闭";
        }else{
            qDebug()<<"udp客户端套接字为空,无需关闭";
        }

        //释放Qt自生成的ui对象
        if(ui){
            qDebug()<<"正在删除UI...";
            delete ui;
            ui=nullptr;
            qDebug()<<"ui已经删除";
        }
    }catch (const std::exception& e){
        qWarning()<<"FromUDPClient析构时发生异常:" <<e.what();
    }catch(...){
        qWarning()<<"FromUDPClient析构时发生未知异常";
    }
    qDebug()<<"FormUDPClient析构函数结束";
}

// 发送按钮槽函数：懒创建UDP套接字并连接readyread；校验IP/端口/发送内容，编码为UTF-8发数据报
// 保存本次IP/端口到QSettings；在日志框追加发送记录并执行trimLog。
void FormUDPClient::on_pushButton_UDPClientSendMsg_clicked()
{
    //生成当前时间戳,用于日志中标记发送时间
    QString timestamp=QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");

    //懒创建:首次点击发送时才创建udp套接字,并连接到readyRead到接收槽
    if(!UDPClientSocket){
        UDPClientSocket=new QUdpSocket(this);
        connect(UDPClientSocket,&QUdpSocket::readyRead,this,&FormUDPClient::ReadServerDatagramFunc);
        socketReady=ture;
    }

    //从界面控件读取用户输入的服务器ip端口
    QString strIpAddress=ui->comboBox_UDPClientIp->currentText();
    int port=ui->spinBox_UDPClientPort->value();

    //使用InputValidator校验ip/主机名格式,非法则弹窗并返回
    auto ipValidation= InputValidator::validatorNetworkAddress(strIpAddress);
    if(!ipValidation.isValid){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,ipValidation.errorMessage,this);
        return;
    }

    //校验端口必须在1-65535之间
    if(port < 1 ||port >65535){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"端口号必须在1-65535范围内",this);
        return;
    }

    //将字符串转为QHostAddress,并要求非空且为ipv4
    QHostAddress hostAddress(strIpAddress);
    if(hostAddress.isNull() || hostAddress.protocol() != QAbstractSocket::IPv4Protocol){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"无效的ip地址",this);
        return;
    }
    QString message=ui->plainTextEdit_UDPClientSendData->toPlainText().trimmed();
    if(message.isEmpty()){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"发送内容不能为空",this);
        return;
    }
}



//日志裁剪
void FormUDPClient::trimLog(int keepBlocks,int trimStep)
{

}

//窗口关闭时保存日志并接受关闭
void FormUDPClient::FormUDPClient::closeEvent(QCloseEvent *event)
{

}


//将指定QPlainTextEdit的全部记录只在到应用数据目录下.追加模式,utf-8
void FormUDPClient::savePlainTextEditToFile(QPlainTextEdit * plainTextEdit)
{

}

void FormUDPClient::saveLog()
{

}



//读取服务器返回的数据并加载到日志框,由readyRead信号触发
void FormUDPClient::ReadServerDatagramFunc()
{

}



