#include "formdataprocessor.h"
#include "ui_formdataprocessor.h"

FormdataProcessor::FormdataProcessor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormdataProcessor)
{
    ui->setupUi(this);
}

FormdataProcessor::~FormdataProcessor()
{
    delete ui;
}
