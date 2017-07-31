#include <iostream>
#include <information.h>


using namespace std;

void init_config(config_information_t &);
void init_device(device_information_t *);

int main(int argc, char *argv[])
{
    config_information_t conf;
    init_config(conf);
    device_information_t device[5];
    init_device(device);
    input_information_t input;
    input.duration = 120;
    input.fps_input = 40;
    input.input = p1080;
    input.output = p720;



    balanceDevices(conf,device,input,5);

    for (int i = 0; i < 5; i++){
        cout << device[i].remaining_time <<" " <<endl;
    }


    return 0;
}

void init_config(config_information_t &conf){
    //conf.CPU[0][0] = 100;
    for (int i = 0; i < 10; i++){
        for (int j = 0; j <= 10; j++){
            conf.CPU[i][j] = -1;
            conf.GPU[i][j] = -1;
        }
    }
    conf.CPU[p240][p240] = 100;
    conf.CPU[p360][p360] = 100;
    conf.CPU[p360][p240] = 80;
    conf.CPU[p480][p480] = 150;
    conf.CPU[p480][p360] = 120;
    conf.CPU[p480][p240] = 90;
    conf.CPU[p720][p720] = 120;
    conf.CPU[p720][p480] = 110;
    conf.CPU[p720][p360] = 100;
    conf.CPU[p720][p240] = 90;
    conf.CPU[p960][p960] = 110;
    conf.CPU[p960][p720] = 100;
    conf.CPU[p960][p480] = 90;
    conf.CPU[p960][p360] = 90;
    conf.CPU[p960][p240] = 100;
    conf.CPU[p1080][p1080] = 100;
    conf.CPU[p1080][p960] = 95;
    conf.CPU[p1080][p720] = 90;
    conf.CPU[p1080][p480] = 85;
    conf.CPU[p1080][p360] = 80;
    conf.CPU[p1080][p240] = 75;


    conf.GPU[p240][p240] = 400;
    conf.GPU[p360][p360] = 400;
    conf.GPU[p360][p240] = 360;
    conf.GPU[p480][p360] = 480;
    conf.GPU[p480][p480] = 600;
    conf.GPU[p480][p240] = 360;
    conf.GPU[p720][p720] = 480;
    conf.GPU[p720][p480] = 440;
    conf.GPU[p720][p360] = 400;
    conf.GPU[p720][p240] = 360;
    conf.GPU[p960][p960] = 440;
    conf.GPU[p960][p720] = 400;
    conf.GPU[p960][p480] = 360;
    conf.GPU[p960][p360] = 360;
    conf.GPU[p960][p240] = 400;
    conf.GPU[p1080][p1080] = 400;
    conf.GPU[p1080][p960] = 380;
    conf.GPU[p1080][p720] = 360;
    conf.GPU[p1080][p480] = 340;
    conf.GPU[p1080][p360] = 320;
    conf.GPU[p1080][p240] = 300;


}

void init_device(device_information_t * device){
    device[0].gpu = false;
    device[0].id = 1;
    device[0].remaining_time = 50;

    device[1].gpu = true;
    device[1].id = 1;
    device[1].remaining_time = 100;

    device[2].gpu = true;
    device[2].id = 2;
    device[2].remaining_time = 50;

    device[3].gpu = true;
    device[3].id = 3;
    device[3].remaining_time = 300;

    device[4].gpu = true;
    device[4].id = 4;
    device[4].remaining_time = 200;
}
