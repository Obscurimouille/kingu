#pragma once

#include "wled.h"

#define LOW_BATTERY_INDICATOR_PRESET 10
#define BATTERY_CHARGED_INDICATOR_PRESET 11
#define USB_PLUGGED_INDICATOR_PRESET 12
#define USB_UNPLUGGED_INDICATOR_PRESET 13
#define CHARGING_INDICATOR_PRESET 14

class Indicator
{
    private:
        uint8_t preset;
        uint32_t duration;
        
    public:
        Indicator(uint8_t preset, uint32_t duration);
        void setPreset(uint8_t preset);
        uint8_t getPreset();
        void setDuration(uint32_t duration);
        uint32_t getDuration();
};