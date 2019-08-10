
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <rf24g.h>

// Pin numbers for the LED and switch
#define SWITCH_PIN 10
#define LED_PIN    11

// Accelerometer and radio objects
MPU6050 mpu6050(Wire);
RF24_G radio;

// Threshold for how hard someone has to 
// flick the wand in order to trigger it
float ACCEL_THRESHOLD = 2.4;

void setup() {
  Serial.begin(9600);
 
  // Initialize the accelerometer
  Wire.begin();
  mpu6050.begin();

  // Tell it we don't need the gyro functionality
  mpu6050.calcGyroOffsets(false);

  // Set up the radio, tell it which pins we're using
  radio = RF24_G(4, 7, 9);

  // Set up the switch and LED pins
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

// Sends whichever wand number the switch is set to
// to the base station
void sendFlick() {
  uint8_t wandNum = 1;
  if (digitalRead(SWITCH_PIN) == LOW) {
    wandNum = 2;
  }
  packet sender;
  sender.setAddress(1);
  sender.addPayload(&wandNum, sizeof(uint8_t));
  if (radio.write(&sender) == true) {
    Serial.println("Sent flick.");
  }
}

// Set the LED to full brightness, then fade out
void doFlash() {
  int brightness = 255;
  while (brightness >= 0) {
    analogWrite(LED_PIN, brightness);
    brightness -= 1;
    delay(2);  
  }
}

void loop() {
  // Update the accelerometer
  mpu6050.update();

  // Calculate the magnitude of the acceleration
  // in the y and z axes
  float accelY = mpu6050.getAccY();
  float accelZ = mpu6050.getAccZ();
  float accel = sqrt((accelY*accelY) + (accelZ*accelZ));

  // If that's above our threshold, send flick and flash the LED
  if (accel >= ACCEL_THRESHOLD) {
    Serial.print("accel: ");Serial.println(accel);
    sendFlick();
    doFlash();
    delay(1000);
  }
}
