#include "mainwidget.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("NDA"));
    QCoreApplication::setApplicationName(QStringLiteral("NDATools"));
    MainWidget w;
    w.show();
    return QApplication::exec();
}
