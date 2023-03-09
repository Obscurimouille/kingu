#include "indicator.h"

Indicator::Indicator(uint8_t preset, uint32_t duration)
{
    this->preset = preset;
    this->duration = duration;
}

void Indicator::setPreset(uint8_t preset) { this->preset = preset; }

uint8_t Indicator::getPreset() { return this->preset; }

void Indicator::setDuration(uint32_t duration) { this->duration = duration; }

uint32_t Indicator::getDuration() { return this->duration; }