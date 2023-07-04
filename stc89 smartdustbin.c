#include<REG51F.h>

sbit irSensor = P1^1;      // IR sensor input pin
sbit servoControl = P2^0;  // Servo motor control pin
sbit trigpin = P1 ^ 2;
sbit echopin = P1 ^ 3;
sbit led1 = P1 ^ 5;

unsigned int distance;
unsigned int isMotorLocked = 0;  // Flag to track if the servo motor is locked

void ddelay_us(unsigned int us) {
    while (us) {
        us--;
    }
}

void ddelay_ms(unsigned int ms) {
    while (ms) {
        unsigned int us = 1000;
        while (us) {
            us--;
        }
        ms--;
    }
}

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

void servoRotate90() {
    unsigned int i;
    for (i = 0; i < 50; i++) {  // Rotate 90 degrees (adjust the value based on calibration)
        servoControl = 1;
        delay_ms(1);            // Pulse width for 1ms (adjust this based on calibration)
        servoControl = 0;
        delay_ms(19);           // Delay to complete the PWM cycle
    }
}

void servoReset() {
    unsigned int i;
    for (i = 0; i < 50; i++) {  // Reset to initial position (0 degrees)
        servoControl = 1;
        delay_ms(2);            // Pulse width for 2ms (adjust this based on calibration)
        servoControl = 0;
        delay_ms(18);           // Delay to complete the PWM cycle
    }
    
    delay_ms(1500);  // Delay for 1.5 seconds before resetting
}

void main() {
    unsigned int isObstacleDetected = 0;  // Flag to track obstacle detection
	
    echopin = 1;
    led1 = 0;
    trigpin = 0;

    delay_ms(200); // Delay for sensor stabilization

    TMOD = 0x01;
    TH0 = 0x00;
    TL0 = 0x00;
	
    while (1) {
        trigpin = 1;
        ddelay_us(10);
        trigpin = 0;

        while (echopin == 0);

        TR0 = 1;
        while (echopin == 1);
        TR0 = 0;

        distance = (TH0 << 8) + TL0;
        distance /= 58; // Conversion factor for distance in centimeters

        TH0 = 0x00;
        TL0 = 0x00;

        if (distance <= 15) {
            led1 = 1;  // Turn on the LED if distance is 2cm or less
            isMotorLocked = 1;  // Lock the servo motor
        } else {
            led1 = 0;  // Turn off the LED if distance is more than 2cm
            isMotorLocked = 0;  // Unlock the servo motor
        }

        ddelay_ms(100);

        if (irSensor && led1 == 0) {
            if (!isObstacleDetected && !isMotorLocked) {
                servoRotate90();  // Activate servo motor if IR sensor detects something, LED is off, and motor is not locked
                isObstacleDetected = 1;
            }
        } else {
            if (isObstacleDetected && led1 == 0) {
                servoReset();  // Reset the servo motor to its initial position if no obstacle is detected and LED is off
                isObstacleDetected = 0;
            }
        }
    }
}
