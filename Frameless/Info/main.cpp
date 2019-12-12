#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDate>
#include <QFile>
#include "info.h"
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QTextStream qin(stdin);
    QTextStream qout(stdout);

    //The last 2 chars in QString each need a double slash for an accent.
    QString szqLine = QString::fromUtf8("abc áéüóöú őű");

    //I want this text console output to be in red text color.
    qout << "Bellow are some unicode characters: " << endl;

    //The Win XP console does not display the unicode chars correctly!!
    //The cosole does not display unicode chars even though it is capable
    //according to wikipedia.  I just don't know how with Qt.
    //I want this text output in white(or default font color, not red.)
    qout << szqLine << endl;

    qout<<"doctor and patient data initializing"<<endl;

    Patient pat;
    Doctor doc;
    pat.putpassword(QString("lme199723|+~GH44"));
    pat.putname(QString("王小丫"));
    pat.putage(12);
    pat.putgender(QString("女"));
    pat.putweight(12.3);
    pat.putheight(1.23);
    pat.putbirthday(QString("19971212"));
    pat.putjiguan(QString("莫斯科"));
    pat.putaddress(QString("广东省越秀区天河大户王和河步行街193号12栋"));
    pat.putphone(QString("13920374863"));
    pat.putbingshi(QString("食物中毒，有时候会有眩晕"));
    pat.putguominyaowu(QString("阿莫西林"));
    pat.putsymptom(QString("心脏病，我打卡掉了奥。迪女莎科技部科技啊放大镜风味哦IE不能说安静的砂率"));
    pat.putbinglihao(12);
    pat.putrecommendation(QString("病症一 病症2"));
    pat.putrecoresult(QString("新宇生气了，皮增不开心，瑞星去找女朋友，子恒在苦想，佳铭在写报告"));

    doc.putpassword(QString("lme199723|+~GH44"));
    doc.putname(QString("王小丫"));
    doc.putage(12);
    doc.putgender(QString("女"));
    doc.putprofessionaltitle("");
    doc.putdepartment(QString("内科"));
    doc.putdescription(QString("全宇宙最好的医生，善良"));
    std::map<QString, bool> liangle;
//    liangle[QString("thuam12")] = 1;
//    liangle[QString("thupm3")] = 1;
//    liangle[QString("wedpm12")] = 0;
//    liangle[QString("wedam17")] = 1;
//    liangle[QString("friam17")] = 0;
//    liangle[QString("fripm3")] = 1;
//    liangle[QString("tuesam13")] = 0;
//    liangle[QString("tuespm15")] = 1;
//    liangle[QString("thuam19")] = 0;
//    liangle[QString("thuam1")] = 0;
//    liangle[QString("thupm17")] = 1;
//    doc.putschedule(liangle);

    doc.putbianhao(1);
    doc.putstrength("心血管搭桥，复检指导");

    qout<<"data initialization finished"<<endl;
    qout<<"docotor and patient data storing begin"<<endl;
    Storage stor;
    stor.storetotxt(&pat);
    stor.storetotxt(&doc);

    pat.display(qout);
    doc.display(qout);
    qout<<"sotring process finished."<<endl;
    qout<<"reloading process start"<<endl;
    Patient pat1;
    Doctor doc1;
    stor.loadfromtxtDoctor("Doctor_1.txt", doc1);
    stor.loadfromtxtPatient("Patient_12.txt", pat1);

    qout<<"reloading process finished, display result:"<<endl;

    pat1.display(qout);
    doc1.display(qout);

    qout<<"displayed process finished, store the data again and assgin new id"<<endl;
    pat1.putbinglihao(15);
    doc1.putbianhao(15);

    stor.storetotxt(&pat1);
    stor.storetotxt(&doc1);

    qout<<"storing process end."<<endl;




    qout << QString("the end") <<endl;
    return a.exec();
}
