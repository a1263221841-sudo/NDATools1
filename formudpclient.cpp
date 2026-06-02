#include "formudpclient.h"
#include "ui_formudpclient.h"

FormUDPClient::FormUDPClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormUDPClient)
{
    ui->setupUi(this);
}

FormUDPClient::~FormUDPClient()
{
    delete ui;
}
