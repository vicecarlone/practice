TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    zsocket.c

HEADERS += \
    zsocket.h

LIBS += -lpcap

QMAKE_CXXFLAGS += -Wincompatible-pointer-types
