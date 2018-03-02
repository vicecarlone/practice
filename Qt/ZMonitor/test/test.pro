TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ultis.cpp

HEADERS += \
    ultis.h

LIBS += -lpcap
