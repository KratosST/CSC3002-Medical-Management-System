#-------------------------------------------------
#
# Project created by QtCreator 2017-09-03T20:04:13
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Frameless
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    chat.cpp \
        mainwindow.cpp \
    patient.cpp \
    patientChat.cpp \
    patientregiter.cpp \
    reserve.cpp \
    worktime.cpp \
    docfilesend.cpp \
    doctor.cpp \
    doctorregister.cpp \
    info.cpp \
    Cutmain.cpp \
    Jieba.cpp \
    Base.cpp \
    check.cpp \
    patientinformation.cpp

HEADERS  += mainwindow.h \
    chat.h \
    patient.h \
    patientChat.h \
    patientregiter.h \
    reserve.h \
    worktime.h \
    docfilesend.h \
    doctor.h \
    info.h \
    doctorregister.h \
    HMMModel.hpp \
    QuerySegment.hpp \
    MixSegment.hpp \
    Unicode.hpp \
    check.h \
    patientinformation.h

FORMS    += mainwindow.ui \
    doctor.ui \
    mychat.ui \
    patientregiter.ui \
    doctorregister.ui \
    chat.ui \
    reserve.ui \
    worktime.ui \
    patient.ui \
    check.ui \
    patientinformation.ui



RC_FILE = app.rc

DISTFILES += \
    disease.csv \
    department.csv

RESOURCES += \
    rc.qrc \
    rc.qrc
