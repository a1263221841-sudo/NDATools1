#include "formudpserver.h"
#include "ui_formudpserver.h"

FormUDPServer::FormUDPServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormUDPServer)
{
    ui->setupUi(this);
}

FormUDPServer::~FormUDPServer()
{
    delete ui;
}
