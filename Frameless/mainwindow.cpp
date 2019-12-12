#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include "patientregiter.h"
#include "doctorregister.h"

#include <QMessageBox>


int skin = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QImage _image;
    _image.load(":/image/login1.jpg");
    setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation)));
    setPalette(pal);


    setWindowFlags(Qt::FramelessWindowHint);

    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;

    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnExit->setIcon(QIcon(":/image/exit.png"));

    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    //另外在设计器内查看widgetTitle样式，内有标题栏各个自控件的样式设置
    port = 5500;
    ServerIP = "10.30.28.137";
    //初始化UDP
    udpSocket = new QUdpSocket;
    udpSocket->bind(port, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(connectServer()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete udpSocket;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
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

void MainWindow::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;
}

void MainWindow::on_btnMin_clicked()
{
    showMinimized();
}


void MainWindow::on_btnExit_clicked()
{
    qApp->exit();
}

void MainWindow::on_pushButton_2_clicked()
{
    patientRegiter patientRegiter;
    patientRegiter.setModal(true);
    patientRegiter.exec();
}

void MainWindow::on_pushButton_3_clicked()
{
    doctorRegister doctorRegister;
    doctorRegister.setModal(true);
    doctorRegister.exec();

}

void MainWindow::on_pushButton_clicked()
{
    ID = ui->lineEdit->text();
    password = ui->lineEdit_2->text();
    if (ID.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("ID或密码不能为空")); return;
    }

    else{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString iplocal = getIP();
    out <<(qint16)15 <<iplocal << ID << password;
    udpSocket->writeDatagram(data, data.length(), (QHostAddress)ServerIP, port);
    }
//    Widget widGet;

}
void MainWindow::connectServer(){
    while(udpSocket->hasPendingDatagrams()){
        qDebug()<< "开始准备登录";
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_4_9);
        qint16 msgType;
        qDebug() <<msgType;
        in>>msgType;
        qDebug() <<"login msgType: "<< msgType;
        switch(msgType)
        {
        case (qint16)17:
        {
           qDebug() << "patient login";
           QString stateOfLogin;
           QString usrType;
           in>>usrType;
           if(usrType=="Patient"){
               in >> stateOfLogin;
               if(stateOfLogin == "success"){
                   Patient1 *Patient2 =new Patient1;
                   Patient2->usrName= ui->lineEdit->text();;
                   Patient2->show();
                   this->close();
               }
               else {
                    QMessageBox::warning(this, tr("错误"), tr("病人ID或密码不匹配!"));
               }
           }
           else if(usrType=="Doctor"){
               qDebug() << "doctor login";
               in >> stateOfLogin;
               if(stateOfLogin == "success"){
                   Doctor1 *mychat =new Doctor1;
                   mychat->usrName= ui->lineEdit->text();;
                   mychat->show();
                   this->close();
               }
               else {
                    QMessageBox::warning(this, tr("错误"), tr("医生ID或密码不匹配!"));
               }
           }
           else if (usrType=="unaviable") {
                QMessageBox::warning(this, tr("错误"), tr("ID不匹配!"));
           }

        }
        }
    }
}

QString MainWindow::getIP(){
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

void MainWindow::on_pushButton_4_clicked()
{
    if (skin == 0){
        QImage _image;
        _image.load(":/image/login2.jpg");
        setAutoFillBackground(true);   // 这个属性一定要设置
        QPalette pal(palette());
        pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation)));
        setPalette(pal);
        skin = 1;
    }
    else {
        QImage _image;
        _image.load(":/image/login1.jpg");
        setAutoFillBackground(true);   // 这个属性一定要设置
        QPalette pal(palette());
        pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation)));
        setPalette(pal);
        skin = 0;
    }
}
