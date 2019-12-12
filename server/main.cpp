#include "widget.h"
#include <QApplication>
#include <QString>
int flag1=0;
int flag2=0;
int flag2Doc=0;
int Registerflag2=0;
int Registerflag2Doc=0;
int setPathCount=0;
int setPathCountDoc=0;
int RegistersetPathCount=0;
int RegistersetPathCountDoc=0;
QString Path;
QString PathDoc;
QString RegisterPath;
QString RegisterPathDoc;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    setPathCount=0;
    setPathCountDoc=0;
    RegistersetPathCount=0;
    RegistersetPathCountDoc=0;
    return a.exec();
}
