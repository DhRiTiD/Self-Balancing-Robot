#include <AccelStepper.h>

// Define driver type (step + dir)
#define DRIVER AccelStepper::DRIVER


#define STEP_PIN_L 26 // 16
#define DIR_PIN_L  25 // 14
#define STEP_PIN_R 14
#define DIR_PIN_R  27

AccelStepper stepper(DRIVER, STEP_PIN_L, DIR_PIN_L);
AccelStepper stepper2(DRIVER, STEP_PIN_R, DIR_PIN_R);

// Motor specs
const int stepsPerRevolution = 200; // Full steps per revolution (1.8°)

void setup() {
  // Torque is best at lower speeds
  stepper.setMaxSpeed(10000); 
  stepper2.setMaxSpeed(10000);     // limit to ~300 steps/sec (≈90 RPM)
  // stepper.setAcceleration(-200);  // gentle acceleration to avoid skipping
          // steady speed
}

void loop() {
    stepper.setSpeed(800);         // steady speedstepper.setMaxSpeed(300);      // limit to ~300 steps/sec (≈90 RPM)
  // stepper2.setAcceleration(200);  // gentle acceleration to avoid skipping
  stepper2.setSpeed(800);
  stepper.runSpeed(); // run continuously
  stepper2.runSpeed(); // run continuously
}