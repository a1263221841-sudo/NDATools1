#include "errorhandler.h"


#include <QStandardPaths>    //  获取平台通用的可写路径
#include <QDir>               // 目录操作,(创建路径等)
#include <QApplication>     //退出应用 应用信息


#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)  //仅qt6需要显式编码转换类
#include <QStringConverter>
#endif

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")   //MSVC编译时按UTF-8解释源文件
#endif


ErrorHandler::ErrorHandler(QObject *parent)
    : QObject{parent}
    ,lofFile(nullptr)  //日志文件指针
    ,logStream(nullptr)  //文本流指针
    ,LoggingEnabled(nullptr)  //默认开启日志
    ,currentLogPath()    //当前日志路径
{
    initializeLogFile();  //创建/打开当日日志并写入启动标记
}

//功能 :析构函数,关闭并释放日志资源
ErrorHandler::~ErrorHandler()
{
    if(logStream){
        delete logStream;  //先删流,再关闭文件
    }
    if(logFile){
        logFile ->close(); //确保落盘
        delete logFile;
    }
}

//功能:获取全局单例,首调时创建并初始化日志
ErrorHandler& ErrorHandler::instance()
{
    static ErrorHandler instance;  //首次调用创建,进程结束自动析构
    return instance;     //返回引用,避免引用
}

//功能 ,弹出消息框并记录日志,按级别选择图标/标题 ,Fatal通常会直接退出
void ErrorHandler::handleError(ErrorType type,ErrorLevel level,const QString &message ,QWidget *parent)
{
    //先写日志 context取自错误类型
    QString context =ErrorTypeToString(type);
    logError(context,message,level);

    //再根据级别弹出消息框
    QMessageBox::Icon icon;
    QString titile;

    switch(level){ //映射
    case Info:
        icon = QMessageBox::information;  //信息图标
        title="信息";
        break;
    case Warning:
        icon = QMessageBox::Warning;   //警告图标
        title="警告";
        break;
    case Critical:
        icon = QMessageBox::Critical;  //错误图标
        title="错误";
        break;
    case Fatal:
        icon = QMessageBox::Critical;
        title="严重错误";
        break;

    }

    //以父窗口作为归属,避免顶层无父导致窗口乱序
    QMessageBox msgBox(parent);
    msgBox.setIcon(icon);  //设置图标
    msgBox.setWindowTitle(title);  //设置标题
    msgBox.setText(message);  //设置文本
    msgBox.setStandardButtons(QMessageBox::OK);  //仅确认按扭
    megBox.exec();   //阻塞显示


    //Fatal 级别的,记录之后直接退出应用
    if(level == Fatal){
        QApplication::quit();
    }
}

//功能:仅记录日志,(不弹窗),包含时间/级别/来源
void ErrorHandler::logError(const QString &context,const QString &error,ErrorLevel level)
{
    //日志关闭或流未初始化时直接退出
    if(!LoggingEnabled ||!logStream){
        return;
    }

    QMutexLocker locker(&logMutex); //保护日志写入的互斥
    rolloverIfNeeded_unlocked();  //按大小判断是否回滚
    reopenForToday_unlocked();  //按日期判断是否切换

    QString timestamp =QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString levelStr =ErrorLevelToString(level);

    //统一日志格式:[时间][级别][来源] 内容
    QString logMessage =QString ("[%1] [%2] [%3] %4")
                             .arg(timestamp)
                             .arg(levelStr)
                             .arg(context)
                             .arg(error);
    writeToLog(logMessage);   //持锁写入
}

//功能:设置/切换日志文件路径(追加) ,后续按日期/大小自动流动
void ErrorHandler::setLogFile(const QString &filename)
{
    QMutexLocker locker(&logMutex);  //切换文件也需互斥保护

    //若已有流/文件,先释放
    if(logStream){
        delete logStream;
        logStream=nullptr;
    }
    if(lofFile){
        logFile->close();
        delete logFile;
        logFile =nullptr;
    }

    //尝试打开新的日志文件(追加模式)

    logFile =new QFile(filename);

}

//功能 :开启/关闭日志写盘
void ErrorHandler::setLoggingEnabled(bool enabled)
{

}

//功能:错误来源枚举转可读字符串
QString ErrorHandler::ErrorTypeToString(ErrorType type)
{

}



//功能:初始化,切换当前日志,创建目录与文件
void ErrorHandler::initializeLogFile()
{

}

//功能:在持锁状态写入日志行并刷新
void ErrorHandler::writeToLog(const QString &message)
{

}

//功能:按大小滚动日志文件
void ErrorHandler::rolloverIfNeeded_unlocked(qint64 maxBytes)
{

}


//功能:按日期切换日志(需要先持锁)
void ErrorHandler::reopenForToday_unlocked()
{

}
