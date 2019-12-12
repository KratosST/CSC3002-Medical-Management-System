#include "chat.h"
#include "ui_mychat.h"
#include <doctor.h>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QUdpSocket>
#include <QHostInfo>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>
#include <QColorDialog>


mychat::mychat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mychat)
{
    ui->setupUi(this);
    ui->textInput->setFocus();//给输入框输入箭头
    loadSize =64*1024;//64K大小的文件
     setWindowFlags(Qt::WindowMinimizeButtonHint);
    //setWindowFlags(Qt::WindowCloseButtonHint);
    qDebug()<<"receive msg is "<<msg_receive;
    qDebug()<<"server_ip  is "<<server_ip;
    //ui->MsgBrowser->setText(msg_receive);
}


//!将接受到的消息弹出显示在窗口
void mychat::showMsg()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->MsgBrowser->setAlignment(Qt::AlignLeft);
    ui->MsgBrowser->setTextColor(Qt::blue);
    ui->MsgBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->MsgBrowser->append("["+chat_name+"--"+time+"]");
    ui->MsgBrowser->append(msg_receive+"\r\n");
    this->setWindowTitle(chat_name);
}

mychat::~mychat()
{
    delete ui;

}

//.发送按钮
void mychat::on_sendBtn_clicked()
{
    QByteArray outBlock;
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QDataStream out(&outBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);


    QString msg = ui->textInput->document()->toHtml();//读取信息
    if(msg.isEmpty())//信息发送不能为空判断
    {
        QMessageBox::warning(nullptr,tr("warning"),tr("发送的消息不能为空!"),
        QMessageBox::Ok);
        return ;
    }
    ui->textInput->clear();//马上清空方面下一次输入

    //将发送内容显示在内容显示栏的右边
    ui->MsgBrowser->setAlignment(Qt::AlignRight);
    ui->MsgBrowser->setTextColor(Qt::green);
    ui->MsgBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->MsgBrowser->append("["+nameself+"--"+time+"]");
    ui->MsgBrowser->append(msg);



    //设置发送的数据的长度
    out.device()->seek(0);
    out << (quint16) 0;

    //设置发送内容
    QString a("transfer");
    out<<a;//消息类型为消息转发
    out<<nameself;//自己的昵称
    out<<ipself ;//自己得IP
    out<<msg;//发送的消息
    out<<chat_name;//发送的目的昵称
    out<<chat_ip;//送的目的ip

    //回到字节流起始位置
    out.device()->seek(0); //重置字节流长度
    out << (quint16) (outBlock.size()-sizeof(quint16));
    tClnt->write(outBlock);//发送消息给服务器让其中转


}




//!文件传输按钮

void mychat::on_FileSendBtn_clicked()
{

    QString fileTosendName;
    fileTcpSocket =new QTcpSocket;//创建文件传输的TcpSocket，文件的端口为7777
    fileName = QFileDialog::getOpenFileName(this);



    if (!fileName.isEmpty())//获得文件按名字后开始传送
    {
        fileTosendName = fileName.right(fileName.size()-fileName.lastIndexOf('/')-1);


       locFile =new QFile(fileName);
       if(!locFile->open(QFile::ReadOnly))
       {
           QMessageBox::warning(this,tr("应用程序"),tr("无法读取文件%1：\n%2.")
                                .arg(fileTosendName)
                                .arg(locFile->errorString()));
       }

       totalBytes =locFile->size();
       QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
       QString currentFile =fileName.right(fileName.size()-
                                           fileName.lastIndexOf('/')-1);//获得文件的大小内容
       sendOut<<qint64(0)<<qint64(0)<<currentFile;

       qDebug()<<"namesize："<<currentFile;


       totalBytes +=outBlock.size();
       qDebug()<<"totalBytes："<<totalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));

       sendOut.device()->seek(0);
       sendOut<<(qint64)totalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));


////////////////////////////////////////////////////////////////////////////////////////连接服务器端
       fileTcpSocket->connectToHost(chat_ip,7777);//开始连接客户端的IP，等待对方响应
       if(fileTcpSocket->waitForConnected(20000))//最多等待20s
       {
           qDebug()<<"文件传输端口已经连接上对方服务器！";
           connect(fileTcpSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateData(qint64)));
           connect(fileTcpSocket,SIGNAL(readyRead()),this,SLOT(isOK()));
       }

       else
       {
           qDebug()<<"无法连接对方的服务器！";
           return;
       }
}

}



//!收到确认接收信息后开始发送文件
void mychat::isOK()
{
    qDebug()<<"收到了确认消息：";
    QString msg;
    QDataStream in(fileTcpSocket);
    in>>msg;
    if(msg =="IamReady")
    {
        qDebug()<<"收到了确认消息："<<"IamReady";
        bytesTowrite=totalBytes - fileTcpSocket->write(outBlock);//发送出去，等待响应
        qDebug()<<"bytesTowrite:"<<bytesTowrite;
        outBlock.resize(0);

    }
    if(msg =="Refuse")
    {
         QMessageBox::information(this,tr("提示"),tr("对方拒绝了您的文件请求"),QMessageBox::Yes);
    }

}


//!发送后续的文件
void mychat::updateData(qint64 numBytes)
{
    byteWritten +=(int)numBytes;
    qDebug()<<"bytesTowrite:"<<bytesTowrite;
    if(bytesTowrite>0)
    {
        outBlock =locFile->read(qMin(bytesTowrite,loadSize));
        bytesTowrite -=(int)fileTcpSocket->write(outBlock);
        outBlock.resize(0);

    }
    else
    {
        locFile->close();
        QMessageBox::information(this,tr("提示"),tr("文件传送成功"),QMessageBox::Yes);

    }
}



//!退出按钮槽函数
void mychat::on_exitBtn_clicked()
{
    accept();
    close();
}

//设置字体
void mychat::on_textFont_currentFontChanged(const QFont &f)
{
    ui->textInput->setCurrentFont(f);
    ui->textInput->setFocus();
}

//设置文本大小
void mychat::on_textSize_currentIndexChanged(const QString &arg1)
{
    ui->textInput->setFontPointSize(arg1.toDouble());
    ui->textInput->setFocus();
}

//设置文本粗体
void mychat::on_textBold_clicked(bool checked)
{
    if (checked) {
        ui->textInput->setFontWeight(QFont::Bold);
    } else {
        ui->textInput->setFontWeight(QFont::Normal);
    }
    ui->textInput->setFocus();
}


//设置文本下斜体
void mychat::on_textItalic_clicked(bool checked)
{
    ui->textInput->setFontItalic(checked);
    ui->textInput->setFocus();
}

//设置文本下划线
void mychat::on_textUnderline_clicked(bool checked)
{
    ui->textInput->setFontUnderline(checked);
    ui->textInput->setFocus();
}

//设置文本颜色
void mychat::on_textColor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid()){
        ui->textInput->setTextColor(color);
        ui->textInput->setFocus();
    }
}


//清除对话框
void mychat::on_textClear_clicked()
{
    ui->MsgBrowser->clear();
}


//根据文本改变格式
void mychat::curFmtChanged(const QTextCharFormat &fmt)
{
    ui->textFont->setCurrentFont(fmt.font());

    if (fmt.fontPointSize() < 9) {
        ui->textSize->setCurrentIndex(4);
    } else {
        ui->textSize->setCurrentIndex(ui->textSize->findText(QString::number(fmt.fontPointSize())));
    }
    ui->textBold->setChecked(fmt.font().bold());
    ui->textItalic->setChecked(fmt.font().italic());
    ui->textUnderline->setChecked(fmt.font().underline());
    color = fmt.foreground().color();
}


