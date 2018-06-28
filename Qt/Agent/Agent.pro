TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../../apps/server/gen-cpp/Zserver.cpp \
    ../../../apps/server/gen-cpp/zserver_constants.cpp \
    ../../../apps/server/gen-cpp/Zserver_server.cpp \
    ../../../apps/server/gen-cpp/zserver_types.cpp \
    agentmaster.cpp


HEADERS += \
    ../../../apps/server/gen-cpp/tinydir.h \
    ../../../apps/server/gen-cpp/Zserver.h \
    ../../../apps/server/gen-cpp/zserver_constants.h \
    ../../../apps/server/gen-cpp/zserver_types.h \
    agentmaster.h


QMAKE_CXXFLAGS += -std=c++0x -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -Wall

INCLUDEPATH += -I/usr/local/include/thrift

LIBS+= -L/usr/local/lib -lthrift -lPocoJSON -lPocoFoundation -lPocoNet -lPocoUtil
