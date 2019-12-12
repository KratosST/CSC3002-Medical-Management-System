#include "info.h"
#include <map>
#include <vector>
#include <qstring.h>
#include <QTextStream>
#include <QFile>


Info::Info()
{}

Info::~Info()
{}

QString Info::getpassword() const
{
    return binfo->password;
}
QString Info::getname() const
{
    return binfo->name;
}


int Info::getage() const
{
    return binfo->age;
}

int Info::getid() const
{
    return 0;
}

QString Info::getgender() const
{
    QString xingbie;
    if (binfo->gen == Male)
    {
        xingbie = "男";
        return xingbie;
    }
        xingbie = "女";
        return xingbie;
}

void Info::putpassword(QString pw)
{
    binfo->password = pw;
}
void Info::putname(QString name)
{
    binfo->name = name;
}

void Info::putage(int age)
{
    binfo->age = age;
}


void Info::putgender(QString xingbie)
{
    if (xingbie == "男")
    {
        binfo->gen = Male;
    }
    else
    {
        binfo->gen = Female;
    }
}

QString Info::passwordTostring()
{
    QString buffer = "password\n";
    buffer+=getpassword()+QString("\n");
    return buffer;
}

QString Info::nameTostring()
{
    QString buffer = "name\n";
    buffer+=getname()+QString("\n");
    return buffer;
}
QString Info::ageTostring()
{
    QString buffer = "age\n";
    buffer+=QString::number(getage())+QString("\n");
    return buffer;
}

QString Info::genderTostring()
{
    QString buffer = "gender\n";
    buffer+= getgender()+QString("\n");
    return buffer;
}

//Patient::Patient(QString name, int age, int id,
//                 QString gen, QString symptom)
//{
//    putname(name);
//    putage(age);
//    putid(id);
//    putgender(gen);
//    putsymptom(symptom);
////    putbinglihao(binglihao);

//}
Patient::Patient()
{
    binfo = new basicinfo;
    patinfo = new patientinfo;
    patinfo->binglihao = 0;
}
double Patient::getweight() const
{
    return patinfo->weight;
}

double Patient::getheight() const
{
    return patinfo->height;
}

QString Patient::getbirthday() const
{
    return patinfo->birthday;
}

QString Patient::getjiguan() const
{
    return patinfo->jiguan;
}

QString Patient::getaddress() const
{
    return patinfo->address;
}

QString Patient::getphone() const
{
    return patinfo->phone;
}

QString Patient::getbingshi() const
{
    return patinfo->bingshi;
}

QString Patient::getguominyaowu() const
{
    return patinfo->guominyaowu;
}

QString Patient::getsymptom() const
{
    return patinfo->symptom;
}


int Patient::getbinglihao() const
{
    return patinfo->binglihao;
}

QString* Patient::getrecommendation() const
{
    return &patinfo->recommendation[0];
}

void Patient::putweight(double w)
{
    patinfo->weight = w;
}

void Patient::putheight(double h)
{
    patinfo->height = h;
}

void Patient::putbirthday(QString bir)
{
    patinfo->birthday = bir;
}
void Patient::putjiguan(QString jg)
{
    patinfo->jiguan = jg;
}
void Patient::putaddress(QString addr)
{
    patinfo->address = addr;
}
void Patient::putphone(QString ph)
{
    patinfo->phone = ph;
}
void Patient::putbingshi(QString bs)
{
    patinfo->bingshi = bs;
}
void Patient::putguominyaowu(QString gmyw)
{
    patinfo->guominyaowu = gmyw;
}

void Patient::putsymptom(QString sym)
{
    patinfo->symptom = sym;
}

void Patient::putbinglihao(int binglihao)
{
    patinfo->binglihao = binglihao;
}

void Patient::putrecommendation(QString recom)
{
    QStringList list1 = recom.split(' ');
    QString store[2];
    patinfo->recommendation[0] = list1[0];
    patinfo->recommendation[1] = list1[1];

}

QString Patient::weightTostring()
{
    QString buffer = "weight\n";
    buffer+= QString::number(getweight(), 'f', 4)+QString("\n");
    return buffer;
}
QString Patient::heightTostring()
{
    QString buffer = "height\n";
    buffer+= QString::number(getheight(), 'f', 4)+QString("\n");
    return buffer;
}
QString Patient::birthdayTostring()
{
    QString buffer = "birthday\n";
    buffer+= getbirthday()+QString("\n");
    return buffer;
}
QString Patient::jiguanTostring()
{
    QString buffer = "jiguan\n";
    buffer+= getjiguan()+QString("\n");
    return buffer;
}
QString Patient::addressTostring()
{
    QString buffer = "address\n";
    buffer+= getaddress()+QString("\n");
    return buffer;
}
QString Patient::phoneTostring()
{
    QString buffer = "phone\n";
    buffer+= getphone()+QString("\n");
    return buffer;
}
QString Patient::bingshiTostring()
{
    QString buffer = "bingshi\n";
    buffer+= getbingshi()+QString("\n");
    return buffer;
}
QString Patient::guominyaowuTostring()
{
    QString buffer = "guominyaowu\n";
    buffer+= getguominyaowu()+QString("\n");
    return buffer;
}
QString Patient::symtomTostring()
{
    QString buffer = "symtom\n";
    buffer+= getsymptom()+QString("\n");
    return buffer;
}

QString Patient::recommendationTostring()
{
    QString buffer = "recommendation\n";
    buffer+= getrecommendation()[0]+QString("\n");
    buffer+= getrecommendation()[1]+QString("\n");
    return buffer;
}

QString Patient::binglihaoTostring()
{
    QString buffer = "binglihao\n";
    buffer+= QString::number(getbinglihao())+QString("\n");
    return buffer;
}

int Patient::getid() const
{
    return patinfo->binglihao;
}

void Patient::putid(int id)
{
    patinfo->binglihao = id;
}
QString Patient::idTostring()
{
    QString buffer = "id\n";
    buffer+= QString::number(getid())+QString("\n");
    return buffer;
}

QString Patient::getrecoresult()const
{
    return patinfo->recoresult;
}

void Patient::putrecoresult(std::vector<QString> vec)
{
    QString str;
    for (uint i = 0; i<vec.size();i++)
    {
        str+=vec[i]+QString("_");
    }
    patinfo->recoresult = str;
}

void Patient::putrecoresult(QString re)
{
    patinfo->recoresult = re;
}


QString Patient::recoresultTostring()
{
    QString buffer = "recoresult\n";
    buffer+= getrecoresult()+QString("\n");
    return buffer;
}

Doctor::Doctor()
{
    binfo = new basicinfo;
    doinfo = new doctorinfo;
    std::map<QString, bool> mymap;
    QString str;
    QString day[] = {QString("Mon"), QString("Tue"), QString("Wed"), QString("Thr"), QString("Fri"), QString("Mon")};
    QString time[] = {QString("9"), QString("10"), QString("11"), QString("2"), QString("3"), QString("4")};
    for (int i = 0;i<5;i++)
    {
        for (int j = 0; j<6; j++)
        {
            str = day[i]+time[j];
            mymap[str] = 0;
        }
    }
    doinfo->schedule = mymap;
}



QString Doctor::getprofessionaltitle() const
{
    return doinfo->protitle;
}

Doctor::Department Doctor::getdepartment() const
{
    return doinfo->depar;
}

QString Doctor::getdescription() const
{
    return doinfo->descri;
}

std::map<QString, bool> Doctor::getschedule() const
{
    return doinfo->schedule;
}

int Doctor::getbianhao() const
{
    return doinfo->bianhao;
}

QString Doctor::getstrength() const
{
    return doinfo->strength;
}

void Doctor::putprofessionaltitle(QString ptitle)
{
    doinfo->protitle = ptitle;
}

void Doctor::putdepartment(QString dep)
{
    doinfo->depar = QStringtoDepartment(dep);
}

void Doctor::putdescription(QString des)
{
    doinfo->descri = des;
}

void Doctor::putschedule(std::map<QString, bool> liangle)
{
    doinfo->schedule = liangle;
}

void Doctor::putbianhao(int bianhao)
{
    doinfo->bianhao = bianhao;
}

void Doctor::putstrength(QString str)
{
    doinfo->strength = str;
}

int Doctor::getid() const
{
    return doinfo->bianhao;
}

void Doctor::putid(int id)
{
    doinfo->bianhao = id;
}

QString Doctor::idTostring()
{
    QString buffer = "id\n";
    buffer+= QString::number(getbianhao())+QString("\n");
    return buffer;
}

QString Doctor::professionaltitleTostring()
{
    QString buffer = "protitle\n";
    buffer+= getprofessionaltitle()+QString("\n");
    return buffer;
}
QString Doctor::departmentTostring()
{
    QString buffer = "department\n";
    buffer+= DepartmentQstring(doinfo->depar)+QString("\n");
    return buffer;
}
QString Doctor::descriptionTostring()
{
    QString buffer = "description\n";
    buffer+= getdescription()+QString("\n");
    return buffer;
}
QString Doctor::scheduleTostring()
{
    QString buffer = "schedule\n";
    std::map<QString, bool> mymap = getschedule();
    for (std::map<QString, bool>::iterator it = mymap.begin(); it!=mymap.end(); it++)
    {
        buffer+=it->first+QString(",")+QString::number(it->second)+QString("\n");
    }
    return buffer;
}

QString Doctor::bianhaoTostring()
{
    QString buffer = "bianhao\n";
    buffer+= QString::number(getbianhao())+QString("\n");
    return buffer;
}
QString Doctor::strengthTostring()
{
    QString buffer = "strength\n";
    buffer+= getstrength()+QString("\n");
    return buffer;
}


void Storage::storetotxt(Doctor *doc)
{
    QFile file;
    QString filename = QString("Doctor_") + QString::number(doc->getbianhao())+QString(".txt");
    file.setFileName(filename);

    bool isOK = file.open(QIODevice::WriteOnly);
    if(true == isOK)
    {
        QTextStream stream(&file);
        //指定编码
        stream.setCodec("UTF-8");
        if (!(doc->getid()==0))
        {
            stream << doc->idTostring();
        }
        if (!(doc->getpassword().isNull()))
        {
            stream << doc->passwordTostring();
        }
        if (!doc->getname().isNull())
        {
            stream << doc->nameTostring();
        }
        if (doc->getage()!= 0)
        {
            stream << doc->ageTostring();
        }
        if (!doc->getgender().isNull())
        {
            stream << doc->genderTostring();

        }
        if (!doc->getprofessionaltitle().isNull())
        {
            stream << doc->professionaltitleTostring();

        }
        if (doc->getdepartment() != Doctor::none)
        {
            stream << doc->departmentTostring();

        }
        if (!doc->getdescription().isNull())
        {
            stream << doc->descriptionTostring();
        }
        if (!doc->getstrength().isNull())
        {
            stream << doc->strengthTostring();
        }
        if (doc->getschedule().size()!= 0)
        {
            stream << doc->scheduleTostring();
        }
        file.close();
    }
}

void Storage::storetotxt(Patient *pat)
{
    QFile file;
    QString filename = QString("Patient_") + QString::number(pat->getbinglihao())+QString(".txt");
    file.setFileName(filename);

    bool isOK = file.open(QIODevice::WriteOnly);
    if(true == isOK)
    {
        QTextStream stream(&file);
        //指定编码
        stream.setCodec("UTF-8");
        if (!(pat->getid()==0))
        {
            stream << pat->idTostring();
        }
        if (!(pat->getpassword().isNull()))
        {
            stream << pat->passwordTostring();
        }
        if (!pat->getname().isNull())
        {
            stream << pat->nameTostring();
        }
        if (pat->getage()!=0)
        {
            stream << pat->ageTostring();
        }
        if (!pat->getgender().isNull())
        {
            stream << pat->genderTostring();

        }
        if (pat->getweight()!=0.0)
        {
            stream << pat->weightTostring();

        }
        if (pat->getheight() != 0.0)
        {
            stream << pat->heightTostring();

        }
        if (!pat->getbirthday().isNull())
        {
            stream << pat->birthdayTostring();
        }
        if (!pat->getjiguan().isNull())
        {
            stream << pat->jiguanTostring();
        }
        if (!pat->getaddress().isNull())
        {
            stream << pat->addressTostring();
        }
        if (!pat->getphone().isNull())
        {
            stream << pat->phoneTostring();
        }
        if (!pat->getbingshi().isNull())
        {
            stream << pat->bingshiTostring();
        }
        if (!pat->getguominyaowu().isNull())
        {
            stream << pat->guominyaowuTostring();
        }
        if (!pat->getsymptom().isNull())
        {
            stream << pat->symtomTostring();
        }
        if ((!(pat->getrecommendation()[0].isNull()))&&(!(pat->getrecommendation()[1].isNull())))
        {
            stream << pat->recommendationTostring();
        }
        if (pat->getbinglihao() != 0)
        {
            stream << pat->binglihaoTostring();
        }
        if (!pat->getrecoresult().isNull())
        {
            stream << pat->recoresultTostring();
        }
        file.close();
    }
}

void Storage::loadfromtxtDoctor(std::string addre, Doctor & doc)
{
    QFile file;
    file.setFileName(QString::fromStdString(addre));
    bool isOK = file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    //指定编码
    stream.setCodec("UTF-8");
    QString str;
    if(true == isOK)
    {
        while(!stream.atEnd())
        {
            str = stream.readLine();
            //translate id
            if (str == QString("id"))
            {
                str = stream.readLine();
                doc.putbianhao(str.toInt());
            }
            //translate password
            else if (str == QString("password"))
            {
                str = stream.readLine();
                doc.putpassword(str);
            }
            //translate name
            else if (str == QString("name"))
            {
                str = stream.readLine();
                doc.putname(str);
            }
            //translate age
            else if (str == QString("age"))
            {
                str = stream.readLine();
                doc.putage(str.toInt());
            }
            //translate gender
            else if (str == QString("gender"))
            {
                str = stream.readLine();
                doc.putgender(str);
            }
            //translate professional title
            else if (str == QString("protitle"))
            {
                str = stream.readLine();
                doc.putprofessionaltitle(str);
            }
            //translate department
            else if (str == QString("department"))
            {
                str = stream.readLine();
                doc.putdepartment(str);
            }
            //translate description
            else if (str == QString("description"))
            {
                str = stream.readLine();
                doc.putdescription(str);
            }
            //translate strength
            else if (str == QString("strength"))
            {
                str = stream.readLine();
                doc.putstrength(str);
            }
            else if (str == QString("schedule"))
            {
                std::map<QString, bool> mymap;
                while (!stream.atEnd())
                {
                    str = stream.readLine();
                    QStringList l = str.split(',');
                    mymap[l[0]] = l[1].toInt();
                }
                doc.putschedule(mymap);
            }
        }
        file.close();
    }
    else doc.putbianhao(0);
}


void Storage::loadfromtxtPatient(std::string addre, Patient & pat)
{
    QFile file;
    file.setFileName(QString::fromStdString(addre));
    bool isOK = file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    //指定编码
    stream.setCodec("UTF-8");
    QString str;
    if(true == isOK)
    {
        while(!stream.atEnd())
        {
            str = stream.readLine();
            //translate id
            if (str == QString("id"))
            {
                str = stream.readLine();
                pat.putbinglihao(str.toInt());
            }
            //translate password
            else if (str == QString("password"))
            {
                str = stream.readLine();
                pat.putpassword(str);
            }
            //translate name
            else if (str == QString("name"))
            {
                str = stream.readLine();
                pat.putname(str);
            }
            //translate age
            else if (str == QString("age"))
            {
                str = stream.readLine();
                pat.putage(str.toInt());
            }
            //translate gender
            else if (str == QString("gender"))
            {
                str = stream.readLine();
                pat.putgender(str);
            }
            //translate weight title
            else if (str == QString("weight"))
            {
                str = stream.readLine();
                pat.putweight(str.toDouble());
            }
            //translate height
            else if (str == QString("height"))
            {
                str = stream.readLine();
                pat.putheight(str.toDouble());
            }
            //translate birthday
            else if (str == QString("birthday"))
            {
                str = stream.readLine();
                pat.putbirthday(str);
            }
            //translate jiguan
            else if (str == QString("jiguan"))
            {
                str = stream.readLine();
                pat.putjiguan(str);
            }
            else if (str == QString("address"))
            {
                str = stream.readLine();
                pat.putaddress(str);
            }
            else if (str == QString("phone"))
            {
                str = stream.readLine();
                pat.putphone(str);
            }
            else if (str == QString("bingshi"))
            {
                str = stream.readLine();
                pat.putbingshi(str);
            }
            else if (str == QString("guominyaowu"))
            {
                str = stream.readLine();
                pat.putguominyaowu(str);
            }
            else if (str == QString("symtom"))
            {
                str = stream.readLine();
                pat.putsymptom(str);
            }
            else if (str == QString("recommendation"))
            {
                str = stream.readLine()+QString(" ");
                str += stream.readLine();
                pat.putrecommendation(str);
            }
            else if (str == QString("binglihao"))
            {
                str = stream.readLine();
                pat.putbinglihao(str.toInt());
            }
            else if (str == QString("recoresult"))
            {
                str = stream.readLine();
                pat.putrecoresult(str);
            }

        }
        file.close();
    }
    else pat.putbinglihao(0);
}




