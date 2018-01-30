#ifndef INFORMATION_H
#define INFORMATION_H

#include <cstdint>
#include <iostream>
enum resolution {p240 = 0, p360 = 1, p480 = 2, p720 = 3, p960 = 4, p1080 = 5};


typedef struct config_information{
    //the first element is input resolution
    //the second element is output resolution
    int GPU[10][10];
    int CPU[10][10];
}config_information_t;


typedef struct device_information{
    //true is gpu device, false is cpu
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


void balanceDevices(config_information_t config,device_information_t* device ,input_information_t input,int num_device);




#endif // INFORMATION_H
