#ifndef CHECK_H
#define CHECK_H

#include <QDialog>
#include "reserve.h"
#include <QNetworkInterface>
#include<QUdpSocket>
#include <QWidget>

namespace Ui {
class check;
}

class check : public QDialog
{
    Q_OBJECT

public:
    explicit check(QWidget *parent = nullptr);
    ~check();
    QString ServerIP;
    QString doctorid;
    QString patientid;
    QString total;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void updateMsg();

private:
    Ui::check *ui;
    QUdpSocket *udpSocket;
    qint16 port;
};

#endif // CHECK_H
