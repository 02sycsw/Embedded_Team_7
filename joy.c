#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#define SLAVE_ADDR_01 0x48 

static const char* I2C_DEV = "/dev/i2c-1"; 

int test1() {
    int i2c_fd; 

    int cnt = 0; 
    int preVal0 = 0;
    int curVal0 = 0;
    int preVal1 = 0;
    int curVal1 = 0;
    int preVal2 = 0;
    int curVal2 = 0;
    int Val1 = 0;
    int Val3 = 0; 
     if(wiringPiSetupGpio() < 0 ){
        printf("wiringPiSetupGpio() is failed\n");
        return 1;
    }
    if ((i2c_fd = wiringPiI2CSetupInterface (I2C_DEV, SLAVE_ADDR_01)) < 0 ){
        printf("wiringPi2CSetup Failed: \n");
        return 1;
    }
    printf("I2C start....\n");

    while(1){
        printf("[%d] ", cnt); 
        wiringPiI2CWrite(i2c_fd, 0x40 | 2);
        preVal0= wiringPiI2CRead(i2c_fd);
        curVal0= wiringPiI2CRead(i2c_fd);
        printf("X %d / " , curVal0); 
        wiringPiI2CWrite(i2c_fd, 0x40 | 0);
        preVal1= wiringPiI2CRead(i2c_fd);
        curVal1= wiringPiI2CRead(i2c_fd);
        printf("Y %d / ", curVal1); 
        wiringPiI2CWrite(i2c_fd, 0x40 | 1);
        preVal2= wiringPiI2CRead(i2c_fd);
        curVal2= wiringPiI2CRead(i2c_fd);
        printf("B %d \n" , curVal2);  
        fflush(stdout);

        cnt++;
        delay(100);
    }
    return 0;

}

int main(){
    test1();

}
