#pragma once

#include "wled.h"
#include <MMA8452.h>

#define SDA_PIN     20
#define SCL_PIN     21

/* -------------------------- ACCELEROMETER CONFIG -------------------------- */
#define MICRO_SAMPLES 20
#define GLOBAL_SAMPLES 7
#define COUNT_TH 3

#define SENSITIVITY 0.095
#define HIT_TH 1.1

#define X_AMP 1.1
#define Y_AMP 1.1
#define Z_AMP 1.2

//This is an empty v2 usermod template. Please see the file usermod_v2_example.h in the EXAMPLE_v2 usermod folder for documentation on the functions you can use!

class MMA8452Usermod : public Usermod {
  private:
    // Private class members. You can declare variables and functions only accessible to your usermod here
    bool enabled = true;
    unsigned long lastTime = 0;
    MMA8452 accelerometer;
    bool is_shaking = false;
    bool is_shaking_flag = false;
    bool led = false;

    // any private methods should go here (non-inline methosd should be defined out of class)
    void handleAccelerometer();
    bool isShaking();
    bool isShakingEvent();
    float getShakingNorm(float x, float y, float z);
    void printXYZ();

  public:

    /**
     * Enable/Disable the usermod
     */
    inline void enable(bool enable) { enabled = enable; }

    /**
     * Get usermod enabled/disabled state
     */
    inline bool isEnabled() { return enabled; }

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * readFromConfig() is called prior to setup()
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      Wire.begin(SDA_PIN, SCL_PIN);
      pinMode(3, OUTPUT);

      // Accelerometer initialization
      if (accelerometer.init())
      {
        accelerometer.setDataRate(MMA_800hz);
        accelerometer.setRange(MMA_RANGE_2G);
        accelerometer.setPowerMode(MMA_HIGH_RESOLUTION);
      }
      else printf("MMA8452 initialization failed.\n");
    }

    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     * 
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() {
      // if usermod is disabled or called during strip updating just exit
      if (!enabled || strip.isUpdating()) return;

      // handleAccelerometer();
      
      if(isShakingEvent()) {
        printf("Shaking Event\n");
      }

      if (millis() - lastTime > 100) {
        led = !led;
        digitalWrite(3, led);
        printXYZ();
        lastTime = millis();
      }
    }
};