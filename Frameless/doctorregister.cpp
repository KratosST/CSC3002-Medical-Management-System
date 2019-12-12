#include "doctorregister.h"
#include "ui_doctorregister.h"
#include <QComboBox>
#include <QLayout>
#include <QDebug>
#include <QMessageBox>
#include <info.h>
#include "docfilesend.h"

doctorRegister::doctorRegister(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::doctorRegister)
{
    setWindowFlags(Qt::FramelessWindowHint);

    ui->setupUi(this);

    QImage _image;
    _image.load(":/image/registerDoctor2.jpg");
    setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation)));
    setPalette(pal);

    mLocation1 = this->geometry();
    mIsMax1 = false;
    mDrag1 = false;
//    QIcon icon;
//    icon.addFile(tr(":/image/button.png"));
//    ui->pushButton->setIcon(icon);

    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnExit1->setIcon(QIcon(":/image/exit.png"));
    QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::white);
    ui->label->setPalette(pe);
    ui->label_2->setPalette(pe);
    ui->label_3->setPalette(pe);
    ui->label_4->setPalette(pe);
    ui->label_5->setPalette(pe);
    ui->label_6->setPalette(pe);
    ui->label_7->setPalette(pe);
    ui->label_8->setPalette(pe);
    ui->label_9->setPalette(pe);
    port = 6001;
    flag=0;
    //定时器
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(recall(void)));
    timer->start(2000);
    //udp初始化
    ServerIP = "10.30.28.137";
    udpSocket = new QUdpSocket;
    udpSocket->bind(port, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    ui->label_9->setText("正在等待服务器分配ID");
}

doctorRegister::~doctorRegister()
{
    delete ui;
    delete  udpSocket;
    delete timer;

}
bool doctorRegister::eventFilter1(QObject *obj, QEvent *e)
{
    if (obj == ui->widgetTitle)
    {
        if(e->type() == QEvent::MouseButtonDblClick)
        {
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}

void doctorRegister::on_btnExit1_clicked()
{
    this->close();
}


void doctorRegister::on_btnMin_clicked()
{
    showMinimized();
}

void doctorRegister::on_pushButton_clicked()
{
    QString ID,name,gender,password,zhicheng, keshi,zhugongfangxiang,telephone,jianjie;
    ID = PatientID;

    name =  ui->lineEdit->text();
    gender = ui->comboBox_2->currentText();
    password = ui->lineEdit_3->text();
    zhicheng = ui->lineEdit_4->text();
    telephone = ui->lineEdit_7->text();
    zhugongfangxiang = ui->lineEdit_9->text();
    jianjie =  ui->textEdit->toPlainText();
    keshi = ui->comboBox->currentText();



    for (int i = 0; i < password.size(); i++) {
        if (password[i] == ' ' || password[i] == '\r' || password[i] == '\n') {
            QMessageBox::warning(this, tr("错误"), tr("密码格式错误!")); return;
        }
    }

    for (int i = 0; i < name.size(); i++) {
        if (name[i] == ' ' || name[i] == '\r' || name[i] == '\n') {
            QMessageBox::warning(this, tr("错误"), tr("名字格式错误!")); return;
        }
    }

    Doctor doc;
    QStringList l = ID.split("_");
    doc.putid(l[1].toInt());
    doc.putname(name);
    doc.putage(telephone.toInt());
    doc.putgender(gender);
    doc.putpassword(password);
    doc.putprofessionaltitle(zhicheng);
    doc.putstrength(zhugongfangxiang);
    doc.putdescription(jianjie);
    doc.putdepartment(keshi);
    Storage stor;
    stor.storetotxt(&doc);

    Client * docfilesend = new Client;
    docfilesend->Path="C:/Doctor";
    docfilesend->docIP=ServerIP;
    docfilesend->docName= PatientID;
    docfilesend->setPort=7783;
    docfilesend->openFile();
    this->close();
}
void doctorRegister::recall()
{
    static int times = 0;
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);//apply the date of IOStream;
    QString iplocal =getIP();
    out<<(qint16)20<<iplocal;
    times++;
    qDebug()<<tr("进入中断");
    if(0==flag)
    {
        qDebug() << "flag: " <<flag;
        udpSocket->writeDatagram(data,data.length(),(QHostAddress)ServerIP,port);//UDP broadcast
    }
    else
    {
        qDebug() << "flag: " <<flag;
         timer->stop();
         times=0;
    }

    if(3==times)
    {
        timer->stop();
        times=0;
        if(QMessageBox::Yes==QMessageBox::warning(this,tr("提示"),tr("请先检查服务器是否连接，或者本地网络接入,是否重新连接？"),
                             QMessageBox::Yes,QMessageBox::No))
        {
           timer->start(2000);
        }
        else
        {
            close();
        }
   }
}

void doctorRegister::processPendingDatagrams()
{
    qDebug()<<"收到UDP返回的广播消息";
    while(udpSocket->hasPendingDatagrams())
    {
        qDebug() << "is ok";
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        in.setVersion(QDataStream::Qt_4_6);
        qint16 msgType;
        in>>msgType;//取出信息类型
        qDebug() <<"Type: "<<msgType;
        switch(msgType)
        {
        case 3://该类型为服务器的返回消息类型
        {
            in>>PatientID;//获得服务器分配的用户名
            qDebug()<< "name is " <<PatientID;
            ui->label_9->setText(PatientID);
//            in>>ServerIP;//获得服务器地址
//            qDebug()<<tr("服务器IP地址：")<<ServerIP;
            flag=1;     //标志位置1，通知重播窗口连接上服务器,关闭定时器
            timer->stop();
            //获得所有在线用户信息
            int   num =0;
            in>>num;
        }
        }
    }
}
QString doctorRegister::getIP()
{
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    for(int i = 0; i < addrs.size(); i++)
    {
        if (addrs.at(i).protocol() == QAbstractSocket::IPv4Protocol &&
                addrs.at(i) != QHostAddress::Null &&
                addrs.at(i) != QHostAddress::LocalHost &&
                !addrs.at(i).toString().contains(QRegExp("^169.*$")))
        {
             qDebug() << addrs.at(i).toString();
            return addrs.at(i).toString();
        }
    }
}
