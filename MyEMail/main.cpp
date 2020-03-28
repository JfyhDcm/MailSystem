
#include "mainwindow.h"
#include "loginin.h"
#include <QApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    if(QT_VERSION>=QT_VERSION_CHECK(5,6,0))
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    LoginIn login;
    login.show();
    return a.exec();
}
