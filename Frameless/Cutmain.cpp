#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QDate>
#include <QFile>
#include <stdlib.h>
#include <stdio.h>
//#include <iostream>
#include <algorithm>
#include <map>
#include "Jieba.cpp"
#include "info.h"

using namespace std;

vector<string> init(void);

vector<string> paths = init();
const char* const DICT_PATH = paths[0].data();
const char* const HMM_PATH = paths[1].data();
const char* const USER_DICT_PATH = paths[2].data();
const char* const STOP_WORD_PATH = paths[3].data();
const char* const DISEASE = paths[4].data();
const char* const DEPARTMENT = paths[5].data();

vector<string> init()
{
    int arg1 = 0;
    char* s="";
    char* arg2[] = {s};
    QCoreApplication a(arg1, arg2);
    const QString runPath = QCoreApplication::applicationDirPath();

    string path = runPath.toStdString();
    path = path.substr(0, path.find_last_of('/'));
    path = path.substr(0, path.find_last_of('/'));
//    cout << path << endl;
//    cout << path.find_last_of('/') << endl;

    string dict_path = path + "/Frameless/dict/jieba.dict.utf8";
    string hmm_path = path + "/Frameless/dict/hmm_model.utf8";
    string user_dict_path = path + "/Frameless/dict/user.dict.utf8";
    string stop_word_path = path + "/Frameless/dict/stop_words.utf8";
    string disease_path = path + "/Frameless/disease.csv";
    string department_path = path + "/Frameless/department.csv";

    vector<string> paths;
    paths.push_back(dict_path);
    paths.push_back(hmm_path);
    paths.push_back(user_dict_path);
    paths.push_back(stop_word_path);
    paths.push_back(disease_path);
    paths.push_back(department_path);

    return paths;
}

string Trim(string& str)
{
    //str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
    str.erase(0,str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    return str;
}

/*
 * symptom match function
 * Usage: if (match(zz, symp)) do
 * ----------------------
 * It decides whether a symptom is contained in a vector of symptoms.
 */

bool match(vector<string> zz, string symptom)
{
    for (int i=0; i<zz.size(); i++) {
        if (symptom == zz[i]) return true;
    }
    return false;
}

/*
 * symptom process function
 * Usage: map<string, vector<string>> result = process(symp);
 * ----------------------
 * It receives a symptom description string and use word segmentation
 * method to extract the keywords inside. The comparison is based on
 * constructed dictionaries that contain a large number of Chinese words
 * and corresponding pre-defined weights.It returns a map with temporal
 * words as keys and vectors containing related symptoms and actions
 * as values.
 */

map<string, vector<string>> process(string symp) {
    _3002jieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH);
    vector<string> words;
    vector<_3002jieba::Word> jiebawords;

    string s;
    string result;

    // 分词+词性
    cout << "[demo] Tagging" << endl;

    s = symp;

    vector<string> sentences;
    vector<string> zz;
//    cout << s << endl;

    map<string, vector<string>> record;

    int last = 0;
    while (s.find("。") != 0 & last != 1)
    {
        cout << s.substr(0, s.find("。")+3) << endl;
        //index = s.find("。")+3;
        sentences.push_back(s.substr(0, s.find("。")+3));

        string time;
        vector<string> events;
        vector<pair<string, string> > tagres;

        string sentence = s.substr(0, s.find("。")+3);
        if (s.find("。") == -1) {
            sentence = s;
            last = 1;
        }
        jieba.Tag(sentence, tagres);

        cout << tagres << endl;

        for (int i=0; i<tagres.size(); i++) {
            if (i > 0 && i < tagres.size()-1 && tagres[i].first == "小时" && tagres[i+1].second == "f" && tagres[i-1].second == "m") {
//                cout << tagres[i-1].first << tagres[i].first << tagres[i+1].first << ": ";
                time = tagres[i-1].first + tagres[i].first + tagres[i+1].first;
            }
            if (tagres[i].second == "t") {
//                cout << tagres[i].first << ": ";
                time = tagres[i].first;
                //record.insert(map<string, vector<string>>::value_type ());
            }
            if (tagres[i].second == "r") {
//                cout << tagres[i].first << " ";
                events.push_back(tagres[i].first);
            }
            if (tagres[i].second == "v" || tagres[i].second == "vg") {
//                cout << tagres[i].first << " ";
                events.push_back(tagres[i].first);
                if (tagres[i].first != Approximation(tagres[i].first)) {
                    zz.push_back(Approximation(tagres[i].first));
                    cout << "zz: " << Approximation(tagres[i].first) << endl;
                }
            }
            if (tagres[i].second == "n") {
//                cout << tagres[i].first << " ";
                events.push_back(tagres[i].first);
            }
            if (tagres[i].second == "zz") {
//                cout << "zz: " << tagres[i].first << endl;
                events.push_back(tagres[i].first);
                zz.push_back(tagres[i].first); // put keyword in the matched symptom vector
            }
        }
        record.insert(map<string, vector<string>>::value_type (time, events));

        if (last == 0) s = s.substr(s.find("。")+3, s.length()-s.find("。")-3);
    }

    cout << endl;

    if (zz.size() == 0) {
        zz.push_back("未知");
    }

    record.insert(map<string, vector<string>>::value_type ("zz", zz));
    return record;
}

vector<QString> writeRecord(string symp)
{
    map<string, vector<string>> record;
    record = process(symp);

    vector<QString> newRecord;
    string symps = "zz: ";
    map<string, vector<string>>::iterator zz = record.find("zz");
    for (int i=0; i<zz->second.size(); i++) {
        symps += zz->second[i];
        symps += " ";
    }
//    cout << symp << endl;
    cout << symps << endl;
//    newRecord.push_back(QString::fromStdString(symp));
    newRecord.push_back(QString::fromStdString(symps));

    string actions;
    map<string, vector<string>>::iterator iter;
    for (iter=record.begin(); iter!=record.end(); iter++) {
        if (iter->first == "zz") continue;
        if (iter->first == "") actions = "未知时间: ";
        else actions = iter->first + ": ";
//        cout << iter->first;
        for (int i=0; i<iter->second.size(); i++) {
//            cout << iter->second[i];
            actions += iter->second[i];
            actions += " ";
        }
        cout << actions << endl;
        newRecord.push_back(QString::fromStdString(actions));
    }

    return newRecord;

}

/*
     * match function
     * Usage: string output = match(symp);
     * ----------------------
     * It receives a symptom description string and calls the process
     * function to analyze it as a group of keywords. Then it assign
     * those keywords to corresponding disease(s) and department(s),
     * and finally decide the most possible result for patients' reference.
*/
QString match(string symp) {

    map<string, vector<string>> record;
    record = process(symp);
    vector<string> zz;

    map<string, vector<string>>::iterator iter;
    for (iter=record.begin(); iter != record.end(); iter++) {
        if (iter->first == "zz") zz = iter->second;
    }

    ifstream fin(DISEASE);
    string line;

    vector<string> diseases;
    vector<string> symptoms;
    vector<int> weights;

    int count = 0;
    getline(fin, line);
    while (count < 2150) {
        getline(fin, line);
        istringstream sin(line);
        vector<string> fields;
        string field;
        while (getline(sin, field, ','))
        {
            fields.push_back(field); //将刚刚读取的字符串添加到向量fields中
        }
//      cout << "wrong here";
        string disease = Trim(fields[0]);
        string symptom = Trim(fields[1]);
        int weight = atoi(Trim(fields[2]).c_str());
        if (match(zz, symptom)) {
            vector<string>::iterator iter = find(diseases.begin(), diseases.end(), disease);
            if (iter != diseases.end()) {
                int index = distance(diseases.begin(), iter);
                weights[index] += weight;
            } else {
                diseases.push_back(disease);
                weights.push_back(weight);
            }
        }
//      cout <<"处理之后的字符串："<< disease << " " << symptom << " " << weight << endl;
        count++;
    }

    int max = 0;
    int in = 0;
    for (int i=0; i<diseases.size(); i++) {
        cout << "Possible diseases: " << diseases[i] << "  Weight: " << weights[i] << endl;
        if (weights[i] > max) {
            in = i;
            max = weights[i];
        }
    }
    string esd = diseases[in];
//    cout << "The disease is: " << esd << endl;

    ifstream fin1(DEPARTMENT);

    string department_name;
    string department_name1;
    while (getline(fin1, line)) {
        vector<string> departments;
        string department;
        istringstream sin1(line);
        while (getline(sin1, department, ','))
        {
            departments.push_back(department); //add the string into this vector
        }
        department_name = Trim(departments[0]);

        for (int i=1; i<departments.size(); i++) {
            if (diseases[in] == Trim(departments[i])) {
                department_name1 = department_name;
//                cout << "The department is: " << department_name << endl;
            }
        }
    }

    string output;
    output = esd + " " + department_name1;
    QString Qoutput = QString::fromStdString(output);
    return Qoutput;

}

