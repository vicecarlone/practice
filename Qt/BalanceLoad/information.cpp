#include "information.h"


void balanceDevices(config_information_t config,device_information_t * list_device, input_information_t input,int num_device)
{

    if ( list_device == NULL) {
        std::cout << "empty list" <<std::endl;
        return;
    }



    //find the minimum remaining time in cpu and gpu
    int id_gpu = 0, id_cpu = 0, re_cpu = -1, re_gpu = -1;
    for (int i = 0; i < num_device; i++){
        if (list_device[i].gpu){
            if(re_gpu < 0)
                re_gpu = list_device[i].remaining_time;
            if (list_device[i].remaining_time < re_gpu){
                re_gpu = list_device[i].remaining_time;
                id_gpu = i;
            }
        }
        else {
            if(re_cpu < 0)
                re_cpu = list_device[i].remaining_time;
            if (list_device[i].remaining_time < re_cpu){
                re_cpu = list_device[i].remaining_time;
                id_cpu = i;
            }
        }
    }


    //compare excution times
    if (config.CPU[input.input][input.output] >= 0 && config.GPU[input.input][input.output] >= 0){
        int64_t exe_gpu = input.duration*input.fps_input/config.GPU[input.input][input.output];
        int64_t exe_cpu = input.duration*input.fps_input/config.CPU[input.input][input.output];

        if (re_gpu + exe_gpu > re_cpu + exe_cpu){
            list_device[id_cpu].remaining_time += exe_cpu;
        }
        else
            list_device[id_gpu].remaining_time += exe_gpu;
    }

    else if (config.CPU[input.input][input.output] < 0  && config.GPU[input.input][input.output] >= 0){
        int64_t exe_gpu = input.duration*input.fps_input/config.GPU[input.input][input.output];
        list_device[id_gpu].remaining_time += exe_gpu;
    }

    else if (config.CPU[input.input][input.output] >= 0  && config.GPU[input.input][input.output] < 0){
        int64_t exe_cpu = input.duration*input.fps_input/config.CPU[input.input][input.output];
        list_device[id_cpu].remaining_time += exe_cpu;
    }

    else {
        std::cout <<"there is no transformation";
        return;
    }
}
