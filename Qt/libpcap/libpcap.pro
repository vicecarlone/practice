TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    utils.cpp

LIBS += -lpcap -lpthread

HEADERS += \
    sniffer.h \
    utils.h
