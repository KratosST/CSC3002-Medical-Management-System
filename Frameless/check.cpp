#include "check.h"
#include "ui_check.h"
#include "reserve.h"
#include "patientChat.h"
#include<QMessageBox>
#include<QDebug>
#include<QFileDialog>
#include<QTime>
#include <QDir>
#include <docfilesend.h>
check::check(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::check)
{
    ui->setupUi(this);
    ServerIP= "10.30.28.137";
    port=5555;
    QString day, time,total;
        switch (c)
        {
        case 0: day = "Mon";break;
        case 1: day = "Tue";break;
        case 2: day = "Wed";break;
        case 3: day = "Thr";break;
        case 4: day = "Fri";break;
        }
        switch(r)
        {
        case 0: time = "9";break;
        case 1: time = "10";break;
        case 2: time = "11";break;
        case 3: time = "2";break;
        case 4: time = "3";break;
        case 5: time = "4";break;
        }
     total = day + time;
     ui->label_2->setText(total);
     //udp初始化
     udpSocket =new QUdpSocket;
     udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);



}

check::~check()
{
    delete ui;
}

void check::on_pushButton_clicked()
{
    QString day, time;
        switch (c)
        {
        case 0: day = "Mon";break;
        case 1: day = "Tue";break;
        case 2: day = "Wed";break;
        case 3: day = "Thr";break;
        case 4: day = "Fri";break;
        }
        switch(r)
        {
        case 0: time = "9";break;
        case 1: time = "10";break;
        case 2: time = "11";break;
        case 3: time = "2";break;
        case 4: time = "3";break;
        case 5: time = "4";break;
        }
    total = day + time;
    this->close();
    updateMsg();

}

void check::on_pushButton_2_clicked()
{

    this->close();
}

void check::updateMsg()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out<<(qint16)30<<patientid<<doctorid<<total;
    udpSocket->writeDatagram(data, data.length(), (QHostAddress)ServerIP, port);
}
