TEMPLATE = app
CONFIG += console c++11
CONFIG += c++11
CONFIG += libc++
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c


INCLUDEPATH += /home/cpu10761-local/Downloads/poco/Foundation/include/
INCLUDEPATH += /usr/include/c++/6/
DEPENDPATH += /home/cpu10761-local/Downloads/poco/lib/Linux/x86_64/

QMAKE_CXXFLAGS += -std=c++11 -std=libc++
