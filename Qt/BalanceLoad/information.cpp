#include "information.h"


void addToQueue(config_information_t config,device_information_t * list_device, input_information_t input, int num_divce)
{
    //find the minimum remaining time in cpu and gpu
    int id_gpu = 0, id_cpu = 0, re_cpu = 0, re_gpu = 0;
    for (int i = 0; i < num_divce -1; i++){
        if (list_device[i].gpu){
            re_gpu = list_device[i].remaining_time < re_gpu ? list_device[i].remaining_time : re_gpu;
            id_gpu = i;
        }
        else {
            re_cpu = list_device[i].remaining_time < re_cpu ? list_device[i].remaining_time : re_cpu;
            id_cpu = i;
        }
    }

    //compare excution times
    int64_t exe_gpu = input.duration*input.fps_input/config.GPU[input.input][input.output];
    int64_t exe_cpu = input.duration*input.fps_input/config.CPU[input.input][input.output];

    if (re_gpu + exe_gpu > re_cpu + exe_cpu){
        list_device[id_cpu].remaining_time += exe_cpu;
    }
    else
        list_device[id_cpu].remaining_time += exe_gpu;
}
