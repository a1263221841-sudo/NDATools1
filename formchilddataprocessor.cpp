#include "formchilddataprocessor.h"
#include "ui_formchilddataprocessor.h"
#include <QApplication>

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QStringConverter>
#else
#include <QTextCodex>
#endif

#ifdef _MSC_VER
#pragma execution_charcter_set("utf-8")
#endif


//最大转换长度限制
static const int kmaxConvetLength = 50000;

//异步处理阈值
static const int kAsyncConvertThreshold = 15000;


FormChildDataProcessor::FormChildDataProcessor(QWidget *parent) :
    QWidget(parent),
    inputUpdateTimer(new QTimer(this)),  //创建防抖定时器,父对象为this
    ui(new Ui::FormChildDataProcessor)
{
    ui->setupUi(this);

    //初始化下拉框
    ui->comboBox_ConversionType->addItem("十进制->二进制","decimal_to_binary");
    ui->comboBox_ConversionType->addItem("二进制->十进制","binary_to_deimal");

    //配置输入更新防抖定时器
    inputUpdateTimer->setSingleShot(true);  //设置单次触发模式
    inputUpdateTimer->setInterval(300); //设置延迟时间为300ms

    //连接定时器信号
    connect(inputUpdateTimer,&QTimer::timeout,this ,[this](){
        QString text = ui->textEdit_EnterData->toPlainText(); //获取文本输入内容
        updateInputInfo();
    });
    //连接ui控件之间的槽函数
    connectSignals();

    //连接异步转换器的finished信号到完成槽函数
    connect(&conversionWatcher,&QFutureWatcher<DataConverter::ConversionResult>::finished,this,&FormChildDataProcessor::onConversionFinished);

    //提示用户输入输出位置
    ui->textEdit_EnterData->setPlaceholderText("请输入要转换的数据..");
    ui->textEdit_BringData->setPlaceholderText("转换结果将显示这里..");

    //日志
    QString timestamp= QDateTime::currentDateTime().toString(QStringLiteral("yyyy-mm-dd hh:mm:ss"));
    QString startMsg = QStringLiteral("[%1] 数据转换工具已启动").arg(timestamp);
    appendLog(startMsg);

}

FormChildDataProcessor::~FormChildDataProcessor()
{
    delete ui;
}

void FormChildDataProcessor::on_pushButton_CoversionOpration_clicked()
{
    //检查ui对象和关键控件是否存在,防止空指针
    if(!ui || !ui->textEdit_BringData ||!ui->textEdit_EnterData ||!ui->comboBox_ConversionType ){
        qWarning<<"FormChildDataConversion:: on_convertButton_clicked: UI or controls is null";
        return;}
    //防止用户重复点击按扭
    if(ConversionRunning){
        return;
    }

    QString input = ui->textEdit_EnterData->toPlainText();

    if(input.isEmpty()){
        showMessage(QStringLiteral("请输入要转换的数据"),true);
        return;
    }
    //步骤,输入验证,检查输入长度是否超过最大限制
    if(input.length()>kmaxConvetLength){
        showMessage(QStringLiteral("过长请分批处理"),true);
        return;
    }

    //获取转换类型
    QString conversionType =ui->comboBox_ConversionType->currentData().toString();
    QString conversionTypeName= ui->comboBox_ConversionType->currentText();

    QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-mm-dd hh:mm:ss"));
    QString logMsg = QStringLiteral("[%1] [数据转化] 开始转换: %2 ,输入长度 : %3 字符" ).arg(timestamp,conversionTypeName).arg(input.length());
    appendLog(logMsg);

    if (input.length() > kAsyncConvertThreshold) {
            // 异步转换模式：数据量 >= 15000 字符
            // 设置转换忙状态，禁用相关按钮
            setConversionBusy(true);

            // 记录异步转换日志
            appendLog(QStringLiteral("[%1] [数据转换] 使用异步转换（输入长度超过阈值 %2）")
                          .arg(timestamp)
                          .arg(kAsyncConvertThreshold));

            // 使用 QtConcurrent::run() 在后台线程执行转换
            // lambda 表达式捕获输入数据和转换类型，在后台线程中执行转换
            auto future = QtConcurrent::run([input, conversionType]() {
                DataConverter::ConversionResult result;
                // 根据转换类型调用相应的转换方法
                if (conversionType == "decimal_to_binary") {
                    result = DataConverter::decimalToBinary(input);
                } else if (conversionType == "binary_to_decimal") {
                    result = DataConverter::binaryToDecimal(input);
                }
                return result;
            });

        //设置要监视的Futrue,当转换完成的会触发finished 信号
            conversionWatcher.setFuture(future);
            return; //异步转换立即返回,不等待结果
    }

    //同步转换模式:数据量<15000字符
    //直接调用转换方法,立即获取结果
    DataConverter::ConversionResult result;
    if(conversionType =="decilmal_to_binary"){
        result=DataConverter::decimalToBinary(input);
    }else if(conversionType =="binary_to_decilmal"){
        result=DataConverter::binaryToDecimal(input);
    }

    //展示转换结果
    showResult(result,ui->textEdit_BringData);
}
void FormChildDataProcessor::on_pushButton_CoversionCopyResult_clicked()
{
    if (!ui || !ui->textEdit_BringData) {
           qWarning() << "FormChildDataProcessor::on_pushButton_CoversionCopyResult_clicked: UI or outputTextEdit is null";
           return;
       }

       // 步骤2：获取输出文本框的内容
       QString text = ui->textEdit_BringData();

       // 步骤3：检查内容是否为空
       if (text.isEmpty()) {
           showMessage(QStringLiteral("没有可复制的内容"), true);
           return;
       }

       // 步骤4：复制到剪贴板
       copyToClipboard(text);

}
void FormChildDataProcessor::on_pushButton_Change_clicked()
{
    if (!ui || !ui->textEdit_BringData|| !ui->textEdit_EnterData) {
           qWarning() << "FormChildDataProcessor::on_pushButton_Change_clicked: UI or controls is null";
           return;
       }

       // 步骤2：获取输入和输出文本框的内容
       QString inputText = ui->textEdit_BringData->toPlainText();
       QString outputText = ui->textEdit_EnterData->toPlainText();

       // 步骤3：交换输入和输出文本框的内容
       ui->textEdit_EnterData->setPlainText(outputText);
       ui->textEdit_BringData->setPlainText(inputText);

       // 步骤4：更新统计信息
       // 输入统计使用原输出内容，输出统计使用原输入内容
       updateInputInfo(outputText);
       updateOutputInfo(inputText);

       // 步骤5：记录交换操作日志
       // 性能优化：使用 QStringLiteral 和时间戳复用
       QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
       QString logMsg = QStringLiteral("[%1] [数据转换] 交换输入输出数据").arg(timestamp);
       appendLog(logMsg);

}
void FormChildDataProcessor::on_pushButton_DeclearData_clicked()
{
    if(!ui||!ui->textEdit_BringData||!ui->textEdit_EnterData){
        qWarning()<<"FormChildDataConversion::on_pushButton_DeclearData_clicked UI or controls is null";
        return;
    }
    //步骤2,清空输入和输出文本框
    ui->textEdit_BringData->clear();
    ui->textEdit_EnterData->clear();

    updateInputInfo("");
    updateOutputInfo("");

    //记录清空操作日志
    //性能优化使用QStringLiteral 和时间戳复用
    QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    QString logMsg = QStringLiteral("[%1] [数据转换] 交换输入输出数据").arg(timestamp);
    appendLog(logMsg);


}

//保存日志,(公共接口,提供主窗口调用)
void FormChildDataProcessor::saveLog()
{
  //检查日志列表是否为空
    if(logEntries.isEmpty())
    {
        return;
    }

    //获取地址
    QString appDataPath= QStandardPaths::writableLocation(QStandardPaths::AppDateLocation);
    QDir appDataDir(appDataPath);
    QString appName= QApplication::applicationName();
    if(!appName.isEmpty()&&!appDataPath.endsWith("/"+appName)||appDataPath.endsWith("\\"+appName)){
        QDir parentDir = appDataDir;
        if(parentDir.cdUp()){
            appDataDir =parentDir.absolutePath();
        }
    }

    //创建目录(如果不存在的话)
    if(!QDir().mkpath(appDataPath)){
        qWarning()<<"FormChildDataConversion::saveLog:无法创建目录"<<appDataPath;
        return;
    }

    //构建文件路径
    //性能优化,使用QStringLiteral,避免临时字符串对象分配
    QString fileName =QDir(appDataPath).filePath(QStringLiteral("DataConversionLogfile.txt") );

    //检查文件是否存在,决定文件打开模式
    bool fileExists = QFile::exists(fileName);
    QIODevice::OpenMode openMode = fileExists ?
                                           (QIODevice::Append | QIODevice::Text) :    // 追加模式（文件存在）
                                           (QIODevice::WriteOnly | QIODevice::Text);  // 创建模式（文件不存在）


    QFile file(fileName);
        if (file.open(openMode)) {
            QTextStream out(&file);

            // 设置文件编码为 UTF-8
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8"); // Qt5 设置编码
    #else
            out.setEncoding(QStringConverter::Utf8); // Qt6 设置编码
    #endif

            // 步骤7：如果是追加模式，先写入分隔符和时间戳
            if (fileExists) {
                // 性能优化：使用 QStringLiteral 避免临时字符串对象分配
                QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                out << "\n" << QStringLiteral("========== ") << timestamp << QStringLiteral(" ==========") << endl;
    #else
                out << "\n" << QStringLiteral("========== ") << timestamp << QStringLiteral(" ==========") << Qt::endl;
    #endif
            }

            // 步骤8：性能优化：批量写入所有日志条目
            // QQueue 可以像容器一样遍历，O(n) 时间复杂度
            for (const QString &entry : logEntries) {
                out << entry;
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                out << endl;
    #else
                out << Qt::endl;
    #endif
            }

            // 步骤9：关闭文件
            file.close();
            qDebug() << "数据转换工具日志已保存至:" << fileName;

            // 步骤10：清空已保存的日志条目
            logEntries.clear();

            // 步骤11：注意：窗口关闭时不显示消息框，避免阻塞
            if (this->isVisible()) {
                // 性能优化：使用 QStringLiteral 避免临时字符串对象分配
                QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("日志已保存至 %1").arg(fileName));
            }
        } else {
            // 文件打开失败，记录警告并显示错误消息框
            qWarning() << "数据转换工具日志保存失败:" << file.errorString();
            if (this->isVisible()) {
                // 性能优化：使用 QStringLiteral 避免临时字符串对象分配
                QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("保存失败: ") + file.errorString());
            }
        }


}
//异步转换完成槽函数
void FormChildDataProcessor::onConversionFinished()
{
 //企业级标准添加空指针检查
    if(!ui ||!ui->textEdit_BringData){
        qWarning()<<"FormChildDataProvessor ui is null or textEdit_BringData is null";
        //即使出错也要恢复UI状态,启动相关按扭
        setConversionBusy(false);
        return;
    }

    //设置转换忙状态为false
    setConversionBusy(false);

    //获取异步转换结果
   auto result = conversionWatcher.result();

   //异步转换完成,完成日志系统
   QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-mm-dd hh:mm:ss"));
   QString logMsg = QStringLiteral("[%1] [数据转换] 异步转换已完成").arg(timestamp);
   appendLog(logMsg);
   showResult(result,ui->textEdit_BringData);
}


//输入文本变化槽函数
void FormChildDataProcessor::opInputTextChanged(){
    // 步骤1：企业级标准：添加空指针检查
        if (!inputUpdateTimer) {
            qWarning() << "FormChildDataConversion::onInputTextChanged: inputUpdateTimer is null";
            return;
        }

        //性能优化,使用防抖
        inputUpdateTimer->stop();
        inputUpdateTimer->start();
}





//统一消息提示函数
void FormChildDataProcessor::showMessage(const QString &message,bool isError )
{
    if(isError){
        QMessageBox::warning(this,QStringLiteral("错误"),message);

    }else{
        qDebug()<<message;
    }

}

//设置转换忙状态
void FormChildDataProcessor::setConversionBusy(bool busy){
    ConversionRunning= busy;
    // 根据 busy 状态启用/禁用相关按钮
       // !busy 表示：如果 busy 为 true，则 !busy 为 false，按钮被禁用
       //            如果 busy 为 false，则 !busy 为 true，按钮被启用
    ui->pushButton_Change->setEnabled(!busy);
    ui->pushButton_CoversionCopyResult->setEnabled(!busy);
    ui->pushButton_CoversionOpration->setEnabled(!busy);
    ui->pushButton_DeclearData->setEnabled(!busy);

}

//展示转换结果或错误信息
void FormChildDataProcessor::showResult(const DataConverter::ConversionResult &result,QTextEdit &outputEdit){
    //添加空指针检查
    if(!outputEdit){
        qWarning()<<"FormChileConversin:showResult : outputEdit is null";
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-mm-dd hh::mm:ss "));

    if(result.success){
        //转换成功,显示结果,更新统计,显示成功提示,记录日志生成记录
        outputEdit->setPlainText(result.data);
        updateOutputInfo(result.data);
        showMessage(QStringLiteral("转换成功"));

        //记录转换成功日志

    }

}

//更新输入统计信息
void FormChildDataProcessor::updateInputInfo(const QString &text);

//更新输出统计信息
void FormChildDataProcessor::updateOutputInfo(const QString &text);

//复制文本到粘贴板
void FormChildDataProcessor::copyToClipboard(const QString &text);

//连接所有信号与槽
void FormChildDataProcessor::connectSignals();

//追加日志条目
void FormChildDataProcessor::appendLog(const QString &message);

//裁剪日志
void FormChildDataProcessor::trimLog();

//保存纯文本编辑框内容到文件
void FormChildDataProcessor::savePlainTextEditToFile(QPlainTextEdit *plaintTextEdit);

protected:
//关闭窗口事件
void closeEvent(QCloseEvent *event) override;


