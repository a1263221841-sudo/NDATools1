/*
 * 主要功能
 * 1. 进制转换
 * 2. 大文本异步处理
 * 3. 输入防抖统计
 * 4. 日志记录与落盘
 * 5. 复制/交换/清空操作
 */

#ifndef FORMCHILDDATAPROCESSOR_H
#define FORMCHILDDATAPROCESSOR_H

#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QQueue>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QWidget>
#include <QtConcurrent>

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

    void saveLog();

private slots:
    void on_pushButton_CoversionOpration_clicked();
    void on_pushButton_CoversionCopyResult_clicked();
    void on_pushButton_Change_clicked();
    void on_pushButton_DeclearData_clicked();
    void onConversionFinished();
    void opInputTextChanged();

private:
    Ui::FormChildDataProcessor *ui;
    bool conversionRunning = false;
    QTimer *inputUpdateTimer = nullptr;
    QQueue<QString> logEntries;
    QFutureWatcher<DataConverter::ConversionResult> conversionWatcher;

    void showMessage(const QString &message, bool isError = false);
    void setConversionBusy(bool busy);
    void showResult(const DataConverter::ConversionResult &result, QTextEdit *outputEdit);
    void updateInputInfo(const QString &text = QString());
    void updateOutputInfo(const QString &text = QString());
    void copyToClipboard(const QString &text);
    void connectSignals();
    void appendLog(const QString &message);
    void trimLog();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // FORMCHILDDATAPROCESSOR_H
