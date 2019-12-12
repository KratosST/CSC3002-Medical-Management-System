#-------------------------------------------------
#
# Project created by QtCreator 2016-11-01T19:35:05
#
#-------------------------------------------------

QT       += core gui network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chat_server
TEMPLATE = app


SOURCES += main.cpp\
        filesend.cpp \
        info.cpp \
        widget.cpp

HEADERS  += widget.h \
    filesend.h \
    info.h

FORMS    += widget.ui

DISTFILES += \
    server.pro.user
