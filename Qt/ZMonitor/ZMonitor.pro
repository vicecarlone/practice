TEMPLATE = subdirs

SUBDIRS += \
    SocketMornitoring \
    ProcessMornitoring \
    PacketCapture \
    InterfaceMornitoring \
    TestCcallCxx \
    test

CONFIG += ordered
SUBDIRS += Build
