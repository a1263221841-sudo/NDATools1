#ifndef NETWORK_H
#define NETWORK_H


//Tcp底层封装
//职责:连接/断开,发送文本/字节,接收并转发数据,支持自动测试定时器与连接统计

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QtCore> //qt核心模块的所有类
#include <QHostAddress> //Qt主机地址类
#include <QAbstractSocket>  //Qt抽象套接字基类
#include <QElapsedTimer>  //qt高精度计数器类
#include <QMutex>   //Qt互斥锁类

#include <QAtomicInt> //Qt原子整型类,为扩展预留
#include <QStringBuilder> //Qt字符串构建器--为扩展预留

#include <memory.h>  //C++标准库智能指针头文件
#include <atomic>      //C++标准库原子操作头文件，提供std::atomic类型支持

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

//Network类,继承自QObject,实现TCP客户端底层通信逻辑封装
//使用信号与槽机制实现异步通信,提供连接管理,数据收发,自动测试,和统计功能
class Network : public QObject
{
    Q_OBJECT
public:
    //构造函数,初始化网络客户端对象,创建套接字和定时器,建立信号槽连接
    explicit Network(QObject *parent =nullptr);
    //析构函数
    ~Network(void);

public:
    //客户端连接到服务器:尝试建立Tcp到指定的服务器地址与端口
    bool ClientConnectToServer(QString ServerIpAddress,int ServerPort);

    //客户端发送文本文件到服务器将utf-8编码的字符串发送到服务器
    void ClientSendMsgToServer(const QString &StrData);

    //客户端发送原始字节数据到服务器,直接发送字节数组,不进行编码
    void ClientSendBytesToServer(const QByteArray &data);

    //设置套接字选项,配置TCP套接字的性能参数和连接参数
    void setSocketOptions();

    //连接统计信心结构体,封装连接相关的统计数据和性能指标
    struct ConnectionStats{
        qint64 connectionTime=0;
        qint64 bytesSent=0;
        qint64 bytesReceived=0;
        qint64 messageCount=0;
    };


    //获得连接统计信息,返回当前连接的统计数据(只读访问)
    ConnectionStats getConnectionStats() const;

    //最近一次接收到的数据缓存,存储最后一次从服务器连接的数据
    QString strTempData;


private:
    QTcpSocket *socket;  //tcp套接字指针
    QTimer *timer;      //自动测试定时器指针
    std::atomic<int> testMessageCount;   //自动测试发送计数 原子类型
    QString autoTestMessage;  //自动测试消息内容
    QByteArray receiveBuffer;  //接收缓冲区
    QElapsedTimer performanceTimer;   //性能计时器
    mutable QMutex bufferMutex;  //缓冲区互斥锁

Q_SIGNALS:  //Qt信号区域
    //建立连接信号
    void connectionEstablished();

    //连接信号失败信号
    void connectionFailed(const QString &errorString);

    //数据接受信号
    void dataReceived(const QString &data);

public Q_SLOTS:  //公开槽函数区域
    //客户端断开连接回调 :当套接字断开连接时自动调用
    void ClientDisconnectionedFunc(void);

    //主动断开连接  :由外部调用主动断开与服务器tcp连接
    void DisconnectFromHost();

    //读取服务器信息当socket有数据的可读时自动调用
    void ReadServerMsg();

    //启动定时器超时功能:启动自动测试模式,定时间向服务器发送消息内容
    void StartTimerOutFunc();

    //设置自动测试消息:
    void setAutoTestMessage(const QString &message);

    //停止定时器超时功能
    void StopTimerOutFunc();


private Q_SLOTS:
    //连接建立回调:当socket成功连接到服务器是自动调用
    void onConnected();

    //套接字错误处理回调
    void onSocketError(QAbstractSocket::SocketError error);
};

#endif // NETWORK_H
