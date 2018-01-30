TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../../lab1_pthread_exercise/mat_mul_pthread.c

HEADERS += \
    ../../../lab1_pthread_exercise/mat_mul_pthread.h

LIBS += -lpthread
