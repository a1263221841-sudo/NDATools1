#include "inputvalidator.h"


//作用:实现各类的输入效验逻辑,返回统一的ValidationResult
//范围:ip/端口/主机名/网络地址/文件路径/字符串非空与长度.数值范围,hex Base64

#include <QFileInfo>  //包含Qt文件信息类,用于检查文件路径是否存在及是否为常规文件
#include <QUrl>    //

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

//定义并初始化ipV4地址正则表达式的静态常量成员
//主要用于检查一个字符串的是否为有效的 ipv4地址（比如：192.168.1.1）
const QRegularExpression InputValidator::ipv4Regex(
    "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
    );

// 定义并初始化IPv6地址正则表达式的静态常量成员
// 比如：AA22:BB11:1122:CDEF:1234:AA99:7654:7410
const QRegularExpression InputValidator::ipv6Regex(
    "^(?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$|^::1$|^::$"
    );

// 定义并初始化主机名正则表达式的静态常量成员
// 用于检查一个字符串是否为有效的主机名称或域名（比如：www.baidu.com，good.com，www.good.co.uk）
const QRegularExpression InputValidator::hostnameRegex(
    "^(?:[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*)$"
    );

// 定义并初始化十六进制字符串正则表达式的静态常量成员
const QRegularExpression InputValidator::hexRegex(
    "^[0-9a-fA-F]+$"
    );

// 定义并初始化Base64字符串正则表达式的静态常量成员
const QRegularExpression InputValidator::base64Regex(
    "^[A-Za-z0-9+/]*={0,2}$"
    );


//InputValidator::InputValidator() {}


//ip地址校验函数,验证输入的字符串是否为有效的ipv4或ipv6地址
InputValidator::ValidationResult InputValidator::validatorIPAddress(const QString &ipAddress)
{
    //第一层检查,判断输入字符串是否为空
    if(ipAddress.isEmpty()){
        return ValidationResult(false,"IP地址不能为空");
    }

    //第二层检查,使用qt的QHostAddress类尝试解析ip地址
    QHostAddress address(ipAddress);
    if (address.isNull()){
        return ValidationResult(false,"ip地址格式无效");
    }

    //第三层检查 ,验证解析出来的地址协议类型是否为ipv4,ipv6
    if(address.protocol() != QAbstractSocket::IPv4Protocol &&address.protocol() !=QAbstractSocket::IPv6Protocol){
        return ValidationResult(false,"不支持的IP地址类型");
    }

    //所有检查通过,返回校验成功结果
    return ValidationResult(true);
}

//端口数值校验函数:验证整数端口是否在有效范围内
InputValidator::ValidationResult InputValidator::validatorPort(int port)
{
    //范围检查:端口号必须在{1,65535}范围内(TCP/UDP标准端口范围内)
    if (port < 1 || port >65535){
    return ValidationResult(false, "端口号必须在1-65535范围内");
    }
    return ValidationResult(true);
}

//端口字符串字串校验,将字符串转换为整数后调用数据校验
InputValidator::ValidationResult InputValidator::validatorPort(const QString &portStr)
{
    //第一层检查:判断字符串是否为空
    if(portStr.isEmpty()){
        return ValidationResult(false,"端口号不能为空");
    }

    //第二层检查 :尝试将字符串转换为整数
    bool ok;
    int port =portStr.toInt(&ok);

    //第三层检查:判断字符串
    if(!ok){
        return ValidationResult(false,"端口号必须为数字");
    }
    return ValidationResult(port);//所有检查通过,返回校验成功结果
}

//主机名校验函数:验证输入是否符合FRC标准的主机名格式
InputValidator::ValidationResult InputValidator::validatorHostname(const QString &hostname)
{
    //第一层检查:判断字符串是否为空
    if(hostname.isEmpty()){
        return ValidationResult(false,"主机名不能为空");
    }

    //第二层检查:验证主机名总长度是否符合RFC标准(最大253字符)
    if(hostname.length() >253){
        return ValidationResult(false,"主机名长度不能超过253");
    }

    //第三层检查,使用预编译的正则表达式验证主机的字符格式
    if(!hostnameRegex.match(hostname).hasMatch()){
        return ValidationResult(false,"主机名格式无效");
    }

    return ValidationResult(true);
}

//网络地址校验::尝试按ip地址校验,失败则按主机名校验
InputValidator::ValidationResult InputValidator::validatorNetworkAddress(const QString &address)
{
    //第一层检查:判断输入字符串是否为空
    if(address.isEmpty()){
        return ValidationResult(false,"网络地址不能为空");
    }

    //第二层:优先尝试按ip地址格式进行检验
    ValidationResult ipResult = validatorIPAddress(address);
    if(ipResult.isValid){
        return ipResult;
    }

    //第三层检查:ip校验失败后,尝试按主机名格式进行检验
    ValidationResult hostnameResult = validatorHostname(address);
    if(hostnameResult.isValid){
        return hostnameResult;
    }
    return ValidationResult(false,"无效的网络地址格式（既不是有效IP也不是有效的主机名）");
}

//文件路径校验函数:验证文件路径格式,可选检查文件是否存在
InputValidator::ValidationResult InputValidator::validatorFilePath(const QString &filePath, bool mustExist)
{
    //第一层检查:判断输入文件路径字符串是否为空
    if(filePath.isEmpty()){
        return ValidationResult(false,"文件路径不能为空");
    }

    //第二层检查:创建一个QFileInfo对象用于文件信息查询
    QFileInfo QFileInfo(filePath);

    //第三层检查:如果要求文件必须存在,则检查文件是否存在
    if(mustExist && !QFileInfo.exists()){
        return ValidationResult(false,"指定的文件不存在");
    }

    //第四层检查:如果要求的文件必须存在,则检查路径是否为常规文件(而非目录或特殊文件)
    if(mustExist && !QFileInfo.isFile()){
        return ValidationResult(false,"指定的路径不是文件");
    }
    return ValidationResult(true);
}

//非空字符串校验函数:验证字符串不为空且不仅包含空白字符串
InputValidator::ValidationResult InputValidator::validatorNonEmptyString(const QString &str,const QString &filedName )
{
    //第一层检查:判断输入字符串是否为空(长度为0)
    if(str.isEmpty()){
        return ValidationResult(false,QString("%1不能为空").arg(filedName));
    }
    //第二层检查:判断去掉收尾空白字符后是否为空(防止仅输入空格,制表符)
    if(str.trimmed().isEmpty()){
        return ValidationResult(false,QString("%1不能只包含空白字符").arg(filedName));
    }
    return ValidationResult(true);
}

//字符串长度区间校验函数,验证字符串函数是否在指定范围内(闭区间)
InputValidator::ValidationResult InputValidator::validatorStringLength(const QString &str,int minLength,int maxLength,const QString &fieldName)
{
    //获取输入字符串的长度(字符数,非字节数)
    int length=str.length();

    //第一层检查:判断字符串长度是否小于最小值
    if(length<minLength){
        return ValidationResult(false,QString("%1长度不能小于%2个字符").arg(fieldName).arg(minLength));
    }

    //检查字符串的长度是否大于最大值
    if(length>maxLength){
        return ValidationResult(false,QString("%1长度不能小于%2个字符").arg(fieldName).arg(maxLength));
    }

    return ValidationResult(true);
}

//数值范围校验函数:验证整数是否在指定范围内(闭区间)
InputValidator::ValidationResult InputValidator::validatorNumberRange(int value,int minValue,int maxValue,const QString &fieldName)
{
    //第一层检查:判断数值是否小于最小值
    if(value<minValue){
        return ValidationResult(false,QString("%1第不能小于%2").arg(fieldName).arg(minValue));
    }

    //第二层检查:判断数值是否大于最大值
    if(value<maxValue){
        return ValidationResult(false, QString("%1不能大于%2").arg(fieldName).arg(maxValue));
    }
    return ValidationResult(true);
}

//十六进制字符串校验函数:验证字符串是否为有效的十六进制格式  //hex 十六进制
InputValidator::ValidationResult InputValidator::validatorHexString(const QString &hexStr)
{
    // 第一层检查：判断输入字符串是否为空
    if(hexStr.isEmpty()){
        return ValidationResult(false,"十六进制字符串不能为空");
    }

    // 第二层检查：直接使用预编译的正则表达式验证字符串是否只包含有效的十六进制字符
    if (!hexRegex.match(hexStr).hasMatch()) {
        return ValidationResult(false,"十六进制字符串只能包含0-9 a-f");
    }

    // 第三层检查：验证字符串长度是滞为偶数（每个字节需要2个十六进制字符表示，确保字符对齐）
    if (hexStr.length() % 2 != 0) {
        return ValidationResult(false,"十六进制字符串长度必须是偶数");
    }

    return ValidationResult(true);
}



//Base64字符串校验函数:验证字符串是否为Base64编码格式
InputValidator::ValidationResult InputValidator::validatorBase64String(const QString &base64Str)
{
    // 第一层检查：判断输入字符串是否为空
    if(base64Str.isEmpty()){
        return ValidationResult(false,"Base64字符串不能为空");
    }

    // 第二层检查：使用预编译正则表达式验证字符串是否只包含有效的Base64字符
    if (!base64Regex.match(base64Str).hasMatch()) {
        return ValidationResult(false,"Base64字符串格式无效");
    }

    // 第三层检查：验证字符串长度是否为4的倍数（Base64编码规则：每3个字节原始数据编码为4个Base64字符）
    if (base64Str.length() % 4 != 0) {
        return ValidationResult(false,"Base64字符串长度必须是4的倍数");
    }

    return ValidationResult(true);

}