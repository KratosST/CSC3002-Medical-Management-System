
/*群聊客户端
 * 1.单人聊天：Tcp(经过Server中转)
 * 2.群聊：UTP-直接广播
 * 3.文件传送：Tcp
 *
 *
 *
 * */
#ifndef PATIENT_H
#define PATIENT_H

#include <QNetworkInterface>
#include<QUdpSocket>
#include <QWidget>
#include<QTcpSocket>
#include<QTimer>
#include<QTcpServer>
#include<QList>
#include<QFile>
#include"patientChat.h"
#include<QTime>


namespace Ui {
class PatientUI;
}


//.消息类型枚举体

class Patient1 : public QWidget
{
    Q_OBJECT

public:
    explicit Patient1(QWidget *parent = 0);
    ~Patient1();
    QString usrName;//用户自己的名字

private slots:
    void on_btnExit1_clicked();
    void on_btnMin_clicked();
    void processPendingDatagrams();
//    void DocprocessPendingDatagrams();
    void recall();//定时连接服务器
    void on_allUsrTable_cellDoubleClicked(int row, int column);


private:
    QString getIP();


private:
    Ui::PatientUI *ui;
private:
//私有成员数据

    QUdpSocket *udpSocket;
    qint16 port;
    QTcpSocket  *tClnt;//建立一个TcpSocket
    qint8 flag;
    QTimer *timer;//重播用的定时器
    QString serverIP;//服务器IP地址
    QTcpServer *fileTcpServer;//用于传送文件的服务器
    QTcpSocket *fileSocket;
    QString ServerIP;
//群聊功能数据
//    QTime time;

private:
//文件接收参数
    bool mDrag1;
    QPoint mDragPos1;
    bool mIsMax1;
    QRect mLocation1;
    QFont mIconFont1;
     qint64 TotalByte;
     qint64 fileNameSize;
     qint64 bytesReceived;
     QString fileName;
     QFile *locFile;
     QByteArray inBlock;

};

#endif // WIDGET_H
