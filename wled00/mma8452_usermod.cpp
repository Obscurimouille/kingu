#include "mma8452_usermod.h"

void MMA8452Usermod::printXYZ()
{
  float x, y, z;
  accelerometer.getAcceleration(&x, &y, &z);
  printf("%.2f\t%.2f\t%.2f\n", x, y, z);
}

void MMA8452Usermod::handleAccelerometer()
{
  static uint16_t cpt = 0;
  float x, y, z, avarage_norm = 0, norm = 0;

  for (uint8_t i = 0; i < GLOBAL_SAMPLES; i++)
  {
    for (uint8_t j = 0; j < MICRO_SAMPLES; j++)
    {
      accelerometer.getAcceleration(&x, &y, &z);
      norm += getShakingNorm(x, y, z);
      delay(1);
    }
    norm /= MICRO_SAMPLES;
    avarage_norm += norm;
  }

  avarage_norm /= GLOBAL_SAMPLES;

  // if(avarage_norm <= 3.0) printf("z:%.2f\n", avarage_norm);

  if (avarage_norm > SENSITIVITY && avarage_norm < HIT_TH)
  {
    if (++cpt >= COUNT_TH && !is_shaking)
    {
      is_shaking = true;
      is_shaking_flag = true;
    }
  }
  else
  {
    cpt = 0;
    is_shaking = false;
    is_shaking_flag = false;
  }
}

bool MMA8452Usermod::isShaking()
{
  return is_shaking;
}

bool MMA8452Usermod::isShakingEvent()
{
  bool res = is_shaking_flag;
  is_shaking_flag = false;
  return res;
}

float MMA8452Usermod::getShakingNorm(float x, float y, float z)
{
  static float last_x = 0, last_y = 0, last_z = 0;

  z -= 1.0; // Remove gravity

  float delta_x = X_AMP * (x - last_x), delta_y = Y_AMP * (y - last_y), delta_z = Z_AMP * (z - last_z);
  float norm = 10 * sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);

  last_x = x, last_y = y, last_z = z;

  return norm * norm;
}

void MMA8452Usermod::indicatorHandler()
{
  // Start new indicator
  if(indicatorFlag)
  {
    indicatorActivationTime = millis();
    lastPreset = currentPreset; // 0 if no selection
    applyPreset(indicator->getPreset());
    indicatorFlag = false;
    runningIndicator = true;
  }

  // Stop indicator
  if(runningIndicator) {
    // At the end of the indicator duration
    if((indicatorActivationTime + indicator->getDuration()) <= millis())
    {
      applyPreset(lastPreset);
      runningIndicator = false;
      delete indicator;
    }
  }
}

void MMA8452Usermod::stateMachineHandler() {

  switch (state)
  {
    case SLEEP_STATE: {
      // Reveil par USB -> CHARGING_STATE
      // reveil par shake -> BATTERY_STATE
      break;
    }
    
    case TRANSIENT_STATE: {
      prevState = state;

      // Plug a usb cable
      if(usb) {
        state = CHARGING_STATE;
        return;
      }
      // Shake to go back to normal state
      if(shake) {
        state = BATTERY_STATE;
        return;
      }
      // Timeout before get to sleep
      if((transientActivationTime + TRANSIENT_DURATION) <= millis()) {
        state = SLEEP_STATE;
        return;
      }
      break;
    }

    case BATTERY_STATE: {
      // Comming from normal state cause usb has been unplugged
      if(prevState == NORMAL_STATE) {
        indicator = new Indicator(USB_UNPLUGGED_INDICATOR_PRESET, 3000);
        indicatorFlag = true;
      }

      prevState = state;

      // Battery is too low
      if(cutoff) {
        state = SLEEP_STATE;
        return;
      }
      // Plug a usb cable
      if(usb) {
        state = NORMAL_STATE;
        return;
      }
      break;
    }

    case CHARGING_STATE: {
      // Comming from battery state so continue on normal state
      if(prevState == BATTERY_STATE) {
        indicator = new Indicator(charged ? USB_PLUGGED_INDICATOR_PRESET : CHARGING_INDICATOR_PRESET, 3000);
        indicatorFlag = true;
        prevState = state;
        state = NORMAL_STATE;
        return;
      }

      // Battery is charged
      if(charged) {
        prevState = state;
        state = OFF_STATE;
        return;
      }

      // Start battery indicator
      if(prevState == TRANSIENT_STATE || prevState == SLEEP_STATE) {
        indicator = new Indicator(CHARGING_INDICATOR_PRESET, 3000);
        indicatorFlag = true;
      }

      prevState = state;

      // Unplug usb cable
      if(!usb) {
        state = TRANSIENT_STATE;
        return;
      }
      // Shake to turn on
      if(shake) {
        state = NORMAL_STATE;
        return;
      }
      break;
    }

    case NORMAL_STATE: {
      prevState = state;
      // Unplug usb cable then run on battery
      if(!usb) {
        state = BATTERY_STATE;
        return;
      }
      break;
    }

    case OFF_STATE: {
      // Start battery indicator if comming from charging state
      if(prevState == CHARGING_STATE) {
        indicator = new Indicator(BATTERY_CHARGED_INDICATOR_PRESET, 3000);
        indicatorFlag = true;
      }

      prevState = state;
      
      // Unplug usb cable
      if(!usb) {
        state = TRANSIENT_STATE;
        return;
      }
      // Shake
      if(shake) {
        state = NORMAL_STATE;
        return;
      }

      break;
    }
  }
}
