#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include<QTcpSocket>
#include<QTcpServer>
#include<patientChat.h>
#include<QTime>
#include<QFile>

namespace Ui {
class chat;
}

class patientChat : public QDialog
{
    Q_OBJECT
public:
    explicit patientChat(QWidget *parent = 0);
    ~patientChat();



public:
///////////////////////////////////////////////////////////共有成员变量
    QString doctor_ip;
    QString doctor_name;
    QString patient_id;

private:
////////////////////////////文件按传输数据
    qint64 loadSize;//文件每次传送的大小
    QTcpSocket *tcpClient;
      QFile *localFile;//要发送的文件
      qint64 totalBytes;//发送数据的总大小
      qint64 bytesWritten;//已经发送数据大小
      qint64 bytesToWrite;//剩余数据大小
      qint64 payloadSize;//每次发送数据大小
      QString fileName;//保存文件路径
      QByteArray outBlock;//数据缓冲区，即存放每次要发送的数据块
      QString Path;
      int setPathCount;
      QString ServerIP;




private:
    Ui::chat *ui;

   // QTcpServer *tServer;//服务器

private slots:
    void send();
    void startTransfer();
    void updateClientProgress(qint64);
    void displayError(QAbstractSocket::SocketError);
    void on_exitBtn_clicked();
    void on_FileSendBtn_clicked();
};

#endif // CHAT_H



