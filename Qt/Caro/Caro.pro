TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../../Gomoku-Five-in-a-row--master/src/Board.cpp \
    ../../../Gomoku-Five-in-a-row--master/src/Computer.cpp \
    ../../../Gomoku-Five-in-a-row--master/src/Game.cpp \
    ../../../Gomoku-Five-in-a-row--master/src/main.cpp

HEADERS += \
    ../../../Gomoku-Five-in-a-row--master/src/Board.h \
    ../../../Gomoku-Five-in-a-row--master/src/Computer.h \
    ../../../Gomoku-Five-in-a-row--master/src/Game.h
