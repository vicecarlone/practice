#ifndef INFORMATION_H
#define INFORMATION_H

#include <cstdint>
enum resolution {p240 = 1, p360 = 2, p480 = 3, p720 = 4, p960 = 5, p1080 = 6};

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


void addToQueue(config_information_t,device_information_t*,input_information_t,int);




#endif // INFORMATION_H
