#include <wiringPi.h>

int main(){
    wiringPiSetupGpio();
    pinMode(19, PWM_OUTPUT);
    pwmSetClock(19);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(20000);
    pwmWrite(19, 1500);
    return 1;
}
