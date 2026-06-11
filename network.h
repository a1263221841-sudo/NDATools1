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


};

#endif // NETWORK_H
