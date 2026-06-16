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
            ui->listWidget_TCPClientMsg->addItem(strTemp); // 追加日志（自动滚动到底部）
            ui->listWidget_TCPClientMsg->scrollToBottom();
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
        ui->listWidget_TCPClientMsg->addItem(strTemp) ;//appendPlainText会自动滚到底部
        ui->listWidget_TCPClientMsg->scrollToBottom();
    }catch(const std::exception &e){    //捕获标准异常
        LOG_ERROR("TCP客户端",QString("连接时发生异常: %1").arg(e.what()));
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
    //savePlainTextEditToFile(ui->listWidget_TCPClientMsg);  //退出前保存日志文件
    // 获取 AppData 路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDir(appDataPath);
    if (!appDir.exists())
        appDir.mkpath(".");

    // 构建日志文件路径（例如按时间命名）
    QString fileName = appDataPath + "/log_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".txt";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // 遍历 QListWidget 的所有条目
        for (int i = 0; i < ui->listWidget_TCPClientMsg->count(); ++i) {
            QListWidgetItem *item = ui->listWidget_TCPClientMsg->item(i);
            if (item)
                out << item->text() << "\n";
        }
        file.close();
    }
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
    ui->listWidget_TCPClientMsg->addItem(logEntry);                   // 追加日志（自动滚动到底部）
    ui->listWidget_TCPClientMsg->scrollToBottom();
    if (!NetworkClient.strTempData.isEmpty()) {                  // 如果底层有附加日志，继续追加
        ui->listWidget_TCPClientMsg->addItem(NetworkClient.strTempData);
        ui->listWidget_TCPClientMsg->scrollToBottom();
    }

    trimLog();                                                   // 裁剪日志

}



// 自动测试开关：勾选则启用定时发送，取消则恢复手动
void FormTcpClient::on_checkBox_TCPClientAutoTesting_clicked()
{
    bool isAutoTesting = ui->checkBox_TCPClientAutoText->isChecked();  //读取复选框状态

    if(isAutoTesting){   //开启自动测试
        if(!connected){
            HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,"未连接服务器,无法开始自动测试",this);
            ui->checkBox_TCPClientAutoText->setChecked(false);  //回退勾选
            return;
        }

        //从消息控件里面获取文本信息
        QString message = ui->plainTextEdit_TCPClientSendData->toPlainText().trimmed();
        if(message.isEmpty()){
            HANDLE_ERROR(ErrorHandler::ValidationError, ErrorHandler::Warning, "发送内容不能为空，无法开始自动测试！", this);
            ui->checkBox_TCPClientAutoText->setChecked(false); // 回退勾选
            return;
        }

        //设置自动发送测试消息内容
        NetworkClient.setAutoTestMessage(message);   //启动自动发送定时器
        NetworkClient.StartTimerOutFunc();
        ui->plainTextEdit_TCPClientSendData->setEnabled(false);  //禁用手动输入
        ui->pushButton_TCPClientSendMsg->setEnabled(false);
    }else{
        NetworkClient.StopTimerOutFunc();  //停止计时器
        enableCommunicationContorols(true); //重新启用手动交互
    }
}
//日志裁剪,保留行数/超限删除步长
void FormTcpClient::trimLog(int keepBlocks,int trimStep)
{
    static const int kKeepDefault = 1000;  //默认保留block数
    static const int kTrimDefault = 200;    //默认删除步长
    const int keep =keepBlocks >0 ? keepBlocks :kKeepDefault;  //实际保留数
    const int step = trimStep >0 ? trimStep :kTrimDefault;  //实际步长
    auto doc = ui->plainTextEdit_TCPClientSendData->document();
    int blocks = doc->blockCount();  //当前的额block数
    if(blocks <= keep + step) return ;
    int removeBlocks = blocks -keep;   //需删除的block数
    QTextCursor cursor(doc);  //创建光标
    cursor.movePosition(QTextCursor::Start);  //移到开头
    for(int i=0; i<removeBlocks; i++){
        cursor.movePosition(QTextCursor::NextBlock ,QTextCursor::KeepAnchor);
    }
    cursor.removeSelectedText();  //删除选中文本
    cursor.deleteChar();  //删除残留换行
}

//IP合法性校验(保留旧接口)
bool FormTcpClient::CheckIPAddrIsValid(QString strIpAddress)
{
    QRegularExpression ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"); // IPv4 正则
    return ipRegex.match(strIpAddress).hasMatch();              // 匹配成功则合法

}

//批量控制与连接相关的UI
void FormTcpClient::enableCommunicationContorols(bool enable)
{
    ui->pushButton_TCPClientConnect->setEnabled(!enable);       // 启用通信时禁用“连接”
    ui->pushButton_TCPClientDisconnect->setEnabled(enable);     // 启用/禁用“断开”
    ui->pushButton_TCPClientSendMsg->setEnabled(enable);        // 启用/禁用“发送”
    ui->checkBox_TCPClientAutoText->setEnabled(enable);      // 启用/禁用自动测试
    ui->plainTextEdit_TCPClientSendData->setEnabled(enable);    // 启用/禁用输入框

}


//窗口关闭事件,保存日志并接受关闭
void FormTcpClient::closeEvent(QCloseEvent *event)
{
    // 如果已连接，先断开连接
    if (connected) {
        NetworkClient.DisconnectFromHost();
        connected = false;
    }

    saveLog();
    event->accept();

}

//保存日志文本到文件
void FormTcpClient::saveListWidgetToFile(QListWidget* listWidget)
{
    if(!listWidget){
        qWarning() <<"FormTcpClient::savePlainTextToFile: listWidget is null";
        return;
    }

    //获取配置文件所在目录（与QSettings配置文件同一目录）
    //使用AppDataLocation的父目录,确保与ini文件在同一目录,%APPDATA%\NDATools
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);
    //如果路径以应用名换成呢归结为,
    QString appName = QApplication::applicationName();
    if (!appName.isEmpty() && (appDataPath.endsWith("/" + appName) ||
                               appDataPath.endsWith("\\" + appName))) {
        QDir parentDir = appDataDir;
        if (parentDir.cdUp()) {
            appDataPath = parentDir.absolutePath();
        }
    }
    QDir().mkpath(appDataPath); // 确保目录存在

    // 使用固定文件名，追加模式
    QString fileName = QDir(appDataPath).filePath("TCPClientLogfile.txt");

    // 检查文件是否存在，决定是追加还是创建
    bool fileExists = QFile::exists(fileName);
    QIODevice::OpenMode openMode = fileExists ?
                                       (QIODevice::Append | QIODevice::Text) :
                                       (QIODevice::WriteOnly | QIODevice::Text);

    QFile file(fileName);
    if (file.open(openMode)) {
        QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec("UTF-8"); // Qt5设置编码
#else
        out.setEncoding(QStringConverter::Utf8); // Qt6设置编码
#endif

        // 如果是追加模式，先写入分隔符和时间戳
        if (fileExists) {
            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out << "\n" << "========== " << timestamp << " ==========" << endl;
#else
            out << "\n" << "========== " << timestamp << " ==========" << Qt::endl;
#endif
        }

        // 【关键修改点：遍历 QListWidget 提取所有文本行】
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem *item = listWidget->item(i);
            if (item) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                out << item->text() << endl;
#else
                out << item->text() << Qt::endl;
#endif
            }
        }
        file.close();
        qDebug() << "TCP客户端日志已保存至:" << fileName;
        // 注意：窗口关闭时不显示消息框，避免阻塞
        if (this->isVisible()) {
            QMessageBox::information(this, "成功", QString("日志已保存至 %1").arg(fileName));
        }
    } else {
        qWarning() << "TCP客户端日志保存失败:" << file.errorString();
        if (this->isVisible()) {
            QMessageBox::critical(this, "错误", "保存失败: " + file.errorString());
        }
    }


}
//保存日志
void FormTcpClient::saveLog()
{
    if(ui &&ui->listWidget_TCPClientMsg){
      saveListWidgetToFile(ui->listWidget_TCPClientMsg);
    }
}



