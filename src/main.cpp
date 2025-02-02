#include "motor.cpp"
#include <Arduino.h>

// TODO: config pins
Motor frontLeftMotor(-1, -1, -1);
Motor frontRightMotor(-1, -1, -1);
Motor backLeftMotor(-1, -1, -1);
Motor backRightMotor(-1, -1, -1);

// TODO: get pwm for servo working
// const int freq = 30000;
// const int pwmChannel = 0;
// const int resolution = 8;
// int dutyCycle = 200;

void setup() {
  Serial.begin(115200);

  Serial.print("Testing DC Motor...");
}

void loop() {
  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  frontLeftMotor.setPower(255);
  frontRightMotor.setPower(255);
  backLeftMotor.setPower(255);
  backRightMotor.setPower(255);
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  frontLeftMotor.setPower(0);
  frontRightMotor.setPower(0);
  backLeftMotor.setPower(0);
  backRightMotor.setPower(0);
  delay(1000);

  // Move DC motor backwards at maximum speed
  Serial.println("Moving Backwards");
  frontLeftMotor.setPower(-255);
  frontRightMotor.setPower(-255);
  backLeftMotor.setPower(-255);
  backRightMotor.setPower(-255);
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  frontLeftMotor.setPower(0);
  frontRightMotor.setPower(0);
  backLeftMotor.setPower(0);
  backRightMotor.setPower(0);
  delay(1000);


}