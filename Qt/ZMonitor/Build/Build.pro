TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS +=  -L../ProcessMornitoring -lProcessMornitoring -L../PacketCapture -lPacketCapture -L../InterfaceMornitoring -lInterfaceMornitoring

SOURCES += main.c
