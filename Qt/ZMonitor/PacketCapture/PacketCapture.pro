TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


SOURCES += \
    packetcap.c

HEADERS += \
    packetcap.h


LIBS += -lpcap
