#ifndef FORMDATAPROCESSOR_H
#define FORMDATAPROCESSOR_H

#include <QWidget>
#include "formchilddataprocessor.h"
#include "formchilddatavalidation.h"
namespace Ui {
class FormdataProcessor;
}

class FormdataProcessor : public QWidget
{
    Q_OBJECT

public:
    explicit FormdataProcessor(QWidget *parent = nullptr);
    ~FormdataProcessor();
    // 保存日志（公共接口，提供主窗口调用）
        void saveLog();


private:
    Ui::FormdataProcessor *ui;
};

#endif // FORMDATAPROCESSOR_H
