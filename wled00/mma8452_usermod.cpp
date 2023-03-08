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