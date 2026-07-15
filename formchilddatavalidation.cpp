#include "formchilddatavalidation.h"
#include "ui_formchilddatavalidation.h"

FormchildDataValidation::FormchildDataValidation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormchildDataValidation)
{
    ui->setupUi(this);
}

FormchildDataValidation::~FormchildDataValidation()
{
    delete ui;
}
