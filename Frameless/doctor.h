
/*群聊客户端
 * 1.单人聊天：Tcp(经过Server中转)
 * 2.群聊：UTP-直接广播
 * 3.文件传送：Tcp
 *
 *
 *
 * */
#ifndef DOCTOR_H
#define DOCTOR_H
//#endif
#include <QNetworkInterface>
#include<QUdpSocket>
#include <QWidget>
#include<QTcpSocket>
#include<QTimer>
#include<QTcpServer>
#include<QList>
#include<QFile>
#include<QTime>
#include "chat.h"

extern QString address1;
class mychat;

namespace Ui {
class DoctorUI;
}


class Doctor1 : public QWidget
{
    Q_OBJECT

public:
    explicit Doctor1(QWidget *parent = 0);
    ~Doctor1();
    QString usrName;//用户自己的名字
    QString Path2;
private:
    QString getIP();
    Ui::DoctorUI *ui;

    QUdpSocket *udpSocket;
    qint16 port;
    QTcpSocket  *tClnt;//建立一个TcpSocket
    qint8 flag;
    QTimer *timer;//重播用的定时器
    QString serverIP;//服务器IP地址
    QTcpServer *fileTcpServer;//用于传送文件的服务器
    QTcpSocket *fileSocket;
    QString ServerIP;
private slots:

    void processPendingDatagrams();
    void recall();//定时连接服务器
    void start();
   // void on_allUsrTable_cellDoubleClicked(int row, int column);
    void on_exitBtn_clicked();

    //传输文件相关函数
    void acceptConnection();
    void updateServerProcess();
    void displayError(QAbstractSocket::SocketError socketError);
    void on_FileSendBtn_clicked();

    void on_pushButton_clicked();
    void on_btnExit1_clicked();
    void on_allUsrTable_cellDoubleClicked(int row, int column);
    void on_btnMin_clicked();
    void comeMsg();


    void on_allUsrTable_2_cellClicked(int row, int column);

private:
    //文件参数
    QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;
    qint64 totalBytes;//总数大小
    qint64 bytesReceived;//收到的数据大小
    qint64 fileNameSize;//文件大小
    QString fileName;//文件名字
    QFile *localFile;//本地文件
    QByteArray inBlock;//缓冲区
    QList<mychat *> dialogChat;

};

#endif // WIDGET_H
