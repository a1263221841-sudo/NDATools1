#include "formchilddataprocessor.h"
#include "ui_formchilddataprocessor.h"

#include <QApplication>
#include <QClipboard>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#else
#include <QTextCodec>
#endif

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

namespace {
const int kMaxConvertLength = 50000;
const int kAsyncConvertThreshold = 15000;
const int kMaxLogEntries = 1000;

DataConverter::ConversionResult convertByType(const QString &input, const QString &conversionType)
{
    if (conversionType == QLatin1String("decimal_to_binary")) {
        return DataConverter::decimalToBinary(input);
    }

    if (conversionType == QLatin1String("binary_to_decimal")) {
        return DataConverter::binaryToDecimal(input);
    }

    return DataConverter::ConversionResult(false, QString(), QStringLiteral("不支持的转换类型"));
}
} // namespace

FormChildDataProcessor::FormChildDataProcessor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormChildDataProcessor)
    , inputUpdateTimer(new QTimer(this))
{
    ui->setupUi(this);

    ui->comboBox_ConversionType->clear();
    ui->comboBox_ConversionType->addItem(QStringLiteral("十进制->二进制"), QStringLiteral("decimal_to_binary"));
    ui->comboBox_ConversionType->addItem(QStringLiteral("二进制->十进制"), QStringLiteral("binary_to_decimal"));

    ui->textEdit_EnterData->setPlaceholderText(QStringLiteral("请输入要转换的数据..."));
    ui->textEdit_BringData->setPlaceholderText(QStringLiteral("转换结果将显示在这里..."));

    inputUpdateTimer->setSingleShot(true);
    inputUpdateTimer->setInterval(300);

    connectSignals();
    updateInputInfo();
    updateOutputInfo();

    appendLog(QStringLiteral("[%1] [数据转换] 工具已启动")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

FormChildDataProcessor::~FormChildDataProcessor()
{
    delete ui;
}

void FormChildDataProcessor::connectSignals()
{
    if (!ui || !inputUpdateTimer) {
        qWarning() << "FormChildDataProcessor::connectSignals: ui or timer is null";
        return;
    }

    connect(inputUpdateTimer, &QTimer::timeout, this, [this]() {
        if (ui && ui->textEdit_EnterData) {
            updateInputInfo(ui->textEdit_EnterData->toPlainText());
        }
    });

    connect(ui->textEdit_EnterData, &QTextEdit::textChanged,
            this, &FormChildDataProcessor::opInputTextChanged);
    connect(&conversionWatcher, &QFutureWatcher<DataConverter::ConversionResult>::finished,
            this, &FormChildDataProcessor::onConversionFinished);
}

void FormChildDataProcessor::on_pushButton_CoversionOpration_clicked()
{
    if (!ui || !ui->textEdit_EnterData || !ui->textEdit_BringData || !ui->comboBox_ConversionType) {
        qWarning() << "FormChildDataProcessor::on_pushButton_CoversionOpration_clicked: UI or controls is null";
        return;
    }

    if (conversionRunning) {
        return;
    }

    const QString input = ui->textEdit_EnterData->toPlainText();
    if (input.isEmpty()) {
        showMessage(QStringLiteral("请输入要转换的数据"), true);
        return;
    }

    if (input.length() > kMaxConvertLength) {
        showMessage(QStringLiteral("输入数据过长，请分批处理"), true);
        return;
    }

    const QString conversionType = ui->comboBox_ConversionType->currentData().toString();
    const QString conversionTypeName = ui->comboBox_ConversionType->currentText();
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));

    appendLog(QStringLiteral("[%1] [数据转换] 开始转换: %2，输入长度: %3 字符")
                  .arg(timestamp, conversionTypeName)
                  .arg(input.length()));

    if (input.length() > kAsyncConvertThreshold) {
        setConversionBusy(true);
        appendLog(QStringLiteral("[%1] [数据转换] 使用异步转换（阈值: %2）")
                      .arg(timestamp)
                      .arg(kAsyncConvertThreshold));

        auto future = QtConcurrent::run([input, conversionType]() {
            return convertByType(input, conversionType);
        });
        conversionWatcher.setFuture(future);
        return;
    }

    showResult(convertByType(input, conversionType), ui->textEdit_BringData);
}

void FormChildDataProcessor::on_pushButton_CoversionCopyResult_clicked()
{
    if (!ui || !ui->textEdit_BringData) {
        qWarning() << "FormChildDataProcessor::on_pushButton_CoversionCopyResult_clicked: output control is null";
        return;
    }

    const QString text = ui->textEdit_BringData->toPlainText();
    if (text.isEmpty()) {
        showMessage(QStringLiteral("没有可复制的内容"), true);
        return;
    }

    copyToClipboard(text);
}

void FormChildDataProcessor::on_pushButton_Change_clicked()
{
    if (!ui || !ui->textEdit_EnterData || !ui->textEdit_BringData) {
        qWarning() << "FormChildDataProcessor::on_pushButton_Change_clicked: controls are null";
        return;
    }

    const QString inputText = ui->textEdit_EnterData->toPlainText();
    const QString outputText = ui->textEdit_BringData->toPlainText();

    ui->textEdit_EnterData->setPlainText(outputText);
    ui->textEdit_BringData->setPlainText(inputText);

    updateInputInfo(outputText);
    updateOutputInfo(inputText);

    appendLog(QStringLiteral("[%1] [数据转换] 交换输入输出数据")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

void FormChildDataProcessor::on_pushButton_DeclearData_clicked()
{
    if (!ui || !ui->textEdit_EnterData || !ui->textEdit_BringData) {
        qWarning() << "FormChildDataProcessor::on_pushButton_DeclearData_clicked: controls are null";
        return;
    }

    ui->textEdit_EnterData->clear();
    ui->textEdit_BringData->clear();

    updateInputInfo(QString());
    updateOutputInfo(QString());

    appendLog(QStringLiteral("[%1] [数据转换] 清空输入输出数据")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

void FormChildDataProcessor::onConversionFinished()
{
    if (!ui || !ui->textEdit_BringData) {
        qWarning() << "FormChildDataProcessor::onConversionFinished: UI or output control is null";
        setConversionBusy(false);
        return;
    }

    setConversionBusy(false);
    appendLog(QStringLiteral("[%1] [数据转换] 异步转换已完成")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
    showResult(conversionWatcher.result(), ui->textEdit_BringData);
}

void FormChildDataProcessor::opInputTextChanged()
{
    if (!inputUpdateTimer) {
        qWarning() << "FormChildDataProcessor::opInputTextChanged: inputUpdateTimer is null";
        return;
    }

    inputUpdateTimer->stop();
    inputUpdateTimer->start();
}

void FormChildDataProcessor::showMessage(const QString &message, bool isError)
{
    if (isError) {
        QMessageBox::warning(this, QStringLiteral("错误"), message);
        return;
    }

    qDebug() << message;
}

void FormChildDataProcessor::setConversionBusy(bool busy)
{
    conversionRunning = busy;

    if (!ui) {
        return;
    }

    if (ui->pushButton_CoversionOpration) {
        ui->pushButton_CoversionOpration->setEnabled(!busy);
    }
    if (ui->pushButton_CoversionCopyResult) {
        ui->pushButton_CoversionCopyResult->setEnabled(!busy);
    }
    if (ui->pushButton_Change) {
        ui->pushButton_Change->setEnabled(!busy);
    }
    if (ui->pushButton_DeclearData) {
        ui->pushButton_DeclearData->setEnabled(!busy);
    }
}

void FormChildDataProcessor::showResult(const DataConverter::ConversionResult &result, QTextEdit *outputEdit)
{
    if (!outputEdit) {
        qWarning() << "FormChildDataProcessor::showResult: outputEdit is null";
        return;
    }

    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));

    if (result.success) {
        outputEdit->setPlainText(result.data);
        updateOutputInfo(result.data);
        showMessage(QStringLiteral("转换成功"));
        appendLog(QStringLiteral("[%1] [数据转换] 转换成功，输出长度: %2 字符")
                      .arg(timestamp)
                      .arg(result.data.length()));
        return;
    }

    const QString errorText = !result.errorMessage.isEmpty() ? result.errorMessage : result.data;
    outputEdit->clear();
    updateOutputInfo(QString());
    showMessage(errorText.isEmpty() ? QStringLiteral("转换失败") : errorText, true);
    appendLog(QStringLiteral("[%1] [数据转换] 转换失败: %2")
                  .arg(timestamp, errorText.isEmpty() ? QStringLiteral("未知错误") : errorText));
}

void FormChildDataProcessor::updateInputInfo(const QString &text)
{
    if (!ui || !ui->label_4) {
        qWarning() << "FormChildDataProcessor::updateInputInfo: label_4 is null";
        return;
    }

    const QString content = text.isNull()
        ? (ui->textEdit_EnterData ? ui->textEdit_EnterData->toPlainText() : QString())
        : text;
    ui->label_4->setText(DataConverter::getDataInfo(content));
}

void FormChildDataProcessor::updateOutputInfo(const QString &text)
{
    if (!ui || !ui->label_6) {
        qWarning() << "FormChildDataProcessor::updateOutputInfo: label_6 is null";
        return;
    }

    const QString content = text.isNull()
        ? (ui->textEdit_BringData ? ui->textEdit_BringData->toPlainText() : QString())
        : text;
    ui->label_6->setText(DataConverter::getDataInfo(content));
}

void FormChildDataProcessor::copyToClipboard(const QString &text)
{
    QClipboard *clipboard = QApplication::clipboard();
    if (!clipboard) {
        qWarning() << "FormChildDataProcessor::copyToClipboard: clipboard is null";
        showMessage(QStringLiteral("无法访问系统剪贴板"), true);
        return;
    }

    clipboard->setText(text);
    showMessage(QStringLiteral("已复制到剪贴板"));

    appendLog(QStringLiteral("[%1] [数据转换] 复制结果到剪贴板")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

void FormChildDataProcessor::appendLog(const QString &message)
{
    logEntries.enqueue(message);
    qDebug() << "数据转换工具日志:" << message;
    trimLog();
}

void FormChildDataProcessor::trimLog()
{
    while (logEntries.size() > kMaxLogEntries) {
        logEntries.dequeue();
    }
}

void FormChildDataProcessor::saveLog()
{
    if (logEntries.isEmpty()) {
        return;
    }

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appDataPath.isEmpty()) {
        qWarning() << "FormChildDataProcessor::saveLog: app data path is empty";
        return;
    }

    if (!QDir().mkpath(appDataPath)) {
        qWarning() << "FormChildDataProcessor::saveLog: failed to create directory" << appDataPath;
        return;
    }

    const QString fileName = QDir(appDataPath).filePath(QStringLiteral("DataConversionLogfile.txt"));
    QFile file(fileName);
    const bool fileExists = file.exists();

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "FormChildDataProcessor::saveLog: failed to open log file" << file.errorString();
        return;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif

    if (fileExists && file.size() > 0) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out << endl;
        out << QStringLiteral("========== ")
            << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))
            << QStringLiteral(" ==========") << endl;
#else
        out << Qt::endl;
        out << QStringLiteral("========== ")
            << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))
            << QStringLiteral(" ==========") << Qt::endl;
#endif
    }

    for (const QString &entry : qAsConst(logEntries)) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out << entry << endl;
#else
        out << entry << Qt::endl;
#endif
    }

    file.close();
    logEntries.clear();
    qDebug() << "数据转换工具日志已保存至:" << fileName;
}

void FormChildDataProcessor::closeEvent(QCloseEvent *event)
{
    saveLog();
    event->accept();
}


