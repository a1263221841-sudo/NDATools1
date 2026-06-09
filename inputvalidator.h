#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H


//作用:集中式输入效验工具,提供IP/端口,主机名,文件路径字符串数值编码等效验窗口

#include <QString>   //Qt字符类,用于处理
#include <QRegularExpression>   // Qt正则表达式类，用于模式匹配和校验
#include <QHostAddress>   //Qt主机地址类，用于解析和验证IP地址
#include <QAbstractSocket>  //qt抽象套接字类,提供协议枚举
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif


class InputValidator  //定义类  作为静态工具类,提供实时校验功能
{
public:
    //通过校验结果 :isValid表示成功,errorMessage给出失败原因
    struct ValidationResult{
        bool isValid;
        QString errorMessage;
        ValidationResult(bool valid =true ,const QString &message = QString())
            : isValid(valid),
            errorMessage(message){}
    };

    //ip地址校验函数,验证输入的字符串是否为有效的ipv4或ipv6地址
    static ValidationResult validatorIPAddress(const QString &ipAdress);

    //端口数值校验函数:验证整数端口是否在有效范围内
    static ValidationResult validatorPort(int port);

    //端口字符串字串校验,将字符串转换为整数后调用数据校验
    static ValidationResult validatorPort(const QString &portStr);

    //主机名校验函数:验证输入是否符合FRC标准的主机名格式
    static ValidationResult validatorHostname(const QString &hostname);

    //网络地址校验::尝试按ip地址校验,失败则按主机名校验
    static ValidationResult validatorNetworkAddress(const QString &address);

    //文件路径校验函数:验证文件路径格式,可选检查文件是否存在
    static ValidationResult validatorFilePath(const QString &filePath, bool mustExist = false);

    //非空字符串校验函数:验证字符串不为空且不仅包含空白字符串
    static ValidationResult validatorEmptyString(const QString &str,const QString &fileName = "字段" );

    //字符串长度区间校验函数,验证字符串函数是否在指定范围内(闭区间)
    static ValidationResult validatorStringLength(const QString &str,int minLength,int maxLength,const QString &fieldName = "字段");

    //数值范围校验函数:验证整数是否在指定范围内(闭区间)
    static ValidationResult validatorNumberRange(int value,int minValue,int maxValue,const QString &fieldName="数值");

    //十六进制字符串校验函数:验证字符串是否为有效的十六进制格式  //hex 十六进制
    static ValidationResult validatorHexString(const QString &hexStr);

    //Base64字符串校验函数:验证字符串是否为Base64编码格式
    static ValidationResult validatorBase64String(const QString &base64Str);


private:
    InputValidator()=default;


    //预编译的正则规则:避免重复构造带来的性能消耗
    static const QRegularExpression ipv4Regex;
    static const QRegularExpression ipv6Regex;
    static const QRegularExpression hostnameRegex;
    static const QRegularExpression hexRegex;
    static const QRegularExpression base64Regex;
};

#endif // INPUTVALIDATOR_H
