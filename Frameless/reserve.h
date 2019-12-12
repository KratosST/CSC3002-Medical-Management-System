#ifndef RESERVE_H
#define RESERVE_H

#include <QDialog>
#include<QTcpSocket>
#include<QTcpServer>
#include<patientChat.h>
#include<QTime>
#include<QFile>
extern int r,c;

namespace Ui {
class reserve;
}

class reserve : public QDialog
{
    Q_OBJECT

public:
    QString doctor_ip;
    QString doctor_name;
    QString patient_id;
    explicit reserve(QWidget *parent = nullptr);
    ~reserve();

private slots:

    void send();
    void startTransfer();
    void updateClientProgress(qint64);
    void displayError(QAbstractSocket::SocketError);
    void on_tableWidget_cellClicked(int row, int column);

    void on_btnExit1_clicked();

    void on_btnMin_clicked();

    void on_pushButton_clicked();

private:
    Ui::reserve *ui;
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

};

#endif // RESERVE_H
