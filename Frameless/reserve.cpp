#include "reserve.h"
#include "ui_reserve.h"
#include <map>
#include <QComboBox>
#include "check.h"
#include<patient.h>
#include<QMessageBox>
#include<QDebug>
#include<QFile>
#include<QFileDialog>
#include <QDir>
#include <QString>

int r = 1; int c = 1;
reserve::reserve(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reserve)
{

    std::map<QString, QString> positionToTime;
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    payloadSize = 128*1024;//每次加载128KB
    bytesWritten = 0;//初始化
    bytesToWrite = 0;
    setPathCount=0;
    ServerIP="10.30.28.137";
    tcpClient = new QTcpSocket(this);//产生新的tcp socket类.端口是7778

    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnExit1->setIcon(QIcon(":/image/exit.png"));

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(tcpClient, SIGNAL(connected()), this, SLOT(startTransfer()));//将tcp的链接成功信号与开始传输槽连接
    connect(tcpClient, SIGNAL(bytesWritten(qint64)),this, SLOT(updateClientProgress(qint64)));//将发送数据的大小信号与更新进度条的槽联系起来
    connect(tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));//将tcp 错误信号与显示错误槽联系起来
    QDir mydir;
    if(setPathCount==0){
        Path="C:/Patient";
        if(!mydir.exists(Path)){
            mydir.mkdir(Path);
        }
    }
}

reserve::~reserve()
{
    delete ui;
}


void reserve::send(){
    bytesWritten = 0;//初始化已发送字节为0
    tcpClient->connectToHost(ServerIP,7778);//连接服务器ip地址7778端口
}


void reserve::startTransfer()
{
    fileName = Path+"/"+patient_id+".txt";
    qDebug() << fileName;
    localFile = new QFile(fileName);//打开文件
    if (!localFile->open(QFile::ReadOnly))//解决打不开问题
    {
        qDebug() << "client:File opening failed!";
        return;
    }
    //获取文件大小
    totalBytes = localFile->size();//获取文件大小
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);//设置输出流属性为只写
    sendOut.setVersion(QDataStream::Qt_5_8);//设置Qt数据流版本，不同版本的数据流格式不同
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);//将绝对路径转化成文件名
    sendOut << qint64(0) << qint64(0) << currentFileName;//预留总大小空间和文件名大小空间+文件名
    totalBytes += outBlock.size();//总大小为预留信息大小以及文件大小之和
    sendOut.device()->seek(0);//将文件写入指针指回开头，为了补充之前预留内容
    sendOut << totalBytes << qint64((outBlock.size() - sizeof(qint64)*2));//写入预留内容=总数据大小+文件大小
    bytesToWrite = totalBytes - tcpClient->write(outBlock);//更新待发送数据大小，发送数据头
    outBlock.resize(0);//重置缓存
}
//!收到确认接收信息后开始发送文件
void reserve::updateClientProgress(qint64 numBytes)
{
    //已经发送数据大小
    bytesWritten += (int)numBytes;
    //如果还有数据要发送
    if (bytesToWrite > 0)
    {
        //每次发送payloadSize大小的数据，如果剩余的数据不足，就发送剩余数据的大小
        outBlock = localFile->read(qMin(bytesToWrite, payloadSize));
        //发送完一次数据后还剩余数据的大小
        bytesToWrite -= (int)tcpClient->write(outBlock);
        //清空发送缓冲区
        outBlock.resize(0);
    }
    //如果没有数据要发送，就关闭文件
    else
    {
        localFile->close();
    }
    //如果发送完，就关闭文件以及tcp
    if(bytesWritten == totalBytes)
    {
        localFile->close();
        tcpClient->close();
        bytesWritten = 0;
        close();
    }
}
void reserve::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpClient->errorString();
    tcpClient->close();//关掉tcp
}
//!退出按钮槽函数

void reserve::on_tableWidget_cellClicked(int row, int column)
{
    r = row; c = column;
    check check;
    check.doctorid= doctor_name;
    check.patientid= patient_id;
    check.setModal(true);
    check.exec();

}
void reserve::on_pushButton_clicked()
{
    send();
}
void reserve::on_btnExit1_clicked()
{
    this->close();
}


void reserve::on_btnMin_clicked()
{
    showMinimized();
}
