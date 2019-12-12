#include "patientinformation.h"
#include "ui_patientinformation.h"
#include "doctor.h"
#include "info.h"

patientInformation::patientInformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::patientInformation)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    QImage _image;
    _image.load(":/image/patientInformation.png");
    setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(_image.scaled(size(), Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation)));
    setPalette(pal);


    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnExit1->setIcon(QIcon(":/image/exit.png"));

//    QString name, age, gender, weight, height, telephone,  birthdate, guomin, bingshi, bingzheng, suggestion;


    Patient pat;
    Storage stor;
    stor.loadfromtxtPatient(address1.toStdString(), pat);
    ui->textBrowser->setText(pat.getname());
    ui->textBrowser_2->setText(pat.getbirthday());
    ui->textBrowser_3->setText(pat.getphone());
    ui->textBrowser_4->setText(pat.getgender());
    ui->textBrowser_5->setText(QString::number(pat.getage()));
    ui->textBrowser_6->setText(QString::number(pat.getheight(),'g', 4));
    ui->textBrowser_7->setText(QString::number(pat.getweight(),'g',4));
    ui->textBrowser_8->setText(pat.getsymptom());
    ui->textBrowser_9->setText(pat.getguominyaowu());
    ui->textBrowser_10->setText(pat.getbingshi());
    ui->textBrowser_11->setText(pat.getrecommendation()[1]);
    QStringList l = pat.getrecoresult().split("_");
    QString disp;
    for (int i = 0; i<l.size();i++)
    {
        disp+= l[i]+QString("\n");
    }
    ui->textBrowser_12->setText(disp);
}

patientInformation::~patientInformation()
{
    delete ui;
}
void patientInformation::on_btnExit1_clicked()
{
    this->close();
}


void patientInformation::on_btnMin_clicked()
{
    showMinimized();
}

