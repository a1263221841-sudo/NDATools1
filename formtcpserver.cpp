#include "formtcpserver.h"
#include "ui_formtcpserver.h"
#include <QApplication>

// 功能：构造函数，初始化 UI、服务器实例、样式与信号
// 说明：恢复上次 IP/端口，填充本机 IPv4，下拉与按钮初始状态，应用统一 QSS

FormTcpServer::FormTcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormTcpServer)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);  //服务器实例,父对象负责回收
    //监听新连接信号
    connect(tcpServer ,&QTcpServer::newConnection,this,&FormTcpServer::TcpServerConnectedFunc);

    ui->pushButton_TCPServerCloseListen->setEnabled(false);  //关闭监听禁用
    ui->comboBox_TCPServerIp->setEditable(true);  //设置ip下拉框为可编辑,允许手动输入ip地址

    //枚举本机iPV4 ,填充下拉框
    QList<QHostAddress> addList = QNetworkInterface::allAddresses();
    QStringList ipList;
    ipList.reserve(addrList.size());//顶分配空间

    for(const QHostAddress &address : addrList){
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            ipList.append(address.toString());
        }
    }

    //批量添加ui地址,减少ui更新次数
    ui->comboBox_TCPServerIp-setUpdatesEnabled(false);
    ui->comboBox_TCPServerIp->addItem(ipList);
    ui->comboBox_TCPServerIp->setUpdatesEnabled(true);


    //恢复上次使用的ip与端口
    {
        QSettings settings;                         //使用默认组织/应用名保存配置
        qDebug()<<"QSettings配置文件路径:"<<settings.fileName();  //输出文件路径路径
        const QString lastIp = settings.value("TCPServer/lastIp").toString();
        const int lastPort = settings.value("TCPServer/lastPort",12345).toInt();  //上次端口

        if(!lastIp.isEmpty()){
            int index = ui->comboBox_TCPServerIp->findText(lastIp);
            if(index >=0){
                ui->comboBox_TCPServerIp->setCurrentIndex(index);  //已存在则选中
            }else{
                ui->comboBox_TCPServerIp->setEditText(lastIp);  //不存在则直接填入可编辑框
            }
        }

        if(lastPort>=ui->spinBox_TCPServerPort->minimum()&& lastPort<= ui->spinBox_TCPServerPort->maximum()) {
            ui->spinBox_TCPServerPort->setValue(lastPort);  //恢复端口
        }
    }

    //控件尺寸策略,固定,避免布局抖动
    ui->groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->groupBox_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->listWidget_TCPServerMsg->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->label_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ui->comboBox_TCPServerIp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->spinBox_TCPServerPort->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_TCPServerQuit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_TCPServerCloseListen->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_TCPServerStartListen->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    if(ui->plainTextEdit_TCPServerSendData){
        ui->plainTextEdit_TCPServerSendData->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    }
    if(ui->pushButton_TCPServerSendMsg){
        ui->pushButton_TCPServerSendMsg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    }


    // QSS：统一浅色主题、字号与按钮/列表样式
    setStyleSheet( // 统一界面样式（浅色系）
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
        "QComboBox, QSpinBox {"
        "  font-size: 14px;"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #D0D7E2;"
        "  border-radius: 4px;"
        "  padding: 2px 6px;"
        "}"
        "QPlainTextEdit {"
        "  font-size: 14px;"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #D0D7E2;"
        "  border-radius: 4px;"
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
        "QListWidget {"
        "  font-size: 14px;"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #D0D7E2;"
        "  border-radius: 4px;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #FEE2E2;"
        "}"
        ); // 结束样式表定义


}

//析构函数,清理Socket,关闭服务器与ui
//说明:主动断开/关闭所有socket ,防止资源
FormTcpServer::~FormTcpServer()
{
    qDebug()<<"FormTcp析构函数开始";
    try{
        for(auto client :tcpServerSocketList)  {  //遍历所有客户端
            if(client){
                client->disconnect();  //断开信号槽

                if(client->state() != QAbstractSocket::UnconnectedState){
                    client->abort();  //立即中断
                }

                client->setParent(nullptr);   //解除父子关系
                delete client;  //删除实例
            }
        }

        tcpServerSocketList.clear();  //清空列表

        if(tcpServer){
            tcpServer->disconnect(); //断开信号槽
            tcpServer->close();   //关闭监听
        }

        tcpServer =nullptr; //滞空指针
        tcpServerSocket = nullptr;

    if(ui){
    delete ui;
        ui= nullptr;
    }
    }catch(const std::exception& e){
        qWarning()<< "FormTcpServer析构时发生异常:"<<e.what();  //捕获标准异常
    }catch(...){
        qWarning()<<"FormTcpServer析构时发生未知异常"<<  //捕获未知异常
    }
    qDebug()<<"FormTcpServer析构函数完成"; //记录析构结束
}


//功能:启动监听 校验ip/端口并持久化配置
//防止重复启动 .基础合法性校验后调用listen 成功则记忆,配置并更新按扭状态
void FormTcpServer::on_pushButton_TCPServerStart_clicked()
{
    QString ipText= ui->comboBox_TCPServerIp->currentText(); //读取选中ip
    QHostAddress address(ipText);
    int port = ui->spinBox_TCPServerPort->value();   //读取端口


    if(serverRunning){
        appendColorLog("[Server already running",QColor("#666666"));  //提示
        return;  //直接返回
    }

    //验证QHostAdress是否有效
    if(address.isNull() || !CheckIPAddressValid(ipText)){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"Invalid IP address",this);
        return;
    }
     if (port < 1 || port > 65535){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"Invalid IP address",this);
        return;
    }
    if(!tcpServer){
        HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Critical,"TCP服务器未初始化",this);
        return;
    }
    if(tcpServer->listen(address,port)){     //尝试开始监听
        appendColorLog("[Server already running",QColor("#666666"));  //提示
        ui->pushButton_TCPServerStartListen->setEnabled(false);
        ui->pushButton_TCPServerCloseListen->setEnabled(true);
        serverRunning =true;

        QSettings settings;
        settings.setValue("TCPServer/lastIp",address.toString());  //记忆最后连接的ip
        settings.setValue("TCPServer/lastPort",port);  //端口号
        settings.sync();  //同步到磁盘
    }else{
        appendColorLog(QString("[Failed to start server]: %1").arg(tcpServer->errorString()), QColor("#CC0000"));
    }

}

//停止监听,并断开所有的客户端
//关闭服务器监听,断开并清空所有socket,复位按扭状态
void FormTcpServer::on_pushButton_TCPServerStop_clicked()
{
    if(!serverRunning){
        appendColorLog("[Server not running",QColor("#666666")); //提示
        return;
    }
    if(!tcpServer){
        appendColorLog("[Server instance is null",QColor("#CC0000"));
        return;
    }
    tcpServer->close();  //停止监听

    //断开所有客户端链接
    QList<QTcpSocket*> clientToClose =tcpServerSocketList;  //复制列表,避免迭代时修改
    for(auto client : clientToClose){  //遍历所有客户端并关闭
        if(client){
            //断开信号槽,避免在关闭过程中 触发ClientDisconnectedFunc
            client->disconnect();  //断开信号槽

            if(client->state() != QAbstractSocket::UnconnectedState){
                client->abort();     //立即中断连接
            }

            //从列表中移除(如果还在列表中)
            tcpServerSocketList.removeOne(client);

            //异步删除,避免在信号处理中直接删除
            client->deleteLater();
        }
    }

    tcpServerSocketList.clear();    //清空列表
    appendColorLog("\n[Prompt:Disconnect all client connections.]\n",QColor("#666666"));

    ui->pushButton_TCPServerStartListen->setEnabled(true);  //允许重新启动
    ui->pushButton_TCPServerCloseListen->setEnabled(true);  //禁用停止按扭
    serverRunning = false;
}

//关闭窗口,保存日志并退出
void FormTcpServer::on_pushButton_TCPServerClose_clicked()
{
    saveListWidgetToFile(ui->listWidget_TCPServerMsg);  //退出前保存日志到文件

    QCoreApplication::instance()->quit();
}

//服务器主动群发输入框中的消息,
//说明校验服务运行,文本非空且存储客户端,再将utf-8字节广播给所有连接
void FormTcpServer::on_pushButton_TCPServerSendMsg_clicked()
{
    if(!ui->plainTextEdit_TCPServerSendData){
        return;
    }
    if(!serverRunning){//
        HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,"服务器未启动,无法发送",this);
        return;
    }

    QString message = ui->plainTextEdit_TCPServerSendData->toPlainText().trimmed();  //获取输入
    if(message.isEmpty()){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"发送内容不能为空",this);
        return;
    }

    //无连接客户端
    if(tcpServerSocketList.isEmpty()){
        HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,"当前为连接任何客户端,无法发送测试消息",this);
        return;
    }

    QByteArray data = message.toUtf8();

    for(QTcpSocket* client:qAsConst(tcpServerSocketList)){   //群发
        if(client && client->state() == QAbstractSocket::ConnectedState){
            client->write(data);
        }
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"); // 时间戳
    QString logEntry = QString("\n[%1] Server send: %2\n").arg(timestamp, message);   // 组装日志
    appendColorLog(logEntry, QColor("#008000"));                                     // 绿色提示

}


//功能:在日志列表追加彩色行
void FormTcpServer::appendColorLog(const QString &text,const QColor &color)
{
    //去掉头尾行并把内部换行压成空格,避免QListWidget
    QString sanitized = text;
    sanitized.replace('\n' ,' ');
    sanitized = sanitized.simplified();
    if(sanitized.isEmpty())
        return;

    //性能优化:批量的更新ui,减少重绘次数
    ui->listWidget_TCPServerMsg->setUpdatesEnabled(false);

    //创建项目并直接设置颜色,避免后续查找
    QListWidgetItem *item = new QListWidgetItem(sanitized);
    item->setForeground(color);
    ui->listWidget_TCPServerMsg->addItem(item);

    trimLog();  //防止日志过长导致ui卡顿

    //恢复更新并一次性滚动
    ui->listWidget_TCPServerMsg->setUpdatesEnabled(true);

    //自动回滚到底部,显示最新消息(只调用一次)
    int row = ui->listWidget_TCPServerMsg->count() -1;
    if(row >=0){
        ui->listWidget_TCPServerMsg->scrollToItem(item,QAbstractItemView::PositionAtBottom);
        ui->listWidget_TCPServerMsg->setCurrentRow(row);  //选中最新行
    }
}

//裁剪日志行数,防止控件过大
//keepRows,期望保留行数,trimStep ,每次删除的行数
void FormTcpServer::trimLog(int keepRows, int trimStep)
{
    static const int kKeepDefalut = 1000;  //默认保留行
    static const int kTrimDefault = 200;  //默认删除步长
    const int targetKeep = keepRows > 0? keepRows :kKeepDefalut;  //实际保留行
    const int step =trimStep >0 ? trimStep :kTrimDefault;

    int count = ui->listWidget_TCPServerMsg->count();  //当前行数
    if(count <= targerKeep +step)
        return;
    int removeCount = count - targetKeep;

//性能优化:批量删除时禁用更新,减少ui重绘
    if(removeCount > 10 ){  //只有删除数量较多时才禁用更新
        ui->listWidget_TCPServerMsg->setUpdatesEnabled(false);

    }
    //批量删除项目
    QList<QListWidgetItem*> itemsToDelete;
    for(int i = 0; i <removeCount; i++){
        QListWidgetItem *item =ui->listWidget_TCPServerMsg->takeItem(0);
        if(item){
            itemsToDelete.append(item);
        }
    }


//恢复更新
    if(removeCount > 10){
        ui->listWidget_TCPServerMsg->setUpdatesEnabled(true);
    }
    //批量删除项目 (在UI更新恢复后)
    qDeleteAll(itemsToDelete);
}


//检验ip字符串是否合法
bool FormTcpServer::CheckIPAddressValid(QString strIpAddress)
{

}

//将列表控件内部保存到文件(追加或覆盖由实现决定)
void FormTcpServer::saveListWidgetToFile(QListWidget* listwidget)
{

}

//保存日志
void FormTcpServer::saveLog()
{

}


//窗口关闭事件,保存日志,清理资源
void FormTcpServer::closeEvent(QCloseEvent *event)
{

}


//处理新客户链接,记录并更新列表
void FormTcpServer::TcpServerConnectedFunc()
{

}

//功能:处理客户端断开,清理列表与ui状态
void FormTcpServer::CientDisconnectedFunc()
{

}

//功能:读取所有可用数据并计数,展示日志
void FormTcpServer::ReadAllDataFunc()
{

}