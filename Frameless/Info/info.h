 #ifndef INFO_H
#define INFO_H
#include<string>
#include <map>
#include <vector>
#include <qstring.h>
#include <QTextStream>
#include <QFile>

#define KESHI 18

class Info
{

public:
    Info();
    enum Gender{Male, Female};

    virtual ~Info() = 0;
    QString getpassword() const;
    QString getname() const;
    int getage() const;
    virtual int getid() const = 0;
    QString getgender() const;

    void putpassword(QString pw);
    void putname(QString name);
    void putage(int age);
    virtual void putid(int id) = 0;
    void putgender(QString xingbie);

    QString passwordTostring();
    QString nameTostring();
    QString ageTostring();
    virtual QString idTostring() = 0;
    QString genderTostring();


protected:
    struct basicinfo
    {
        int id;//医生病人登录编号
        QString password;
        QString name;
        int age = 0;
        Gender gen;
    };
    basicinfo* binfo;
};


class Patient:public Info
{
public:
    Patient();
    ~Patient()
    {
        free((void*)(patinfo->recommendation));
        delete patinfo;
        delete binfo;
    }
    double getweight() const;
    double getheight() const;
    QString getbirthday() const;
    QString getjiguan() const;
    QString getaddress() const;
    QString getphone() const;
    QString getbingshi() const;
    QString getguominyaowu() const;
    QString getsymptom() const;
    int getbinglihao() const;
    QString* getrecommendation()const;

    void putweight(double w);
    void putheight(double h);
    void putbirthday(QString bir);
    void putjiguan(QString jg);
    void putaddress(QString addr);
    void putphone(QString ph);
    void putbingshi(QString bs);
    void putguominyaowu(QString gmyw);
    void putsymptom(QString sym);
    void putbinglihao(int binglihao);
    void putrecommendation(QString recom);

    QString weightTostring();
    QString heightTostring();
    QString birthdayTostring();
    QString jiguanTostring();
    QString addressTostring();
    QString phoneTostring();
    QString bingshiTostring();
    QString guominyaowuTostring();
    QString symtomTostring();
    QString binglihaoTostring();
    QString recommendationTostring();


    virtual int getid() const;
    virtual void putid(int id);
    virtual QString idTostring();

    QString getrecoresult() const;
    void putrecoresult(std::vector<QString> vec);
    void putrecoresult(QString re);
    QString recoresultTostring();

    void display(QTextStream &qout)
    {
        qout<<idTostring();
        qout<<passwordTostring();
        qout<<nameTostring();
        qout<<ageTostring();
        qout<<genderTostring();
        qout<<weightTostring();
        qout<<heightTostring();
        qout<<birthdayTostring();
        qout<<jiguanTostring();
        qout<<addressTostring();
        qout<<phoneTostring();
        qout<<bingshiTostring();
        qout<<guominyaowuTostring();
        qout<<symtomTostring();
        qout<<idTostring();
        qout<<recommendationTostring();
        qout<<binglihaoTostring();

    }




private:
    struct patientinfo
    {
        double weight = 0.0;
        double height = 0.0;
        QString birthday;
        QString jiguan;
        QString address;
        QString phone;
        QString bingshi;
        QString guominyaowu;
        QString symptom;
        QString recommendation[2];//可能的疾病 建议科室
        int binglihao = 0;
        QString recoresult;

    };
    patientinfo* patinfo;

};



class Doctor:public Info
{
public:
    enum Department {nei = 0,//内科
                     xinxueguan,//心血管科
                     xiaohua,//消化科
                     huxi,//呼吸科
                     miniao,//泌尿科
                     neifenmi,//内分泌科
                     shenjin,//神经科
                     er,//儿科
                     xinli,//心理科
                     zhongliu,//肿瘤科
                     xueye,//血液科
                     pifu,//皮肤科
                     gu,//骨科
                     koqiang,//口腔科
                     naowai,//脑外科
                     yan,//眼科
                     erbihou,//耳鼻喉科
                     gangchang,
                     none};//肛肠科
    Doctor();
    ~Doctor()
    {
        doinfo->schedule.~map();
        delete doinfo;
        delete binfo;
    }
    Doctor(QString name, int age, QString id, QString gen,
           QString ptitle, QString dep,QString des);


    QString getprofessionaltitle() const;
    Department getdepartment() const;
    QString getdescription() const;
    std::map<QString, bool> getschedule() const;
    int getbianhao() const;
    QString getstrength() const;


    void putprofessionaltitle(QString ptitle);
    void putdepartment(QString dep);
    void putdescription(QString des);
    void putschedule(std::map<QString, bool> liangle);
    void putbianhao(int bianhao);
    void putstrength(QString str);

    virtual int getid() const;
    virtual void putid(int id);
    virtual QString idTostring();

    QString professionaltitleTostring();
    QString departmentTostring();
    QString descriptionTostring();
    QString scheduleTostring();
    QString bianhaoTostring();
    QString strengthTostring();

    void display(QTextStream &qout)
    {
        qout<<idTostring();
        qout<<passwordTostring();
        qout<<nameTostring();
        qout<<ageTostring();
        qout<<genderTostring();
        qout<<passwordTostring();
        qout<<nameTostring();
        qout<<ageTostring();
        qout<<idTostring();
        qout<<genderTostring();
        qout<<professionaltitleTostring();
        qout<<departmentTostring();
        qout<<descriptionTostring();
        qout<<scheduleTostring();
        qout<<bianhaoTostring();
        qout<<strengthTostring();
    }

    QString DepartmentQstring(Department dep)
    {
        switch (dep)
        {
        case 0: return QString("内科");
        case 1: return QString("心血管科");
        case 2: return QString("消化科");
        case 3: return QString("呼吸科");
        case 4: return QString("泌尿科");
        case 5: return QString("内分泌科");
        case 6: return QString("神经科");
        case 7: return QString("儿科");
        case 8: return QString("心理科");
        case 9: return QString("肿瘤科");
        case 10: return QString("血液科");
        case 11: return QString("皮肤科");
        case 12: return QString("骨科");
        case 13: return QString("口腔科");
        case 14: return QString("脑外科");
        case 15: return QString("眼科");
        case 16: return QString("耳鼻喉科");
        case 17: return QString("肛肠科");
        case 18: return QString("none");
        }
    }
    Department QStringtoDepartment(QString dep)
    {
        if (dep == QString("内科")) return nei;
        else if (dep == QString("心血管科")) return xinxueguan;
        else if (dep == QString("消化科")) return xiaohua;
        else if (dep == QString("呼吸科")) return huxi;
        else if (dep == QString("泌尿科")) return miniao;
        else if (dep == QString("内分泌科")) return neifenmi;
        else if (dep == QString("神经科")) return shenjin;
        else if (dep == QString("儿科")) return er;
        else if (dep == QString("心理科")) return xinli;
        else if (dep == QString("肿瘤科")) return zhongliu;
        else if (dep == QString("血液科")) return xueye;
        else if (dep == QString("皮肤科")) return pifu;
        else if (dep == QString("骨科")) return gu;
        else if (dep == QString("口腔科")) return koqiang;
        else if (dep == QString("脑外科")) return naowai;
        else if (dep == QString("眼科")) return yan;
        else if (dep == QString("耳鼻喉科")) return erbihou;
        else if (dep == QString("肛肠科")) return gangchang;
        else return none;
    }



private:

    struct doctorinfo
    {
        QString protitle;
        Department depar;
        QString descri;
        QString strength;
        std::map<QString, bool> schedule;
        int bianhao;
    };
    doctorinfo* doinfo;

    
};

class Storage
{
public:
    void storetotxt(Doctor *doc);

    void storetotxt(Patient *doc);

    void loadfromtxtDoctor(std::string addre, Doctor & doc);

    void loadfromtxtPatient(std::string addre, Patient & pat);

//    void addDoctor(Doctor doc);

//    void addPatient(Patient pat);

//    void removeDoctor(int bianhao);

//    void removePatient(int binglihao);

//    Doctor FinddocBybianhao(int bianhao);

//    Doctor FinddocByName(QString n);

//    Patient FindPatBybinglihao(int blh);

//    Patient FindPatByname(QString n);




private:
    std::vector<Doctor*> doctorstorage[KESHI];
    std::vector<Patient*> patientstorage[10];

};

#endif // INFO_H
