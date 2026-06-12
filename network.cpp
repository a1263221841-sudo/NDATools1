#include "network.h"


#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif


//构造函数,初始化网络客户端对象,创建套接字和定时器,建立信号槽连接
 Network::Network(QObject *parent)
  : QObject(parent)
    , testMessageCount(0)
    ,receiveBuffer()
    ,performanceTimer()
    ,bufferMutex()
{
    //在堆上创建QTcpSocket对象
    socket= new QTcpSocket(this);

    //创建QTimeer(this)对象
    timer =new QTimer(this);

    //未接收缓冲区预分配内存空间,避免频繁重新分配(性能优化)
    receiveBuffer.reserve(8192);

    //设置套接字选项
    setSocketOptions();

    //信号槽函数连接
    connect(socket,&QTcpSocket::connected,this ,&Network::onConnected,Qt::QueuedConnection);
    connect(socket,&QTcpSocket::disconnected,this ,&Network::ClientDisconnectionedFunc,Qt::QueuedConnection);
    connect(socket,&QTcpSocket::readyRead,this ,&Network::ReadServerMsg,Qt::DirectConnection);

    connect(socket,&QAbstractSocket::errorOccurred,this,&Network::onSocketError);

    connect(timer,&QTimer::timeout,this,&Network::StartTimerOutFunc,Qt::DirectConnection);

    //设置定时器类型为精确定时器
    timer->setTimerType(Qt::PreciseTimer);
}

//析构函数  //清理资源
Network::~Network(void)
{
    qDebug()<<"Network析构函数开始";
    try{
        if(timer){
            timer->stop();
            timer->disconnect();
        }
        if(socket){
            socket->disconnect();


        if(socket->state()!=QAbstractSocket::UnconnectedState){
            socket->disconnectFromHost();
            if(socket->state()!=QAbstractSocket::UnconnectedState){
                socket->abort();
                }
            }
        }
    }catch(const std::exception& e){
        qDebug()<<"Network析构时发生异常"<<e.what();
    }catch(...){
        qDebug()<<"Network析构时发生未知异常";
    }
    qDebug()<<"Network析构函数完成";
}



//客户端连接到服务器:尝试建立Tcp到指定的服务器地址与端口
bool Network::ClientConnectToServer(QString ServerIpAddress,int ServerPort)
{
    //获取套接字状态
    QAbstractSocket::SocketState currentState=socket->state();

    //检查套接字是否处于连接状态
    if(currentState==QAbstractSocket::ConnectedState  ||currentState==QAbstractSocket::ConnectingState){
        //检查是否已连接到相同的服务器地址和端口,如果相同的则返回成功
        if(socket->peerAddress().toString()==ServerIpAddress &&socket->peerPort()==ServerPort)
        {
            // 如果已连接到相同的地址和端口，输出调试信息并直接返回成功（避免重复连接）
            qDebug()<<"Already connected to"<<ServerIpAddress<<":"<<ServerPort;
            return true;
        }
        // 如果已经连接到不同的地址，需要先断开当前连接才能建立新连接
        qDebug()<<"Disconnecting from current connection before connecting to new address";

        // 请求断开当前连接，优雅断开，等待未发送数据完成（异步操作）
        socket->disconnectFromHost();

        // 检查断开操作后的状态：如果仍未断开（可能在阻塞状态），则强制中断
        if(socket->state()!=QAbstractSocket::UnconnectedState){
            socket->abort(); // 立即强制关闭套接字
        }

    }

    //最终状态检验  确保套接字处于未连接状态才发起新连接
    if(socket->state()!= QAbstractSocket::UnconnectedState)
    {
        qWarning()<<"Socket is not in unconnected state,aboring connection";
        return false;
    }

    //发送连接
    qDebug()<<"Connecting to"<<ServerIpAddress<<":"<<ServerPort;

    //异步连接:connectToHost是异步操作,不会阻塞,连接结果通过Connected或errorOccurred 信号处理
    socket->connectToHost(ServerIpAddress,ServerPort);

    //返回成功
    return true;
}

//客户端发送文本文件到服务器将utf-8编码的字符串发送到服务器
void Network::ClientSendMsgToServer(const QString &StrData)
{
    //第一层 检查套接字是否有效且处于已连接状态
    if(!socket || socket->state()!=QAbstractSocket::ConnectedState){
        qWarning()<<"Socket not connected,cannot send data";
        return;
    }

    //编码转换
    const QByteArray data = StrData.toUtf8();

    //发送数据  //将字节数组写入套接字,发送到服务器
    const qint64 bytesWritten=socket->write(data);

    //第二层检查:判断数据是否写入失败
    if(bytesWritten<0){
        qWarning()<<"Write failed:"<<socket->errorString();
        return ;
    }

    //第三层处理,处理成功的话写入数据,可能需要更新统计信息
    if(bytesWritten>0){
        QMutexLocker locket(&bufferMutex);  //构造QMutexLocker对象 ,传入bufferMutex指针,自动加锁

    }

}

//客户端发送原始字节数据到服务器,直接发送字节数组,不进行编码
void Network::ClientSendBytesToServer(const QByteArray &data)
{
    //第一层 检查套接字是否有效且处于已连接状态
    if(!socket || socket->state()!=QAbstractSocket::ConnectedState){
        qWarning()<<"Socket not connected,cannot send data";
        return;
    }
    // 第二层检查：判断数据写入是否失败
    const qint64 bytesWritten=socket->write(data);

    // 第三层处理：如果成功写入了数据，可能需要更新统计信息
    if(bytesWritten>0){
        QMutexLocker locket(&bufferMutex);  // 构造QMutexLocker对象，传入bufferMutex指针，自动加锁

    }

}

//设置套接字选项,配置TCP套接字的性能参数和连接参数
void Network::setSocketOptions()
{
    // 第一层检查：验证套接字指针是否有效（非空指针检查）
    if(!socket)
        return;
    // 设置低延迟选项：减少数据包延迟
    socket->setSocketOption(QAbstractSocket::LowDelayOption,1);

    // 设置保活选项：启用TCP保活机制，定期检测连接是否存活
    socket->setSocketOption(QAbstractSocket::KeepAliveOption,1);

    // 设置读取缓冲区大小：设置套接字内部读取缓冲区的大小（16KB)
    socket->setReadBufferSize(16384);

    // 设置发送缓冲区大小：设置套接字发送缓冲区的大小（32KB）
    socket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption,32768);

    // 设置接收缓冲区大小：设置套接字扪缓冲区的大小（32KB）
    socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,32768);
}



//获得连接统计信息,返回当前连接的统计数据(只读访问)
Network::ConnectionStats Network::getConnectionStats() const
{
    ConnectionStats stats;
    return stats;
}

//客户端断开连接回调 :当套接字断开连接时自动调用
void Network::ClientDisconnectionedFunc(void)
{
    // 第一层检查：验证定时器指针是否有效
    if(!timer){
        qWarning()<<"Network::ClientDisconnectedFunc: timer is null";
        return;
    }

    // 停止自动测试定时器
    timer->stop();

}

//主动断开连接  :由外部调用主动断开与服务器tcp连接
void Network::DisconnectFromHost()
{
    // 第一层检查：验证套接字指针是否有效
    if(!socket){
        qWarning()<<"Network::DisconnectFromHost: socket is null";
        return;
    }

    // 请示断开连接
    socket->disconnectFromHost();

}

//读取服务器信息当socket有数据的可读时自动调用
void Network::ReadServerMsg()
{
    // 第一层检查：验证套接字有效且处于已连接状态
    if(!socket || socket->state()!=QAbstractSocket::ConnectedState){
        qWarning()<<"Socket not ready, skip read";
        return;
    }
    QMutexLocker locker(&bufferMutex);
    const qint64 bytesAvailable=socket->bytesAvailable();

    //第二层检查  验证是否有数据可读（数据有效性检查
    if(bytesAvailable<=0){
        return;
    }

    static const qint64 kMaxBytes=1024*1024; // 1048576 1MB

    // 第三检查：限流保护，丢弃超过阈值的数据包（防止内存溢出攻击）
    if(bytesAvailable>kMaxBytes){
        qWarning()<<"Too much incoming data, drop packet of size"<<bytesAvailable;

        socket->read(bytesAvailable);

        return;
    }

    // 动态调整接收缓冲区容量：根据数据大小预分配内存，避免频繁重新分配（性能优化）
    if(receiveBuffer.capacity()<bytesAvailable){
        // 扩展缓冲区容量：bytesAvailable*2
        receiveBuffer.reserve(bytesAvailable*2);
    }

    // 读取所有可用数据
    receiveBuffer=socket->readAll();

    // 编辑转换
    strTempData=QString::fromUtf8(receiveBuffer);

    // 提前释放互斥锁
    locker.unlock();

    // 发出数据接收信号：通知外部对象已接收到数据，触发后续处理逻辑
    emit dataReceived(strTempData);

#ifdef QT_DEBUG
    static qint64 totalBytesReceived=0;
    totalBytesReceived+=bytesAvailable;
    qDebug()<<"Received"<<bytesAvailable<<"bytes，total:"<<totalBytesReceived;
#endif

}

//启动定时器超时功能:启动自动测试模式,定时间向服务器发送消息内容
void Network::StartTimerOutFunc()
{
    // 第一层检查：验证套接字指针是否有效
    if(!socket){
        qWarning()<<"Network::StartTimerOutFunc: socket is null";
        return;
    }

    // 第二层检查：验证定时器指针是否有效
    if(!timer){
        qWarning()<<"Network::StartTimerOutFunc: timer is null";
        return;
    }

    // 第三层检查：验证套接字是否处于已经连接状态
    if(socket->state()!=QAbstractSocket::ConnectedState){
        timer->stop();
        return;
    }

    // 原子递增消息计数：线程安全地递增自动测试消息计数器，并获取递增前的值
    const int currentCount=testMessageCount.fetch_add(1);

    QString messageToSend;

    if(!autoTestMessage.isEmpty()){
        messageToSend=autoTestMessage;
        if(messageToSend.contains("%1")){
            messageToSend=messageToSend.arg(currentCount);
        }
    }
    else
    {
        messageToSend=QStringLiteral("\n[Prompt:Client automatic testing(%1)]").arg(currentCount);
    }

    // 发送消息到服务器
    ClientSendMsgToServer(messageToSend);

    // 确保定时器运行
    if(!timer->isActive()){
        timer->start(1500);
    }


}

//设置自动测试消息:
void Network::setAutoTestMessage(const QString &message)
{
  autoTestMessage=message;
}

//停止定时器超时功能
void Network::StopTimerOutFunc()
{
    timer->stop();
}



//连接建立回调:当socket成功连接到服务器是自动调用
void Network::onConnected()
{
    testMessageCount.store(0);

    performanceTimer.start();

    setSocketOptions();

    emit connectionEstablished();

}

//套接字错误处理回调
void Network::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorString;
    switch(error){
    case QAbstractSocket::ConnectionRefusedError:
        errorString="连接被拒绝";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorString="远程主机关闭连接";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorString="主机未找到";
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorString="连接超时";
        break;
    case QAbstractSocket::NetworkError:
        errorString="网络错误";
        break;
    default:
        errorString=socket->errorString();
        break;
    }

    emit connectionFailed(errorString);

}

