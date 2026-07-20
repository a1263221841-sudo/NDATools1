 #include "formudpclient.h"
#include "ui_formudpclient.h"
#include "logpathhelper.h"
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
    ui->pushButton_UDPClientSendMsg->setIconSize(QSize(16,16));

    //DPI设置已在main.cpp中统一配置,此处不再重复设置
    //从QSettings恢复上次使用的ip和端口,便于用户连续使用同一目标
    {
        QSettings settings;
        const QString lastIp =settings.value("UDPClient/lastIp","127.0.0.1").toString();
        const int lastPort = settings.value("UDPClient/lastPort",ui->spinBox_UDPClientPort->value()).toInt();

        //若下拉框控件中已有该IP则选中对应项,否则也可以编辑方式输入lastip
        int index=ui->comboBox_UDPClientIp->findText(lastIp);
        if(index>=0){
            ui->comboBox_UDPClientIp->setCurrentIndex(index);
        }   else{
            ui->comboBox_UDPClientIp->setEditText(lastIp);
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
        socketReady=tr;
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
    if( port < 1 ||port >65535){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"端口号必须在1-65535范围内",this);
        return;
    }

    //将字符串转为QHostAddress,并要求非空且为ipv4
    QHostAddress hostAddress(strIpAddress);
    if(hostAddress.isNull() || hostAddress.protocol() != QAbstractSocket::IPv4Protocol){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"无效的ip地址",this);
        return;
    }
    //获取发送框件内容并去除收尾空白,空内容不允许发送
    QString message=ui->plainTextEdit_UDPClientSendData->toPlainText().trimmed();
    if(message.isEmpty()){
        HANDLE_ERROR(ErrorHandler::ValidationError,ErrorHandler::Warning,"发送内容不能为空",this);
        return;
    }

    //防御性检查:确保套接字已创建(理论上懒创建之后不应为空)
    if(!UDPClientSocket){
        HANDLE_ERROR(ErrorHandler::NetworkError,ErrorHandler::Warning,"UDP套接字未初始化",this);
        return;
    }

    //将QString转为UTF-8字节数组,并调用writeDatagram发送到指定地址和端口
    QByteArray data = message.toUtf8();
    qint64 byteWritten = UDPClientSocket->writeDatagram(data,hostAddress,port);
    if(byteWritten == -1){
        HANDLE_ERROR(ErrorHandler::NetworkError, ErrorHandler::Warning,
                     QString("发送失败: %1").arg(UDPClientSocket->errorString()), this);
        return;
    }

    //将本次使用的ip和端口写入QSettings,下次打开界面时自动恢复
    {
        QSettings settings;
        settings.setValue("UDPClient/lastIp",strIpAddress);
        settings.setValue("UDPClient/lastPort",static_cast<int>(port));
        settings.sync();
    }
    //在日志列表中追加一行:"时间戳+"Spending:
    ui->plainTextEdit_UDPClientMsgList->appendPlainText(QString("\n%1\nSending:%2").arg(timestamp,message));

    trimLog();
}




//日志裁剪
void FormUDPClient::trimLog(int keepBlocks,int trimStep)
{
    //定义默认值常量,使用静态常量定义默认的保留行数和裁剪步长
    static const int kKeepDefault=1000;
    static const int kTrimDefault=200;

    //确定实际保留行数:如果参数有效则使用参数值,否则使用默认值
    const int keep = keepBlocks>0?keepBlocks:kKeepDefault;
    const int step = trimStep>0?trimStep:kTrimDefault;

    //获取日志多行文本框对应的QTextDocument及当前块数
    auto doc =ui->plainTextEdit_UDPClientMsgList->document();
    int blocks=doc->blockCount();

    //块数未超过保留数+步长则不裁剪,直接返回
    if(blocks<=keep +step)  {
        return;
    }

    //计算需要从文档开关删除的块数
    int removeBlocks= blocks-keep;
    QTextCursor cursor(doc);

    //将光标移到文档开头
    cursor.movePosition(QTextCursor::Start);

    //从开头向下扩展选区
    for(int i =0; i<removeBlocks;i++){
        cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor);
    }

    //删除选中的文本,并再删除一个字符以去掉可能残留的换行,保存文档的整洁性
    cursor.removeSelectedText();
    cursor.deleteChar();
    
}

//窗口关闭时保存日志并接受关闭
void FormUDPClient::FormUDPClient::closeEvent(QCloseEvent *event)
{
    saveLog();
    event->accept();
}


//将指定QPlainTextEdit的全部记录只在到应用数据目录下.追加模式,utf-8
void FormUDPClient::savePlainTextEditToFile(QPlainTextEdit * plainTextEdit)
{
    if(!plainTextEdit){
        qWarning()<<"FormUDPClient::savePlainTextEditToFile : plainTextEdit is null";
        return;
    }

    const QString logDir = ensureProjectLogDirPath(QStringLiteral("udp"));

    // 在数据目录下拼接日志文件名
    QString fileName = QDir(logDir).filePath("UDPClientLogfile.txt");

    // 根据文件是否已存在决定以追加或覆盖方式打开
    bool fileExists = QFile::exists(fileName);
    QIODevice::OpenMode openMode = fileExists ?
                                       (QIODevice::Append | QIODevice::Text) :
                                       (QIODevice::WriteOnly | QIODevice::Text);


    QFile file(fileName);
    if (file.open(openMode)) {
        QTextStream out(&file);
        /* 设置输出流编码为UTF-8，Qt5与Qt6的API不同 */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec("UTF-8");
#else
        out.setEncoding(QStringConverter::Utf8);
#endif

        /* 追加模式时先写入换行、分隔符与当前时间戳，便于区分多次保存的内容 */
        if (fileExists) {
            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out << "\n" << "========== " << timestamp << " ==========" << endl;
#else
            out << "\n" << "========== " << timestamp << " ==========" << Qt::endl;
#endif
        }

        /* 将文本框全文写入文件，然后关闭文件 */
        out << plainTextEdit->toPlainText();
        file.close();
        qDebug() << "UDP客户端日志已保存至:" << fileName;
        if (this->isVisible()) {
            QMessageBox::information(this, "成功", QString("日志已保存至 %1").arg(fileName));
        }
    } else {
        qWarning() << "UDP客户端日志保存失败:" << file.errorString();
        if (this->isVisible()) {
            QMessageBox::critical(this, "错误", "保存失败: " + file.errorString());
        }
    }


}

//保存当前洁面膏日志到文件
void FormUDPClient::saveLog()
{
    if(ui && ui->plainTextEdit_UDPClientMsgList){
        savePlainTextEditToFile(ui->plainTextEdit_UDPClientMsgList);
    }
}



//读取服务器返回的数据并加载到日志框,由readyRead信号触发
void FormUDPClient::ReadServerDatagramFunc()
{
    // 如果套接字未创建或已释放，直接返回，避免空指针访问
    if(!UDPClientSocket){
        return;
    }

    // 循环处理缓冲区中所有等待读取的数据报，直到没有更多数据
    while(UDPClientSocket->hasPendingDatagrams()){
        // 获取当前队首数据报的大小，用于预分配缓冲区
        qint64 datagramSize = UDPClientSocket->pendingDatagramSize();
        if (datagramSize <= 0) {
            break;
        }

        // 分配足够大的QByteArray用于接收数据，并准备接收发送方的地址与端口（未使用）
        QByteArray data;
        data.resize(static_cast<int>(datagramSize));
        QHostAddress senderAddress;
        quint16 senderPort = 0;

        // 从套接字读取一个数据报到data，并可选地得到发送方地址与端口
        qint64 bytesRead = UDPClientSocket->readDatagram(data.data(), data.size(),
                                                         &senderAddress, &senderPort);
        if (bytesRead == -1) {
            qWarning() << "读取UDP数据报失败:" << UDPClientSocket->errorString();
            continue;
        }

        /* 若实际读取字节数小于预分配大小，缩小data避免末尾未初始化数据参与解码 */
        if (bytesRead < datagramSize) {
            data.resize(static_cast<int>(bytesRead));
        }

        /* 生成当前时间戳，并按UTF-8将数据解码为QString */
        QString timestamp = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        QString payload = QString::fromUtf8(data);

        /* 在日志列表中追加一行：时间戳 + “Receive:” + 接收内容 */
        ui->plainTextEdit_UDPClientMsgList->appendPlainText(
            QString("\n%1\nReceive:%2").arg(timestamp, payload));
    }

    /* 接收一批数据后统一做一次日志裁剪，防止块数过多导致卡顿 */
    trimLog();

}



