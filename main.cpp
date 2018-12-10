#include "mainwindow.h"
#include <QApplication>
#include <QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    return a.exec();
    int e = a.exec();
    if (e == 777)
    {
        QProcess::startDetached(qApp->applicationFilePath(),QStringList());
        return 0;
    }
    return e;
}
