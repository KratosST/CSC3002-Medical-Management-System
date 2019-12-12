#ifndef DOCTORREGISTER_H
#define DOCTORREGISTER_H

#include <QDialog>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QWidget>
#include <QTime>
#include <QTimer>
class QComboBox;
namespace Ui {
class doctorRegister;
}

class doctorRegister : public QDialog
{

    Q_OBJECT

public:
    explicit doctorRegister(QWidget *parent = nullptr);
    ~doctorRegister();
    QString getIP();

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
    bool mDrag1;
    QPoint mDragPos1;
    bool mIsMax1;
    QRect mLocation1;
    QFont mIconFont1;
    Ui::doctorRegister *ui;
    QUdpSocket *udpSocket;
    QString ServerIP;
    QTimer *timer;
    qint8 flag;
    QString PatientID;
};

#endif // DOCTORREGISTER_H
