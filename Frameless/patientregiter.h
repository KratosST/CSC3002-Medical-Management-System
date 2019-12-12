#ifndef PATIENTREGITER_H
#define PATIENTREGITER_H

#include <QDialog>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QWidget>
#include <QTime>
#include <QTimer>

namespace Ui {
class patientRegiter;
}

class patientRegiter : public QDialog
{
    Q_OBJECT

public:
    explicit patientRegiter(QWidget *parent = nullptr);
    ~patientRegiter();
    QString getIP();
    void updatechoose();

    qint16 port;
protected:
    bool eventFilter1(QObject *obj, QEvent *event);
    void mousePressEvent1(QMouseEvent *event);
    void mouseMoveEvent1(QMouseEvent *event);
    void mouseReleaseEvent1(QMouseEvent *event);

private slots:
    void on_btnExit1_clicked();

    void on_btnMin_clicked();

    void on_pushButton_clicked();
    void processPendingDatagrams();
    void recall();

private:
    Ui::patientRegiter *ui;
    bool mDrag1;
    QPoint mDragPos1;
    bool mIsMax1;
    QRect mLocation1;
    QFont mIconFont1;
    QUdpSocket *udpSocket;
    QString ServerIP;
    QTimer *timer;
    qint8 flag;
    QString PatientID;
};

#endif // PATIENTREGITER_H
