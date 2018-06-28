#-------------------------------------------------
#
# Project created by QtCreator 2018-06-06T09:37:19
#
#-------------------------------------------------

QT       -= core gui

TARGET = ProcAgent
TEMPLATE = lib

DEFINES += PROCAGENT_LIBRARY

SOURCES += procagent.cpp

HEADERS += procagent.h\
        procagent_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
