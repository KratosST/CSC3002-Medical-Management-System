#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include "doctor.h"
#include "patient.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_btnMin_clicked();
    void on_btnExit_clicked();

    void on_pushButton_2_clicked();
    void connectServer();
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;

    //标题拖动、双击最大化
    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;
    QString ID;
    QString password;
    QUdpSocket *udpSocket;
    QString  ServerIP;
    qint16 port;

    QString getIP();
};

#endif // MAINWINDOW_H
