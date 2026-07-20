#include "formprotocoltools.h"
#include "logpathhelper.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QStandardPaths>
#include <QSpinBox>
#include <QSslError>
#include <QSslSocket>
#include <QTabWidget>
#include <QTextStream>
#include <QUrl>
#include <QVBoxLayout>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

namespace {
constexpr int kProtocolPageMinWidth = 860;
constexpr int kProtocolPageMinHeight = 560;
constexpr int kResultPanelMinWidth = 430;
constexpr int kConfigPanelMinWidth = 280;
constexpr int kScrollPanelMinWidth = 300;
constexpr int kActionButtonMinHeight = 28;
constexpr int kLogEditMinHeight = 250;
constexpr int kBodyEditMinHeight = 88;

QString nowText()
{
    return QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
}

QByteArray buildLengthPrefixed(const QByteArray &data)
{
    QByteArray result;
    result.append(static_cast<char>((data.size() >> 8) & 0xFF));
    result.append(static_cast<char>(data.size() & 0xFF));
    result.append(data);
    return result;
}

QScrollArea *createScrollPanel(QWidget *content, QWidget *parent)
{
    QScrollArea *scrollArea = new QScrollArea(parent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(content);
    return scrollArea;
}
} // namespace

FormProtocolTools::FormProtocolTools(QWidget *parent)
    : QWidget(parent)
    , httpManager(new QNetworkAccessManager(this))
    , webSocketSocket(new QSslSocket(this))
    , mailSocket(new QSslSocket(this))
    , mqttSocket(new QTcpSocket(this))
    , modbusSocket(new QTcpSocket(this))
{
    buildUi();
    initConnections();
    loadSettings();
}

FormProtocolTools::~FormProtocolTools() = default;

void FormProtocolTools::buildUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(4);
    setMinimumSize(kProtocolPageMinWidth, kProtocolPageMinHeight);

    protocolTabWidget = new QTabWidget(this);
    protocolTabWidget->setDocumentMode(true);
    protocolTabWidget->setUsesScrollButtons(true);
    protocolTabWidget->addTab(createHttpTab(), QStringLiteral("HTTP/HTTPS"));
    protocolTabWidget->addTab(createWebSocketTab(), QStringLiteral("WebSocket"));
    protocolTabWidget->addTab(createMailTab(), QStringLiteral("POP3/IMAP"));
    protocolTabWidget->addTab(createMqttTab(), QStringLiteral("MQTT"));
    protocolTabWidget->addTab(createModbusTab(), QStringLiteral("Modbus TCP"));

    mainLayout->addWidget(protocolTabWidget);
}

QWidget *FormProtocolTools::createHttpTab()
{
    httpUi.page = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(httpUi.page);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(6);

    QGroupBox *resultGroup = new QGroupBox(QStringLiteral("响应结果"), httpUi.page);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    resultGroup->setMinimumWidth(kResultPanelMinWidth);
    httpUi.responseHeadersEdit = new QPlainTextEdit(resultGroup);
    httpUi.responseHeadersEdit->setReadOnly(true);
    httpUi.responseHeadersEdit->setPlaceholderText(QStringLiteral("响应头"));
    httpUi.responseBodyEdit = new QPlainTextEdit(resultGroup);
    httpUi.responseBodyEdit->setReadOnly(true);
    httpUi.responseBodyEdit->setPlaceholderText(QStringLiteral("响应体"));
    resultLayout->addWidget(new QLabel(QStringLiteral("响应头"), resultGroup));
    resultLayout->addWidget(httpUi.responseHeadersEdit, 1);
    resultLayout->addWidget(new QLabel(QStringLiteral("响应体"), resultGroup));
    resultLayout->addWidget(httpUi.responseBodyEdit, 2);

    QWidget *sideWidget = new QWidget(httpUi.page);
    QVBoxLayout *sideLayout = new QVBoxLayout(sideWidget);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(6);
    QGroupBox *configGroup = new QGroupBox(QStringLiteral("请求设置"), httpUi.page);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->setSpacing(4);
    configGroup->setMinimumWidth(kConfigPanelMinWidth);
    httpUi.methodCombo = new QComboBox(configGroup);
    httpUi.methodCombo->addItems({QStringLiteral("GET"), QStringLiteral("POST"),
                                  QStringLiteral("PUT"), QStringLiteral("DELETE"),
                                  QStringLiteral("PATCH")});
    httpUi.urlEdit = new QLineEdit(QStringLiteral("https://httpbin.org/get"), configGroup);
    httpUi.ignoreSslCheck = new QCheckBox(QStringLiteral("忽略 SSL 证书错误"), configGroup);
    httpUi.headersEdit = new QPlainTextEdit(configGroup);
    httpUi.headersEdit->setPlaceholderText(QStringLiteral("请求头，每行一项，例如：\nContent-Type: application/json"));
    httpUi.bodyEdit = new QPlainTextEdit(configGroup);
    httpUi.bodyEdit->setPlaceholderText(QStringLiteral("请求体"));
    httpUi.bodyEdit->setMinimumHeight(kBodyEditMinHeight);
    httpUi.sendButton = new QPushButton(QStringLiteral("发送请求"), configGroup);
    httpUi.sendButton->setMinimumHeight(kActionButtonMinHeight);
    configLayout->addWidget(new QLabel(QStringLiteral("请求方法"), configGroup));
    configLayout->addWidget(httpUi.methodCombo);
    configLayout->addWidget(new QLabel(QStringLiteral("目标 URL"), configGroup));
    configLayout->addWidget(httpUi.urlEdit);
    configLayout->addWidget(httpUi.ignoreSslCheck);
    configLayout->addWidget(new QLabel(QStringLiteral("请求头"), configGroup));
    configLayout->addWidget(httpUi.headersEdit, 1);
    configLayout->addWidget(new QLabel(QStringLiteral("请求体"), configGroup));
    configLayout->addWidget(httpUi.bodyEdit, 1);
    configLayout->addWidget(httpUi.sendButton);

    QGroupBox *logGroup = new QGroupBox(QStringLiteral("请求日志"), httpUi.page);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    httpUi.logEdit = new QPlainTextEdit(logGroup);
    httpUi.logEdit->setReadOnly(true);
    httpUi.logEdit->setMinimumHeight(120);
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    httpUi.clearLogButton = new QPushButton(QStringLiteral("清空日志"), logGroup);
    httpUi.saveLogButton = new QPushButton(QStringLiteral("保存日志"), logGroup);
    httpUi.clearLogButton->setMinimumHeight(kActionButtonMinHeight);
    httpUi.saveLogButton->setMinimumHeight(kActionButtonMinHeight);
    logButtonLayout->addWidget(httpUi.clearLogButton);
    logButtonLayout->addWidget(httpUi.saveLogButton);
    logLayout->addWidget(httpUi.logEdit);
    logLayout->addLayout(logButtonLayout);

    sideLayout->addWidget(configGroup, 3);
    sideLayout->addWidget(logGroup, 2);
    sideLayout->addStretch();

    QScrollArea *sideScroll = createScrollPanel(sideWidget, httpUi.page);
    sideScroll->setMinimumWidth(kScrollPanelMinWidth);

    rootLayout->addWidget(resultGroup, 3);
    rootLayout->addWidget(sideScroll, 2);
    return httpUi.page;
}

QWidget *FormProtocolTools::createWebSocketTab()
{
    webSocketUi.page = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(webSocketUi.page);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(6);

    QGroupBox *logGroup = new QGroupBox(QStringLiteral("WebSocket 收发日志"), webSocketUi.page);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logGroup->setMinimumWidth(kResultPanelMinWidth);
    webSocketUi.logEdit = new QPlainTextEdit(logGroup);
    webSocketUi.logEdit->setReadOnly(true);
    webSocketUi.logEdit->setMinimumHeight(kLogEditMinHeight);
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    webSocketUi.clearLogButton = new QPushButton(QStringLiteral("清空日志"), logGroup);
    webSocketUi.saveLogButton = new QPushButton(QStringLiteral("保存日志"), logGroup);
    webSocketUi.clearLogButton->setMinimumHeight(kActionButtonMinHeight);
    webSocketUi.saveLogButton->setMinimumHeight(kActionButtonMinHeight);
    logButtonLayout->addWidget(webSocketUi.clearLogButton);
    logButtonLayout->addWidget(webSocketUi.saveLogButton);
    logLayout->addWidget(webSocketUi.logEdit);
    logLayout->addLayout(logButtonLayout);

    QGroupBox *configGroup = new QGroupBox(QStringLiteral("连接与发送"), webSocketUi.page);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->setSpacing(4);
    configGroup->setMinimumWidth(kConfigPanelMinWidth);
    webSocketUi.urlEdit = new QLineEdit(QStringLiteral("ws://echo.websocket.events"), configGroup);
    webSocketUi.connectButton = new QPushButton(QStringLiteral("连接"), configGroup);
    webSocketUi.disconnectButton = new QPushButton(QStringLiteral("断开"), configGroup);
    webSocketUi.sendEdit = new QPlainTextEdit(configGroup);
    webSocketUi.sendEdit->setPlaceholderText(QStringLiteral("输入要发送的文本消息"));
    webSocketUi.sendEdit->setMinimumHeight(110);
    webSocketUi.sendButton = new QPushButton(QStringLiteral("发送消息"), configGroup);
    webSocketUi.connectButton->setMinimumHeight(kActionButtonMinHeight);
    webSocketUi.disconnectButton->setMinimumHeight(kActionButtonMinHeight);
    webSocketUi.sendButton->setMinimumHeight(kActionButtonMinHeight);
    webSocketUi.disconnectButton->setEnabled(false);
    webSocketUi.sendButton->setEnabled(false);
    configLayout->addWidget(new QLabel(QStringLiteral("WebSocket 地址"), configGroup));
    configLayout->addWidget(webSocketUi.urlEdit);
    configLayout->addWidget(webSocketUi.connectButton);
    configLayout->addWidget(webSocketUi.disconnectButton);
    configLayout->addWidget(new QLabel(QStringLiteral("发送文本"), configGroup));
    configLayout->addWidget(webSocketUi.sendEdit, 1);
    configLayout->addWidget(webSocketUi.sendButton);

    QWidget *configWidget = new QWidget(webSocketUi.page);
    QVBoxLayout *configWrapperLayout = new QVBoxLayout(configWidget);
    configWrapperLayout->setContentsMargins(0, 0, 0, 0);
    configWrapperLayout->addWidget(configGroup);
    configWrapperLayout->addStretch();

    QScrollArea *configScroll = createScrollPanel(configWidget, webSocketUi.page);
    configScroll->setMinimumWidth(kScrollPanelMinWidth);

    rootLayout->addWidget(logGroup, 3);
    rootLayout->addWidget(configScroll, 2);
    return webSocketUi.page;
}

QWidget *FormProtocolTools::createMailTab()
{
    mailUi.page = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(mailUi.page);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(6);

    QGroupBox *logGroup = new QGroupBox(QStringLiteral("服务器响应日志"), mailUi.page);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logGroup->setMinimumWidth(kResultPanelMinWidth);
    mailUi.logEdit = new QPlainTextEdit(logGroup);
    mailUi.logEdit->setReadOnly(true);
    mailUi.logEdit->setMinimumHeight(kLogEditMinHeight);
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    mailUi.clearLogButton = new QPushButton(QStringLiteral("清空日志"), logGroup);
    mailUi.saveLogButton = new QPushButton(QStringLiteral("保存日志"), logGroup);
    mailUi.clearLogButton->setMinimumHeight(kActionButtonMinHeight);
    mailUi.saveLogButton->setMinimumHeight(kActionButtonMinHeight);
    logButtonLayout->addWidget(mailUi.clearLogButton);
    logButtonLayout->addWidget(mailUi.saveLogButton);
    logLayout->addWidget(mailUi.logEdit);
    logLayout->addLayout(logButtonLayout);

    QGroupBox *configGroup = new QGroupBox(QStringLiteral("连接与命令"), mailUi.page);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->setSpacing(4);
    configGroup->setMinimumWidth(kConfigPanelMinWidth);
    mailUi.protocolCombo = new QComboBox(configGroup);
    mailUi.protocolCombo->addItems({QStringLiteral("POP3"), QStringLiteral("IMAP")});
    mailUi.hostEdit = new QLineEdit(QStringLiteral("pop.qq.com"), configGroup);
    mailUi.portSpin = new QSpinBox(configGroup);
    mailUi.portSpin->setMaximum(65535);
    mailUi.portSpin->setValue(995);
    mailUi.sslCheck = new QCheckBox(QStringLiteral("使用 SSL"), configGroup);
    mailUi.sslCheck->setChecked(true);
    mailUi.userEdit = new QLineEdit(configGroup);
    mailUi.passwordEdit = new QLineEdit(configGroup);
    mailUi.passwordEdit->setEchoMode(QLineEdit::Password);
    mailUi.commandEdit = new QLineEdit(QStringLiteral("STAT"), configGroup);
    mailUi.connectButton = new QPushButton(QStringLiteral("连接服务器"), configGroup);
    mailUi.loginButton = new QPushButton(QStringLiteral("登录"), configGroup);
    mailUi.listButton = new QPushButton(QStringLiteral("列出邮箱内容"), configGroup);
    mailUi.commandButton = new QPushButton(QStringLiteral("发送自定义命令"), configGroup);
    mailUi.connectButton->setMinimumHeight(kActionButtonMinHeight);
    mailUi.loginButton->setMinimumHeight(kActionButtonMinHeight);
    mailUi.listButton->setMinimumHeight(kActionButtonMinHeight);
    mailUi.commandButton->setMinimumHeight(kActionButtonMinHeight);

    configLayout->addWidget(new QLabel(QStringLiteral("协议"), configGroup));
    configLayout->addWidget(mailUi.protocolCombo);
    configLayout->addWidget(new QLabel(QStringLiteral("主机地址"), configGroup));
    configLayout->addWidget(mailUi.hostEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("端口"), configGroup));
    configLayout->addWidget(mailUi.portSpin);
    configLayout->addWidget(mailUi.sslCheck);
    configLayout->addWidget(new QLabel(QStringLiteral("用户名"), configGroup));
    configLayout->addWidget(mailUi.userEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("密码 / 授权码"), configGroup));
    configLayout->addWidget(mailUi.passwordEdit);
    configLayout->addWidget(mailUi.connectButton);
    configLayout->addWidget(mailUi.loginButton);
    configLayout->addWidget(mailUi.listButton);
    configLayout->addWidget(new QLabel(QStringLiteral("自定义命令"), configGroup));
    configLayout->addWidget(mailUi.commandEdit);
    configLayout->addWidget(mailUi.commandButton);

    QWidget *configWidget = new QWidget(mailUi.page);
    QVBoxLayout *configWrapperLayout = new QVBoxLayout(configWidget);
    configWrapperLayout->setContentsMargins(0, 0, 0, 0);
    configWrapperLayout->addWidget(configGroup);
    configWrapperLayout->addStretch();

    QScrollArea *configScroll = createScrollPanel(configWidget, mailUi.page);
    configScroll->setMinimumWidth(kScrollPanelMinWidth);

    rootLayout->addWidget(logGroup, 3);
    rootLayout->addWidget(configScroll, 2);
    return mailUi.page;
}

QWidget *FormProtocolTools::createMqttTab()
{
    mqttUi.page = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(mqttUi.page);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(6);

    QGroupBox *logGroup = new QGroupBox(QStringLiteral("MQTT 消息日志"), mqttUi.page);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logGroup->setMinimumWidth(kResultPanelMinWidth);
    mqttUi.logEdit = new QPlainTextEdit(logGroup);
    mqttUi.logEdit->setReadOnly(true);
    mqttUi.logEdit->setMinimumHeight(kLogEditMinHeight);
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    mqttUi.clearLogButton = new QPushButton(QStringLiteral("清空日志"), logGroup);
    mqttUi.saveLogButton = new QPushButton(QStringLiteral("保存日志"), logGroup);
    mqttUi.clearLogButton->setMinimumHeight(kActionButtonMinHeight);
    mqttUi.saveLogButton->setMinimumHeight(kActionButtonMinHeight);
    logButtonLayout->addWidget(mqttUi.clearLogButton);
    logButtonLayout->addWidget(mqttUi.saveLogButton);
    logLayout->addWidget(mqttUi.logEdit);
    logLayout->addLayout(logButtonLayout);

    QGroupBox *configGroup = new QGroupBox(QStringLiteral("连接与发布订阅"), mqttUi.page);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->setSpacing(4);
    configGroup->setMinimumWidth(kConfigPanelMinWidth);
    mqttUi.hostEdit = new QLineEdit(QStringLiteral("broker.emqx.io"), configGroup);
    mqttUi.portSpin = new QSpinBox(configGroup);
    mqttUi.portSpin->setMaximum(65535);
    mqttUi.portSpin->setValue(1883);
    mqttUi.clientIdEdit = new QLineEdit(QStringLiteral("NDAToolsClient"), configGroup);
    mqttUi.userEdit = new QLineEdit(configGroup);
    mqttUi.passwordEdit = new QLineEdit(configGroup);
    mqttUi.passwordEdit->setEchoMode(QLineEdit::Password);
    mqttUi.topicEdit = new QLineEdit(QStringLiteral("ndatools/demo"), configGroup);
    mqttUi.qosCombo = new QComboBox(configGroup);
    mqttUi.qosCombo->addItem(QStringLiteral("QoS 0"), 0);
    mqttUi.qosCombo->addItem(QStringLiteral("QoS 1"), 1);
    mqttUi.payloadEdit = new QPlainTextEdit(configGroup);
    mqttUi.payloadEdit->setPlaceholderText(QStringLiteral("发布消息内容"));
    mqttUi.payloadEdit->setMinimumHeight(kBodyEditMinHeight);
    mqttUi.connectButton = new QPushButton(QStringLiteral("连接 Broker"), configGroup);
    mqttUi.disconnectButton = new QPushButton(QStringLiteral("断开"), configGroup);
    mqttUi.subscribeButton = new QPushButton(QStringLiteral("订阅主题"), configGroup);
    mqttUi.publishButton = new QPushButton(QStringLiteral("发布消息"), configGroup);
    mqttUi.connectButton->setMinimumHeight(kActionButtonMinHeight);
    mqttUi.disconnectButton->setMinimumHeight(kActionButtonMinHeight);
    mqttUi.subscribeButton->setMinimumHeight(kActionButtonMinHeight);
    mqttUi.publishButton->setMinimumHeight(kActionButtonMinHeight);
    mqttUi.disconnectButton->setEnabled(false);
    mqttUi.subscribeButton->setEnabled(false);
    mqttUi.publishButton->setEnabled(false);

    configLayout->addWidget(new QLabel(QStringLiteral("Broker 地址"), configGroup));
    configLayout->addWidget(mqttUi.hostEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("端口"), configGroup));
    configLayout->addWidget(mqttUi.portSpin);
    configLayout->addWidget(new QLabel(QStringLiteral("Client ID"), configGroup));
    configLayout->addWidget(mqttUi.clientIdEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("用户名"), configGroup));
    configLayout->addWidget(mqttUi.userEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("密码"), configGroup));
    configLayout->addWidget(mqttUi.passwordEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("主题"), configGroup));
    configLayout->addWidget(mqttUi.topicEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("QoS"), configGroup));
    configLayout->addWidget(mqttUi.qosCombo);
    configLayout->addWidget(new QLabel(QStringLiteral("消息载荷"), configGroup));
    configLayout->addWidget(mqttUi.payloadEdit, 1);
    configLayout->addWidget(mqttUi.connectButton);
    configLayout->addWidget(mqttUi.disconnectButton);
    configLayout->addWidget(mqttUi.subscribeButton);
    configLayout->addWidget(mqttUi.publishButton);

    QWidget *configWidget = new QWidget(mqttUi.page);
    QVBoxLayout *configWrapperLayout = new QVBoxLayout(configWidget);
    configWrapperLayout->setContentsMargins(0, 0, 0, 0);
    configWrapperLayout->addWidget(configGroup);
    configWrapperLayout->addStretch();

    QScrollArea *configScroll = createScrollPanel(configWidget, mqttUi.page);
    configScroll->setMinimumWidth(kScrollPanelMinWidth);

    rootLayout->addWidget(logGroup, 3);
    rootLayout->addWidget(configScroll, 2);
    return mqttUi.page;
}

QWidget *FormProtocolTools::createModbusTab()
{
    modbusUi.page = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(modbusUi.page);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(6);

    QGroupBox *logGroup = new QGroupBox(QStringLiteral("Modbus TCP 日志"), modbusUi.page);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logGroup->setMinimumWidth(kResultPanelMinWidth);
    modbusUi.logEdit = new QPlainTextEdit(logGroup);
    modbusUi.logEdit->setReadOnly(true);
    modbusUi.logEdit->setMinimumHeight(kLogEditMinHeight);
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    modbusUi.clearLogButton = new QPushButton(QStringLiteral("清空日志"), logGroup);
    modbusUi.saveLogButton = new QPushButton(QStringLiteral("保存日志"), logGroup);
    modbusUi.clearLogButton->setMinimumHeight(kActionButtonMinHeight);
    modbusUi.saveLogButton->setMinimumHeight(kActionButtonMinHeight);
    logButtonLayout->addWidget(modbusUi.clearLogButton);
    logButtonLayout->addWidget(modbusUi.saveLogButton);
    logLayout->addWidget(modbusUi.logEdit);
    logLayout->addLayout(logButtonLayout);

    QGroupBox *configGroup = new QGroupBox(QStringLiteral("连接与读写参数"), modbusUi.page);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->setSpacing(4);
    configGroup->setMinimumWidth(kConfigPanelMinWidth);
    modbusUi.hostEdit = new QLineEdit(QStringLiteral("127.0.0.1"), configGroup);
    modbusUi.portSpin = new QSpinBox(configGroup);
    modbusUi.portSpin->setMaximum(65535);
    modbusUi.portSpin->setValue(502);
    modbusUi.unitIdSpin = new QSpinBox(configGroup);
    modbusUi.unitIdSpin->setRange(1, 247);
    modbusUi.unitIdSpin->setValue(1);
    modbusUi.readFunctionCombo = new QComboBox(configGroup);
    modbusUi.readFunctionCombo->addItem(QStringLiteral("03 读保持寄存器"), 0x03);
    modbusUi.readFunctionCombo->addItem(QStringLiteral("04 读输入寄存器"), 0x04);
    modbusUi.readAddressSpin = new QSpinBox(configGroup);
    modbusUi.readAddressSpin->setRange(0, 65535);
    modbusUi.readCountSpin = new QSpinBox(configGroup);
    modbusUi.readCountSpin->setRange(1, 125);
    modbusUi.readCountSpin->setValue(4);
    modbusUi.writeAddressSpin = new QSpinBox(configGroup);
    modbusUi.writeAddressSpin->setRange(0, 65535);
    modbusUi.writeValueSpin = new QSpinBox(configGroup);
    modbusUi.writeValueSpin->setRange(0, 65535);
    modbusUi.connectButton = new QPushButton(QStringLiteral("连接设备"), configGroup);
    modbusUi.disconnectButton = new QPushButton(QStringLiteral("断开"), configGroup);
    modbusUi.readButton = new QPushButton(QStringLiteral("读取保持寄存器"), configGroup);
    modbusUi.writeButton = new QPushButton(QStringLiteral("写单个寄存器"), configGroup);
    modbusUi.connectButton->setMinimumHeight(kActionButtonMinHeight);
    modbusUi.disconnectButton->setMinimumHeight(kActionButtonMinHeight);
    modbusUi.readButton->setMinimumHeight(kActionButtonMinHeight);
    modbusUi.writeButton->setMinimumHeight(kActionButtonMinHeight);
    modbusUi.disconnectButton->setEnabled(false);
    modbusUi.readButton->setEnabled(false);
    modbusUi.writeButton->setEnabled(false);

    configLayout->addWidget(new QLabel(QStringLiteral("设备地址"), configGroup));
    configLayout->addWidget(modbusUi.hostEdit);
    configLayout->addWidget(new QLabel(QStringLiteral("端口"), configGroup));
    configLayout->addWidget(modbusUi.portSpin);
    configLayout->addWidget(new QLabel(QStringLiteral("单元 ID"), configGroup));
    configLayout->addWidget(modbusUi.unitIdSpin);
    configLayout->addWidget(new QLabel(QStringLiteral("读取功能码"), configGroup));
    configLayout->addWidget(modbusUi.readFunctionCombo);
    configLayout->addWidget(new QLabel(QStringLiteral("读取起始地址"), configGroup));
    configLayout->addWidget(modbusUi.readAddressSpin);
    configLayout->addWidget(new QLabel(QStringLiteral("读取数量"), configGroup));
    configLayout->addWidget(modbusUi.readCountSpin);
    configLayout->addWidget(new QLabel(QStringLiteral("写入地址"), configGroup));
    configLayout->addWidget(modbusUi.writeAddressSpin);
    configLayout->addWidget(new QLabel(QStringLiteral("写入值"), configGroup));
    configLayout->addWidget(modbusUi.writeValueSpin);
    configLayout->addWidget(modbusUi.connectButton);
    configLayout->addWidget(modbusUi.disconnectButton);
    configLayout->addWidget(modbusUi.readButton);
    configLayout->addWidget(modbusUi.writeButton);

    QWidget *configWidget = new QWidget(modbusUi.page);
    QVBoxLayout *configWrapperLayout = new QVBoxLayout(configWidget);
    configWrapperLayout->setContentsMargins(0, 0, 0, 0);
    configWrapperLayout->addWidget(configGroup);
    configWrapperLayout->addStretch();

    QScrollArea *configScroll = createScrollPanel(configWidget, modbusUi.page);
    configScroll->setMinimumWidth(kScrollPanelMinWidth);

    rootLayout->addWidget(logGroup, 3);
    rootLayout->addWidget(configScroll, 2);
    return modbusUi.page;
}

void FormProtocolTools::initConnections()
{
    connect(httpUi.sendButton, &QPushButton::clicked, this, &FormProtocolTools::sendHttpRequest);
    connect(httpManager, &QNetworkAccessManager::finished, this, &FormProtocolTools::handleHttpReply);
    connect(httpUi.clearLogButton, &QPushButton::clicked, this, [this]() { clearLog(httpUi.logEdit); });
    connect(httpUi.saveLogButton, &QPushButton::clicked, this, [this]() { saveLogToFile(QStringLiteral("http"), httpUi.logEdit); });

    connect(webSocketUi.connectButton, &QPushButton::clicked, this, &FormProtocolTools::connectWebSocket);
    connect(webSocketUi.disconnectButton, &QPushButton::clicked, this, &FormProtocolTools::disconnectWebSocket);
    connect(webSocketUi.sendButton, &QPushButton::clicked, this, &FormProtocolTools::sendWebSocketMessage);
    connect(webSocketUi.clearLogButton, &QPushButton::clicked, this, [this]() { clearLog(webSocketUi.logEdit); });
    connect(webSocketUi.saveLogButton, &QPushButton::clicked, this, [this]() { saveLogToFile(QStringLiteral("websocket"), webSocketUi.logEdit); });

    connect(webSocketSocket, &QSslSocket::connected, this, [this]() {
        const QUrl url(webSocketUi.urlEdit->text().trimmed());
        if (url.scheme() == QLatin1String("ws")) {
            const QByteArray randomKey(16, Qt::Uninitialized);
            QByteArray seed = randomKey;
            for (int i = 0; i < seed.size(); ++i) {
                seed[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
            }
            webSocketHandshakeKey = seed.toBase64();
            QString path = url.path().isEmpty() ? QStringLiteral("/") : url.path();
            if (!url.query().isEmpty()) {
                path += QStringLiteral("?") + url.query();
            }
            const QString request =
                QStringLiteral("GET %1 HTTP/1.1\r\n"
                               "Host: %2:%3\r\n"
                               "Upgrade: websocket\r\n"
                               "Connection: Upgrade\r\n"
                               "Sec-WebSocket-Key: %4\r\n"
                               "Sec-WebSocket-Version: 13\r\n\r\n")
                    .arg(path, url.host())
                    .arg(url.port(url.scheme() == QLatin1String("wss") ? 443 : 80))
                    .arg(webSocketHandshakeKey);
            webSocketSocket->write(request.toUtf8());
            appendWebSocketLog(QStringLiteral("[%1] 发送握手请求").arg(nowText()));
        }
    });

    connect(webSocketSocket, &QSslSocket::encrypted, this, [this]() {
        const QUrl url(webSocketUi.urlEdit->text().trimmed());
        if (url.scheme() == QLatin1String("wss")) {
            const QByteArray randomKey(16, Qt::Uninitialized);
            QByteArray seed = randomKey;
            for (int i = 0; i < seed.size(); ++i) {
                seed[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
            }
            webSocketHandshakeKey = seed.toBase64();
            QString path = url.path().isEmpty() ? QStringLiteral("/") : url.path();
            if (!url.query().isEmpty()) {
                path += QStringLiteral("?") + url.query();
            }
            const QString request =
                QStringLiteral("GET %1 HTTP/1.1\r\n"
                               "Host: %2:%3\r\n"
                               "Upgrade: websocket\r\n"
                               "Connection: Upgrade\r\n"
                               "Sec-WebSocket-Key: %4\r\n"
                               "Sec-WebSocket-Version: 13\r\n\r\n")
                    .arg(path, url.host())
                    .arg(url.port(443))
                    .arg(webSocketHandshakeKey);
            webSocketSocket->write(request.toUtf8());
            appendWebSocketLog(QStringLiteral("[%1] 发送加密握手请求").arg(nowText()));
        }
    });

    connect(webSocketSocket, &QSslSocket::readyRead, this, [this]() {
        webSocketBuffer.append(webSocketSocket->readAll());
        processWebSocketBuffer();
    });
    connect(webSocketSocket, &QSslSocket::disconnected, this, [this]() {
        webSocketHandshakeDone = false;
        webSocketBuffer.clear();
        webSocketUi.connectButton->setEnabled(true);
        webSocketUi.disconnectButton->setEnabled(false);
        webSocketUi.sendButton->setEnabled(false);
        appendWebSocketLog(QStringLiteral("[%1] WebSocket 已断开").arg(nowText()));
    });
    connect(webSocketSocket, &QSslSocket::sslErrors, this, [this](const QList<QSslError> &) {
        webSocketSocket->ignoreSslErrors();
        appendWebSocketLog(QStringLiteral("[%1] 已忽略 WebSocket SSL 错误").arg(nowText()));
    });

    connect(mailUi.protocolCombo, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == QLatin1String("POP3")) {
            mailUi.hostEdit->setText(QStringLiteral("pop.qq.com"));
            mailUi.portSpin->setValue(mailUi.sslCheck->isChecked() ? 995 : 110);
            return;
        }
        mailUi.hostEdit->setText(QStringLiteral("imap.qq.com"));
        mailUi.portSpin->setValue(mailUi.sslCheck->isChecked() ? 993 : 143);
    });
    connect(mailUi.sslCheck, &QCheckBox::toggled, this, [this](bool checked) {
        if (mailUi.protocolCombo->currentText() == QLatin1String("POP3")) {
            mailUi.portSpin->setValue(checked ? 995 : 110);
        } else {
            mailUi.portSpin->setValue(checked ? 993 : 143);
        }
    });
    connect(mailUi.connectButton, &QPushButton::clicked, this, &FormProtocolTools::connectMailServer);
    connect(mailUi.loginButton, &QPushButton::clicked, this, &FormProtocolTools::loginMailServer);
    connect(mailUi.listButton, &QPushButton::clicked, this, &FormProtocolTools::listMailData);
    connect(mailUi.commandButton, &QPushButton::clicked, this, &FormProtocolTools::sendMailCommand);
    connect(mailUi.clearLogButton, &QPushButton::clicked, this, [this]() { clearLog(mailUi.logEdit); });
    connect(mailUi.saveLogButton, &QPushButton::clicked, this, [this]() { saveLogToFile(QStringLiteral("mail"), mailUi.logEdit); });
    connect(mailSocket, &QSslSocket::connected, this, [this]() {
        appendMailLog(QStringLiteral("[%1] 邮件服务器连接成功").arg(nowText()));
    });
    connect(mailSocket, &QSslSocket::encrypted, this, [this]() {
        appendMailLog(QStringLiteral("[%1] 邮件 SSL 通道已建立").arg(nowText()));
    });
    connect(mailSocket, &QSslSocket::readyRead, this, [this]() {
        appendMailLog(QStringLiteral("[%1] 收到响应:\n%2").arg(nowText(), QString::fromUtf8(mailSocket->readAll())));
    });
    connect(mailSocket, &QSslSocket::sslErrors, this, [this](const QList<QSslError> &) {
        mailSocket->ignoreSslErrors();
        appendMailLog(QStringLiteral("[%1] 已忽略邮件服务器 SSL 错误").arg(nowText()));
    });

    connect(mqttUi.connectButton, &QPushButton::clicked, this, &FormProtocolTools::connectMqttBroker);
    connect(mqttUi.disconnectButton, &QPushButton::clicked, this, &FormProtocolTools::disconnectMqttBroker);
    connect(mqttUi.subscribeButton, &QPushButton::clicked, this, &FormProtocolTools::subscribeMqttTopic);
    connect(mqttUi.publishButton, &QPushButton::clicked, this, &FormProtocolTools::publishMqttMessage);
    connect(mqttUi.clearLogButton, &QPushButton::clicked, this, [this]() { clearLog(mqttUi.logEdit); });
    connect(mqttUi.saveLogButton, &QPushButton::clicked, this, [this]() { saveLogToFile(QStringLiteral("mqtt"), mqttUi.logEdit); });
    connect(mqttSocket, &QTcpSocket::connected, this, [this]() {
        appendMqttLog(QStringLiteral("[%1] 已连接到 MQTT Broker，发送 CONNECT").arg(nowText()));
        QByteArray variableHeader;
        variableHeader.append('\0');
        variableHeader.append('\x04');
        variableHeader.append("MQTT", 4);
        variableHeader.append('\x04');
        quint8 flags = 0x02;
        if (!mqttUi.userEdit->text().isEmpty()) {
            flags |= 0x80;
        }
        if (!mqttUi.passwordEdit->text().isEmpty()) {
            flags |= 0x40;
        }
        variableHeader.append(static_cast<char>(flags));
        variableHeader.append('\0');
        variableHeader.append('\x3C');

        QByteArray payload;
        payload.append(buildLengthPrefixed(mqttUi.clientIdEdit->text().toUtf8()));
        if (!mqttUi.userEdit->text().isEmpty()) {
            payload.append(buildLengthPrefixed(mqttUi.userEdit->text().toUtf8()));
        }
        if (!mqttUi.passwordEdit->text().isEmpty()) {
            payload.append(buildLengthPrefixed(mqttUi.passwordEdit->text().toUtf8()));
        }

        QByteArray packet;
        packet.append(static_cast<char>(0x10));
        packet.append(encodeMqttRemainingLength(variableHeader.size() + payload.size()));
        packet.append(variableHeader);
        packet.append(payload);
        mqttSocket->write(packet);
    });
    connect(mqttSocket, &QTcpSocket::readyRead, this, [this]() {
        mqttBuffer.append(mqttSocket->readAll());
        processMqttBuffer();
    });
    connect(mqttSocket, &QTcpSocket::disconnected, this, [this]() {
        mqttUi.connectButton->setEnabled(true);
        mqttUi.disconnectButton->setEnabled(false);
        mqttUi.subscribeButton->setEnabled(false);
        mqttUi.publishButton->setEnabled(false);
        appendMqttLog(QStringLiteral("[%1] MQTT 连接已断开").arg(nowText()));
    });

    connect(modbusUi.connectButton, &QPushButton::clicked, this, &FormProtocolTools::connectModbusServer);
    connect(modbusUi.disconnectButton, &QPushButton::clicked, this, &FormProtocolTools::disconnectModbusServer);
    connect(modbusUi.readButton, &QPushButton::clicked, this, &FormProtocolTools::readModbusHoldingRegisters);
    connect(modbusUi.writeButton, &QPushButton::clicked, this, &FormProtocolTools::writeModbusSingleRegister);
    connect(modbusUi.clearLogButton, &QPushButton::clicked, this, [this]() { clearLog(modbusUi.logEdit); });
    connect(modbusUi.saveLogButton, &QPushButton::clicked, this, [this]() { saveLogToFile(QStringLiteral("modbus"), modbusUi.logEdit); });
    connect(modbusSocket, &QTcpSocket::connected, this, [this]() {
        modbusUi.connectButton->setEnabled(false);
        modbusUi.disconnectButton->setEnabled(true);
        modbusUi.readButton->setEnabled(true);
        modbusUi.writeButton->setEnabled(true);
        appendModbusLog(QStringLiteral("[%1] 已连接到 Modbus 设备").arg(nowText()));
    });
    connect(modbusSocket, &QTcpSocket::readyRead, this, [this]() {
        modbusBuffer.append(modbusSocket->readAll());
        processModbusBuffer();
    });
    connect(modbusSocket, &QTcpSocket::disconnected, this, [this]() {
        modbusUi.connectButton->setEnabled(true);
        modbusUi.disconnectButton->setEnabled(false);
        modbusUi.readButton->setEnabled(false);
        modbusUi.writeButton->setEnabled(false);
        appendModbusLog(QStringLiteral("[%1] Modbus 连接已断开").arg(nowText()));
    });
}

void FormProtocolTools::sendHttpRequest()
{
    const QUrl url(httpUi.urlEdit->text().trimmed());
    if (!url.isValid() || url.scheme().isEmpty()) {
        appendHttpLog(QStringLiteral("[%1] URL 无效").arg(nowText()));
        return;
    }

    QNetworkRequest request(url);
    const auto headers = parseHeaderLines(httpUi.headersEdit->toPlainText());
    for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    const QByteArray body = httpUi.bodyEdit->toPlainText().toUtf8();
    const QString method = httpUi.methodCombo->currentText();
    appendHttpLog(QStringLiteral("[%1] 发送 %2 %3").arg(nowText(), method, url.toString()));

    QNetworkReply *reply = nullptr;
    if (method == QLatin1String("GET")) {
        reply = httpManager->get(request);
    } else if (method == QLatin1String("POST")) {
        reply = httpManager->post(request, body);
    } else if (method == QLatin1String("PUT")) {
        reply = httpManager->put(request, body);
    } else if (method == QLatin1String("DELETE")) {
        reply = httpManager->sendCustomRequest(request, "DELETE", body);
    } else {
        reply = httpManager->sendCustomRequest(request, method.toUtf8(), body);
    }

    if (httpUi.ignoreSslCheck->isChecked()) {
        connect(reply, &QNetworkReply::sslErrors, this, [this, reply](const QList<QSslError> &) {
            reply->ignoreSslErrors();
            appendHttpLog(QStringLiteral("[%1] 已忽略 HTTPS 证书错误").arg(nowText()));
        });
    }
}

void FormProtocolTools::handleHttpReply(QNetworkReply *reply)
{
    QStringList headerLines;
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    headerLines << QStringLiteral("HTTP 状态码: %1").arg(status);
    const auto rawHeaders = reply->rawHeaderPairs();
    for (const auto &pair : rawHeaders) {
        headerLines << QString::fromUtf8(pair.first) + QStringLiteral(": ") + QString::fromUtf8(pair.second);
    }

    httpUi.responseHeadersEdit->setPlainText(headerLines.join(QStringLiteral("\n")));
    httpUi.responseBodyEdit->setPlainText(QString::fromUtf8(reply->readAll()));

    if (reply->error() == QNetworkReply::NoError) {
        appendHttpLog(QStringLiteral("[%1] 请求完成，状态码 %2").arg(nowText()).arg(status));
    } else {
        appendHttpLog(QStringLiteral("[%1] 请求失败: %2").arg(nowText(), reply->errorString()));
    }
    reply->deleteLater();
}

QMap<QString, QString> FormProtocolTools::parseHeaderLines(const QString &text) const
{
    QMap<QString, QString> result;
    const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        const int pos = line.indexOf(':');
        if (pos <= 0) {
            continue;
        }
        result.insert(line.left(pos).trimmed(), line.mid(pos + 1).trimmed());
    }
    return result;
}

void FormProtocolTools::appendHttpLog(const QString &message)
{
    httpUi.logEdit->appendPlainText(message);
}

void FormProtocolTools::connectWebSocket()
{
    const QUrl url(webSocketUi.urlEdit->text().trimmed());
    if (!url.isValid() || (url.scheme() != QLatin1String("ws") && url.scheme() != QLatin1String("wss"))) {
        appendWebSocketLog(QStringLiteral("[%1] WebSocket 地址无效").arg(nowText()));
        return;
    }

    webSocketHandshakeDone = false;
    webSocketBuffer.clear();
    webSocketUi.connectButton->setEnabled(false);
    appendWebSocketLog(QStringLiteral("[%1] 正在连接 %2").arg(nowText(), url.toString()));

    if (url.scheme() == QLatin1String("wss")) {
        webSocketSocket->connectToHostEncrypted(url.host(), static_cast<quint16>(url.port(443)));
    } else {
        webSocketSocket->connectToHost(url.host(), static_cast<quint16>(url.port(80)));
    }
}

void FormProtocolTools::disconnectWebSocket()
{
    webSocketSocket->disconnectFromHost();
}

void FormProtocolTools::sendWebSocketMessage()
{
    if (!webSocketHandshakeDone) {
        appendWebSocketLog(QStringLiteral("[%1] 尚未完成握手").arg(nowText()));
        return;
    }

    const QString text = webSocketUi.sendEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }

    webSocketSocket->write(buildWebSocketFrame(text));
    appendWebSocketLog(QStringLiteral("[%1] 发送: %2").arg(nowText(), text));
}

void FormProtocolTools::processWebSocketBuffer()
{
    if (!webSocketHandshakeDone) {
        const int headerEnd = webSocketBuffer.indexOf("\r\n\r\n");
        if (headerEnd < 0) {
            return;
        }

        const QByteArray handshakeReply = webSocketBuffer.left(headerEnd + 4);
        webSocketBuffer.remove(0, headerEnd + 4);
        if (!handshakeReply.startsWith("HTTP/1.1 101")) {
            appendWebSocketLog(QStringLiteral("[%1] 握手失败:\n%2")
                               .arg(nowText(), QString::fromUtf8(handshakeReply)));
            webSocketSocket->disconnectFromHost();
            return;
        }

        webSocketHandshakeDone = true;
        webSocketUi.disconnectButton->setEnabled(true);
        webSocketUi.sendButton->setEnabled(true);
        appendWebSocketLog(QStringLiteral("[%1] 握手成功，WebSocket 已连接").arg(nowText()));
    }

    while (webSocketBuffer.size() >= 2) {
        const quint8 b0 = static_cast<quint8>(webSocketBuffer.at(0));
        const quint8 b1 = static_cast<quint8>(webSocketBuffer.at(1));
        int payloadLength = b1 & 0x7F;
        int offset = 2;
        if (payloadLength == 126) {
            if (webSocketBuffer.size() < 4) {
                return;
            }
            payloadLength = (static_cast<quint8>(webSocketBuffer.at(2)) << 8)
                            | static_cast<quint8>(webSocketBuffer.at(3));
            offset = 4;
        } else if (payloadLength == 127) {
            appendWebSocketLog(QStringLiteral("[%1] 暂不支持超长帧").arg(nowText()));
            return;
        }

        const bool masked = (b1 & 0x80) != 0;
        QByteArray maskKey;
        if (masked) {
            if (webSocketBuffer.size() < offset + 4) {
                return;
            }
            maskKey = webSocketBuffer.mid(offset, 4);
            offset += 4;
        }

        if (webSocketBuffer.size() < offset + payloadLength) {
            return;
        }

        QByteArray payload = webSocketBuffer.mid(offset, payloadLength);
        webSocketBuffer.remove(0, offset + payloadLength);

        if (masked) {
            for (int i = 0; i < payload.size(); ++i) {
                payload[i] = payload.at(i) ^ maskKey.at(i % 4);
            }
        }

        const quint8 opcode = b0 & 0x0F;
        if (opcode == 0x1) {
            appendWebSocketLog(QStringLiteral("[%1] 收到: %2").arg(nowText(), QString::fromUtf8(payload)));
        } else if (opcode == 0x8) {
            appendWebSocketLog(QStringLiteral("[%1] 服务端关闭连接").arg(nowText()));
            webSocketSocket->disconnectFromHost();
            return;
        }
    }
}

QByteArray FormProtocolTools::buildWebSocketFrame(const QString &message) const
{
    const QByteArray payload = message.toUtf8();
    QByteArray frame;
    frame.append(static_cast<char>(0x81));

    if (payload.size() < 126) {
        frame.append(static_cast<char>(0x80 | payload.size()));
    } else {
        frame.append(static_cast<char>(0x80 | 126));
        frame.append(static_cast<char>((payload.size() >> 8) & 0xFF));
        frame.append(static_cast<char>(payload.size() & 0xFF));
    }

    QByteArray maskKey(4, Qt::Uninitialized);
    for (int i = 0; i < 4; ++i) {
        maskKey[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    frame.append(maskKey);

    QByteArray maskedPayload = payload;
    for (int i = 0; i < maskedPayload.size(); ++i) {
        maskedPayload[i] = maskedPayload.at(i) ^ maskKey.at(i % 4);
    }
    frame.append(maskedPayload);
    return frame;
}

void FormProtocolTools::appendWebSocketLog(const QString &message)
{
    webSocketUi.logEdit->appendPlainText(message);
}

void FormProtocolTools::connectMailServer()
{
    mailSocket->abort();
    appendMailLog(QStringLiteral("[%1] 正在连接 %2:%3")
                  .arg(nowText(), mailUi.hostEdit->text())
                  .arg(mailUi.portSpin->value()));
    if (mailUi.sslCheck->isChecked()) {
        mailSocket->connectToHostEncrypted(mailUi.hostEdit->text(), static_cast<quint16>(mailUi.portSpin->value()));
    } else {
        mailSocket->connectToHost(mailUi.hostEdit->text(), static_cast<quint16>(mailUi.portSpin->value()));
    }
}

void FormProtocolTools::loginMailServer()
{
    if (mailSocket->state() != QAbstractSocket::ConnectedState) {
        appendMailLog(QStringLiteral("[%1] 请先连接服务器").arg(nowText()));
        return;
    }

    QString command;
    if (mailUi.protocolCombo->currentText() == QLatin1String("POP3")) {
        command = QStringLiteral("USER %1\r\nPASS %2\r\n")
                      .arg(mailUi.userEdit->text(), mailUi.passwordEdit->text());
    } else {
        command = QStringLiteral("a001 LOGIN %1 %2\r\n")
                      .arg(mailUi.userEdit->text(), mailUi.passwordEdit->text());
    }
    mailSocket->write(command.toUtf8());
    appendMailLog(QStringLiteral("[%1] 已发送登录命令").arg(nowText()));
}

void FormProtocolTools::listMailData()
{
    QString command;
    if (mailUi.protocolCombo->currentText() == QLatin1String("POP3")) {
        command = QStringLiteral("LIST\r\n");
    } else {
        command = QStringLiteral("a002 LIST \"\" *\r\na003 SELECT INBOX\r\n");
    }
    mailSocket->write(command.toUtf8());
    appendMailLog(QStringLiteral("[%1] 已发送列表命令").arg(nowText()));
}

void FormProtocolTools::sendMailCommand()
{
    QString command = mailUi.commandEdit->text().trimmed();
    if (command.isEmpty()) {
        return;
    }
    if (!command.endsWith(QStringLiteral("\r\n"))) {
        command.append(QStringLiteral("\r\n"));
    }
    mailSocket->write(command.toUtf8());
    appendMailLog(QStringLiteral("[%1] 已发送: %2").arg(nowText(), command.trimmed()));
}

void FormProtocolTools::appendMailLog(const QString &message)
{
    mailUi.logEdit->appendPlainText(message);
}

void FormProtocolTools::connectMqttBroker()
{
    mqttSocket->abort();
    mqttBuffer.clear();
    mqttPacketId = 1;
    mqttUi.connectButton->setEnabled(false);
    mqttSocket->connectToHost(mqttUi.hostEdit->text(), static_cast<quint16>(mqttUi.portSpin->value()));
}

void FormProtocolTools::disconnectMqttBroker()
{
    if (mqttSocket->state() == QAbstractSocket::ConnectedState) {
        QByteArray disconnectPacket;
        disconnectPacket.append(static_cast<char>(0xE0));
        disconnectPacket.append(static_cast<char>(0x00));
        mqttSocket->write(disconnectPacket);
    }
    mqttSocket->disconnectFromHost();
}

void FormProtocolTools::subscribeMqttTopic()
{
    const QByteArray topic = mqttUi.topicEdit->text().toUtf8();
    const quint8 qos = static_cast<quint8>(mqttUi.qosCombo->currentData().toInt());
    QByteArray variableHeader;
    variableHeader.append(static_cast<char>((mqttPacketId >> 8) & 0xFF));
    variableHeader.append(static_cast<char>(mqttPacketId & 0xFF));

    QByteArray payload = buildLengthPrefixed(topic);
    payload.append(static_cast<char>(qos));

    QByteArray packet;
    packet.append(static_cast<char>(0x82));
    packet.append(encodeMqttRemainingLength(variableHeader.size() + payload.size()));
    packet.append(variableHeader);
    packet.append(payload);
    mqttSocket->write(packet);
    appendMqttLog(QStringLiteral("[%1] 已订阅主题: %2，QoS=%3")
                  .arg(nowText(), mqttUi.topicEdit->text())
                  .arg(qos));
    ++mqttPacketId;
}

void FormProtocolTools::publishMqttMessage()
{
    const QByteArray topic = mqttUi.topicEdit->text().toUtf8();
    const QByteArray payloadText = mqttUi.payloadEdit->toPlainText().toUtf8();
    const quint8 qos = static_cast<quint8>(mqttUi.qosCombo->currentData().toInt());
    QByteArray variableHeader = buildLengthPrefixed(topic);
    if (qos > 0) {
        variableHeader.append(static_cast<char>((mqttPacketId >> 8) & 0xFF));
        variableHeader.append(static_cast<char>(mqttPacketId & 0xFF));
    }

    QByteArray packet;
    packet.append(static_cast<char>(0x30 | (qos << 1)));
    packet.append(encodeMqttRemainingLength(variableHeader.size() + payloadText.size()));
    packet.append(variableHeader);
    packet.append(payloadText);
    mqttSocket->write(packet);
    appendMqttLog(QStringLiteral("[%1] 发布到 %2 (QoS=%3): %4")
                  .arg(nowText(), mqttUi.topicEdit->text())
                  .arg(qos)
                  .arg(mqttUi.payloadEdit->toPlainText()));
    if (qos > 0) {
        ++mqttPacketId;
    }
}

void FormProtocolTools::processMqttBuffer()
{
    auto decodeLength = [](const QByteArray &buffer, int start, int *lengthBytes) -> int {
        int multiplier = 1;
        int value = 0;
        int index = start;
        *lengthBytes = 0;
        while (index < buffer.size()) {
            const quint8 encodedByte = static_cast<quint8>(buffer.at(index));
            value += (encodedByte & 127) * multiplier;
            ++(*lengthBytes);
            ++index;
            if ((encodedByte & 128) == 0) {
                return value;
            }
            multiplier *= 128;
        }
        return -1;
    };

    while (mqttBuffer.size() >= 2) {
        const quint8 fixedHeader = static_cast<quint8>(mqttBuffer.at(0));
        int lengthBytes = 0;
        const int remainingLength = decodeLength(mqttBuffer, 1, &lengthBytes);
        if (remainingLength < 0 || mqttBuffer.size() < 1 + lengthBytes + remainingLength) {
            return;
        }

        const quint8 packetType = fixedHeader >> 4;
        const QByteArray packet = mqttBuffer.mid(1 + lengthBytes, remainingLength);
        mqttBuffer.remove(0, 1 + lengthBytes + remainingLength);

        if (packetType == 2 && packet.size() >= 2) {
            const quint8 returnCode = static_cast<quint8>(packet.at(1));
            if (returnCode == 0) {
                mqttUi.disconnectButton->setEnabled(true);
                mqttUi.subscribeButton->setEnabled(true);
                mqttUi.publishButton->setEnabled(true);
                appendMqttLog(QStringLiteral("[%1] MQTT 连接成功").arg(nowText()));
            } else {
                mqttUi.connectButton->setEnabled(true);
                appendMqttLog(QStringLiteral("[%1] MQTT 连接失败，返回码: %2").arg(nowText()).arg(returnCode));
            }
        } else if (packetType == 9) {
            appendMqttLog(QStringLiteral("[%1] 订阅确认").arg(nowText()));
        } else if (packetType == 4 && packet.size() >= 2) {
            const quint16 packetId = (static_cast<quint8>(packet.at(0)) << 8) | static_cast<quint8>(packet.at(1));
            appendMqttLog(QStringLiteral("[%1] 收到 PUBACK，PacketId=%2").arg(nowText()).arg(packetId));
        } else if (packetType == 3 && packet.size() >= 2) {
            const int topicLength = (static_cast<quint8>(packet.at(0)) << 8) | static_cast<quint8>(packet.at(1));
            const quint8 qos = (fixedHeader >> 1) & 0x03;
            int payloadOffset = 2 + topicLength;
            if (qos > 0) {
                payloadOffset += 2;
            }
            if (packet.size() >= payloadOffset) {
                const QString topic = QString::fromUtf8(packet.mid(2, topicLength));
                const QString payload = QString::fromUtf8(packet.mid(payloadOffset));
                appendMqttLog(QStringLiteral("[%1] 收到消息 [%2] (QoS=%3): %4")
                              .arg(nowText(), topic)
                              .arg(qos)
                              .arg(payload));
            }
        }
    }
}

QByteArray FormProtocolTools::encodeMqttRemainingLength(int length) const
{
    QByteArray encoded;
    do {
        quint8 byte = length % 128;
        length /= 128;
        if (length > 0) {
            byte |= 0x80;
        }
        encoded.append(static_cast<char>(byte));
    } while (length > 0);
    return encoded;
}

QByteArray FormProtocolTools::encodeMqttString(const QString &text) const
{
    return buildLengthPrefixed(text.toUtf8());
}

void FormProtocolTools::appendMqttLog(const QString &message)
{
    mqttUi.logEdit->appendPlainText(message);
}

void FormProtocolTools::connectModbusServer()
{
    modbusSocket->connectToHost(modbusUi.hostEdit->text(), static_cast<quint16>(modbusUi.portSpin->value()));
}

void FormProtocolTools::disconnectModbusServer()
{
    modbusSocket->disconnectFromHost();
}

void FormProtocolTools::readModbusHoldingRegisters()
{
    QByteArray request;
    const quint16 tx = modbusTransactionId++;
    const quint16 startAddr = static_cast<quint16>(modbusUi.readAddressSpin->value());
    const quint16 count = static_cast<quint16>(modbusUi.readCountSpin->value());
    const quint8 functionCode = static_cast<quint8>(modbusUi.readFunctionCombo->currentData().toInt());
    request.append(static_cast<char>((tx >> 8) & 0xFF));
    request.append(static_cast<char>(tx & 0xFF));
    request.append('\0');
    request.append('\0');
    request.append('\0');
    request.append('\x06');
    request.append(static_cast<char>(modbusUi.unitIdSpin->value()));
    request.append(static_cast<char>(functionCode));
    request.append(static_cast<char>((startAddr >> 8) & 0xFF));
    request.append(static_cast<char>(startAddr & 0xFF));
    request.append(static_cast<char>((count >> 8) & 0xFF));
    request.append(static_cast<char>(count & 0xFF));
    modbusSocket->write(request);
    appendModbusLog(QStringLiteral("[%1] 读取寄存器 FC=%2: 起始=%3 数量=%4")
                    .arg(nowText())
                    .arg(functionCode)
                    .arg(startAddr)
                    .arg(count));
}

void FormProtocolTools::writeModbusSingleRegister()
{
    QByteArray request;
    const quint16 tx = modbusTransactionId++;
    const quint16 address = static_cast<quint16>(modbusUi.writeAddressSpin->value());
    const quint16 value = static_cast<quint16>(modbusUi.writeValueSpin->value());
    request.append(static_cast<char>((tx >> 8) & 0xFF));
    request.append(static_cast<char>(tx & 0xFF));
    request.append('\0');
    request.append('\0');
    request.append('\0');
    request.append('\x06');
    request.append(static_cast<char>(modbusUi.unitIdSpin->value()));
    request.append('\x06');
    request.append(static_cast<char>((address >> 8) & 0xFF));
    request.append(static_cast<char>(address & 0xFF));
    request.append(static_cast<char>((value >> 8) & 0xFF));
    request.append(static_cast<char>(value & 0xFF));
    modbusSocket->write(request);
    appendModbusLog(QStringLiteral("[%1] 写单寄存器: 地址=%2 值=%3")
                    .arg(nowText())
                    .arg(address)
                    .arg(value));
}

void FormProtocolTools::processModbusBuffer()
{
    while (modbusBuffer.size() >= 7) {
        const quint16 length = (static_cast<quint8>(modbusBuffer.at(4)) << 8)
                               | static_cast<quint8>(modbusBuffer.at(5));
        const int totalFrameSize = 6 + length;
        if (modbusBuffer.size() < totalFrameSize) {
            return;
        }

        const QByteArray frame = modbusBuffer.left(totalFrameSize);
        modbusBuffer.remove(0, totalFrameSize);

        const quint8 functionCode = static_cast<quint8>(frame.at(7));
        if ((functionCode == 0x03 || functionCode == 0x04) && frame.size() >= 9) {
            const int byteCount = static_cast<quint8>(frame.at(8));
            QStringList values;
            for (int i = 0; i + 1 < byteCount; i += 2) {
                const int base = 9 + i;
                if (base + 1 >= frame.size()) {
                    break;
                }
                const quint16 value = (static_cast<quint8>(frame.at(base)) << 8)
                                      | static_cast<quint8>(frame.at(base + 1));
                values << QString::number(value);
            }
            appendModbusLog(QStringLiteral("[%1] FC=%2 读取结果: %3")
                            .arg(nowText())
                            .arg(functionCode)
                            .arg(values.join(QStringLiteral(", "))));
        } else if (functionCode == 0x06 && frame.size() >= 12) {
            const quint16 address = (static_cast<quint8>(frame.at(8)) << 8)
                                    | static_cast<quint8>(frame.at(9));
            const quint16 value = (static_cast<quint8>(frame.at(10)) << 8)
                                  | static_cast<quint8>(frame.at(11));
            appendModbusLog(QStringLiteral("[%1] 写入确认: 地址=%2 值=%3")
                            .arg(nowText())
                            .arg(address)
                            .arg(value));
        } else if (functionCode & 0x80) {
            const quint8 exceptionCode = static_cast<quint8>(frame.at(8));
            appendModbusLog(QStringLiteral("[%1] Modbus 异常码: %2").arg(nowText()).arg(exceptionCode));
        }
    }
}

void FormProtocolTools::appendModbusLog(const QString &message)
{
    modbusUi.logEdit->appendPlainText(message);
}

void FormProtocolTools::loadSettings()
{
    QSettings settings;

    protocolTabWidget->setCurrentIndex(settings.value(QStringLiteral("ProtocolTools/currentTab"), 0).toInt());

    httpUi.methodCombo->setCurrentText(settings.value(QStringLiteral("ProtocolTools/Http/method"), QStringLiteral("GET")).toString());
    httpUi.urlEdit->setText(settings.value(QStringLiteral("ProtocolTools/Http/url"), httpUi.urlEdit->text()).toString());
    httpUi.ignoreSslCheck->setChecked(settings.value(QStringLiteral("ProtocolTools/Http/ignoreSsl"), false).toBool());
    httpUi.headersEdit->setPlainText(settings.value(QStringLiteral("ProtocolTools/Http/headers")).toString());
    httpUi.bodyEdit->setPlainText(settings.value(QStringLiteral("ProtocolTools/Http/body")).toString());

    webSocketUi.urlEdit->setText(settings.value(QStringLiteral("ProtocolTools/WebSocket/url"), webSocketUi.urlEdit->text()).toString());
    webSocketUi.sendEdit->setPlainText(settings.value(QStringLiteral("ProtocolTools/WebSocket/send")).toString());

    mailUi.protocolCombo->setCurrentText(settings.value(QStringLiteral("ProtocolTools/Mail/protocol"), QStringLiteral("POP3")).toString());
    mailUi.hostEdit->setText(settings.value(QStringLiteral("ProtocolTools/Mail/host"), mailUi.hostEdit->text()).toString());
    mailUi.portSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Mail/port"), mailUi.portSpin->value()).toInt());
    mailUi.sslCheck->setChecked(settings.value(QStringLiteral("ProtocolTools/Mail/ssl"), true).toBool());
    mailUi.userEdit->setText(settings.value(QStringLiteral("ProtocolTools/Mail/user")).toString());
    mailUi.commandEdit->setText(settings.value(QStringLiteral("ProtocolTools/Mail/command"), mailUi.commandEdit->text()).toString());

    mqttUi.hostEdit->setText(settings.value(QStringLiteral("ProtocolTools/Mqtt/host"), mqttUi.hostEdit->text()).toString());
    mqttUi.portSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Mqtt/port"), mqttUi.portSpin->value()).toInt());
    mqttUi.clientIdEdit->setText(settings.value(QStringLiteral("ProtocolTools/Mqtt/clientId"), mqttUi.clientIdEdit->text()).toString());
    mqttUi.userEdit->setText(settings.value(QStringLiteral("ProtocolTools/Mqtt/user")).toString());
    mqttUi.topicEdit->setText(settings.value(QStringLiteral("ProtocolTools/Mqtt/topic"), mqttUi.topicEdit->text()).toString());
    mqttUi.payloadEdit->setPlainText(settings.value(QStringLiteral("ProtocolTools/Mqtt/payload")).toString());
    mqttUi.qosCombo->setCurrentIndex(settings.value(QStringLiteral("ProtocolTools/Mqtt/qosIndex"), 0).toInt());

    modbusUi.hostEdit->setText(settings.value(QStringLiteral("ProtocolTools/Modbus/host"), modbusUi.hostEdit->text()).toString());
    modbusUi.portSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Modbus/port"), modbusUi.portSpin->value()).toInt());
    modbusUi.unitIdSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Modbus/unitId"), modbusUi.unitIdSpin->value()).toInt());
    modbusUi.readFunctionCombo->setCurrentIndex(settings.value(QStringLiteral("ProtocolTools/Modbus/readFcIndex"), 0).toInt());
    modbusUi.readAddressSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Modbus/readAddress"), modbusUi.readAddressSpin->value()).toInt());
    modbusUi.readCountSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Modbus/readCount"), modbusUi.readCountSpin->value()).toInt());
    modbusUi.writeAddressSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Modbus/writeAddress"), modbusUi.writeAddressSpin->value()).toInt());
    modbusUi.writeValueSpin->setValue(settings.value(QStringLiteral("ProtocolTools/Modbus/writeValue"), modbusUi.writeValueSpin->value()).toInt());
}

void FormProtocolTools::saveSettings() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("ProtocolTools/currentTab"), protocolTabWidget->currentIndex());

    settings.setValue(QStringLiteral("ProtocolTools/Http/method"), httpUi.methodCombo->currentText());
    settings.setValue(QStringLiteral("ProtocolTools/Http/url"), httpUi.urlEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Http/ignoreSsl"), httpUi.ignoreSslCheck->isChecked());
    settings.setValue(QStringLiteral("ProtocolTools/Http/headers"), httpUi.headersEdit->toPlainText());
    settings.setValue(QStringLiteral("ProtocolTools/Http/body"), httpUi.bodyEdit->toPlainText());

    settings.setValue(QStringLiteral("ProtocolTools/WebSocket/url"), webSocketUi.urlEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/WebSocket/send"), webSocketUi.sendEdit->toPlainText());

    settings.setValue(QStringLiteral("ProtocolTools/Mail/protocol"), mailUi.protocolCombo->currentText());
    settings.setValue(QStringLiteral("ProtocolTools/Mail/host"), mailUi.hostEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Mail/port"), mailUi.portSpin->value());
    settings.setValue(QStringLiteral("ProtocolTools/Mail/ssl"), mailUi.sslCheck->isChecked());
    settings.setValue(QStringLiteral("ProtocolTools/Mail/user"), mailUi.userEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Mail/command"), mailUi.commandEdit->text());

    settings.setValue(QStringLiteral("ProtocolTools/Mqtt/host"), mqttUi.hostEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Mqtt/port"), mqttUi.portSpin->value());
    settings.setValue(QStringLiteral("ProtocolTools/Mqtt/clientId"), mqttUi.clientIdEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Mqtt/user"), mqttUi.userEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Mqtt/topic"), mqttUi.topicEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Mqtt/payload"), mqttUi.payloadEdit->toPlainText());
    settings.setValue(QStringLiteral("ProtocolTools/Mqtt/qosIndex"), mqttUi.qosCombo->currentIndex());

    settings.setValue(QStringLiteral("ProtocolTools/Modbus/host"), modbusUi.hostEdit->text());
    settings.setValue(QStringLiteral("ProtocolTools/Modbus/port"), modbusUi.portSpin->value());
    settings.setValue(QStringLiteral("ProtocolTools/Modbus/unitId"), modbusUi.unitIdSpin->value());
    settings.setValue(QStringLiteral("ProtocolTools/Modbus/readFcIndex"), modbusUi.readFunctionCombo->currentIndex());
    settings.setValue(QStringLiteral("ProtocolTools/Modbus/readAddress"), modbusUi.readAddressSpin->value());
    settings.setValue(QStringLiteral("ProtocolTools/Modbus/readCount"), modbusUi.readCountSpin->value());
    settings.setValue(QStringLiteral("ProtocolTools/Modbus/writeAddress"), modbusUi.writeAddressSpin->value());
    settings.setValue(QStringLiteral("ProtocolTools/Modbus/writeValue"), modbusUi.writeValueSpin->value());
}

void FormProtocolTools::clearLog(QPlainTextEdit *edit)
{
    if (edit) {
        edit->clear();
    }
}

void FormProtocolTools::saveLogToFile(const QString &moduleName, QPlainTextEdit *edit) const
{
    if (!edit || edit->toPlainText().trimmed().isEmpty()) {
        return;
    }

    const QString logDir = ensureProjectLogDirPath(QStringLiteral("protocol"));
    const QString fileName = QDir(logDir).filePath(
        QStringLiteral("%1_%2.txt")
            .arg(moduleName, QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_hhmmss"))));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif
    out << edit->toPlainText();
    file.close();
}

void FormProtocolTools::closeEvent(QCloseEvent *event)
{
    saveSettings();
    saveLogToFile(QStringLiteral("http"), httpUi.logEdit);
    saveLogToFile(QStringLiteral("websocket"), webSocketUi.logEdit);
    saveLogToFile(QStringLiteral("mail"), mailUi.logEdit);
    saveLogToFile(QStringLiteral("mqtt"), mqttUi.logEdit);
    saveLogToFile(QStringLiteral("modbus"), modbusUi.logEdit);
    QWidget::closeEvent(event);
}
