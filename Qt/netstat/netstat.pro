TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../../net-tools-master/lib/activate.c \
    ../../../net-tools-master/lib/af.c \
    ../../../net-tools-master/lib/arcnet.c \
    ../../../net-tools-master/lib/ash.c \
    ../../../net-tools-master/lib/ax25.c \
    ../../../net-tools-master/lib/ax25_gr.c \
    ../../../net-tools-master/lib/ddp.c \
    ../../../net-tools-master/lib/ddp_gr.c \
    ../../../net-tools-master/lib/ec_hw.c \
    ../../../net-tools-master/lib/econet.c \
    ../../../net-tools-master/lib/ether.c \
    ../../../net-tools-master/lib/eui64.c \
    ../../../net-tools-master/lib/fddi.c \
    ../../../net-tools-master/lib/frame.c \
    ../../../net-tools-master/lib/getargs.c \
    ../../../net-tools-master/lib/getroute.c \
    ../../../net-tools-master/lib/hdlclapb.c \
    ../../../net-tools-master/lib/hippi.c \
    ../../../net-tools-master/lib/hw.c \
    ../../../net-tools-master/lib/ib.c \
    ../../../net-tools-master/lib/inet.c \
    ../../../net-tools-master/lib/inet6.c \
    ../../../net-tools-master/lib/inet6_gr.c \
    ../../../net-tools-master/lib/inet6_sr.c \
    ../../../net-tools-master/lib/inet_gr.c \
    ../../../net-tools-master/lib/inet_sr.c \
    ../../../net-tools-master/lib/interface.c \
    ../../../net-tools-master/lib/ipx.c \
    ../../../net-tools-master/lib/ipx_gr.c \
    ../../../net-tools-master/lib/ipx_sr.c \
    ../../../net-tools-master/lib/irda.c \
    ../../../net-tools-master/lib/loopback.c \
    ../../../net-tools-master/lib/masq_info.c \
    ../../../net-tools-master/lib/netrom.c \
    ../../../net-tools-master/lib/netrom_gr.c \
    ../../../net-tools-master/lib/netrom_sr.c \
    ../../../net-tools-master/lib/nstrcmp.c \
    ../../../net-tools-master/lib/ppp.c \
    ../../../net-tools-master/lib/ppp_ac.c \
    ../../../net-tools-master/lib/proc.c \
    ../../../net-tools-master/lib/rose.c \
    ../../../net-tools-master/lib/rose_gr.c \
    ../../../net-tools-master/lib/setroute.c \
    ../../../net-tools-master/lib/sit.c \
    ../../../net-tools-master/lib/slip.c \
    ../../../net-tools-master/lib/slip_ac.c \
    ../../../net-tools-master/lib/sockets.c \
    ../../../net-tools-master/lib/strip.c \
    ../../../net-tools-master/lib/tr.c \
    ../../../net-tools-master/lib/tunnel.c \
    ../../../net-tools-master/lib/unix.c \
    ../../../net-tools-master/lib/util-ank.c \
    ../../../net-tools-master/lib/util.c \
    ../../../net-tools-master/lib/x25.c \
    ../../../net-tools-master/lib/x25_gr.c \
    ../../../net-tools-master/lib/x25_sr.c \
    ../../../net-tools-master/arp.c \
    ../../../net-tools-master/hostname.c \
    ../../../net-tools-master/ifconfig.c \
    ../../../net-tools-master/ipmaddr.c \
    ../../../net-tools-master/iptunnel.c \
    ../../../net-tools-master/mii-tool.c \
    ../../../net-tools-master/nameif.c \
    ../../../net-tools-master/netstat.c \
    ../../../net-tools-master/plipconfig.c \
    ../../../net-tools-master/rarp.c \
    ../../../net-tools-master/route.c \
    ../../../net-tools-master/slattach.c \
    ../../../net-tools-master/statistics.c

HEADERS += \
    ../../../net-tools-master/include/interface.h \
    ../../../net-tools-master/include/ipx.h \
    ../../../net-tools-master/include/sockets.h \
    ../../../net-tools-master/include/util-ank.h \
    ../../../net-tools-master/lib/net-features.h \
    ../../../net-tools-master/lib/net-support.h \
    ../../../net-tools-master/lib/pathnames.h \
    ../../../net-tools-master/lib/proc.h \
    ../../../net-tools-master/lib/util.h \
    ../../../net-tools-master/intl.h \
    ../../../net-tools-master/config.h \
    ../../../net-tools-master/version.h
