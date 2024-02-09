#include "servo.h"

GoPlus2 goPlus;

void SetServoToA() {
    goPlus.Servo_write_angle(0,  20);
    goPlus.Servo_write_angle(1 , 20);
    goPlus.Servo_write_angle(2 , 20);
    goPlus.Servo_write_angle(3 , 20);
}

void SetServoToB() {
    goPlus.Servo_write_angle(0,  35);
    goPlus.Servo_write_angle(1 , 35);
    goPlus.Servo_write_angle(2 , 35);
    goPlus.Servo_write_angle(3 , 35);
}

void SetServoToC(){
    goPlus.Servo_write_angle(0,  60);
    goPlus.Servo_write_angle(1 , 60);
    goPlus.Servo_write_angle(2 , 60);
    goPlus.Servo_write_angle(3 , 60);
}

void SetServoByWareHouse(int warehouseRef) {
    if(warehouseRef == 1) {
        SetServoToA();
    }
    if(warehouseRef == 2) {
        SetServoToB();
    }
    if(warehouseRef == 3) {
        SetServoToC();
    }
}
