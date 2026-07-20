#include "formchilddatavalidation.h"
#include "ui_formchilddatavalidation.h"
#include "logpathhelper.h"

#include <QApplication>
#include <QClipboard>

namespace {
const int kMaxInputLength = 1000000;
const int kAsyncChecksumThreshold = 40000;
const int kMaxLogEntries = 1000;

QString formatByCase(const QString &text, bool upperCase)
{
    return upperCase ? text.toUpper() : text.toLower();
}
} // namespace

FormchildDataValidation::FormchildDataValidation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormchildDataValidation)
{
    ui->setupUi(this);

    if (ui->textEdit_Enter) {
        ui->textEdit_Enter->setPlaceholderText(QStringLiteral("请输入要计算校验值的数据..."));
    }

    if (ui->plainTextEdit_CRC16) {
        ui->plainTextEdit_CRC16->setReadOnly(true);
    }
    if (ui->plainTextEdit_CRC32) {
        ui->plainTextEdit_CRC32->setReadOnly(true);
    }
    if (ui->plainTextEdit_MD5) {
        ui->plainTextEdit_MD5->setReadOnly(true);
    }
    if (ui->plainTextEdit_SHA1) {
        ui->plainTextEdit_SHA1->setReadOnly(true);
    }
    if (ui->plainTextEdit_SHA256) {
        ui->plainTextEdit_SHA256->setReadOnly(true);
    }

    connectSignals();

    appendLog(QStringLiteral("[%1] [数据校验] 工具已启动")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

FormchildDataValidation::~FormchildDataValidation()
{
    delete ui;
}

void FormchildDataValidation::connectSignals()
{
    connect(&checksumWatcher, &QFutureWatcher<DataConverter::ChecksumResult>::finished,
            this, &FormchildDataValidation::onChecksumFinished);
}

void FormchildDataValidation::on_pushButton_Calculate_clicked()
{
    if (!ui || !ui->textEdit_Enter) {
        qWarning() << "FormchildDataValidation::on_pushButton_Calculate_clicked: UI or input control is null";
        return;
    }

    if (checksumRunning) {
        return;
    }

    const QString input = ui->textEdit_Enter->toPlainText();
    if (input.isEmpty()) {
        showMessage(QStringLiteral("请输入要计算校验值的数据"), true);
        return;
    }

    if (input.length() > kMaxInputLength) {
        showMessage(QStringLiteral("输入数据过长，请分批处理"), true);
        return;
    }

    appendLog(QStringLiteral("[%1] [数据校验] 开始计算校验值，输入长度: %2 字符")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")))
                  .arg(input.length()));

    if (input.length() >= kAsyncChecksumThreshold) {
        setChecksumBusy(true);
        appendLog(QStringLiteral("[%1] [数据校验] 使用异步计算（阈值: %2）")
                      .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")))
                      .arg(kAsyncChecksumThreshold));

        auto future = QtConcurrent::run([input]() {
            return DataConverter::calulateChecksums(input);
        });
        checksumWatcher.setFuture(future);
        return;
    }

    applyChecksumResult(DataConverter::calulateChecksums(input));
    showMessage(QStringLiteral("校验值计算完成"));
    appendLog(QStringLiteral("[%1] [数据校验] 同步校验值计算完成")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

void FormchildDataValidation::on_pushButton_ValidationCopy_clicked()
{
    const QString allChecksumText = buildAllChecksumText();
    if (allChecksumText.isEmpty()) {
        showMessage(QStringLiteral("没有可复制的校验值"), true);
        return;
    }

    copyToClipboard(allChecksumText);
}

void FormchildDataValidation::on_checkBox_Unpper_toggled(bool checked)
{
    Q_UNUSED(checked)

    if (!ui || !ui->plainTextEdit_CRC16 || !ui->plainTextEdit_CRC32 || !ui->plainTextEdit_MD5 ||
        !ui->plainTextEdit_SHA1 || !ui->plainTextEdit_SHA256) {
        qWarning() << "FormchildDataValidation::on_checkBox_Unpper_toggled: UI or result controls is null";
        return;
    }

    if (ui->plainTextEdit_CRC16->toPlainText().isEmpty() &&
        ui->plainTextEdit_CRC32->toPlainText().isEmpty() &&
        ui->plainTextEdit_MD5->toPlainText().isEmpty() &&
        ui->plainTextEdit_SHA1->toPlainText().isEmpty() &&
        ui->plainTextEdit_SHA256->toPlainText().isEmpty()) {
        return;
    }

    const bool upperCase = ui->checkBox_Unpper && ui->checkBox_Unpper->isChecked();
    ui->plainTextEdit_CRC16->setPlainText(formatByCase(ui->plainTextEdit_CRC16->toPlainText(), upperCase));
    ui->plainTextEdit_CRC32->setPlainText(formatByCase(ui->plainTextEdit_CRC32->toPlainText(), upperCase));
    ui->plainTextEdit_MD5->setPlainText(formatByCase(ui->plainTextEdit_MD5->toPlainText(), upperCase));
    ui->plainTextEdit_SHA1->setPlainText(formatByCase(ui->plainTextEdit_SHA1->toPlainText(), upperCase));
    ui->plainTextEdit_SHA256->setPlainText(formatByCase(ui->plainTextEdit_SHA256->toPlainText(), upperCase));
}

void FormchildDataValidation::onChecksumFinished()
{
    if (!ui) {
        qWarning() << "FormchildDataValidation::onChecksumFinished: ui is null";
        setChecksumBusy(false);
        return;
    }

    setChecksumBusy(false);
    applyChecksumResult(checksumWatcher.result());
    showMessage(QStringLiteral("校验值计算完成"));
    appendLog(QStringLiteral("[%1] [数据校验] 异步校验值计算完成")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

void FormchildDataValidation::applyChecksumResult(const DataConverter::ChecksumResult &result)
{
    if (!ui || !ui->plainTextEdit_CRC16 || !ui->plainTextEdit_CRC32 || !ui->plainTextEdit_MD5 ||
        !ui->plainTextEdit_SHA1 || !ui->plainTextEdit_SHA256) {
        qWarning() << "FormchildDataValidation::applyChecksumResult: UI or result controls is null";
        return;
    }

    const bool upperCase = ui->checkBox_Unpper && ui->checkBox_Unpper->isChecked();
    ui->plainTextEdit_CRC16->setPlainText(formatByCase(result.crc16, upperCase));
    ui->plainTextEdit_CRC32->setPlainText(formatByCase(result.crc32, upperCase));
    ui->plainTextEdit_MD5->setPlainText(formatByCase(result.md5, upperCase));
    ui->plainTextEdit_SHA1->setPlainText(formatByCase(result.sha1, upperCase));
    ui->plainTextEdit_SHA256->setPlainText(formatByCase(result.sha256, upperCase));
}

QString FormchildDataValidation::buildAllChecksumText() const
{
    if (!ui || !ui->plainTextEdit_CRC16 || !ui->plainTextEdit_CRC32 || !ui->plainTextEdit_MD5 ||
        !ui->plainTextEdit_SHA1 || !ui->plainTextEdit_SHA256) {
        return QString();
    }

    const QString crc16 = ui->plainTextEdit_CRC16->toPlainText().trimmed();
    const QString crc32 = ui->plainTextEdit_CRC32->toPlainText().trimmed();
    const QString md5 = ui->plainTextEdit_MD5->toPlainText().trimmed();
    const QString sha1 = ui->plainTextEdit_SHA1->toPlainText().trimmed();
    const QString sha256 = ui->plainTextEdit_SHA256->toPlainText().trimmed();

    if (crc16.isEmpty() && crc32.isEmpty() && md5.isEmpty() && sha1.isEmpty() && sha256.isEmpty()) {
        return QString();
    }

    return QStringLiteral("CRC16: %1\nCRC32: %2\nMD5: %3\nSHA1: %4\nSHA256: %5")
        .arg(crc16, crc32, md5, sha1, sha256);
}

void FormchildDataValidation::copyToClipboard(const QString &text)
{
    QClipboard *clipboard = QApplication::clipboard();
    if (!clipboard) {
        qWarning() << "FormchildDataValidation::copyToClipboard: clipboard is null";
        showMessage(QStringLiteral("无法访问系统剪贴板"), true);
        return;
    }

    clipboard->setText(text);
    showMessage(QStringLiteral("已复制到剪贴板"));
    appendLog(QStringLiteral("[%1] [数据校验] 复制校验值到剪贴板")
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))));
}

void FormchildDataValidation::showMessage(const QString &message, bool isError)
{
    if (isError) {
        QMessageBox::warning(this, QStringLiteral("错误"), message);
        return;
    }

    qDebug() << message;
}

void FormchildDataValidation::setChecksumBusy(bool busy)
{
    checksumRunning = busy;

    if (!ui) {
        return;
    }

    if (ui->pushButton_Calculate) {
        ui->pushButton_Calculate->setEnabled(!busy);
    }
    if (ui->pushButton_ValidationCopy) {
        ui->pushButton_ValidationCopy->setEnabled(!busy);
    }
}

void FormchildDataValidation::appendLog(const QString &message)
{
    logEntries.enqueue(message);
    qDebug() << "数据校验工具日志:" << message;
    trimLog();
}

void FormchildDataValidation::trimLog()
{
    while (logEntries.size() > kMaxLogEntries) {
        logEntries.dequeue();
    }
}

void FormchildDataValidation::saveLog()
{
    if (logEntries.isEmpty()) {
        return;
    }

    const QString logDir = ensureProjectLogDirPath(QStringLiteral("tools"));
    if (logDir.isEmpty()) {
        qWarning() << "FormchildDataValidation::saveLog: log dir path is empty";
        return;
    }

    const QString fileName = QDir(logDir).filePath(QStringLiteral("DataValidationLogfile.txt"));
    QFile file(fileName);
    const bool fileExists = file.exists();

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "FormchildDataValidation::saveLog: failed to open log file" << file.errorString();
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
    qDebug() << "数据校验工具日志已保存至:" << fileName;
}

void FormchildDataValidation::closeEvent(QCloseEvent *event)
{
    saveLog();
    event->accept();
}
