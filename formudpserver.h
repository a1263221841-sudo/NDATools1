#ifndef FORMUDPSERVER_H
#define FORMUDPSERVER_H

#include <QWidget>

namespace Ui {
class FormUDPServer;
}

class FormUDPServer : public QWidget
{
    Q_OBJECT

public:
    explicit FormUDPServer(QWidget *parent = nullptr);
    ~FormUDPServer();

private:
    Ui::FormUDPServer *ui;
};

#endif // FORMUDPSERVER_H
