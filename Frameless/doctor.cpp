#include "doctor.h"
#include "ui_doctor.h"
#include<QMessageBox>
#include<QDebug>
#include<QFileDialog>
#include<QTime>
#include <QDir>
#include <docfilesend.h>
#include<info.h>
#include "chat.h"
#include<patientinformation.h>
extern int flag2;
extern QString Path;
extern int setPathCount;
int deleteRow = 0;
QString address1 = "";

Doctor1::Doctor1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoctorUI)
{
    ui->setupUi(this);
////////////////////////////////////////////////////////////////////初始化变量和窗口状态
    flag=0;
    fileNameSize=0;
    bytesReceived=0;
    port=5556;//udp端口
    setWindowFlags(Qt::FramelessWindowHint);
    QImage _image;
    _image.load(":/image/doctor.png");
    setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation)));
    setPalette(pal);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnExit1->setIcon(QIcon(":/image/exit.png"));


///////////////////////////////////////////////////////////////////重播定时器
    timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(recall(void)));
    timer->start(2000);// 一秒钟后重连接服务器
//.////////////////////////////////////////////////////////////////客户端udp初始化
    udpSocket =new QUdpSocket;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    ui->usrNameLine->setText(tr("正在连接服务器……"));
    ui->allUsrTable->setColumnHidden(1,true);
    ui->allUsrTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

//    Path = "C:/Patient";
//    QDir *dirpat=new QDir(Path);
//    QStringList filterPat;
//    filterPat<<"*.txt";
//    dirpat->setNameFilters(filterPat);
//    QList<QFileInfo> *fileInfoPat=new QList<QFileInfo>(dirpat->entryInfoList(filterPat));
//    int count_patient = (int)fileInfoPat->count();
//    qDebug() << count_patient;
//    for(int i=0; i<count_patient; i++){
//        qDebug() << fileInfoPat->at(i).fileName();
//        qDebug() << fileInfoPat->at(i).filePath();

//        }
//

//./////////////////////////////////////////////////////////////////表格的设置
    tClnt =new QTcpSocket(this);
////////////////////////////////////////////////////////////////////文件传输服务器配置
/// setPathCount=0;
    fileTcpServer=new QTcpServer;
    ServerIP="10.30.28.137";
    connect(&tcpServer, SIGNAL(newConnection()),this, SLOT(acceptConnection()));//关联tcp server新链接的信号与服务器接收请求的槽
    QDir mydir;
    if(setPathCount==0)//控制路径问题
    {
        Path="C:/Doctor";
        if (!mydir.exists(Path))
        {
            mydir.mkpath(Path);
        }
    }
    QDir mydir2;
    Path2="C:/Doctor/Patient";
    if (!mydir2.exists(Path2))
    {
        mydir2.mkpath(Path2);
    }
    qDebug()<<setPathCount;
    qDebug()<<Path;
    start();
}

//!析构函数，清除内存
Doctor1::~Doctor1()
{
    delete ui;
    delete udpSocket;
    delete  tClnt;//建立一个TcpSocket
    delete timer;//重播用的定时器
    delete localFile;
}
void Doctor1::start(){
    if (!tcpServer.listen(QHostAddress::Any, 7777))//监听所有ip地址的7777端口的主机
    {
        qDebug()<< tcpServer.errorString();//报错
        close();//tcp关闭
        return;
    }
    totalBytes = 0;//初始化文件总大小为0
    bytesReceived = 0;//初始化已接收字节为0
    fileNameSize = 0;//初始化文件名大小为0

}

void Doctor1::acceptConnection()//接受请求槽
{
    tcpServerConnection = tcpServer.nextPendingConnection();//接收到请求并获取套接字//Returns the next pending connection as a connected QTcpSocket object.
    connect(tcpServerConnection, SIGNAL(readyRead()),this, SLOT(updateServerProcess()));//关联准备tcp接收的信号与更新进度的槽
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));//关联tcp 错误信息与显示错误的槽
    tcpServer.close();//关掉tcp,但是这里要求一直接受文件
}

void Doctor1::updateServerProcess()//文件的接受工作
{
    qDebug() <<"收到请求";
    QDataStream in(tcpServerConnection);//The QDataStream class implements the serialization of C++'s basic data types, like char, short, int, char *, etc. Serialization of more complex data is accomplished by breaking up the data into primitive units.
    in.setVersion(QDataStream::Qt_5_8);//设置数据流版本
    if (bytesReceived <= sizeof(qint64)*2)//根据客户端的定义，将文件名大小信息空间与总大小信息空间个定义为qint64
    {//如果接收到的数据小于预留值
        if((tcpServerConnection->bytesAvailable() >= sizeof(qint64)*2)&& (fileNameSize == 0)) //如果可以获得的数据大于预留值并且文件名大小为0.也就是说文件大小信息与文件名大小信息都已经完整的传过来了//Returns the number of bytes that are available for reading. This function is commonly used with sequential devices to determine the number of bytes to allocate in a buffer before reading.
        {
            in >> totalBytes >> fileNameSize;//保存文件大小信息与文件名大小信息
            bytesReceived += sizeof(qint64) * 2;//记录已经接收到的数据大小
        }
        if((tcpServerConnection->bytesAvailable() >= fileNameSize)&& (fileNameSize != 0))//如果文件名还没接收完
        {
            in >> fileName;//生成文件名
            if(fileName.at(0)=="D"){
            bytesReceived += fileNameSize;//更新接收数据总大小
            localFile = new QFile(Path +"/"+ fileName);//根据指定path建立文件
            }
            else {
                bytesReceived += fileNameSize;//更新接收数据总大小
                localFile = new QFile(Path2 +"/"+ fileName);//根据指定path建立文件
            }
            if (!localFile->open(QFile::WriteOnly))//解决无法写入错误
            {
                QMessageBox::warning(this,tr("Error!"),tr("Server: File opening failed!"),QMessageBox::Yes);
                qDebug()<<Path;
                return;
            }
        }
        else//如果没有足够多的数据就返回
        {
            return;
        }
    }
    if (bytesReceived < totalBytes)//如果接收数据大小小于总大小，则把数据写入文件
    {
        bytesReceived += tcpServerConnection->bytesAvailable();//增加读入数据的值
        inBlock = tcpServerConnection->readAll();//把所有的数据全部写入缓存
        localFile->write(inBlock);//将缓存写入硬盘
        inBlock.resize(0);//清空缓存
    }

    if (bytesReceived == totalBytes)//如果接收完成
    {
        tcpServerConnection->close();//关闭链接
        localFile->close();//完成文件写入
        qDebug()<<"接收完成";
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out << (qint16)11;
        udpSocket->writeDatagram(data,data.length(),(QHostAddress)ServerIP,port);
        start();
    }

}
void Doctor1::displayError(QAbstractSocket::SocketError socketError)//错误槽
{
    qDebug()<<tcpServerConnection->errorString();
    tcpServerConnection->close();
    start();//重新开始
}

//.连接服务器重播函数
//每次重播时间为3秒
void Doctor1::recall()
{
     static int times = 0;
     QByteArray data;
     QDataStream out(&data,QIODevice::WriteOnly);//apply the date of IOStream;
     QString iplocal =getIP();
     out<<(qint16)2<<iplocal<<usrName;//写入自己得IP地址
     times++;
     qDebug()<<tr("进入中断");
     if(0==flag)
     {
         udpSocket->writeDatagram(data,data.length(),(QHostAddress)ServerIP,port);//UDP broadcast
     }
     else
     {
          timer->stop();
          times=0;
     }

     if(3==times)//一次只会连续重播3秒
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

//处理udp收到的广播消息
//1.对服务器返回的信息进行处理
//2.接受服务器各种广播消息
void Doctor1::processPendingDatagrams()
{
    qDebug()<<"收到UDP返回的广播消息";
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        in.setVersion(QDataStream::Qt_4_7);
        qint16 msgType;
        in>>msgType;//取出信息类型
        qDebug() <<msgType;
        switch(msgType)
        {
        case (qint16)3://该类型为服务器的返回消息类型
        {
            in>>serverIP;//获得服务器分配的用户名
            ui->usrNameLine->setText(usrName);
            in>>serverIP;//获得服务器地址
            qDebug()<<tr("服务器IP地址：")<<serverIP;
            flag=1;     //标志位置1，通知重播窗口连接上服务器,关闭定时器
            timer->stop();
            //.//////////////////////////////////////////////////////////////////////////////获得所有在线用户信息
            int   num =0;
            in>>num;

            //./////////////////////////////////////////////////////////////////////////////建立Tcp连接
            tClnt->connectToHost(ServerIP,6667);

            if(tClnt->waitForConnected(2000))
            {
                qDebug()<<"客户端以Tcp方式连接上服务器";
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

        case 6://更新在线用户
        {
            qint16 size;
            in>>size;
             ui->allUsrTable->setRowCount(0);
            for(int i=0; i<size; i++)
            {
                QString usr,ip;
                in>>usr;
                in>>ip;
                ui->allUsrTable->insertRow(0);//将在线用户信息显示在表格
                ui->allUsrTable->setItem(0,0,new QTableWidgetItem(usr));
                ui->allUsrTable->setItem(0,1,new QTableWidgetItem(ip));
            }
        }
        case 9:
        {
            QByteArray data;
            qDebug()<<"ok";
            QDataStream out(&data,QIODevice::WriteOnly);
            out << (qint16)9;
            udpSocket->writeDatagram(data,data.length(),(QHostAddress)ServerIP,port);
        }
        }
    }
}

//.获得本机IP地址
QString  Doctor1:: getIP()
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

//！退出按钮槽函数
void Doctor1::on_exitBtn_clicked()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);//apply the date of IOStream;
    out<<(qint16)4<<usrName;//写入自己得IP地址
    qDebug()<<tr("用户退出");
    udpSocket->writeDatagram(data,data.length(),(QHostAddress)ServerIP,port);//UDP broadcast
    tClnt->abort();
    tClnt->disconnect();
    tClnt->close();//关闭当前Tcp连接
    fileTcpServer->close();
    close();
}
void Doctor1::on_FileSendBtn_clicked()
{
    Client * patfilesend = new Client;
    patfilesend->Path="C:/Doctor";
    patfilesend->docIP=ServerIP;
    patfilesend->docName= ui->usrNameLine->text();
    patfilesend->openFile();
}





void Doctor1::on_pushButton_clicked()
{
    for(int i=0; i<deleteRow; i++){
        ui->allUsrTable_2->removeRow(i);
    }
    ui->allUsrTable_2->clearContents();

    QDir *dirpat=new QDir(Path2);
    QStringList filterPat;
    filterPat<<"*.txt";
    dirpat->setNameFilters(filterPat);
    QList<QFileInfo> *fileInfoPat=new QList<QFileInfo>(dirpat->entryInfoList(filterPat));
    int count_patient = (int)fileInfoPat->count();
    qDebug() << count_patient;
    deleteRow = count_patient;
    for(int i=0; i<count_patient; i++){
       ui->allUsrTable_2->insertRow(0);

       qDebug() << fileInfoPat->at(i).filePath();
       Patient pat;
       Storage stor;
       stor.loadfromtxtPatient(fileInfoPat->at(i).filePath().toStdString(), pat);

       ui->allUsrTable_2->setItem(i,0,new QTableWidgetItem(pat.getname()));
       ui->allUsrTable_2->setItem(i,1,new QTableWidgetItem(fileInfoPat->at(i).fileName().split(".")[0]));
     }

}

void Doctor1::on_allUsrTable_2_cellClicked(int row, int column)
{

    QDir *dirpat=new QDir(Path2);
    QStringList filterPat;
    filterPat<<"*.txt";
    dirpat->setNameFilters(filterPat);
    QList<QFileInfo> *fileInfoPat=new QList<QFileInfo>(dirpat->entryInfoList(filterPat));
    int count_patient = (int)fileInfoPat->count();
    qDebug() << count_patient;

    qDebug() << fileInfoPat->at(row).filePath();
    address1 = fileInfoPat->at(row).filePath();
    Patient pat;
    Storage stor;
    stor.loadfromtxtPatient(fileInfoPat->at(row).filePath().toStdString(), pat);
    patientInformation pI;
    pI.setModal(true);
    pI.exec();


}
void Doctor1::on_btnExit1_clicked()
{
    this->close();
}


void Doctor1::on_btnMin_clicked()
{
    showMinimized();
}

//！双击表格后弹出聊天窗口
void Doctor1::on_allUsrTable_cellDoubleClicked(int row, int column)
{
    QString usr;
    QString IP;
    usr = ui->allUsrTable->item(row,0)->text();
    IP = ui->allUsrTable->item(row,1)->text();
    if (usr == usrName) {
        if (QMessageBox::Yes==QMessageBox::warning(this,tr("提示"),tr("不能和自己聊天！"),QMessageBox::Yes)) {
            return;
        }
    } else {
///////////////////////////////////////////////////////////////////////新对话框需要的参数信息
        mychat * mchat = new mychat;
        mchat->chat_ip=IP;//写入信息
        mchat->chat_name=usr;//写入名字
        mchat->server_ip=serverIP;
        mchat->nameself=usrName;
        mchat->ipself=getIP();
        mchat->tClnt =tClnt;
        mchat->msg_receive =" ";
        mchat->showMsg();
        dialogChat.append(mchat);
        mchat->show();
        //    if(mychat->exec()==QDialog::Accepted)
        //    {
        //        dialogChat.removeOne(mychat);
        //        delete mychat;
        //        mychat =NULL;
        //    }
    }

}



//接受服务器T返回的Tcp消息处理函数
void Doctor1::comeMsg()
{
    // QTextCodec *tc=QTextCodec::codecForName("UTF-8");
    qDebug()<<tr("接受到了服务器返回信息！");
    QByteArray data = tClnt->readAll();
    QDataStream out(&data,QIODevice::ReadOnly);

    QString fromUsr,ip,msg,toUsr,toip;
    out>>fromUsr>>ip>>msg>>toUsr>>toip;

    mychat *chat;
    char flag=1;
//////////////////////////////////////////////////////////首先检查要传达的窗口数据是否存在
    QList<mychat *>::iterator i;
    for(i =dialogChat.begin(); i != dialogChat.end(); ++i)
    {
        if((*i)->chat_ip == ip)
        {
           chat = *i;
           flag=0;
           qDebug()<<"已经存在窗口";
           chat->msg_receive =msg;
           chat->showMsg();
           chat->show();
           break;
        }
    }

    if(flag)//如果窗口不存在就自己新建
    {
        qDebug()<<"新建窗口";
        chat =new mychat;
        chat->chat_ip=ip;
        chat->chat_name=fromUsr;//写入名字
        chat->server_ip=ServerIP;
        chat->nameself=usrName;
        chat->ipself=getIP();
        chat->tClnt =tClnt;
        chat->msg_receive =msg;
        chat->showMsg();
        dialogChat.append(chat);
        chat->show();

/////////////////////////////////////////////////////////////////////////等待返回
//        if(mychat->exec()==QDialog::Accepted)//登录成功之后的操作
//        {
//            dialogChat.removeOne(mychat);
//            delete mychat;
//            mychat =NULL;
//        }
    }

}
