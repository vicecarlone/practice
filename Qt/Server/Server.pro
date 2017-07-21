QT += core
QT -= gui

CONFIG += c++11

TARGET = Server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -Wall
INCLUDEPATH += -l/usr/local/include/thrift
LIBS += -L/usr/local/lib -lthrift

SOURCES += \
    ../../thrift/gen-cpp/Calculator.cpp \
    ../../thrift/gen-cpp/shared_constants.cpp \
    ../../thrift/gen-cpp/shared_types.cpp \
    ../../thrift/gen-cpp/SharedService.cpp \
    ../../thrift/gen-cpp/tutorial_constants.cpp \
    ../../thrift/gen-cpp/tutorial_types.cpp \
    ../../thrift/server.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ../../thrift/gen-cpp/Calculator.h \
    ../../thrift/gen-cpp/shared_constants.h \
    ../../thrift/gen-cpp/shared_types.h \
    ../../thrift/gen-cpp/SharedService.h \
    ../../thrift/gen-cpp/tutorial_constants.h \
    ../../thrift/gen-cpp/tutorial_types.h
