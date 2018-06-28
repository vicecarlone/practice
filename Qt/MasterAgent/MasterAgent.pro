TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    childagent.cpp \
    masteragent.cpp

HEADERS += \
    childagent.h \
    masteragent.h

unix:!macx: LIBS += -L$$PWD/../../../../Poco/bin/ -lPocoFoundation

INCLUDEPATH += $$PWD/../../../../Poco/include
DEPENDPATH += $$PWD/../../../../Poco/bin

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../Poco/bin/libPocoFoundation.a
