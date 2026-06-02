#ifndef FORMUDPCLIENT_H
#define FORMUDPCLIENT_H

#include <QWidget>

namespace Ui {
class FormUDPClient;
}

class FormUDPClient : public QWidget
{
    Q_OBJECT

public:
    explicit FormUDPClient(QWidget *parent = nullptr);
    ~FormUDPClient();

private:
    Ui::FormUDPClient *ui;
};

#endif // FORMUDPCLIENT_H
