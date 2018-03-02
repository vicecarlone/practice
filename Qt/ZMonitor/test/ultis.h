#ifndef ULTIS_H
#define ULTIS_H

#include <pcap.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <arpa/inet.h>


class Interfaces
{
public:
    Interfaces();

    pcap_if_t *alldev;

    std::vector<std::string> ethernet;

    void filt_ethernet();
};


class Capturer
{
public:
    Capturer();
    Capturer(std::string);
    void setInterface(std::string);
    void capture();

private:
    std::string interface;
};


#endif // ULTIS_H
