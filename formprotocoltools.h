#ifndef FORMPROTOCOLTOOLS_H
#define FORMPROTOCOLTOOLS_H

#include <QByteArray>
#include <QCloseEvent>
#include <QMap>
#include <QSettings>
#include <QTcpSocket>
#include <QWidget>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QNetworkAccessManager;
class QNetworkReply;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QSslSocket;
class QTabWidget;

class FormProtocolTools : public QWidget
{
    Q_OBJECT

public:
    explicit FormProtocolTools(QWidget *parent = nullptr);
    ~FormProtocolTools() override;

private:
    struct HttpUi {
        QWidget *page = nullptr;
        QComboBox *methodCombo = nullptr;
        QLineEdit *urlEdit = nullptr;
        QCheckBox *ignoreSslCheck = nullptr;
        QPlainTextEdit *headersEdit = nullptr;
        QPlainTextEdit *bodyEdit = nullptr;
        QPlainTextEdit *responseHeadersEdit = nullptr;
        QPlainTextEdit *responseBodyEdit = nullptr;
        QPlainTextEdit *logEdit = nullptr;
        QPushButton *sendButton = nullptr;
        QPushButton *clearLogButton = nullptr;
        QPushButton *saveLogButton = nullptr;
    };

    struct WebSocketUi {
        QWidget *page = nullptr;
        QLineEdit *urlEdit = nullptr;
        QPushButton *connectButton = nullptr;
        QPushButton *disconnectButton = nullptr;
        QPushButton *sendButton = nullptr;
        QPlainTextEdit *sendEdit = nullptr;
        QPlainTextEdit *logEdit = nullptr;
        QPushButton *clearLogButton = nullptr;
        QPushButton *saveLogButton = nullptr;
    };

    struct MailUi {
        QWidget *page = nullptr;
        QComboBox *protocolCombo = nullptr;
        QLineEdit *hostEdit = nullptr;
        QSpinBox *portSpin = nullptr;
        QCheckBox *sslCheck = nullptr;
        QLineEdit *userEdit = nullptr;
        QLineEdit *passwordEdit = nullptr;
        QLineEdit *commandEdit = nullptr;
        QPushButton *connectButton = nullptr;
        QPushButton *loginButton = nullptr;
        QPushButton *listButton = nullptr;
        QPushButton *commandButton = nullptr;
        QPlainTextEdit *logEdit = nullptr;
        QPushButton *clearLogButton = nullptr;
        QPushButton *saveLogButton = nullptr;
    };

    struct MqttUi {
        QWidget *page = nullptr;
        QLineEdit *hostEdit = nullptr;
        QSpinBox *portSpin = nullptr;
        QLineEdit *clientIdEdit = nullptr;
        QLineEdit *userEdit = nullptr;
        QLineEdit *passwordEdit = nullptr;
        QLineEdit *topicEdit = nullptr;
        QComboBox *qosCombo = nullptr;
        QPlainTextEdit *payloadEdit = nullptr;
        QPushButton *connectButton = nullptr;
        QPushButton *disconnectButton = nullptr;
        QPushButton *subscribeButton = nullptr;
        QPushButton *publishButton = nullptr;
        QPlainTextEdit *logEdit = nullptr;
        QPushButton *clearLogButton = nullptr;
        QPushButton *saveLogButton = nullptr;
    };

    struct ModbusUi {
        QWidget *page = nullptr;
        QLineEdit *hostEdit = nullptr;
        QSpinBox *portSpin = nullptr;
        QSpinBox *unitIdSpin = nullptr;
        QComboBox *readFunctionCombo = nullptr;
        QSpinBox *readAddressSpin = nullptr;
        QSpinBox *readCountSpin = nullptr;
        QSpinBox *writeAddressSpin = nullptr;
        QSpinBox *writeValueSpin = nullptr;
        QPushButton *connectButton = nullptr;
        QPushButton *disconnectButton = nullptr;
        QPushButton *readButton = nullptr;
        QPushButton *writeButton = nullptr;
        QPlainTextEdit *logEdit = nullptr;
        QPushButton *clearLogButton = nullptr;
        QPushButton *saveLogButton = nullptr;
    };

    QTabWidget *protocolTabWidget = nullptr;
    HttpUi httpUi;
    WebSocketUi webSocketUi;
    MailUi mailUi;
    MqttUi mqttUi;
    ModbusUi modbusUi;

    QNetworkAccessManager *httpManager = nullptr;
    QSslSocket *webSocketSocket = nullptr;
    QByteArray webSocketBuffer;
    bool webSocketHandshakeDone = false;
    QString webSocketHandshakeKey;

    QSslSocket *mailSocket = nullptr;

    QTcpSocket *mqttSocket = nullptr;
    QByteArray mqttBuffer;
    quint16 mqttPacketId = 1;

    QTcpSocket *modbusSocket = nullptr;
    QByteArray modbusBuffer;
    quint16 modbusTransactionId = 1;

    void buildUi();
    void loadSettings();
    void saveSettings() const;
    QWidget *createHttpTab();
    QWidget *createWebSocketTab();
    QWidget *createMailTab();
    QWidget *createMqttTab();
    QWidget *createModbusTab();
    void initConnections();

    void sendHttpRequest();
    void handleHttpReply(QNetworkReply *reply);
    QMap<QString, QString> parseHeaderLines(const QString &text) const;
    void appendHttpLog(const QString &message);

    void connectWebSocket();
    void disconnectWebSocket();
    void sendWebSocketMessage();
    void processWebSocketBuffer();
    QByteArray buildWebSocketFrame(const QString &message) const;
    void appendWebSocketLog(const QString &message);

    void connectMailServer();
    void loginMailServer();
    void listMailData();
    void sendMailCommand();
    void appendMailLog(const QString &message);

    void connectMqttBroker();
    void disconnectMqttBroker();
    void subscribeMqttTopic();
    void publishMqttMessage();
    void processMqttBuffer();
    QByteArray encodeMqttRemainingLength(int length) const;
    QByteArray encodeMqttString(const QString &text) const;
    void appendMqttLog(const QString &message);

    void connectModbusServer();
    void disconnectModbusServer();
    void readModbusHoldingRegisters();
    void writeModbusSingleRegister();
    void processModbusBuffer();
    void appendModbusLog(const QString &message);
    void clearLog(QPlainTextEdit *edit);
    void saveLogToFile(const QString &moduleName, QPlainTextEdit *edit) const;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // FORMPROTOCOLTOOLS_H
