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
    ui->comboBox_UDPServerIp->addItems(ipList);
    ui->comboBox_UDPServerIp->setUpdatesEnabled(true);

    //加载上次保存的配置,从QSetting读取上次的ip和端口
    QSettings settings;
    const QString lastIp = settings.value("UDPServer/lastIp").toString();
    const int lastPort = settings.value("UDPServer/lastPort",9999).toInt();

    if(!lastIp.isEmpty()){
        int index = ui->comboBox_UDPServerIp->findText(lastIp);

        if(index>= 0){
            ui->comboBox_UDPServerIp->setCurrentIndex(index);
        }else{
            ui->comboBox_UDPServerIp->setEditText(lastIp);
        }
    }

    if(lastPort>=ui->spinBox_UDPServerPort->minimum() &&
        lastPort<=ui->spinBox_UDPServerPort->maximum()){
        ui->spinBox_UDPServerPort->setValue(lastPort);
    }


    //创建udp套接字
    udpServerSocket = new QUdpSocket(this);

    //建立信号槽函数连接
    connect(udpServerSocket,&QUdpSocket::readyRead,this,&FormUDPServer::udpSeverReadPendingDatagrams);

    //设置界面各控件固定大小,防止抖动
    ui->groupBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->groupBox_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->listWidget_UDPServerMsg->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->label_5->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->label_6->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->label_7->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->comboBox_UDPServerIp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->spinBox_UDPServerPort->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_UDPServerQuit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->pushButton_UDPServerStartListen->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if(ui->pushButton_UDPServerSendMsg){
        ui->pushButton_UDPServerSendMsg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    }
    if(ui->plainTextEdit_UDPServerSendData){
        ui->plainTextEdit_UDPServerSendData->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    }

    //设置界面样式,使用QSS来美化界面效果,定义控件的视觉效果
    setStyleSheet(
        //基础窗口样式,设置所有控件默认状态下的背景颜色和文字颜色
        "QWidget{"
        "background-color : #F3F4f6"
        "  color: #1F2933;"                 // 深灰色文字（RGB: 31, 41, 51）
        "}"
        // 分组框样式：设置分组框（QGroupBox）的外观
        "QGroupBox {"
        "  font-size: 14px;"                // 字体大小 14 像素
        "  background-color: #FFFFFF;"      // 白色背景
        "  border: 1px solid #D0D7E2;"      // 浅灰色边框，1 像素实线
        "  border-radius: 6px;"             // 圆角半径 6 像素
        "  margin-top: 10px;"               // 上边距 10 像素
        "}"
        // 分组框标题样式：设置分组框标题的样式
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"      // 标题位置基于边距
        "  left: 10px;"                     // 距离左边 10 像素
        "  padding: 0 4px;"                 // 内边距：上下 0，左右 4 像素
        "  background-color: #F3F4F6;"      // 浅灰色背景（与窗口背景一致）
        "  color: #111827;"                 // 深灰色文字
        "}"
        // 标签样式：设置标签（QLabel）的样式
        "QLabel {"
        "  font-size: 14px;"                // 字体大小 14 像素
        "  color: #374151;"                 // 中灰色文字（RGB: 55, 65, 81）
        "  background-color: transparent;"  // 透明背景
        "}"
        // 下拉框和数字输入框样式：设置下拉框（QComboBox）和数字输入框（QSpinBox）的样式
        "QComboBox, QSpinBox {"
        "  font-size: 14px;"                // 字体大小 14 像素
        "  background-color: #FFFFFF;"      // 白色背景
        "  border: 1px solid #D0D7E2;"      // 浅灰色边框
        "  border-radius: 4px;"             // 圆角半径 4 像素
        "  padding: 2px 6px;"                // 内边距：上下 2 像素，左右 6 像素
        "}"
        // 文本编辑框样式：设置多行文本编辑框（QPlainTextEdit）的样式
        "QPlainTextEdit {"
        "  font-size: 14px;"                // 字体大小 14 像素
        "  background-color: #FFFFFF;"      // 白色背景
        "  border: 1px solid #D0D7E2;"      // 浅灰色边框
        "  border-radius: 4px;"             // 圆角半径 4 像素
        "}"
        // 按钮基础样式：设置按钮（QPushButton）的默认样式
        "QPushButton {"
        "  font-size: 14px;"                // 字体大小 14 像素
        "  background-color: #2563EB;"      // 蓝色背景（RGB: 37, 99, 235）
        "  color: #FFFFFF;"                 // 白色文字
        "  border-radius: 4px;"             // 圆角半径 4 像素
        "  padding: 4px 12px;"               // 内边距：上下 4 像素，左右 12 像素
        "  border: none;"                    // 无边框
        "}"
        // 按钮悬停样式：鼠标悬停在按钮上时的样式
        "QPushButton:hover {"
        "  background-color: #1D4ED8;"      // 深蓝色背景（悬停效果）
        "}"
        // 按钮按下样式：鼠标按下按钮时的样式
        "QPushButton:pressed {"
        "  background-color: #1E40AF;"      // 更深蓝色背景（按下效果）
        "}"
        // 按钮禁用样式：按钮被禁用时的样式
        "QPushButton:disabled {"
        "  background-color: #CBD5F5;"      // 浅蓝色背景（禁用状态）
        "  color: #6B7280;"                 // 灰色文字（禁用状态）
        "}"
        // 列表控件样式：设置列表控件（QListWidget）的样式
        "QListWidget {"
        "  font-size: 14px;"                // 字体大小 14 像素
        "  background-color: #FFFFFF;"      // 白色背景
        "  border: 1px solid #D0D7E2;"      // 浅灰色边框
        "  border-radius: 4px;"             // 圆角半径 4 像素
        "}"
        // 列表项选中样式：设置列表项被选中时的样式
        "QListWidget::item:selected {"
        "  background-color: #FEE2E2;"      // 浅红色背景（选中高亮）
        "}"

        );

    //设置列表控件属性
    ui->listWidget_UDPServerMsg->setUniformItemSizes(true);

    //设置选项模式
    ui->listWidget_UDPServerMsg->setSelectionMode(QAbstractItemView::SingleSelection);
}


//析构函数,清理资源,释放套接字和ui对象,确保资源正确释放
FormUDPServer::~FormUDPServer()
{
    qDebug()<<"FormUdpServer析构函数开始";
    try{
    if(udpServerSocket){
        udpServerSocket->disconnect();
        udpServerSocket->close();
        delete udpServerSocket;
        udpServerSocket = nullptr;
    }
    if(ui){
    delete ui;
        ui= nullptr;
    }
    }catch(const std::exception & e){
        qWarning() << "FormUDPServer析构时发生异常:" << e.what();

    }catch(...){
        qWarning() << "FormUDPServer析构时发生未知异常";

    }

    qDebug()<<"FormUdpServer析构函数结束";
    }
}


//启动停止按扭职责
//点击事件:切换成udp监听状态
// 启动/停止服务器按钮点击事件：切换UDP服务器的监听状态
//流程:读取ip地址和端口,验证输入有效性,启动监听,停止监听,更新文本按扭状态,保存配置到QSettings
void FormUDPServer::on_pushButton_UDPServerStart_clicked()
{
    //获取ip地址
    QString ip = ui->comboBox_UDPServerIp->currentText();

    //把ip地址字符串转换成QHostAddress对象,用于验证和绑定操作
    QHostAddress hostAddress(ip);

    //获取端口号
    qint64 port = ui->spinBox_UDPServerPort->value();

    //判断按扭状态,是启动还是停止
    if(ui->pushButton_UDPServerStartListen->text()=="启动监听"){
        auto ipValidation = InputValidator::validatorNetworkAddress(ip);
        if(!ipValidation.isValid){
            HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,ipValidation.errorMessage,this);

            return;
        }
        // 第二层验证：验证QHostAddress对象是否有效且为IPv4协议
        if(hostAddress.isNull() || hostAddress.protocol()!=QAbstractSocket::IPv4Protocol){
            HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"无效的IP地址",this);

            return;
        }
        // 第三层验证：验证端口号是否在有效范围内
        if(port < 1 || port > 65535){
            HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"端口号必须在1-65535范围内",this);

            return;
        }
        // 第四层验证：验证UDP套接字是否已初始化
        if(!udpServerSocket){
            HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Critical,"UDP套接字未初始化",this);

            return;
        }
        // 绑定IP地址和端口
        if(udpServerSocket->bind(hostAddress,port)){
            ui->pushButton_UDPServerStartListen->setText("关闭监听");
            udpServerAppendStrItem(1,"UDP服务器启动监听成功",false);
            serverRunning=true;

            // 保存配置IP和端口
            QSettings settings;
            settings.setValue("UDPServer/lastIp",hostAddress.toString());
            settings.setValue("UDPServer/lastPort",static_cast<int>(port));

            // 立即同步磁盘
            settings.sync();
        }else{
            // 绑定失败
            if(udpServerSocket){
                HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,udpServerSocket->errorString(),this);
            }else{
                HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,"UDP套接字未初始化",this);
            }
        }
    }else{
        // 停止监听：当前按钮文本为“关闭监听”，需要停止服务器监听
        if(udpServerSocket){
            udpServerSocket->abort();
        }

        // 更新按钮文本：将按钮文本改回“启动监听”，表示当前已停止，下次点击将启动
        ui->pushButton_UDPServerStartListen->setText("启动监听");

        // 追加状态日志
        udpServerAppendStrItem(1,"UDP服务器已停止监听",false);

        // 更新运行状态标志
        serverRunning=false;

    }
}

// 关闭按钮点击事件：保存日志并退出应用程序
void FormUDPServer::on_pushButton_UDPServerClose_clicked()
{
    saveListWidgetToFile(ui->listWidget_UDPServerMsg);

    QCoreApplication::quit();
}


// 发送测试消息按钮点击事件：向最近连接的客户端发送测试消息
/*处理流程：
1、验证套接字和服务器状态
2、验证是否有客户端地址
3、获取并验证消息内容
4、编码消息并发送
5、记录发送日志
*/

// 发送测试消息按钮点击事件：向最近连接的客户端发送测试消息
void FormUDPServer::on_pushButton_UDPServerSendMsg_clicked()
{
    if(!udpServerSocket){
        HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,"UDP套接字未初始化,请初始化",this);

        return;
    }
        if(!serverRunning){
            HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,"服务器未运行,请运行",this);
            return;
        }

        if(m_lastClientPort ==0 || m_lastClientAddress.isNull()){
            HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"当前还没有收到任何客户端的消息，无法确定发送目标。",this);

            return;
        }

        if(!ui->plainTextEdit_UDPServerSendData){
            return;
        }

        QString message=ui->plainTextEdit_UDPServerSendData->toPlainText().trimmed();

        // 第五层检查：验证消息内容是否为空
        if(message.isEmpty()){
            QMessageBox::warning(this,"提示","发送内容不能为空！");

            ui->plainTextEdit_UDPServerSendData->setFocus();

            return;
        }
        QByteArray data= message.toUtf8();

        //发送数据到客户端
        qint64 bytes = udpServerSocket->writeDatagram(data,m_lastClientAddress,m_lastClientPort);
        //检查发送结果,验证数据报是否都成功发送
        if(bytes == -1)
        {
            QMessageBox::warning(this,"错误",QString("发送失败: %1").arg(udpServerSocket->errorString()));

            return;
        }
        //记录发送日志
        QString timestamp =QDateTime::currentDateTime().toString("yyyy-mm-dd  hh:mm:ss");

        QString logEntry =QString("\n[%1] 服务器发送到 %2:%3\n%4\n ")
                               .arg(timestamp,m_lastClientAddress.toString())
                               .arg(m_lastClientPort)
                               .arg(message);

        //添加日志到列表
        ui->listWidget_UDPServerMsg->addItem(logEntry);
        //自动滚动到底部
        ui->listWidget_UDPServerMsg->scrollToBottom();

}

// 读取并处理收到的数据报：当UDP套接字有数据可读时自动调用
void FormUDPServer::udpSeverReadPendingDatagrams()
{
    if(!serverRunning){
        return;
    }
    if(!udpServerSocket){
        qWarning()<<"FormUDPServer::udpServerReadPendingDatagrams: udpServerScoket is null";
        return;
    }

    while(udpServerSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        qint64 datagramSize =udpServerSocket->pendingDatagramSize();
        if(datagramSize<= 0){
            break;  //无效的数据报大小
        }

        datagram.resize(static_cast<int>(datagramSize));
        QHostAddress clientAddress;
        quint16 clientPort;

        qint64 bytesRead=udpServerSocket->readDatagram(datagram.data(),datagram.size(),&clientAddress,&clientPort);

        if(bytesRead==-1){
            qWarning()<<"读取UDP数据报失败："<<udpServerSocket->errorString();
            continue;
        }

        m_lastClientAddress=clientAddress;
        m_lastClientPort=clientPort;

        udpServerAppendStrItem(0,QString::formUtf8(datagram),false);

        QByteArray reponse=QString("[Server reply:")
    }
}

// 追加日志项到列表：将消息或状态信息追加到日志列表控件
void FormUDPServer::udpServerAppendStrItem(int type,const QString &strData,bool clear)
{

}


// 日志裁剪函数：当日志列表项数超过阈值时，删除前部项以控制内存使用
void FormUDPServer::trimLog(int keepRows,int trimStep)
{

}

// 窗口关闭事件处理函数：当窗口关闭时自动调用，保存日志并接受关闭
void FormUDPServer::closeEvent(QCloseEvent *event)
{

}



// 保存日志列表到文本文件：将QListWidget中的所有项保存到文本文件
void FormUDPServer::saveListWidgetToFile(QListWidget* listWidget)
{

}

// 保存日志函数：保存当前界面的日志列表到文件（便捷掊）
void FormUDPServer::saveLog()
{

}



