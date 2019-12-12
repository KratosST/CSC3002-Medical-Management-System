#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QTcpServer>
#include<QUdpSocket>
#include <QMap>
#include "info.h"

class QTcpSocket;
class QFile;
using namespace std;
namespace Ui {
class Widget;
}
enum MsgType{ServerMsg,PatEnter,PatLeft, DocEnter, DocLeft, FileName,Refuse,MsgSend};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void PatupDateMsg();
    void DocupDateMsg();

    void closeEvent(QCloseEvent *event);
private:
        Ui::Widget *ui;
        //私有变量
        int PatNum;//病人总数
        int DocNum;//医生总数
        int DocOnLine;
        int PatOnLine;
        QTcpServer* Pat_Tcpserver;
        QTcpServer* Doc_Tcpserver;
        QTcpSocket  *sTcpSock;
        QUdpSocket * udpSocketPat;
        QUdpSocket * udpSocketDoc;
        QUdpSocket * udpSocketLogin;
        QUdpSocket * udpSocketRegisterPatient;
        QUdpSocket * udpSocketRegisterDoctor;
        int PatPort;
        int DocPort;
        int LoginPort;
        int RegisterPortPatient;
        int RegisterPortDoctor;
        QMap<QString,QString>  PatMap;
        QMap<QString,QString>  DocMap;
        QMap<QString,QString>  relation;
        QMap<QString, Patient*> allPatient;
        QMap<QString, Doctor*> allDoctor;
        QMap<QString, QString> DocRecom;
        QMap<QString, QString> DocDepart;
        QMap<QString, QString> DocName;
        QMap<QString, QString> DocGender;

        QMap<QString,QTcpSocket*> tcpPat;
        QMap<QString,QTcpSocket*> tcpDoc;

        //文件参数
        QTcpServer tcpServer;
        QTcpSocket *tcpServerConnection;
        qint64 totalBytes;//总数大小
        qint64 bytesReceived;//收到的数据大小
        qint64 fileNameSize;//文件大小
        QString fileName;//文件名字
        QFile *localFile;//本地文件
        QByteArray inBlock;//缓冲区
        //医生文件参数
        QTcpServer tcpServerDoc;
        QTcpSocket *tcpServerConnectionDoc;
        qint64 totalBytesDoc;//总数大小
        qint64 bytesReceivedDoc;//收到的数据大小
        qint64 fileNameSizeDoc;//文件大小
        QString fileNameDoc;//文件名字
        QFile *localFileDoc;//本地文件
        QByteArray inBlockDoc;//缓冲区
        QString fileReceiveFromDoc;
        //病人注册界面文件参数
        QTcpServer RegistertcpServer;
        QTcpSocket *RegistertcpServerConnection;
        qint64 RegistertotalBytes;//总数大小
        qint64 RegisterbytesReceived;//收到的数据大小
        qint64 RegisterfileNameSize;//文件大小
        QString RegisterfileName;//文件名字
        QFile *RegisterlocalFile;//本地文件
        QByteArray RegisterinBlock;//缓冲区

        //医生注册界面文件参数
        QTcpServer RegistertcpServerDoc;
        QTcpSocket *RegistertcpServerConnectionDoc;
        qint64 RegistertotalBytesDoc;//总数大小
        qint64 RegisterbytesReceivedDoc;//收到的数据大小
        qint64 RegisterfileNameSizeDoc;//文件大小
        QString RegisterfileNameDoc;//文件名字
        QFile *RegisterlocalFileDoc;//本地文件
        QByteArray RegisterinBlockDoc;//缓冲区
        QString RegisterfileReceiveFromDoc;


        QString getIP();
        QString pat_name, doc_name;
        QString Registerpat_name, Registerdoc_name;

    private slots:
        //传输文件相关函数
        void start();
        void acceptConnection();
        void updateServerProcess();
        void displayError(QAbstractSocket::SocketError socketError);
        //医生文件接收函数
        void startDoc();
        void acceptConnectionDoc();
        void updateServerProcessDoc();
        void displayErrorDoc(QAbstractSocket::SocketError socketError);

        //病人注册界面文件函数
        void Registerstart();
        void RegisteracceptConnection();
        void RegisterupdateServerProcess();
        void RegisterdisplayError(QAbstractSocket::SocketError socketError);
        //医生注册界面文件函数
        void RegisterstartDoc();
        void RegisteracceptConnectionDoc();
        void RegisterupdateServerProcessDoc();
        void RegisterdisplayErrorDoc(QAbstractSocket::SocketError socketError);

        void PatprocessPendingDatagrams();
        void DocprocessPendingDatagrams();
        void LoginConnect();
        void RegisterPatient();
        void RegisterDoctor();
        void PattdpConnectionProcess(); // 病人连接请求处理函数
        void DoctdpConnectionProcess(); // 医生连接请求处理函数
        void PatcomeMsg(void); // 病人消息处理函数
        void DoccomeMsg(void); // 医生消息处理函数



};

#endif // WIDGET_H
