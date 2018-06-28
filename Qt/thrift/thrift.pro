TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    client.cpp \
    server.cpp \
    gen-cpp/VersionChecker_constants.cpp \
    gen-cpp/VersionChecker_types.cpp \
    gen-cpp/VersionChecker.cpp

HEADERS += \
    gen-cpp/VersionChecker_constants.h \
    gen-cpp/VersionChecker_types.h \
    gen-cpp/VersionChecker.h

QMAKE_CXXFLAGS += -std=c++0x -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -Wall

INCLUDEPATH += -I/usr/local/include/thrift

LIBS+= -L/usr/local/lib -lthrift
