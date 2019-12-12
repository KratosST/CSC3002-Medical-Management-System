#include "mainwindow.h"
#include <QApplication>
#include "doctor.h"
int flag1=0;
int flag2=0;
int setPathCount=0;
QString Path;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
