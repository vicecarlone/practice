TEMPLATE = subdirs

SUBDIRS += \
    SocketMornitoring \
    ProcessMornitoring \
    PacketCapture \
    InterfaceMornitoring \
    TestCcallCxx

CONFIG += ordered
SUBDIRS += Build
