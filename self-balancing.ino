#include <AccelStepper.h>
#include "BluetoothSerial.h"
#include <Adafruit_BNO055.h>
#include <Wire.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth not enabled error"
#endif

Adafruit_BNO055 bno = Adafruit_BNO055(55);

float Kp = 70;
float Ki = 0;
float Kd = 10;
float setpoint = 0.0;
float deadband = 0.5;
float integral = 0;
float lastError = 0;
unsigned long lastTime = 0;
float maxS = 5000;
float maxAccel = 200;
float speed;
float pitch = 0;

#define STEP_PIN_L 26 // 16
#define DIR_PIN_L  25 // 14
#define STEP_PIN_R 14
#define DIR_PIN_R  27

BluetoothSerial bt;
char cmd;

AccelStepper leftMotor(AccelStepper::DRIVER, STEP_PIN_L, DIR_PIN_L);
AccelStepper rightMotor(AccelStepper::DRIVER, STEP_PIN_R, DIR_PIN_R);

void setup() {
  Serial.begin(115200);
  if (!bno.begin()) {
    while (1);
  }

  sensors_event_t event;
  bno.getEvent(&event,  Adafruit_BNO055::VECTOR_EULER);
  setpoint = event.orientation.z;
  
  // initializing the bluetooth conneciton with esp32 its name is 'lollypolly' OFFICIALLY
  bt.begin("lollypolly");
  bno.setExtCrystalUse(true);
  lastTime = millis();

  // defining max speed to reduce chances of damage to the robot
  leftMotor.setMaxSpeed(maxS);
  rightMotor.setMaxSpeed(maxS);
  // leftMotor.setAcceleration(maxAccel);
  // rightMotor.setAcceleration(maxAccel);
}

void loop() {
  sensors_event_t event;
  bno.getEvent(&event, Adafruit_BNO055::VECTOR_EULER);

 if (bt.available()) {
  String cmd = bt.readStringUntil('\n'); // use newline terminated command
  cmd.trim();
  int sep = cmd.indexOf(' ');
  if (sep > 0) {
    String key = cmd.substring(0, sep);
    float v = cmd.substring(sep+1).toFloat();
    if      (key.equalsIgnoreCase("KP")) Kp = v;
    else if (key.equalsIgnoreCase("KI")) Ki = v;
    else if (key.equalsIgnoreCase("KD")) Kd = v;
    else if (key.equalsIgnoreCase("DB")) deadband = v;
    else if (key.equalsIgnoreCase("SP")) maxS = v;    
    else if (key.equalsIgnoreCase("AC")) maxAccel = v;    
    else if (key.equalsIgnoreCase("X")) { pitch = event.orientation.x; setpoint = event.orientation.x; }
    else if (key.equalsIgnoreCase("Y")) { pitch = event.orientation.y; setpoint = event.orientation.y; }
    else if (key.equalsIgnoreCase("Z")) { pitch = event.orientation.z; setpoint = event.orientation.z; }
  }
}

  // now pitch already has a value from event
  float pitch = event.orientation.z; // default axis you're balancing on (change to y/z if needed)

  float error = setpoint - pitch;
  float output = 0;

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  if (abs(error) < deadband) {
    error = 0;
    integral = 0;
  } else {
    integral += error * dt;
    //integral = constrain(integral, -1000, 1000);
    float derivative = (error - lastError) / dt;
    output = Kp * error + Ki * integral + Kd * derivative; // can be changed
    lastError = error;
   }

  speed = constrain(output, -maxS, maxS);

  leftMotor.setSpeed(speed);
  // leftMotor.setAcceleration(maxAccel);
  rightMotor.setSpeed(-speed);
  // rightMotor.setAcceleration(maxAccel);

  leftMotor.runSpeed();  
  rightMotor.runSpeed();

  // Serial.print("X: "); Serial.print(event.orientation.x);
  // Serial.print(" | Y: "); Serial.print(event.orientation.y);
  // Serial.print(" | Z: "); Serial.print(event.orientation.z);
  // Serial.print(" | Pitch: "); Serial.print(pitch);
  Serial.print(" | Kp: "); Serial.print(Kp);
  Serial.print(" | Ki: "); Serial.print(Ki);
  Serial.print(" | Kd: "); Serial.print(Kd);
  Serial.print(" | Speed: "); Serial.println(speed);
  // Serial.print(" | Speed: "); Serial.println(speed);  
  
  if (lastTime >= 80000){
    integral=0;
    derivative=0;
    now=millis();
    lastTime=0;
  }
}



// void serial_control() {
//   // to enter through serial monitor the values for speed, acceleration, kp, ki, pd values and alter their global values
// }

// void control(char cmd) {
  
//   switch(cmd) {
//     case 'w':  // Forward
//       leftMotor.setSpeed(-speed_max);
//       rightMotor.setSpeed(speed_max);
//       break;
//     case 's':  // Backward
//       leftMotor.setSpeed(speed_max);
//       rightMotor.setSpeed(-speed_max);
//       break;
//     case 'a':  // Left
//       leftMotor.setSpeed(-speed_min);
//       rightMotor.setSpeed(-speed_max);
//       break;
//     case 'd':  // Right
//       leftMotor.setSpeed(speed_max);
//       rightMotor.setSpeed(speed_min);
//       break;
//     case '0':  // Stop
//       leftMotor.setSpeed(0);
//       rightMotor.setSpeed(0);
//       break;
//     default:
//       Serial.println("default");
//   }
  
//   leftMotor.runSpeed();
//   rightMotor.runSpeed();
//   Serial.print("Command: ");
//   Serial.println(cmd);
// }
