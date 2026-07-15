#ifndef FORMCHILDDATAVALIDATION_H
#define FORMCHILDDATAVALIDATION_H

#include <QWidget>

namespace Ui {
class FormchildDataValidation;
}

class FormchildDataValidation : public QWidget
{
    Q_OBJECT

public:
    explicit FormchildDataValidation(QWidget *parent = nullptr);
    ~FormchildDataValidation();

private:
    Ui::FormchildDataValidation *ui;
};

#endif // FORMCHILDDATAVALIDATION_H
