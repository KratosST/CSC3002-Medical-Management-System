#ifndef CHAT_H
#define CHAT_H
//#endif

#include <QDialog>
#include<QTcpSocket>
#include<QTcpServer>
#include<doctor.h>

#include<QTime>
#include<QFile>
#include <QTextCharFormat>


namespace Ui {
class mychat;
}

class mychat : public QDialog
{
    Q_OBJECT
public:
    explicit mychat(QWidget *parent = 0);
    ~mychat();

    void showMsg();

public:
///////////////////////////////////////////////////////////共有成员变量
    QString chat_ip;
    QString chat_name;
    QString server_ip;
    QString ipself;
    QString nameself;
    QTcpSocket  *tClnt;//Tcp套接字
    QString msg_receive;
    QTcpSocket *fileTcpSocket;

private:
////////////////////////////文件按传输数据
    qint16 totalBytes;
    qint64 bytesReceived;
    qint64 fileNameSize;
    qint64 bytesTowrite;
    qint64 byteWritten;
    qint64 loadSize;//文件每次传送的大小

    QFile *locFile;//文件操作
    QByteArray outBlock;//发送数据缓存区
    QString fileName;//传送的文件名字

    QColor color;


private:
    Ui::mychat *ui;

   // QTcpServer *tServer;//服务器

private slots:
    void on_sendBtn_clicked();
    void on_exitBtn_clicked();
    void on_FileSendBtn_clicked();
    void updateData(qint64);
    void isOK();//是否收到回复信息
    void on_textFont_currentFontChanged(const QFont &f);
    void on_textSize_currentIndexChanged(const QString &arg1);
    void on_textBold_clicked(bool checked);
    void on_textItalic_clicked(bool checked);
    void on_textUnderline_clicked(bool checked);
    void on_textColor_clicked();
    void curFmtChanged(const QTextCharFormat &fmt);
    void on_textClear_clicked();
};


#endif // CHAT_H

















