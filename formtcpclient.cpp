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
        const int lastPort = settings.value("TCPClient/lastIp",ui->spinBox_TCPClientPort->value()).toInt(); //读取上次端口,默认当前的spinBox

        int index = ui->comboBox_TCPClientIP->findText(lastIp);
        if(index >= 0){
            ui->comboBox_TCPClientIP->setCurrentIndex(index);  //已存在则选中
        }else{
            ui->comboBox_TCPClientIP->setEditText(lastIp);  //不存在则直接填入可编辑框

        }
        if(lastPort >= ui->spinBox_TCPClientPort->minimum()&&lastPort <= ui->spinBox_TCPClientPort->maximum()){
            ui->spinBox_TCPClientPort->setValue(lastPort);
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

        connect(&NetworkClient,&Network::dataReceived,this ,[this](const QString &data){  //绑定"收到数据:的信号
            // 性能优化：appendPlainText会自动移动到底部，不需要手动设置cursor
            QString strTemp = QString("\n[%1] 接收: %2")               // 拼接时间戳与内容
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                  .arg(data);
            ui->listWidget_TCPClientMsg->appendPlainText(strTemp); // 追加日志（自动滚动到底部）

            trimLog();                                                 // 裁剪日志


        });
}
//析构函数,释放ui,Network内部自管资源
FormTcpClient::~FormTcpClient()
{
    qDebug()<<"FormTcpClient析构函数开始";
    try{
        if(ui){
            delete ui;
            ui=nullptr;
        }
    }catch(const std::exception& e){
        qWarning()<<"FormTcpClient析构函数发生异常:"<<e.what();
    }catch(...){
        qWarning()<<"FormTcpClient析构时发生未知异常";
    }
    qDebug()<<"FormTcpClient析构函数结束";
}

//发送连接,验证ip/端口,保存最近配置并请示连接
void FormTcpClient::on_pushButton_TCPClientConnect_clicked()
{
    try{
        if(!ui->pushButton_TCPClientConnect->isEnabled()){  //防止按扭被禁用时误触
            qDebug()  << "Connection button is disabled ,ignoring click";
            return;
        }
        connected = false ; //重置连接状态,等待结果

        QString ipAddress = ui->comboBox_TCPClientIP->currentText();  //读取
        int port = ui->spinBox_TCPClientPort->value() ; //读取端口

        auto ipValidation = InputValidator::validatorNetworkAddress(ipAddress);   //校验ip/主机名
        if(!ipValidation.isValid){
            HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,ipValidation.errorMessage,this); //弹出警告
            ui->comboBox_TCPClientIP->setFocus();  //聚焦便于修改
            return;
        }
        auto portValidation= InputValidator::validatorPort(port);  //效验端口范围
        if(!portValidation.isValid){
            HANDLE_ERROR(ErrorHandler::ValidationError, ErrorHandler::Warning,
                         portValidation.errorMessage, this);    // 弹警告
            ui->spinBox_TCPClientPort->setFocus();              // 聚焦端口
            return;
        }

        LOG_INFO("TCP客户端",QString("尝试连接到 %1:%2").arg(ipAddress).arg(port)); // 记录连接意图

        {                                           // 持久化最新IP/端口
            QSettings settings;
            settings.setValue("TCPClient/lastIp", ipAddress);
            settings.setValue("TCPClient/lastPort", port);
            settings.sync();                                   // 立即同步到磁盘，确保数据保存
        }
        ui->pushButton_TCPClientConnect->setEnabled(false);     // 禁用连接按钮
        ui->pushButton_TCPClientDisconnect->setEnabled(true);   // 允许断开

        NetworkClient.ClientConnectToServer(ipAddress, port);   // 发起连接

        QString strTemp = QString("[%1] 正在连接到 %2:%3...")        //追加正在连接日志
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(ipAddress)
                              .arg(port);
        ui->listWidget_TCPClientMsg->appendPlainText(strTemp) ;//appendPlainText会自动滚到底部
    }catch(const std::exception &e){    //捕获标准异常
        LOG_ERROR("TCP客户端",QStirng("连接时发生异常: %1").arg(e.what()));
        HANDLE_ERROR(ErrorHandler::UnknownError, ErrorHandler::Critical,QString("连接时发生异常:%1").arg(e.what()),this);

        ui->pushButton_TCPClientConnect->setEnabled(true);
        ui->pushButton_TCPClientDisconnect->setEnabled(false);

    }catch(...){
        LOG_ERROR("TCP客户端","连接时发生未知异常");
        HANDLE_ERROR(ErrorHandler::UnknownError, ErrorHandler::Critical,
                     "连接时发生未知异常", this);
        ui->pushButton_TCPClientConnect->setEnabled(true);
        ui->pushButton_TCPClientDisconnect->setEnabled(false);
    }
}

//断开连接 关闭socket并重置ui
void FormTcpClient::on_pushButton_TCPClientDisconnect_clicked()
{
    NetworkClient.DisconnectFromHost(); //主动断开连接

    ui->pushButton_TCPClientConnect->setEnabled(true);          // 允许重新连接
    ui->pushButton_TCPClientDisconnect->setEnabled(false);      // 禁用断开
    ui->pushButton_TCPClientSendMsg->setEnabled(false);         // 禁用发送
    ui->checkBox_TCPClientAutoText->setEnabled(false);       // 禁用自动测试
    ui->plainTextEdit_TCPClientSendData->setEnabled(false);     // 禁用输入框
    connected = false;                                          // 更新状态


     ui->listWidget_TCPClientMsg->addItem("\n[Prompt:Disconnect from server]"); // 记录提示（自动滚动到底部）
}

//关闭窗口
void FormTcpClient::on_pushButton_TCPClientClose_clicked()
{
    savePlainTextEditToFile(ui->listWidget_TCPClientMsg);  //退出前保存日志文件
    QCoreApplication::quit();   //触发应用退出
}

//发送消息 需要已经连接且输入非空
void FormTcpClient::on_pushButton_TCPClientSendMsg_clicked()
{
    QString message= ui->plainTextEdit_TCPClientSendData->toPlainText().trimmed();  //获取空白的输入
    if(message.isEmpty()){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"发送内容不能为空",this);
        return;
    }

    if(!connected){
         HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"未连接服务器,无法发送",this);
        return;
    }
    NetworkClient.ClientSendMsgToServer(message);                // 发送消息

    QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"); // 时间戳
    QString logEntry = QString("\n[%1]\n客户端发送: %2").arg(timestamp, message);     // 发送日志
    ui->listWidget_TCPClientMsg->appendPlainText(logEntry);                   // 追加日志（自动滚动到底部）

    if (!NetworkClient.strTempData.isEmpty()) {                  // 如果底层有附加日志，继续追加
        ui->listWidget_TCPClientMsg->appendPlainText(NetworkClient.strTempData);
    }

    trimLog();                                                   // 裁剪日志

}



// 自动测试开关：勾选则启用定时发送，取消则恢复手动
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
