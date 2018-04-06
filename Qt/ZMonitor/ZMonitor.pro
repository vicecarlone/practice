TEMPLATE = subdirs

SUBDIRS += \
    SocketMornitoring \
    ProcessMornitoring \
    PacketCapture \
    InterfaceMornitoring \
    TestCcallCxx \
    test \
    test_conn

CONFIG += ordered
SUBDIRS += Build
