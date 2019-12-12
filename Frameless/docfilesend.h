#ifndef FILESEND_H
#define FILESEND_H
#include <QMainWindow>
#include <QAbstractSocket>
#include <QTime>
class QTcpSocket;
class QFile;
namespace Ui
{
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client();
    ~Client();

    QString docName;
    QString docIP;
    void openFile();
    QString Path;
    int setPort;
private:

      QTcpSocket *tcpClient;
      QFile *localFile;//要发送的文件
      qint64 totalBytes;//发送数据的总大小
      qint64 bytesWritten;//已经发送数据大小
      qint64 bytesToWrite;//剩余数据大小
      qint64 payloadSize;//每次发送数据大小
      QString fileName;//保存文件路径
      QByteArray outBlock;//数据缓冲区，即存放每次要发送的数据块


  private slots:

      void send();
      void startTransfer();
      void updateClientProgress(qint64);
      void displayError(QAbstractSocket::SocketError);

};


#endif // FILESEND_H
