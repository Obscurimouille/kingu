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

#define INT_PIN_MASK 0x10

//This is an empty v2 usermod template. Please see the file usermod_v2_example.h in the EXAMPLE_v2 usermod folder for documentation on the functions you can use!

class MMA8452Usermod : public Usermod {
  private:
    // Private class members. You can declare variables and functions only accessible to your usermod here
    unsigned long lastTime = 0;
    unsigned long cpt = 0;
    MMA8452 accelerometer;
    bool is_shaking = false;
    bool is_shaking_flag = false;
    bool led = false;
    bool no_color_mode = false;

    // any private methods should go here (non-inline methosd should be defined out of class)
    void handleAccelerometer();
    bool isShaking();
    bool isShakingEvent();
    float getShakingNorm(float x, float y, float z);
    void printXYZ();

    /*
     * Turn off all leds
     */
    void turnOff()
    {
      bri = 0;
      stateUpdated(CALL_MODE_DIRECT_CHANGE);
    }

  public:

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * readFromConfig() is called prior to setup()
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      Wire.begin(SDA_PIN, SCL_PIN);
      pinMode(4, INPUT_PULLUP);
      pinMode(3, OUTPUT);
      digitalWrite(3, HIGH);
      
      esp_sleep_wakeup_cause_t wakeup_reason;
      wakeup_reason = esp_sleep_get_wakeup_cause();
      printf("esp_sleep_get_wakeup_cause(): %d", wakeup_reason);

      // Accelerometer initialization
      if (accelerometer.init())
      {
        accelerometer.setDataRate(MMA_800hz);
        accelerometer.setRange(MMA_RANGE_2G);
        accelerometer.setPowerMode(MMA_HIGH_RESOLUTION);
      }
      else printf("MMA8452 initialization failed.\n");
      lastTime = millis();
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
      if (strip.isUpdating()) return;

      // handleAccelerometer();
      
      if(isShakingEvent()) {
        printf("Shaking Event\n");
      }

      if (millis() - lastTime > 200) {
        printXYZ();
        
        // digitalWrite(3, LOW);
        // led = !led;
        // digitalWrite(3, led);

        if(!digitalRead(9)) {
          printf("Button pressed\n");
          cpt++;
          if(cpt == 5) {
            no_color_mode = true;
          }
          if(cpt == 6) {
            esp_deep_sleep_enable_gpio_wakeup(INT_PIN_MASK, ESP_GPIO_WAKEUP_GPIO_LOW);
            esp_deep_sleep_start(); 
            printf("This should not be printed\n");
          }
        }
        
        lastTime = millis();
      }
    }

    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw()
    {
      if(no_color_mode)
        strip.fill(RGBW32(0,0,0,0)); // set all pixels to black
    }

    /**
     * handleButton() can be used to override default button behaviour. Returning true
     * will prevent button working in a default way.
     * Replicating button.cpp
     */
    bool handleButton(uint8_t b) {
      yield();
      // ignore certain button types as they may have other consequences
      if (buttonType[b] == BTN_TYPE_NONE
       || buttonType[b] == BTN_TYPE_RESERVED
       || buttonType[b] == BTN_TYPE_PIR_SENSOR
       || buttonType[b] == BTN_TYPE_ANALOG
       || buttonType[b] == BTN_TYPE_ANALOG_INVERTED) {
        return false;
      }

      bool handled = false;

      return handled;
    }
};