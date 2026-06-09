#ifndef NETWORK_H
#define NETWORK_H


//Tcp底层封装
//职责:连接/断开,发送文本/字节,接收并转发数据,支持自动测试定时器与连接统计

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QtCore> //qt核心模块的所有类
#include <QHostAddress> //Qt主机地址类
#include <QAbstractSocket>
class Network : public QObject
{
    Q_OBJECT
public:
    ~Network(void);
};

#endif // NETWORK_H
