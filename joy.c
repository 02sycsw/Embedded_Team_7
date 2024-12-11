#include "func.h"
#define SLAVE_ADDR_01 0x48 
static const char* I2C_DEV = "/dev/i2c-1"; 

int adc_fd;
int joyFlag;
int i2c_fd;

int initJoy(){
     if(wiringPiSetupGpio() < 0 ){
        printf("wiringPiSetupGpio() is failed\n");
        return 1;
    }
    if ((i2c_fd = wiringPiI2CSetupInterface (I2C_DEV, SLAVE_ADDR_01)) < 0 ){
        printf("wiringPi2CSetup Failed: \n");
        return 1;
    }
}

int joystick() {

    int cnt = 0; 
    int preVal0 = 0;
    int curVal0 = 0;
    int preVal1 = 0;
    int curVal1 = 0;
    int preVal2 = 0;
    int curVal2 = 0;
    int Val1 = 0;
    int Val3 = 0; 

    while(1){
        //printf("[%d] ", cnt); 
        wiringPiI2CWrite(i2c_fd, 0x40 | 1);
        preVal0= wiringPiI2CRead(i2c_fd);
        curVal0= wiringPiI2CRead(i2c_fd);
        //printf("X %d / " , curVal0);
        wiringPiI2CWrite(i2c_fd, 0x40 | 0);
        preVal1= wiringPiI2CRead(i2c_fd);
        curVal1= wiringPiI2CRead(i2c_fd);
        //printf("Y %d / \n", curVal1); 
        fflush(stdout);
        if(curVal0 > 220) joyFlag = 85; //U
        else if(curVal0 < 20) joyFlag = 68; //D
        else if (curVal1 > 220) joyFlag = 82; //R
        else if(curVal1 < 20) joyFlag = 76; //L
        else joyFlag = 20; // " "
        cnt++;

        delay(100);
    }

}

void *joystickHandler(void *arg) {
    joystick();
}
/*
int main(){
    joystick();
}
*/
