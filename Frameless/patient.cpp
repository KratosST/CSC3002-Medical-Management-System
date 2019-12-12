#include "Patient.h"
#include "ui_Patient.h"
#include<QMessageBox>
#include"PatientChat.h"
#include<QDebug>
#include<QFileDialog>
#include<QTime>
#include"reserve.h"
#include <info.h>



Patient1::Patient1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatientUI)
{
    ui->setupUi(this);
//初始化变量和窗口状态

    setWindowFlags(Qt::FramelessWindowHint);
    flag=0;
    fileNameSize=0;
    bytesReceived=0;
    port=5555;//udp端口
    ServerIP = "10.30.28.137";
//    setWindowTitle(tr("病人端"));
    //setWindowFlags(Qt::FramelessWindowHint);
//    setWindowFlags(Qt::WindowMinimizeButtonHint);
    //setWindowFlags(Qt::WindowCloseButtonHint);

    QImage _image;
    _image.load(":/image/patient.jpg");
    setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation)));


    setPalette(pal);


    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnExit1->setIcon(QIcon(":/image/exit.png"));

//重播定时器
    timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(recall(void)));
    timer->start(2000);// 一秒钟后重连接服务器
//客户端udp初始化
    udpSocket =new QUdpSocket;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    ui->textBrowser->setText(tr("正在连接服务器……"));
    ui->allUsrTable->setColumnHidden(4,true);
    ui->allUsrTable->setColumnHidden(5,true);
//表格的设置
    tClnt =new QTcpSocket(this);
//文件传输服务器配置

    ui->allUsrTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

}


//!析构函数，清除内存
Patient1::~Patient1()
{
    delete ui;

    delete udpSocket;
    delete  tClnt;//建立一个TcpSocket
    delete timer;//重播用的定时器
    delete fileTcpServer;//用于传送文件的服务器
    delete fileSocket;
    delete locFile;
}


//.连接服务器重播函数
//每次重播时间为3秒
void Patient1::recall()
{
     static int times = 0;
     QByteArray data;
     QDataStream out(&data,QIODevice::WriteOnly);//apply the date of IOStream;
     QString iplocal =getIP();
     out<<(qint16)20<<iplocal<<usrName;//写入自己得IP地址
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

     if(3==times)//一次只会连续重3秒
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



void Patient1::processPendingDatagrams()
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
            in>>usrName;//获得服务器分配的用户名
            qDebug()<< "name is " <<usrName;
            ui->textBrowser->setText(usrName);
            in>>serverIP;//获得服务器地址
            qDebug()<<tr("服务器IP地址：")<<serverIP;
            flag=1;     //标志位置1，通知重播窗口连接上服务器,关闭定时器
            timer->stop();
            //获得所有在线用户信息
            int   num =0;
            in>>num;

            Storage stor;
            std::string addre = std::string("C:\\Patient\\")+usrName.toStdString()+".txt";
            Patient pat;
            stor.loadfromtxtPatient(addre, pat);
            ui->textBrowser_2->setText(pat.getrecommendation()[1]);


            //建立Tcp连接
            tClnt->connectToHost(ServerIP,6666);

            if(tClnt->waitForConnected(2000))
            {
                qDebug()<<"客户端以Tcp方式连接上服务器";
               //connect(tClnt,SIGNAL(readyRead()),this,SLOT(comeMsg()));
            }
            else
            {
                qDebug()<<"无法连接连接上服务器！";
            }
            break;
        }

        case 5://提交时注意加上来自自己消息的屏蔽
        {
            QString msg,name;
            in>>msg;
            in>>name;
            break;
        }

        case 60://更新在线用户
        {
            qint16 size;
            in>>size;
            ui->allUsrTable->setRowCount(0);

            for(int i=0; i<size; i++)
            {
                QString usr,ip;
                QString DocName, Gender, depart,major;
                in>>DocName>>Gender>>depart>>major;
                qDebug()<<DocName<<Gender<<depart;
                in>>usr;
                in>>ip;
                qDebug()<<usr<<ip;
                ui->allUsrTable->insertRow(0);//将在线用户信息显示在表格
                ui->allUsrTable->setItem(0,0,new QTableWidgetItem(DocName));
                ui->allUsrTable->setItem(0,1,new QTableWidgetItem(Gender));
                ui->allUsrTable->setItem(0,2,new QTableWidgetItem(depart));
                ui->allUsrTable->setItem(0,3,new QTableWidgetItem(major));
                ui->allUsrTable->setItem(0,4,new QTableWidgetItem(usr));
                ui->allUsrTable->setItem(0,5,new QTableWidgetItem(ip));
                qDebug()<<ui->allUsrTable->item(0,4)->text();
                qDebug()<<ui->allUsrTable->item(0,5)->text();
            }

        }
        default:{break;}
        }
    }
}


//.获得本机IP地址
QString  Patient1:: getIP()
{
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    for(int i = 0; i < addrs.size(); i++)
    {
        if (addrs.at(i).protocol() == QAbstractSocket::IPv4Protocol &&
                addrs.at(i) != QHostAddress::Null &&
                addrs.at(i) != QHostAddress::LocalHost &&
                !addrs.at(i).toString().contains(QRegExp("^169.*$")))//正则表达式，不包含169开头的默认无网络连接IP
        {
             qDebug() << addrs.at(i).toString();
            return addrs.at(i).toString();
        }
    }
}

//！双击表格后弹出聊天窗口
void Patient1::on_allUsrTable_cellDoubleClicked(int row, int column)
{
    QString usr;
    QString IP;
    usr = ui->allUsrTable->item(row,3)->text();
    IP = ui->allUsrTable->item(row,4)->text();
    usr= "Doctor_"+usr;
//新对话框需要的参数信息
    reserve reserve;
    reserve.doctor_ip=ServerIP;
    reserve.doctor_name=usr;
    reserve.patient_id=usrName;
    reserve.setModal(true);
    reserve.exec();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);//apply the date of IOStream;
    out<<(qint16)8<<usrName<<usr;//写入自己得IP地址
    qDebug()<<usrName+"选择了"+usr;
    udpSocket->writeDatagram(data,data.length(),(QHostAddress) ServerIP,port);//UDP broadcast
}


//！退出按钮槽函数

void Patient1::on_btnExit1_clicked()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);//apply the date of IOStream;
    out<<(qint16)4<<usrName;//写入自己得IP地址
    qDebug()<<tr("用户退出");
    udpSocket->writeDatagram(data,data.length(),(QHostAddress) ServerIP,port);//UDP broadcast
    tClnt->abort();
    tClnt->disconnect();
    tClnt->close();//关闭当前Tcp连接
    fileTcpServer->close();
    this->close();
}


void Patient1::on_btnMin_clicked()
{
    showMinimized();
}
