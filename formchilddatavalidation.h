/*
 * 数据校验功能子窗口类声明
 *
 * 主要功能：
 * 1. 数据校验：支持 CRC16、CRC32、MD5、SHA1、SHA256
 * 2. 异步处理：大数据量时自动切换后台计算
 * 3. 大小写切换：支持校验结果统一大小写显示
 * 4. 日志记录：记录操作过程并支持保存到文件
 * 5. 剪贴板操作：支持一键复制全部校验值
 */

#ifndef FORMCHILDDATAVALIDATION_H
#define FORMCHILDDATAVALIDATION_H

#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QQueue>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTextStream>
#include <QWidget>
#include <QtConcurrent>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#else
#include <QTextCodec>
#endif

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "dataconverter.h"

namespace Ui {
class FormchildDataValidation;
}

class FormchildDataValidation : public QWidget
{
    Q_OBJECT

public:
    explicit FormchildDataValidation(QWidget *parent = nullptr);
    ~FormchildDataValidation();

    void saveLog();

private slots:
    // 对应 UI: pushButton_Calculate
    void on_pushButton_Calculate_clicked();

    // 对应 UI: pushButton_ValidationCopy
    void on_pushButton_ValidationCopy_clicked();

    // 对应 UI: checkBox_Unpper
    void on_checkBox_Unpper_toggled(bool checked);

    // 异步校验完成回调
    void onChecksumFinished();

private:
    void connectSignals();
    void copyToClipboard(const QString &text);
    void showMessage(const QString &message, bool isError = false);
    void setChecksumBusy(bool busy);
    void appendLog(const QString &message);
    void trimLog();
    void applyChecksumResult(const DataConverter::ChecksumResult &result);
    QString buildAllChecksumText() const;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::FormchildDataValidation *ui = nullptr;
    bool checksumRunning = false;
    QFutureWatcher<DataConverter::ChecksumResult> checksumWatcher;
    QQueue<QString> logEntries;
};

#endif // FORMCHILDDATAVALIDATION_H
