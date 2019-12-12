#ifndef INFO_H
#define INFO_H
#include<string>
#include <map>
#include <vector>
#include <qstring.h>
#include <QTextStream>
#include <QFile>

#define KESHI 18


/*
* Class: Info
* -------------------------
* This is an abstract class served as the super class for
* Patient and Doctor. It stores some shared information
* of patient and doctors like password or ID. It also provides
* virtual functions.
*/
class Info
{

public:
   Info();
   enum Gender{Male, Female};

   /*
    * Functions: get, put, Tostring
    * Usage: QString getpassword() const,
    * void putname(QString name),
    * QString ageTostring();
    * -------------------------
    * Functions mainly ware divided into three types. get functions
    * are provided for reading the current values. Put functions are
    * used to change the values of object's varibles. Tostring
    * functions are used when the object is written into a txt file
    * for communication between server and client. Notice that these
    * function of identity value is of virtual type, Since for doctor
    * and patient have different types of IDs.
    */
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
   //data that are stored.
   struct basicinfo
   {
       //this is the password for accounts of doctors and patients.
       QString password;
       //this is the name of doctors and patients.
       QString name;
       //age varible store age when it is a patient. it store telephone when it is a doctor.
       int age = 0;
       Gender gen;
   };
   basicinfo* binfo;
};

/*
* Class: Patient
* -------------------------
* This is an class for patient information storage. It is a sub class for Info.
* It content storage and aceess functions to data for patient object.
*/
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

   /*
    * Functions: get, put, Tostring
    * Usage: QString getbirthday() const;
    * void putheight(double h);
    * QString birthdayTostring();
    * -------------------------
    * Functions mainly ware divided into three types.
    */
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

   int getreserveddoctorID() const;
   void putreserveddoctorID(int id);
   QString reserveddoctorIDTostring();

   /*
    * Functions: display
    * Usage: void display(QTextStream &qout)
    * -------------------------
    * This function is for output object information to QTextStream.
    * It output information in the way it is stored in txt file.
    */
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
       qout<<recoresultTostring();
       qout<<reserveddoctorIDTostring();


   }



private:
   struct patientinfo
   {
       //basic physical and identity information about patients.
       double weight = 0.0;
       double height = 0.0;
       QString birthday;
       QString jiguan;
       QString address;
       QString phone;
       QString bingshi;//病史
       QString guominyaowu;//过敏药物
       QString symptom;
       //output from semetic analysis is stored here.
       QString recommendation[2];//可能的疾病 建议科室
       //This is the assgined number for patient, also serves as the account for patient.
       int binglihao = 0;
       //This is the recommendation result from semetic analysis,
       //which content more concise information for doctor's convenience.
       QString recoresult;

   };
   patientinfo* patinfo;
   //This is the doctor's ID who is reserved by this patient.
   int reserveddoctorID = 0;

};


/*
* Class: Doctor
* -------------------------
* This is an class for Doctor information storage. It is a sub class for Info.
* It content storage and aceess functions to data for Doctor object.
*/
class Doctor:public Info
{
public:
   //This is a enumeration of all departments
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
                    zhuanjiake};//肛肠科
   Doctor();
   ~Doctor()
   {
       doinfo->schedule.~map();
       delete doinfo;
       delete binfo;
   }
   /*
    * Functions: get, put, Tostring
    * Usage: QString getprofessionaltitle() const;
    * void putdepartment(QString dep);
    * QString professionaltitleTostring();
    * -------------------------
    * Functions mainly ware divided into three types.
    */
   QString getprofessionaltitle() const;
   Department getdepartment() const;
   QString getdescription() const;
   std::map<QString, int> getschedule() const;
   int getbianhao() const;
   QString getstrength() const;


   void putprofessionaltitle(QString ptitle);
   void putdepartment(QString dep);
   void putdescription(QString des);
   void putschedule(std::map<QString, int> liangle);
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

   /*
    * Functions: display
    * Usage: void display(QTextStream &qout)
    * -------------------------
    * This function is for output object information to QTextStream.
    * It output information in the way it is stored in txt file.
    */
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

   /*
    * Functions: DepartmentQstring, QStringtoDepartment
    * Usage: vQString DepartmentQstring(Department dep);
    * Department QStringtoDepartment(QString dep)
    * -------------------------
    * These functions converts the department varible
    * between enumerated type and QString type.
    */
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
       case 18: return QString("专家科");
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
       else return zhuanjiake;
   }



private:

   struct doctorinfo
   {
       //职称
       QString protitle;
       //所在科室
       Department depar;
       //个人简介
       QString descri;
       //专长领域
       QString strength;
       //工作安排
       //This is a mapping between QString represented date and time
       //with respect to patient ID. value is 0 when it is empty. When
       //it was nonzero, value is patients' ID.
       std::map<QString, int> schedule;
       //This is the assigned number for doctors and it is used as login account.
       int bianhao;
   };
   doctorinfo* doinfo;


};


/*
* Class: Storage
* -------------------------
* This is a class due with output stream from Doctor and Patient
* object data to txt file storage. txt file will be used during
* communication between server and clients.
*/
class Storage
{
public:

   /*
    * Functions: storetotxt
    * Usage: void storetotxt(Doctor *doc);
    * void storetotxt(Patient *doc);
    * -------------------------
    * These functions store information in objects into seperate
    * txt file. Each txt store one object's information.
    */
   void storetotxt(Doctor *doc);

   void storetotxt(Patient *doc);

   /*
    * Functions: loadfromtxtDoctor, loadfromtxtPatient
    * Usage: void loadfromtxtDoctor(std::string addre, Doctor & doc);
    * void loadfromtxtPatient(std::string addre, Patient & pat);
    * -------------------------
    * These functions load information in txt file and create an object
    * for corresponding class.
    */
   void loadfromtxtDoctor(std::string addre, Doctor & doc);

   void loadfromtxtPatient(std::string addre, Patient & pat);



};

#endif // INFO_H
