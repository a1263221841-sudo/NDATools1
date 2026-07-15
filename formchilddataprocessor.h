/*主要功能
 * 进制转换
 * 异步处理
 * 输入防抖
 * 日志记录
 * 统计信息
 */


#ifndef FORMCHILDDATAPROCESSOR_H
#define FORMCHILDDATAPROCESSOR_H

#include <QWidget>
#include <QCloseEvent>
#include<QString>
#include <QDebug>
#include <QtConcurrent>
#include <QMessageBox>
#include <QTextEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QClipboard>
#include <QTimer>
#include <QDateTime>
#include <QStandardItem>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QQueue>


#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QStringConverter>
#else
#include <QTextCodec>
#endif


#include "dataconverter.h"
namespace Ui {
class FormChildDataProcessor;
}

class FormChildDataProcessor : public QWidget
{
    Q_OBJECT

public:
    explicit FormChildDataProcessor(QWidget *parent = nullptr);
    ~FormChildDataProcessor();

    //保存日志,(公共接口,提供主窗口调用)
    void saveLog();

private slots:
    void on_pushButton_CoversionOperation_clicked();
    void on_copyButton_clicked();
    void on_swapButton_clicked();
    void on_clearButton_clicked();

    //异步转换完成槽函数
void onConversionFinished();


//输入文本变化槽函数
void opInputTextChanged();

void on_pushButton_CoversionOpration_clicked();

private:
    Ui::FormChildDataProcessor *ui;

    //转换运行状态槽函数
    bool ConversionRunning =false;

    //输入更新防抖定时器
    QTimer *inputUpdateTimer;

    //日志条目队列
    QQueue<QString> logEntries;

    //统一消息提示函数
    void showMessage(const QString &message,bool isError =false);

    //异步转换监视器
    QFutureWatcher<DataConverter::ConversionResult> conversionWatcher;

    //设置转换忙状态
    void setConversionBusy(bool busy);

    //展示转换结果或错误信息
    void showResult(const DataConverter::ConversionResult &result,QTextEdit &outputEdit);

    //更新输入统计信息
    void updateInputInfo(const QString &text);

    //更新输出统计信息
    void updateOutputInfo(const QString &text);

    //复制文本到粘贴板
    void copyToClipboard(const QString &text);

    //连接所有信号与槽
    void connectSignals();

    //追加日志条目
    void appendLog(const QString &message);

    //裁剪日志
    void trimLog();

    //保存纯文本编辑框内容到文件
    void savePlainTextEditToFile(QPlianTextEdit *plaintTextEdit);

protected:
    //关闭窗口事件
    void closeEvent(QCloseEvent *event) override;
};

#endif // FORMCHILDDATAPROCESSOR_H
