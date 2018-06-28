TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += static

SOURCES += main.cpp \
    testlibrary.cpp

HEADERS +=

unix:!macx: LIBS += -L$$PWD/../../../../Poco/bin/ -lPocoFoundation

INCLUDEPATH += $$PWD/../../../../Poco/include
DEPENDPATH += $$PWD/../../../../Poco/bin

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../Poco/bin/libPocoFoundation.a
