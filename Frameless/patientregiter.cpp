#include "patientregiter.h"
#include "ui_patientregiter.h"
#include <QMessageBox>
#include <info.h>
#include<QDebug>
#include"Cutmain.cpp"
#include "docfilesend.h"
patientRegiter::patientRegiter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::patientRegiter)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    QImage _image;
    _image.load(":/image/registerPatient.jpg");
    setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation)));
    setPalette(pal);



    ui->pushButton->setStyleSheet(tr("background-image: url(:/image/login2.jpg);"));
    mLocation1 = this->geometry();
    mIsMax1 = false;
    mDrag1 = false;
    QDateTime curDateTime=QDateTime::currentDateTime();
    ui->dateEdit->setDate(curDateTime.date());


    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnExit1->setIcon(QIcon(":/image/exit.png"));

    //date
//    ui->dateEdit->setDisplayFormat("2019/05/19");
    ui->dateEdit->setCalendarPopup(true);

    port = 6000;
    flag=0;
    //定时器
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(recall(void)));
    timer->start(2000);
    //udp初始化
    ServerIP = "10.30.28.137";
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    ui->textBrowser->setText("正在等待服务器分配ID");
}

patientRegiter::~patientRegiter()
{
    delete ui;
    delete  udpSocket;
    delete timer;

}

bool patientRegiter::eventFilter1(QObject *obj, QEvent *e)
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

//void patientRegiter::mousePressEvent1(QMouseEvent *e)//鼠标按下事件
//{
//    if (e->button() == Qt::LeftButton)
//    {
//        mDrag1 = true;
//        mDragPos1 = e->globalPos() - pos();
//        e->accept();
//    }
//}

//void patientRegiter::mouseMoveEvent1(QMouseEvent *e)//鼠标移动事件
//{
//    if (mDrag1 && (e->buttons() && Qt::LeftButton))
//    {
//        move(e->globalPos() - mDragPos);
//        e->accept();
//    }
//}

//void patientRegiter::mouseReleaseEvent1(QMouseEvent *e)//鼠标释放事件
//{
//    mDrag1 = false;
//}



void patientRegiter::on_btnExit1_clicked()
{
    this->close();
}


void patientRegiter::on_btnMin_clicked()
{
    showMinimized();
}

void patientRegiter::on_pushButton_clicked()
{
    QString ID,name,gender,password,age,birthDate, weight,height,jiguan,telephone,guomin,bingShi,symptom;
    QDate time;
    ID = PatientID;
    name =  ui->lineEdit->text();
    gender = ui->comboBox->currentText();
    password = ui->lineEdit_3->text();
    age = ui->lineEdit_4->text();
    weight = ui->lineEdit_5->text();
    height = ui->lineEdit_6->text();
    telephone = ui->lineEdit_7->text();
    jiguan = ui->lineEdit_8->text();
    guomin = ui->lineEdit_9->text();
    bingShi =  ui->textEdit->toPlainText();
    symptom =  ui->textEdit_2->toPlainText();

    time = ui->dateEdit->date();
    birthDate = time.toString("yyyy/MM/dd");


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
    for (int i = 0; i < age.size(); i++) {
        if (!age[i].isDigit()) {
            QMessageBox::warning(this, tr("错误"), tr("年龄必须为数字!")); return;
        }
    }
    for (int i = 0; i < weight.size(); i++) {
        if (!weight[i].isDigit()) {
            QMessageBox::warning(this, tr("错误"), tr("体重必须为数字！")); return;
        }
    }
    for (int i = 0; i < height.size(); i++) {
        if (!height[i].isDigit()) {
            QMessageBox::warning(this, tr("错误"), tr("身高必须为数字!")); return;
        }
    }

    Patient pat;
    QStringList l = ID.split("_");

    pat.putid(l[1].toInt());
    qDebug()<<"l[1]"<<l[1];
//    pat.putid(ID.toInt());
//    qDebug()<<"ID "<<ID<< " "<<ID.toInt();
    pat.putname(name);
    pat.putgender(gender);
    pat.putpassword(password);
    pat.putage(age.toInt());
    pat.putweight(weight.toDouble());
    pat.putheight(height.toDouble());
    pat.putphone(telephone);
    pat.putjiguan(jiguan);
    pat.putguominyaowu(guomin);
    pat.putbingshi(bingShi);
    pat.putsymptom(symptom);
    pat.putbirthday(birthDate);
    pat.putrecommendation(match(symptom.toStdString()));
    pat.putrecoresult(writeRecord(symptom.toStdString()));
    Storage stor;
    stor.storetotxt(&pat);


    Client * patfilesend = new Client;
    patfilesend->Path="C:/Patient";
    patfilesend->docIP=ServerIP;
    patfilesend->docName= PatientID;
    patfilesend->setPort=7782;
    patfilesend->openFile();
    qDebug()<<"文件传输开始";
    updatechoose();
    this->close();
}
void patientRegiter::recall()
{
    static int times = 0;
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);//apply the date of IOStream;
    QString iplocal =getIP();
    out<<(qint16)2<<iplocal;
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

void patientRegiter::processPendingDatagrams()
{
    qDebug()<<"收到UDP返回的广播消息";
    while(udpSocket->hasPendingDatagrams())
    {
        qDebug() << "is ok";
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());//读取UDP消息
        QDataStream in(&datagram,QIODevice::ReadOnly);//io流管理
        in.setVersion(QDataStream::Qt_4_9);
        qint16 msgType;
        in>>msgType;//取出信息类型
        qDebug() <<"Type: "<<msgType;
        switch(msgType)
        {
        case 3://该类型为服务器的返回消息类型
        {
            in>>PatientID;//获得服务器分配的用户名
            qDebug()<< "name is " <<PatientID;
            ui->textBrowser->setText(PatientID);
            qDebug()<<tr("服务器IP地址：")<<ServerIP;
            flag=1;     //标志位置1，通知重播窗口连接上服务器,关闭定时器
            timer->stop();
        }
        }
    }
}
QString patientRegiter::getIP()
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

void patientRegiter::updatechoose()
{
    Patient choosePat;
    Storage stor;
    string temp_addre=("C:/Patient/"+PatientID+".txt").toStdString();
    stor.loadfromtxtPatient(temp_addre, choosePat);
    QString disease = *(choosePat.getrecommendation());
    QString depart = *(choosePat.getrecommendation()+1);

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << (qint16)40 << PatientID<<depart;
    qDebug()<<"case " <<40<<" " <<PatientID<< " "<< depart;
    udpSocket->writeDatagram(data, data.length(), (QHostAddress)ServerIP, port);
}
