#ifndef INFORMATION_H
#define INFORMATION_H

#include <cstdint>
#include <iostream>
enum resolution {p240 = 0, p360 = 1, p480 = 2, p720 = 3, p960 = 4, p1080 = 5};

typedef struct config_information{
    int GPU[10][10];
    int CPU[10][10];
}config_information_t;


typedef struct device_information{
    bool gpu;
    int id;
    int64_t remaining_time;
}device_information_t;

typedef struct input_information{
    resolution input;
    resolution output;
    int fps_input;
    int64_t duration;
}input_information_t;


void balanceDevices(config_information_t,device_information_t*,input_information_t,int);




#endif // INFORMATION_H
