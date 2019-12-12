#include "widget.h"
#include "ui_widget.h"
#include<QDebug>
#include<QList>
#include<QNetworkInterface>
#include<QMap>
#include<QTableWidget>
#include<QTcpSocket>
#include<QTcpServer>
#include<QMessageBox>
#include <QDir>
#include <QString>
#include <QFileDialog>
#include "filesend.h"
#include "info.h"

extern int flag2;
extern int flag2Doc;
extern int Registerflag2;
extern int Registerflag2Doc;
extern QString Path;
extern QString PathDoc;
extern QString RegisterPath;
extern QString RegisterPathDoc;
extern int setPathCount;
extern int setPathCountDoc;
extern int RegistersetPathCount;
extern int RegistersetPathCountDoc;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    PatNum =0;
    DocNum =0;
    DocOnLine=0;
    PatOnLine=0;

    PatPort=6666;
    DocPort=6667;
    LoginPort= 5500;
    RegisterPortPatient=6000;
    RegisterPortDoctor = 6001;
    //udp初设设置
    udpSocketPat =new QUdpSocket(this);
    udpSocketDoc = new QUdpSocket(this);
    udpSocketLogin = new QUdpSocket(this);
    udpSocketRegisterPatient = new QUdpSocket(this);
    udpSocketRegisterDoctor = new QUdpSocket(this);

    udpSocketPat->bind(5555,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocketPat,SIGNAL(readyRead()),this,SLOT(PatprocessPendingDatagrams()));
    udpSocketDoc->bind(5556,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocketDoc,SIGNAL(readyRead()),this,SLOT(DocprocessPendingDatagrams()));
    udpSocketLogin->bind(LoginPort,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocketLogin,SIGNAL(readyRead()),this,SLOT(LoginConnect()));
    udpSocketRegisterPatient->bind(RegisterPortPatient,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocketRegisterPatient,SIGNAL(readyRead()),this,SLOT(RegisterPatient()));
    udpSocketRegisterDoctor->bind(RegisterPortDoctor,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocketRegisterDoctor,SIGNAL(readyRead()),this,SLOT(RegisterDoctor()));
    //处理用户表格
    QStringList PatHeader;
    QStringList DocHeader;
    PatHeader<<tr("病人")<<tr("IP地址");
    DocHeader<<tr("医生")<<tr("IP地址");

    ui->patientTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->patientTable->setHorizontalHeaderLabels(PatHeader);
    ui->patientTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->patientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->patientTable->show();

    ui->doctorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->doctorTable->setHorizontalHeaderLabels(DocHeader);
    ui->doctorTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->doctorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->doctorTable->show();
    //QTcpserver设置
    Pat_Tcpserver=new QTcpServer;
    Doc_Tcpserver=new QTcpServer;
    connect(Pat_Tcpserver, SIGNAL(newConnection()),this,SLOT(PatcomeMsg()));//有新连接请求时执行
    connect(Doc_Tcpserver, SIGNAL(newConnection()),this,SLOT(DoccomeMsg()));


    if (!Pat_Tcpserver->listen(QHostAddress::Any,PatPort))
    {
        qDebug() << Pat_Tcpserver->errorString();
        close();
        return;
    }
    if (!Doc_Tcpserver->listen(QHostAddress::Any,DocPort))
    {
        qDebug() << Doc_Tcpserver->errorString();
        close();
        return;
    }
    //文件传输部分操作
    connect(&tcpServer, SIGNAL(newConnection()),this, SLOT(acceptConnection()));//关联tcp server新链接的信号与服务器接收请求的槽
    QDir mydir;
    if(setPathCount==0)//控制路径问题
    {
        Path="C:/ProgramData/Patient";
        if (!mydir.exists(Path))
        {
            mydir.mkpath(Path);
        }
    }
    start();
    //医生文件传输部分操作
    connect(&tcpServerDoc, SIGNAL(newConnection()),this, SLOT(acceptConnectionDoc()));//关联tcp server新链接的信号与服务器接收请求的槽
    QDir mydirDoc;
    if(setPathCountDoc==0)//控制路径问题
    {
        PathDoc="C:/ProgramData/Doctor";
        if (!mydir.exists(PathDoc))
        {
            mydir.mkpath(PathDoc);
        }
    }
    startDoc();
    //病人注册见面文件传输
    connect(&RegistertcpServer, SIGNAL(newConnection()),this, SLOT(RegisteracceptConnection()));//关联tcp server新链接的信号与服务器接收请求的槽
    QDir Registermydir;
    if(RegistersetPathCount==0)//控制路径问题
    {
        RegisterPath="C:/ProgramData/Patient";
        if (!Registermydir.exists(RegisterPath))
        {
            Registermydir.mkpath(RegisterPath);
        }
    }
    Registerstart();
    //医生注册界面文件传输
    connect(&RegistertcpServerDoc, SIGNAL(newConnection()),this, SLOT(RegisteracceptConnectionDoc()));//关联tcp server新链接的信号与服务器接收请求的槽
    QDir RegistermydirDoc;
    if(RegistersetPathCountDoc==0)//控制路径问题
    {
        RegisterPathDoc="C:/ProgramData/Doctor";
        if (!RegistermydirDoc.exists(RegisterPathDoc))
        {
            RegistermydirDoc.mkpath(RegisterPathDoc);
        }
    }
    RegisterstartDoc();
    //服务器资料读取操作： 病人和医生
    //病人资料读取
    QDir *dirpat=new QDir(Path);
    QStringList filterPat;
    filterPat<<"*.txt";
    dirpat->setNameFilters(filterPat);
    QList<QFileInfo> *fileInfoPat=new QList<QFileInfo>(dirpat->entryInfoList(filterPat));
    int count_patient = (int)fileInfoPat->count();
    qDebug() << count_patient;
    for(int i=0; i<count_patient; i++){
        qDebug() << fileInfoPat->at(i).fileName();
        qDebug() << fileInfoPat->at(i).filePath();
        Patient temp_pat1;
        Storage stor;
        string temp_addre = (fileInfoPat->at(i).filePath()).toStdString();
        stor.loadfromtxtPatient(temp_addre, temp_pat1);
        Patient *temp_pat = &temp_pat1;
        QString temp_id= QString("Patient_")+QString::number(temp_pat->getid());
        qDebug()<<temp_id;
        allPatient.insert(temp_id,temp_pat);
        PatNum++;

    }
    //医生资料读取
    QDir *dirDoc=new QDir(PathDoc);
    QStringList filterDoc;
    filterDoc<<"*.txt";
    dirDoc->setNameFilters(filterDoc);
    QList<QFileInfo> *fileInfoDoc=new QList<QFileInfo>(dirDoc->entryInfoList(filterDoc));
    int count_doctor = (int)fileInfoDoc->count();
    qDebug() << count_doctor;
    for(int i=0; i<count_doctor; i++){
        qDebug() << fileInfoDoc->at(i).fileName();
        qDebug() << fileInfoDoc->at(i).filePath();
        Doctor temp_doc1;
        Storage stor;
        string temp_addre = (fileInfoDoc->at(i).filePath()).toStdString();
        stor.loadfromtxtDoctor(temp_addre, temp_doc1);
        Doctor *temp_doc= &temp_doc1;
        Doctor::Department departName= temp_doc->getdepartment();
        QString DepartName = temp_doc->DepartmentQstring(departName);
        QString DoctorName = temp_doc->getname();
        QString DoctorGender = temp_doc->getgender();
        QString temp_id= QString("Doctor_")+QString::number(temp_doc->getid());

        qDebug()<<temp_id<<DepartName;
        allDoctor.insert(temp_id,temp_doc);
        //qDebug()<<temp_doc->getname()<< " "<< temp_doc.getgender()<<" "<<temp_doc.DepartmentQstring(temp_doc.getdepartment())<< " "<< temp_doc.getdescription();
        DocNum++;
        DocDepart.insert(temp_id, DepartName);
        DocName.insert(temp_id, DoctorName);
        DocGender.insert(temp_id, DoctorGender);
    }
}

//!析构函数，释放申请的内存
Widget::~Widget()
{
    delete ui;

    delete Pat_Tcpserver;
    delete Doc_Tcpserver;
    delete sTcpSock;
    delete udpSocketPat;
    delete udpSocketDoc;
    PatMap.clear();
    tcpPat.clear();
    DocMap.clear();
    tcpDoc.clear();
    allDoctor.clear();
    allPatient.clear();
}

void Widget::start()
{
    if (!tcpServer.listen(QHostAddress::Any, 7778))//监听所有ip地址的7778端口的主机,用于接受patient的文件
    {
        qDebug()<< tcpServer.errorString();//报错
        close();//tcp关闭
        return;
    }
    totalBytes = 0;//初始化文件总大小为0
    bytesReceived = 0;//初始化已接收字节为0
    fileNameSize = 0;//初始化文件名大小为0
}
//病人注册界面文件传输
void Widget::Registerstart()
{
    if (!RegistertcpServer.listen(QHostAddress::Any, 7782))//监听所有ip地址的7782端口的主机,用于接受patient注册界面的文件
    {
        qDebug()<< RegistertcpServer.errorString();//报错
        close();//tcp关闭
        return;
    }
    RegistertotalBytes = 0;//初始化文件总大小为0
    RegisterbytesReceived = 0;//初始化已接收字节为0
    RegisterfileNameSize = 0;//初始化文件名大小为0
}
void Widget::startDoc()
{
    if (!tcpServerDoc.listen(QHostAddress::Any, 7780))//监听所有ip地址的7780端口的主机,用于接受Doctor的文件
    {
        qDebug()<< tcpServerDoc.errorString();//报错
        close();//tcp关闭
        return;
    }
    totalBytesDoc = 0;//初始化文件总大小为0
    bytesReceivedDoc = 0;//初始化已接收字节为0
    fileNameSizeDoc = 0;//初始化文件名大小为0
}
//医生注册界面文件传输
void Widget::RegisterstartDoc()
{
    if (!RegistertcpServerDoc.listen(QHostAddress::Any, 7783))//监听所有ip地址的7783端口的主机,用于接受Doctor注册界面的文件
    {
        qDebug()<< RegistertcpServerDoc.errorString();//报错
        close();//tcp关闭
        return;
    }
    RegistertotalBytesDoc = 0;//初始化文件总大小为0
    RegisterbytesReceivedDoc = 0;//初始化已接收字节为0
    RegisterfileNameSizeDoc = 0;//初始化文件名大小为0
}
void Widget::acceptConnection()//接受请求槽
{
    tcpServerConnection = tcpServer.nextPendingConnection();//接收到请求并获取套接字//Returns the next pending connection as a connected QTcpSocket object.
    connect(tcpServerConnection, SIGNAL(readyRead()),this, SLOT(updateServerProcess()));//关联准备tcp接收的信号与更新进度的槽
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));//关联tcp 错误信息与显示错误的槽
    tcpServer.close();//关掉tcp,但是这里要求一直接受文件
}
void Widget::RegisteracceptConnection()//接受病人注册界面请求槽
{
    RegistertcpServerConnection = RegistertcpServer.nextPendingConnection();//接收到请求并获取套接字//Returns the next pending connection as a connected QTcpSocket object.
    connect(RegistertcpServerConnection, SIGNAL(readyRead()),this, SLOT(RegisterupdateServerProcess()));//关联准备tcp接收的信号与更新进度的槽
    connect(RegistertcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(RegisterdisplayError(QAbstractSocket::SocketError)));//关联tcp 错误信息与显示错误的槽
    RegistertcpServer.close();//关掉tcp,但是这里要求一直接受文件
}
void Widget::acceptConnectionDoc()//接受请求槽
{
    tcpServerConnectionDoc = tcpServerDoc.nextPendingConnection();//接收到请求并获取套接字//Returns the next pending connection as a connected QTcpSocket object.
    connect(tcpServerConnectionDoc, SIGNAL(readyRead()),this, SLOT(updateServerProcessDoc()));//关联准备tcp接收的信号与更新进度的槽
    connect(tcpServerConnectionDoc, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayErrorDoc(QAbstractSocket::SocketError)));//关联tcp 错误信息与显示错误的槽
    tcpServerDoc.close();//关掉tcp,但是这里要求一直接受文件
}
void Widget::RegisteracceptConnectionDoc()//接受医生注册界面请求槽
{
    RegistertcpServerConnectionDoc = RegistertcpServerDoc.nextPendingConnection();//接收到请求并获取套接字//Returns the next pending connection as a connected QTcpSocket object.
    connect(RegistertcpServerConnectionDoc, SIGNAL(readyRead()),this, SLOT(RegisterupdateServerProcessDoc()));//关联准备tcp接收的信号与更新进度的槽
    connect(RegistertcpServerConnectionDoc, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(RegisterdisplayErrorDoc(QAbstractSocket::SocketError)));//关联tcp 错误信息与显示错误的槽
    RegistertcpServerDoc.close();//关掉tcp,但是这里要求一直接受文件
}
void Widget::updateServerProcess()//文件的接受工作
{
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
            bytesReceived += fileNameSize;//更新接收数据总大小
            localFile = new QFile(Path +"/"+ fileName);//根据指定path建立文件
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
        relation[doc_name]= pat_name;
        if(DocMap.contains(doc_name)){
            QByteArray data2;
            qDebug()<<"yes";
            qDebug() << DocMap[doc_name];
            QDataStream out(&data2,QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_7);
            out<< (qint16) 9<< pat_name << doc_name;
            udpSocketDoc->writeDatagram(data2,data2.length(),(QHostAddress)DocMap[doc_name],5556);//发送数据
            qDebug() <<"传送完成";
            }

        start();//重新开始监听
    }
}
//病人注册界面文件接收工作
void Widget::RegisterupdateServerProcess()//文件的接受工作
{
    QDataStream in(RegistertcpServerConnection);//The QDataStream class implements the serialization of C++'s basic data types, like char, short, int, char *, etc. Serialization of more complex data is accomplished by breaking up the data into primitive units.
    in.setVersion(QDataStream::Qt_5_8);//设置数据流版本
    if (RegisterbytesReceived <= sizeof(qint64)*2)//根据客户端的定义，将文件名大小信息空间与总大小信息空间个定义为qint64
    {//如果接收到的数据小于预留值
        if((RegistertcpServerConnection->bytesAvailable() >= sizeof(qint64)*2)&& (RegisterfileNameSize == 0)) //如果可以获得的数据大于预留值并且文件名大小为0.也就是说文件大小信息与文件名大小信息都已经完整的传过来了//Returns the number of bytes that are available for reading. This function is commonly used with sequential devices to determine the number of bytes to allocate in a buffer before reading.
        {
            in >> RegistertotalBytes >> RegisterfileNameSize;//保存文件大小信息与文件名大小信息
            RegisterbytesReceived += sizeof(qint64) * 2;//记录已经接收到的数据大小
        }
        if((RegistertcpServerConnection->bytesAvailable() >= RegisterfileNameSize)&& (RegisterfileNameSize != 0))//如果文件名还没接收完
        {
            in >> RegisterfileName;//生成文件名
            RegisterbytesReceived += RegisterfileNameSize;//更新接收数据总大小
            RegisterlocalFile = new QFile(RegisterPath +"/"+ RegisterfileName);//根据指定path建立文件
            if (!RegisterlocalFile->open(QFile::WriteOnly))//解决无法写入错误
            {
                QMessageBox::warning(this,tr("Error!"),tr("Server: File opening failed!"),QMessageBox::Yes);
                qDebug()<<RegisterPath;
                return;
            }
        }
        else//如果没有足够多的数据就返回
        {
            return;
        }
    }
    if (RegisterbytesReceived < RegistertotalBytes)//如果接收数据大小小于总大小，则把数据写入文件
    {
        RegisterbytesReceived += RegistertcpServerConnection->bytesAvailable();//增加读入数据的值
        RegisterinBlock = RegistertcpServerConnection->readAll();//把所有的数据全部写入缓存
        RegisterlocalFile->write(RegisterinBlock);//将缓存写入硬盘
        RegisterinBlock.resize(0);//清空缓存
    }

    if (RegisterbytesReceived == RegistertotalBytes)//如果接收完成
    {
        RegistertcpServerConnection->close();//关闭链接
        RegisterlocalFile->close();//完成文件写入
        qDebug()<<"接收病人注册文件完成";
        Patient temp_pat1;
        Storage stor;
        string temp_addre = (Path +"/"+RegisterfileName).toStdString();
        stor.loadfromtxtPatient(temp_addre, temp_pat1);
        Patient * temp_pat = & temp_pat1;
        QString temp_id= QString("Patient_")+QString::number(temp_pat->getid());
        allPatient.insert(temp_id,temp_pat);
//        PatNum++;
        Registerstart();//重新开始监听
    }
}

void Widget::updateServerProcessDoc()//文件的接受工作
{
    QDataStream in(tcpServerConnectionDoc);//The QDataStream class implements the serialization of C++'s basic data types, like char, short, int, char *, etc. Serialization of more complex data is accomplished by breaking up the data into primitive units.
    in.setVersion(QDataStream::Qt_5_8);//设置数据流版本
    if (bytesReceivedDoc <= sizeof(qint64)*2)//根据客户端的定义，将文件名大小信息空间与总大小信息空间个定义为qint64
    {//如果接收到的数据小于预留值
        if((tcpServerConnectionDoc->bytesAvailable() >= sizeof(qint64)*2)&& (fileNameSizeDoc == 0)) //如果可以获得的数据大于预留值并且文件名大小为0.也就是说文件大小信息与文件名大小信息都已经完整的传过来了//Returns the number of bytes that are available for reading. This function is commonly used with sequential devices to determine the number of bytes to allocate in a buffer before reading.
        {
            in >> totalBytesDoc>> fileNameSizeDoc;//保存文件大小信息与文件名大小信息
            bytesReceivedDoc += sizeof(qint64) * 2;//记录已经接收到的数据大小
        }
        if((tcpServerConnectionDoc->bytesAvailable() >= fileNameSizeDoc)&& (fileNameSizeDoc != 0))//如果文件名还没接收完
        {
            in >> fileNameDoc;//生成文件名
            bytesReceivedDoc += fileNameSizeDoc;//更新接收数据总大小
            localFileDoc = new QFile(PathDoc +"/"+ fileNameDoc);//根据指定path建立文件
            fileReceiveFromDoc=fileNameDoc;
            if (!localFileDoc->open(QFile::WriteOnly))//解决无法写入错误
            {
                QMessageBox::warning(this,tr("Error!"),tr("Server: File opening failed!"),QMessageBox::Yes);
                qDebug()<<PathDoc;
                return;
            }
        }
        else//如果没有足够多的数据就返回
        {
            return;
        }
    }
    if (bytesReceivedDoc < totalBytesDoc)//如果接收数据大小小于总大小，则把数据写入文件
    {
        bytesReceivedDoc += tcpServerConnectionDoc->bytesAvailable();//增加读入数据的值
        inBlockDoc = tcpServerConnectionDoc->readAll();//把所有的数据全部写入缓存
        localFileDoc->write(inBlockDoc);//将缓存写入硬盘
        inBlockDoc.resize(0);//清空缓存
    }

    if (bytesReceivedDoc == totalBytesDoc)//如果接收完成
    {
        tcpServerConnectionDoc->close();//关闭链接
        localFileDoc->close();//完成文件写入
        qDebug()<<"接收完成";
    }

    startDoc();//重新开始监听
}
//医生注册界面文件接收工作
void Widget::RegisterupdateServerProcessDoc()//文件的接受工作
{
    QDataStream in(RegistertcpServerConnectionDoc);//The QDataStream class implements the serialization of C++'s basic data types, like char, short, int, char *, etc. Serialization of more complex data is accomplished by breaking up the data into primitive units.
    in.setVersion(QDataStream::Qt_5_8);//设置数据流版本
    if (RegisterbytesReceivedDoc <= sizeof(qint64)*2)//根据客户端的定义，将文件名大小信息空间与总大小信息空间个定义为qint64
    {//如果接收到的数据小于预留值
        if((RegistertcpServerConnectionDoc->bytesAvailable() >= sizeof(qint64)*2)&& (RegisterfileNameSizeDoc == 0)) //如果可以获得的数据大于预留值并且文件名大小为0.也就是说文件大小信息与文件名大小信息都已经完整的传过来了//Returns the number of bytes that are available for reading. This function is commonly used with sequential devices to determine the number of bytes to allocate in a buffer before reading.
        {
            in >> RegistertotalBytesDoc>> RegisterfileNameSizeDoc;//保存文件大小信息与文件名大小信息
            RegisterbytesReceivedDoc += sizeof(qint64) * 2;//记录已经接收到的数据大小
        }
        if((RegistertcpServerConnectionDoc->bytesAvailable() >= RegisterfileNameSizeDoc)&& (RegisterfileNameSizeDoc != 0))//如果文件名还没接收完
        {
            in >> RegisterfileNameDoc;//生成文件名
            RegisterbytesReceivedDoc += RegisterfileNameSizeDoc;//更新接收数据总大小
            RegisterlocalFileDoc = new QFile(RegisterPathDoc +"/"+ RegisterfileNameDoc);//根据指定path建立文件
            RegisterfileReceiveFromDoc=RegisterfileNameDoc;
            if (!RegisterlocalFileDoc->open(QFile::WriteOnly))//解决无法写入错误
            {
                QMessageBox::warning(this,tr("Error!"),tr("Server: File opening failed!"),QMessageBox::Yes);
                qDebug()<<RegisterPathDoc;
                return;
            }
        }
        else//如果没有足够多的数据就返回
        {
            return;
        }
    }
    if (RegisterbytesReceivedDoc < RegistertotalBytesDoc)//如果接收数据大小小于总大小，则把数据写入文件
    {
        RegisterbytesReceivedDoc += RegistertcpServerConnectionDoc->bytesAvailable();//增加读入数据的值
        RegisterinBlockDoc = RegistertcpServerConnectionDoc->readAll();//把所有的数据全部写入缓存
        RegisterlocalFileDoc->write(RegisterinBlockDoc);//将缓存写入硬盘
        RegisterinBlockDoc.resize(0);//清空缓存
    }

    if (RegisterbytesReceivedDoc == RegistertotalBytesDoc)//如果接收完成
    {
        RegistertcpServerConnectionDoc->close();//关闭链接
        RegisterlocalFileDoc->close();//完成文件写入
        qDebug()<<"接收医生注册文件完成";
        RegisterstartDoc();//重新开始监听
    }
    //这里进行读取文件操作，
    Doctor temp_doc1;
    Storage stor;
    string temp_addre = (RegisterPathDoc +"/"+ RegisterfileReceiveFromDoc).toStdString();
    stor.loadfromtxtDoctor(temp_addre, temp_doc1);
    Doctor*temp_doc = &temp_doc1;
    QString temp_id= QString("Doctor_")+QString::number(temp_doc->getid());
    allDoctor.insert(temp_id,temp_doc);
    Doctor::Department departName= temp_doc->getdepartment();
    QString DepartName = temp_doc->DepartmentQstring(departName);
    QString DoctorName = temp_doc->getname();
    QString DoctorGender = temp_doc->getgender();
    qDebug()<<temp_id<<DepartName;
    allDoctor.insert(temp_id,temp_doc);
    //qDebug()<<temp_doc->getname()<< " "<< temp_doc.getgender()<<" "<<temp_doc.DepartmentQstring(temp_doc.getdepartment())<< " "<< temp_doc.getdescription();
    DocNum++;
    DocDepart.insert(temp_id, DepartName);
    DocDepart.insert(temp_id, DepartName);
    DocName.insert(temp_id, DoctorName);
    DocGender.insert(temp_id, DoctorGender);
//    DocNum++;

}

void Widget::displayError(QAbstractSocket::SocketError socketError)//错误槽
{
    qDebug()<<tcpServerConnection->errorString();
    tcpServerConnection->close();
    start();//重新开始
}
void Widget::RegisterdisplayError(QAbstractSocket::SocketError socketError)//错误槽
{
    qDebug()<<RegistertcpServerConnection->errorString();
    RegistertcpServerConnection->close();
    Registerstart();//重新开始
}
void Widget::displayErrorDoc(QAbstractSocket::SocketError socketError)//错误槽
{
    qDebug()<<tcpServerConnectionDoc->errorString();
    tcpServerConnectionDoc->close();
    startDoc();//重新开始
}
void Widget::RegisterdisplayErrorDoc(QAbstractSocket::SocketError socketError)//错误槽
{
    qDebug()<<RegistertcpServerConnectionDoc->errorString();
    RegistertcpServerConnectionDoc->close();
    RegisterstartDoc();//重新开始
}
void Widget::closeEvent(QCloseEvent *event)//窗口被关掉事件处理，解决内存溢出问题
{
    tcpServer.deleteLater();//关掉tcp
    totalBytes = 0;//重置
    bytesReceived = 0;
    fileNameSize = 0;
    flag2=0;
}
//，有新的connect来的时候,启动一个线程

void Widget::PatcomeMsg()
{

    QTcpSocket * tcp ;
    tcp=Pat_Tcpserver->nextPendingConnection();//获得当前套接字
    qDebug()<<"收到一个新的病人连接\r\n客户端的IP:"<<tcp->peerAddress().toString();
    tcpPat.insert(tcp->peerAddress().toString().mid(7),tcp);//获得套接字对方的IP并储存
    connect(tcp,SIGNAL(readyRead()),this,SLOT(PattdpConnectionProcess()));
}
void Widget::DoccomeMsg()
{
    QTcpSocket * tcp;

    tcp=Doc_Tcpserver->nextPendingConnection();//获得当前套接字
    qDebug()<<"收到一个新的医生连接\r\n客户端的IP:"<<tcp->peerAddress().toString();
    tcpDoc.insert(tcp->peerAddress().toString().mid(7),tcp);//获得套接字对方的IP并储存
    connect(tcp,SIGNAL(readyRead()),this,SLOT(DoctdpConnectionProcess()));
}

void Widget::DoctdpConnectionProcess()
{
    QTcpSocket * tcp ;
    QTcpSocket * tcp_to ;
    tcp=qobject_cast <QTcpSocket*>(sender());//获得那个信号过来的
    qDebug()<<"即将转发一条医生消息\r\n"<<"消息来自："<<tcp->localAddress();
 //   int i=tcp->bytesAvailable();
    if(tcp->isReadable())//消息可读
    {
        quint16 blockSize=0;
        QByteArray block =tcp->readAll();
        block.resize(block.size());
        QDataStream in(&block,QIODevice::ReadOnly);
        //in.setVersion(QDataStream::Qt_5_6);

        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        //out.setVersion(QDataStream::Qt_5_6);
        if (blockSize == 0) {
            in >> blockSize;
        }

       QString MsgType,fromname,fromip,msg,toname,toip;
       in>>MsgType>>fromname>>fromip>>msg>>toname>>toip;

        if(MsgType=="transfer")
        {
            tcp_to=tcpDoc.value(toip);
            out<<fromname<<fromip<<msg<<toname<<toip;//把来源和信息发送过去
            tcp_to->write(data);//发送信息
            qDebug()<<"医生消息转发完成\r\n"<<"内容："<<msg<<"转发的目的地址："<<toip;
        }
    }

}

//连接请求处理函数
void Widget::PattdpConnectionProcess()
{
    QTcpSocket * tcp ;
    QTcpSocket * tcp_to ;
    tcp=qobject_cast <QTcpSocket*>(sender());//获得那个信号过来的
    qDebug()<<"即将转发一条病人消息\r\n"<<"消息来自："<<tcp->localAddress();
 //   int i=tcp->bytesAvailable();

    if(tcp->isReadable())//消息可读
    {
        quint16 blockSize=0;
        QByteArray block =tcp->readAll();
        block.resize(block.size());
        QDataStream in(&block,QIODevice::ReadOnly);
        //in.setVersion(QDataStream::Qt_5_6);

        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        //out.setVersion(QDataStream::Qt_5_6);

        if (blockSize == 0) {
            in >> blockSize;
        }

       QString MsgType,fromname,fromip,msg,toname,toip;
       in>>MsgType>>fromname>>fromip>>msg>>toname>>toip;


        if(MsgType=="transfer")
        {

            tcp_to=tcpPat.value(toip);
            out<<fromname<<fromip<<msg<<toname<<toip;//把来源和信息发送过去
            tcp_to->write(data);//发送信息
            qDebug()<<"病人消息转发完成\r\n"<<"内容："<<msg<<"转发的目的地址："<<toip;
        }
    }

}
//.udp消息处理函数
//功能：当有新的用户进入时，返回自己的HostIP和已经登录的用户昵称
void Widget::PatprocessPendingDatagrams()
{
    while(udpSocketPat->hasPendingDatagrams())
    {
        QByteArray datagram;
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        datagram.resize(udpSocketPat->pendingDatagramSize());
        udpSocketPat->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        qint16 msgType;
        in>>msgType;//取出信息类型
        switch(msgType)
        {
         case (qint16)4://usr Left,清除注册的信息
        {
             QString name;
             in>>name;
             qDebug() << name <<"病人退出";
             PatOnLine--;
             tcpPat[PatMap.value(name)]->disconnect();
             tcpPat[PatMap.value(name)]->close();
             tcpPat[PatMap.value(name)]->abort();
             tcpPat.remove(PatMap.value(name));//移除TcpSocket
             PatMap.remove(name);
             ui->patientTable->setRowCount(0);
             QMapIterator<QString, QString> i(PatMap);//Map的迭代器
             while (i.hasNext())//刷新用户信息
             {
                 i.next();
                 ui->patientTable->insertRow(0);//服务器数据存储
                 ui->patientTable->setItem(0,0,new QTableWidgetItem(i.key()));
                 ui->patientTable->setItem(0,1,new QTableWidgetItem(i.value()));
             }
             PatupDateMsg();
             DocupDateMsg();
             break;

        }
        case (qint16)8://收到病人端病人的选择
        {
            in>>pat_name>> doc_name;
            qDebug()<<pat_name+ "选择了"+doc_name;
            break;
        }
        case (qint16)20://处理新病人加入信息
        {
            qDebug()<<"receive a new patient";
            PatOnLine++;
            Patient newPatient;
            QString ip;
            QString usrName;
            in>>ip;
            //防止重复IP加入
            foreach (QString value,  PatMap)
            {
               if(value==ip)
               {
                  if (QMessageBox::Yes==QMessageBox::warning(this,tr("提示"),tr("有重复的IP地址,请检查!")
                   ,QMessageBox::Yes))
                  {
                    return;
                  }
               }
            }
            in>>usrName;
            PatMap.insert(usrName,ip);//存入容器
            out<<PatMap.size();//获得在线病人的总数，放入数据
            qDebug() <<"patient ip is " << ip;
            qDebug() <<"patient name is "<< usrName;
            QMapIterator<QString, QString> i(DocMap);//Map的迭代器
            QMapIterator<QString, QString> sentdata(PatMap);
            while (i.hasNext())//迭代器返回所有病人
            {
                i.next();
                out << i.key() << i.value();
                qDebug() <<"Doctor name is " << i.key();
                qDebug() <<"Doctor ip is " << i.value();

            }
             while (sentdata.hasNext())//迭代器返回所有病人
             {
                 QByteArray data2;
                 QDataStream out(&data2,QIODevice::WriteOnly);
                 //发送数据
                 sentdata.next();
                 qDebug() << "The return name is "<<sentdata.key();
                 qDebug() << "The return ip is "<<sentdata.value();
                 out<<(qint16)3<<sentdata.key(); //返还服务器信息
                 udpSocketPat->writeDatagram(data2,data2.length(),(QHostAddress)sentdata.value(),5555);
                 qDebug() << sentdata.value();
             }
            ui->patientTable->insertRow(0);//服务器数据存储
            ui->patientTable->setItem(0,0,new QTableWidgetItem(usrName));
            ui->patientTable->setItem(0,1,new QTableWidgetItem(ip));
            qDebug() <<"发送验证完成";
            PatupDateMsg();
            DocupDateMsg();
            break;
         }
        case (qint16)30:
        {
            qDebug()<<"修改时间";
            QString timelabel;
            QString ModifyDoctorID;
            QString ModifyPatientID;
            in>>ModifyPatientID>>ModifyDoctorID>>timelabel;
            Storage stor;
            //modify doctor's object's schedule map.
            Doctor doc;
            std::string add = std::string("C:/Doctor/")+ModifyDoctorID.toStdString()+".txt";
            stor.loadfromtxtDoctor(add, doc);
            int patid = ModifyPatientID.split("_")[1].toInt();
            std::map<QString, int> mymap = doc.getschedule();
            mymap[timelabel] =patid;
            doc.putschedule(mymap);

            //modify patient's varible reserveddoctorID.
            Patient pat;
            add = std::string("C:/Patient/")+ModifyPatientID.toStdString()+".txt";
            stor.loadfromtxtPatient(add, pat);
            int docid = ModifyDoctorID.split("_")[1].toInt();
            pat.putreserveddoctorID(docid);
        }
           default:{break;}
        }
    }
}

void Widget::DocprocessPendingDatagrams()
{
    while(udpSocketDoc->hasPendingDatagrams())
    {
        QByteArray datagram;
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        datagram.resize(udpSocketDoc->pendingDatagramSize());
        udpSocketDoc->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        qint16 msgType;
        in>>msgType;//取出信息类型
        switch(msgType)
        {
        case (qint16)2://处理新医生加入信息
        {
            DocOnLine++;
            QString ip;
            QString usrName;
            in>>ip>>usrName;
            //防止重复IP加入
            foreach (QString value,  DocMap)
            {
               if(value==ip)
               {
                  if (QMessageBox::Yes==QMessageBox::warning(this,tr("提示"),tr("有重复的IP地址,请检查!")
                   ,QMessageBox::Yes))
                  {
                    return;
                  }
               }
            }
            out<<(qint16)3<<getIP(); //返还服务器信息
            DocMap.insert(usrName,ip);//存入容器
            out<<DocMap.size();//获得在线医生的总数，放入数据
            QMapIterator<QString, QString> i(DocMap);//Map的迭代器
            QMapIterator<QString, QString> sendDocData(DocMap);
            while (i.hasNext())//迭代器返回所有医生
            {
                i.next();
                out << i.key() << i.value();
            }
             QHostAddress addr;
             while (sendDocData.hasNext())//迭代器返回所有医生
             {
                 sendDocData.next();
                 addr.setAddress(sendDocData.value());//这里必须用指定的IP不然不可以发出消息,不知为何
                 udpSocketDoc->writeDatagram(data,data.length(),(QHostAddress)sendDocData.value(),5556);//发送数据
             }
            ui->doctorTable->insertRow(0);//服务器数据存储
            ui->doctorTable->setItem(0,0,new QTableWidgetItem(usrName));
            ui->doctorTable->setItem(0,1,new QTableWidgetItem(ip));
            DocupDateMsg();
            PatupDateMsg();
            break;
         }
         case (qint16)4://医生 离开,清除注册的信息
        {
             DocOnLine--;
             QString name;
             in>>name;
             qDebug() << name <<"医生退出";
             tcpDoc[DocMap.value(name)]->disconnect();
             tcpDoc[DocMap.value(name)]->close();
             tcpDoc[DocMap.value(name)]->abort();
             tcpDoc.remove(DocMap.value(name));//移除TcpSocket
             DocMap.remove(name);
             ui->doctorTable->setRowCount(0);
             QMapIterator<QString, QString> i(DocMap);//Map的迭代器
             while (i.hasNext())//刷新用户信息
             {
                 i.next();
                 ui->doctorTable->insertRow(0);//服务器数据存储
                 ui->doctorTable->setItem(0,0,new QTableWidgetItem(i.key()));
                 ui->doctorTable->setItem(0,1,new QTableWidgetItem(i.value()));
             }
             DocupDateMsg();
             PatupDateMsg();
             break;
        }
        case (qint16)9://医生返回确认信息
        {
            if(relation.contains(doc_name)){
                qDebug() <<"在线";
                Client * filesend = new Client;
                filesend->docIP=DocMap[doc_name];
                filesend->docName= doc_name;
                filesend->openFile();
            }
            break;
        }
        case (qint16)11:
        {
            if(relation.contains(doc_name)){
                Client * patfilesend = new Client;
                patfilesend->Path="C:/ProgramData/Patient";
                patfilesend->docIP=DocMap[doc_name];
                patfilesend->docName= relation[doc_name];
                patfilesend->openFile();
                relation.remove(doc_name);
            }
        }
           default:{break;}
        }
    }
}

void Widget::LoginConnect()
{
    while(udpSocketLogin->hasPendingDatagrams())
    {
        QByteArray datagram;
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_9);

        datagram.resize(udpSocketLogin->pendingDatagramSize());
        udpSocketLogin->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        qint16 msgType;
        in>>msgType;//取出信息类型
        switch(msgType)
        {
        case (qint16)15:
        {
            qDebug() << "登陆开始";
            QString loginIP, loginID, loginPW;
            QString stateOfLogin;
            QString usrType;
            QByteArray state;
            QDataStream out(&state,QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_9);
            in>> loginIP>>loginID>>loginPW;
            QChar ch = loginID.at(0);
            if(ch == "P"){  //patient
                usrType= "Patient";
                qDebug() << "Patient";
                Patient pat;
                Storage stor;
                string addre = (Path+QString("/")+loginID+QString(".txt")).toStdString();
                stor.loadfromtxtPatient(addre, pat);
                if ((pat.getid()==0)||(pat.getpassword()!= loginPW))//not sucessful in matching the file.
                {
                    stateOfLogin = "failture";
                }
                else
                {
                    stateOfLogin ="success";
                }
                out<< (qint16)17<<usrType<<stateOfLogin;
                qDebug() << 17;
                udpSocketLogin->writeDatagram(state,state.length(), (QHostAddress)loginIP, LoginPort);
            }else if(ch == "D"){    //doctor
                usrType= "Doctor";
                qDebug() << "doctor";
                Doctor doc;
                Storage stor;
                string addre = (PathDoc+QString("/")+loginID+QString(".txt")).toStdString();
                stor.loadfromtxtDoctor(addre, doc);
                if ((doc.getid()==0)||(doc.getpassword()!= loginPW))//not sucessful in matching the file.
                {
                    stateOfLogin = "failture";
                }
                else
                {
                    stateOfLogin ="success";
                }
                out<< (qint16)17<<usrType<<stateOfLogin;
                qDebug()<<18;
                udpSocketLogin->writeDatagram(state,state.length(), (QHostAddress)loginIP, LoginPort);
            }
            else{
                usrType="unaviable";
                out<< (qint16)17<<usrType ;
                udpSocketLogin->writeDatagram(state,state.length(), (QHostAddress)loginIP, LoginPort);
            }
            break;
        }
        }
    }
}
void Widget::RegisterPatient()
{
    while(udpSocketRegisterPatient->hasPendingDatagrams())
    {
        QByteArray datagram;
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_9);
        qDebug() << "收到病人注册信号";
        datagram.resize(udpSocketRegisterPatient->pendingDatagramSize());
        udpSocketRegisterPatient->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        qint16 msgType;
        in>>msgType;//取出信息类型
        switch(msgType)
        {
        case (qint16)2://处理新病人加入信息
        {
            qDebug()<<"receive a new patient";
            Patient newPatient;
            QString ip;
            QString usrName;
            PatNum++;
            in>>ip;
            usrName =tr("Patient_") +QString::number(PatNum);//分配用户名
//            PatMap.insert(usrName,ip);//存入容器
            qDebug() <<"patient ip is " << ip;
            qDebug() <<"patient name is "<< usrName;
             //发送数据
             out<<(qint16)3<<usrName; //返还服务器信息
            udpSocketRegisterPatient->writeDatagram(data,data.length(),(QHostAddress)ip,RegisterPortPatient);
            qDebug() << ip;
            qDebug() <<"发送验证完成";
            break;
            }
        case (qint16)40:
        {
            QString patientid, depart;
            in>>patientid>>depart;
            qDebug() << "case 40 触发";
            DocRecom.insert(patientid, depart);
            QMapIterator<QString, QString> remm(DocRecom);
            while(remm.hasNext()){
                remm.next();
                qDebug()<<"key "<<remm.key() << "value "<<remm.value();
            }
        }
        }
    }
}

void Widget::RegisterDoctor()
{
    while(udpSocketRegisterDoctor->hasPendingDatagrams())
    {
        QByteArray datagram;
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_9);
        qDebug() << "收到病人注册信号";
        datagram.resize(udpSocketRegisterDoctor->pendingDatagramSize());
        udpSocketRegisterDoctor->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        qint16 msgType;
        in>>msgType;//取出信息类型
        switch(msgType)
        {
        case (qint16)20://处理新医生注册信息
        {
            qDebug()<<"receive a new doctor";
            DocNum++;
            QString ip;
            QString usrName;
            in>>ip;
            usrName =tr("Doctor_") +QString::number(DocNum);//分配用户名
//            PatMap.insert(usrName,ip);//存入容器
            qDebug() <<"patient ip is " << ip;
            qDebug() <<"patient name is "<< usrName;
             //发送数据
            out<<(qint16)3<<usrName; //返还服务器信息
            udpSocketRegisterDoctor->writeDatagram(data,data.length(),(QHostAddress)ip,RegisterPortDoctor);
            qDebug() << ip;
            qDebug() <<"发送验证完成";
            break;
         }
        }
    }
}

//!更新现有的病人
//!每个病人都会收到相对应的医生的信息
void Widget::PatupDateMsg()
{
    QByteArray data;
//    QMapIterator<QString, QString> j(DocMap);//在线医生
    QMapIterator<QString, QString> p(PatMap);//


    udpSocketPat->writeDatagram(data,data.length(),QHostAddress::Broadcast,5555);//发送数据
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    QString docName, docGender, docDepart, docMajor;

//    while (j.hasNext())//刷新用户信息
//    {
//        j.next();
//        QHostAddress addr;
//        addr.setAddress(j.value());
//        udpSocketPat->writeDatagram(data,data.length(),(QHostAddress)j.value(),5555);//发送数据
//    }
    while(p.hasNext()){
        p.next();
        out<<(qint16)60<<(qint16)DocMap.size();
        qDebug()<<60;
        QMapIterator<QString, Doctor*> tem_doc(allDoctor);
        while(tem_doc.hasNext()){
            qDebug()<<1;
            tem_doc.next();
            QMapIterator<QString, QString> q(DocMap);
            while(q.hasNext()){
                qDebug()<<2;
                q.next();
                qDebug()<<q.key()<<" "<<tem_doc.key();
                if(q.key()==tem_doc.key()){
                    Doctor *docTemp= allDoctor[QString("Doctor_1")];
                    qDebug()<<"进入比较";
                    docName=DocName[tem_doc.key()];
                    qDebug()<<"name1";
                    docGender=DocGender[tem_doc.key()];
                    qDebug()<<"gender";
//                    Doctor::Department dep = docTemp->getdepartment();
                    docDepart = DocDepart[tem_doc.key()];
                    qDebug()<<"depart";
;
                    qDebug()<<docName<<docGender<<docDepart;
                    out<<docName<<docGender<<docDepart;
                    qDebug()<<3;

                    out<<q.key()<<q.value();
                    qDebug()<<q.key()<<q.value();
                }
                qDebug()<<4;
            }
        }
        udpSocketPat->writeDatagram(data, data.length(),(QHostAddress)p.value(),5555);
    }

    //    //用病人的department寻找全部医生匹配，得到医生ID
    //    QMap<QString, QMap<QString, Doctor>> allMatchedDoc; //所有病人对应的医生组
    //    while (rem.hasNext())
    //    {
    //        rem.next();
    //        QMapIterator<QString, Doctor> ite(allDoctor);//全部注册过的医生

    //        QMap<QString, Doctor> MatchedDoc;
    //        QString pat_depart = rem.value();
    //        while (ite.hasNext())
    //        {
    //            ite.next();
    //            if (ite.value().getdepartment() == pat_depart) {
    //                MatchedDoc.insert(ite.key(), ite.value());
    //            }
    //        }
    //        allMatchedDoc.insert(rem.key(), MatchedDoc);
    //    }
    //    QMap<QString, QMap<QString, Doctor>>::iterator alldoc;

//    for (alldoc = allMatchedDoc.begin(); alldoc != allMatchedDoc.end(); alldoc++)
//    {
//        QString patID = alldoc.key();
//        if (PatMap.find(patID) == PatMap.end()) {
//            allMatchedDoc.erase(alldoc);
//        }
//    }


}

void Widget::DocupDateMsg()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out<<(qint16)6<<(qint16)DocMap.size();//获得在线医生的总数，放入数据

    QMapIterator<QString, QString> i(DocMap);//Map的迭代器
    while (i.hasNext())//刷新用户信息
    {
        i.next();
        out << i.key() << i.value();
    }
    QMapIterator<QString, QString> j(DocMap);//Map的迭代器
//    QMapIterator<QString, QString> k(PatMap);
    udpSocketDoc->writeDatagram(data,data.length(),QHostAddress::Broadcast,5556);//发送数据
    while (j.hasNext())//刷新用户信息
    {
        j.next();
        QHostAddress addr;
        addr.setAddress(j.value());
        udpSocketDoc->writeDatagram(data,data.length(),addr,5556);//发送数据
    }
//    while (k.hasNext())//刷新用户信息
//    {
//        k.next();
//        QHostAddress addr;
//        addr.setAddress(k.value());
//        udpSocketDoc->writeDatagram(data,data.length(),addr,5555);//发送数据
//    }
}

//.本地IP地址
QString  Widget:: getIP()
{
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    for(int i = 0; i < addrs.size(); i++)
    {
        if (addrs.at(i).protocol() == QAbstractSocket::IPv4Protocol &&
                addrs.at(i) != QHostAddress::Null &&
                addrs.at(i) != QHostAddress::LocalHost &&
                !addrs.at(i).toString().contains(QRegExp("^169.*$")))
        {
            QString a;
             a = addrs.at(i).toString();
            return addrs.at(i).toString();
        }
    }
}
