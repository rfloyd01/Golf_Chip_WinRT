/*
  This file is part of the Arduino_LSM9DS1 library.
  Copyright (c) 2019 Arduino SA. All rights reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Bobby_Arduino_LSM9DS1.h"

#define LSM9DS1_ADDRESS            0x6b

#define LSM9DS1_WHO_AM_I           0x0f
#define LSM9DS1_CTRL_REG1_G        0x10
#define LSM9DS1_CTRL_REG2_G        0x11
#define LSM9DS1_CTRL_REG3_G        0x12
#define LSM9DS1_STATUS_REG         0x17
#define LSM9DS1_OUT_X_G            0x18
#define LSM9DS1_CTRL_REG6_XL       0x20
#define LSM9DS1_CTRL_REG7_XL       0x21
#define LSM9DS1_CTRL_REG8          0x22
#define LSM9DS1_OUT_X_XL           0x28

// magnetometer
#define LSM9DS1_ADDRESS_M          0x1e

#define LSM9DS1_CTRL_REG1_M        0x20
#define LSM9DS1_CTRL_REG2_M        0x21
#define LSM9DS1_CTRL_REG3_M        0x22
#define LSM9DS1_CTRL_REG4_M        0x23
#define LSM9DS1_STATUS_REG_M       0x27
#define LSM9DS1_OUT_X_L_M          0x28

// setting values
#define ODR                        0
#define FULLSCALE_RANGE            1
#define FILTER_SETTINGS            2
#define HPFILTER_FREQ              3
#define LPFILTER_FREQ              4
#define POWER_MODE                 5

// sensor values
#define ACCELEROMETER              0
#define GYROSCOPE                  1
#define MAGNETOMETER               2

LSM9DS1Class::LSM9DS1Class(TwoWire& wire) :
  continuousMode(false), _wire(&wire)
{
  //Reset all settings arrays 
  //18 bytes have been set aside for settings even though only 6 are necessary for
  //each sensor. This is just in case expansion is ever necessary.
  for (int i = 0; i < 18; i++)
  {
    acc_settings[i] = 0x00;
    gyr_settings[i] = 0x00;
    mag_settings[i] = 0x00;
  }
}

LSM9DS1Class::~LSM9DS1Class()
{
}

int LSM9DS1Class::begin()
{
  _wire->begin();

  // reset
  writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG8, 0x05);
  writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG2_M, 0x0c);

  delay(10);

  if (readRegister(LSM9DS1_ADDRESS, LSM9DS1_WHO_AM_I) != 0x68) {
    end();

    return 0;
  }

  if (readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_WHO_AM_I) != 0x3d) {
    end();

    return 0;
  }

  //Gyroscope set up
  writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, 0x78); // 119 Hz, 2000 dps, 16 Hz BW

  //Accelerometer set up
  writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, 0x70); // 119 Hz, 4G

  //Magnetometer set up
  writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, 0xb4); // Temperature compensation enable, medium performance, 20 Hz
  writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG2_M, 0x00); // 4 Gauss
  writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG3_M, 0x00); // Continuous conversion mode

  //Get the rest of the default settings which weren't set with the above lines
  getCurrentSettings();

  return 1;
}

void LSM9DS1Class::setContinuousMode() {
  // Enable FIFO (see docs https://www.st.com/resource/en/datasheet/DM00103319.pdf)
  writeRegister(LSM9DS1_ADDRESS, 0x23, 0x02);
  // Set continuous mode
  writeRegister(LSM9DS1_ADDRESS, 0x2E, 0xC0);

  continuousMode = true;
}

void LSM9DS1Class::setOneShotMode() {
  // Disable FIFO (see docs https://www.st.com/resource/en/datasheet/DM00103319.pdf)
  writeRegister(LSM9DS1_ADDRESS, 0x23, 0x00);
  // Disable continuous mode
  writeRegister(LSM9DS1_ADDRESS, 0x2E, 0x00);

  continuousMode = false;
}

void LSM9DS1Class::end()
{
  writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG3_M, 0x03);
  writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, 0x00);
  writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, 0x00);

  _wire->end();
}

uint8_t* LSM9DS1Class::getAccelerometerSettings()
{
  return &acc_settings[0];
}

uint8_t* LSM9DS1Class::getGyroscopeSettings()
{
  return &gyr_settings[0];
}

uint8_t* LSM9DS1Class::getMagnetometerSettings()
{
  return &mag_settings[0];
}

int LSM9DS1Class::readAcceleration(float& x, float& y, float& z)
{
  int16_t data[3];

  if (!readRegisters(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_XL, (uint8_t*)data, sizeof(data))) {
    x = NAN;
    y = NAN;
    z = NAN;

    return 0;
  }

  x = data[0] * 4.0 / 32768.0;
  y = data[1] * 4.0 / 32768.0;
  z = data[2] * 4.0 / 32768.0;

  return 1;
}

int LSM9DS1Class::accelerationAvailable()
{
  if (continuousMode) {
    // Read FIFO_SRC. If any of the rightmost 8 bits have a value, there is data.
    if (readRegister(LSM9DS1_ADDRESS, 0x2F) & 63) {
      return 1;
    }
  } else {
    if (readRegister(LSM9DS1_ADDRESS, LSM9DS1_STATUS_REG) & 0x01) {
      return 1;
    }
  }

  return 0;
}

float LSM9DS1Class::accelerationSampleRate()
{
  return 119.0F;
}

int LSM9DS1Class::readGyroscope(float& x, float& y, float& z)
{
  int16_t data[3];

  if (!readRegisters(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_G, (uint8_t*)data, sizeof(data))) {
    x = NAN;
    y = NAN;
    z = NAN;

    return 0;
  }

  x = data[0] * 2000.0 / 32768.0;
  y = data[1] * 2000.0 / 32768.0;
  z = data[2] * 2000.0 / 32768.0;

  return 1;
}

int LSM9DS1Class::gyroscopeAvailable()
{
  if (readRegister(LSM9DS1_ADDRESS, LSM9DS1_STATUS_REG) & 0x02) {
    return 1;
  }

  return 0;
}

float LSM9DS1Class::gyroscopeSampleRate()
{
  return 119.0F;
}

int LSM9DS1Class::readMagneticField(float& x, float& y, float& z)
{
  int16_t data[3];

  if (!readRegisters(LSM9DS1_ADDRESS_M, LSM9DS1_OUT_X_L_M, (uint8_t*)data, sizeof(data))) {
    x = NAN;
    y = NAN;
    z = NAN;

    return 0;
  }

  x = data[0] * 4.0 * 100.0 / 32768.0;
  y = data[1] * 4.0 * 100.0 / 32768.0;
  z = data[2] * 4.0 * 100.0 / 32768.0;

  return 1;
}

int LSM9DS1Class::magneticFieldAvailable()
{
  if (readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_STATUS_REG_M) & 0x08) {
    return 1;
  }

  return 0;
}

float LSM9DS1Class::magneticFieldSampleRate()
{
  return 20.0;
}

bool LSM9DS1Class::readRawData(int16_t *data)
{
  readRegisters(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_XL, (uint8_t*)data, sizeof(int16_t) * 3);
  readRegisters(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_G, (uint8_t*)(data + 3), sizeof(int16_t) * 3);
  readRegisters(LSM9DS1_ADDRESS_M, LSM9DS1_OUT_X_L_M, (uint8_t*)(data + 6), sizeof(int16_t) * 3);
}

int LSM9DS1Class::readRegister(uint8_t slaveAddress, uint8_t address)
{
  _wire->beginTransmission(slaveAddress);
  _wire->write(address);
  if (_wire->endTransmission() != 0) {
    return -1;
  }

  if (_wire->requestFrom(slaveAddress, 1) != 1) {
    return -1;
  }

  return _wire->read();
}

int LSM9DS1Class::readRegisters(uint8_t slaveAddress, uint8_t address, uint8_t* data, size_t length)
{
  _wire->beginTransmission(slaveAddress);
  _wire->write(0x80 | address);
  if (_wire->endTransmission(false) != 0) {
    return -1;
  }

  if (_wire->requestFrom(slaveAddress, length) != length) {
    return 0;
  }

  for (size_t i = 0; i < length; i++) {
    *data++ = _wire->read();
  }

  return 1;
}

void LSM9DS1Class::getCurrentSettings()
{
  //Reads all registers that are tied to the settings arrays and populates
  //the arrays with current sensor information
  int registerValue = 0;

  //Gyroscope Information
  registerValue = readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G);
  gyr_settings[ODR] = (registerValue >> 5);
  gyr_settings[FULLSCALE_RANGE] = ((registerValue & 0x18) >> 3);
  gyr_settings[LPFILTER_FREQ] = (registerValue & 0x03);

  registerValue = readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG2_G);
  gyr_settings[FILTER_SETTINGS] &= 0xFC;
  gyr_settings[FILTER_SETTINGS] |= (registerValue & 0x03);

  registerValue = readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG3_G);
  gyr_settings[POWER_MODE] = ((registerValue & 0x80) >> 7);
  gyr_settings[FILTER_SETTINGS] &= 0x03;
  gyr_settings[FILTER_SETTINGS] |= ((registerValue & 0x40) >> 4);
  gyr_settings[HPFILTER_FREQ] = (registerValue & 0x0F);

  //Accelerometer Information
  if (gyr_settings[ODR] != 0x00) acc_settings[POWER_MODE] = 0x01; //Acc + Gyro mode active

  registerValue = readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL);
  acc_settings[ODR] = (registerValue >> 5);
  acc_settings[FULLSCALE_RANGE] = ((registerValue & 0x18) >> 3);
  acc_settings[FILTER_SETTINGS] &= 0x03;
  acc_settings[FILTER_SETTINGS] |= (registerValue & 0x04);
  acc_settings[LPFILTER_FREQ] = (registerValue & 0x03);
  
  registerValue = readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG7_XL);
  acc_settings[FILTER_SETTINGS] &= 0x05;
  acc_settings[FILTER_SETTINGS] |= ((registerValue & 0x80) >> 6);
  acc_settings[HPFILTER_FREQ] = ((registerValue & 0x60) >> 5);
  acc_settings[FILTER_SETTINGS] &= 0x06;
  acc_settings[FILTER_SETTINGS] |= ((registerValue & 0x04) >> 2);

  //Magnetometer Information
  registerValue = readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M);
  mag_settings[POWER_MODE] &= 0xCF;
  mag_settings[POWER_MODE] |= ((registerValue & 0x60) >> 1);
  mag_settings[ODR] = ((registerValue & 0x1E) >> 1);

  registerValue = readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG2_M);
  mag_settings[FULLSCALE_RANGE] = ((registerValue & 0x60) >> 5);
  
  registerValue = readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG3_M);
  mag_settings[POWER_MODE] &= 0xF3;
  mag_settings[POWER_MODE] |= ((registerValue & 0x03) << 2); 

  registerValue = readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG4_M);
  mag_settings[POWER_MODE] &= 0xFC;
  mag_settings[POWER_MODE] |= ((registerValue & 0x0C) << 2);
}

int LSM9DS1Class::writeRegister(uint8_t slaveAddress, uint8_t address, uint8_t value)
{
  _wire->beginTransmission(slaveAddress);
  _wire->write(address);
  _wire->write(value);
  if (_wire->endTransmission() != 0) {
    //an error occured during transmission
    return 0;
  }

  //after successful transmission to the LSM9DS1, update the local settings array
  //This settings array allows us to show current settings to any Central devices
  //that connect, as well as give the central devices the ability to change settings.

  //need separate switch statements for each slave device as some addresses overlap

  //TODO: Delete all of the sensor_settings references at some point
  if (slaveAddress == LSM9DS1_ADDRESS)
  {
    switch(address)
    {
      case LSM9DS1_CTRL_REG1_G:
        //sensor_settings[3 * ODR + GYROSCOPE] = (value >> 5);
        gyr_settings[ODR] = (value >> 5);
        //sensor_settings[3 * FULLSCALE_RANGE + GYROSCOPE] = ((value & 0x18) >> 3);
        gyr_settings[FULLSCALE_RANGE] = ((value & 0x18) >> 3);
        //sensor_settings[3 * LPFILTER_FREQ + GYROSCOPE] = (value & 0x03);
        gyr_settings[LPFILTER_FREQ] = (value & 0x03);
        break;
      case LSM9DS1_CTRL_REG2_G:
        //sensor_settings[3 * FILTER_SETTINGS + GYROSCOPE] &= 0xFC; //first, zero out the existing 2 LSB
        gyr_settings[FILTER_SETTINGS] &= 0xFC;
        //sensor_settings[3 * FILTER_SETTINGS + GYROSCOPE] |= (value & 0x03); //set the new 2 LSB
        gyr_settings[FILTER_SETTINGS] |= (value & 0x03);
        break;
      case LSM9DS1_CTRL_REG3_G:
        //sensor_settings[3 * POWER_MODE + GYROSCOPE] = ((value & 0x80) >> 7);
        gyr_settings[POWER_MODE] = ((value & 0x80) >> 7);
        //sensor_settings[3 * FILTER_SETTINGS + GYROSCOPE] &= 0x03; //first, zero out everything but the 2 LSB
        gyr_settings[FILTER_SETTINGS] &= 0x03;
        //sensor_settings[3 * FILTER_SETTINGS + GYROSCOPE] |= ((value & 0x40) >> 4); //then, add the new bit to the 3 LSB position
        gyr_settings[FILTER_SETTINGS] |= ((value & 0x40) >> 4);
        //sensor_settings[3 * HPFILTER_FREQ + GYROSCOPE] = (value & 0x0F);
        gyr_settings[HPFILTER_FREQ] = (value & 0x0F);
        break;
      case LSM9DS1_CTRL_REG6_XL:
        //sensor_settings[3 * ODR + ACCELEROMETER] = (value >> 5);
        acc_settings[ODR] = (value >> 5);
        //sensor_settings[3 * FULLSCALE_RANGE + ACCELEROMETER] = ((value & 0x18) >> 3);
        acc_settings[FULLSCALE_RANGE] = ((value & 0x18) >> 3);
        //sensor_settings[3 * FILTER_SETTINGS + ACCELEROMETER] &= 0x03;//first, zero out the bit that will be changing (by removing everything but 2LSB)
        acc_settings[FILTER_SETTINGS] &= 0x03;
        //sensor_settings[3 * FILTER_SETTINGS + ACCELEROMETER] |= (value & 0x04); //then add the new bit (it happens to be in the correct position already)
        acc_settings[FILTER_SETTINGS] |= (value & 0x04);
        //sensor_settings[3 * LPFILTER_FREQ + ACCELEROMETER] = (value & 0x03); //this setting is for the anti-aliasing filter
        acc_settings[LPFILTER_FREQ] = (value & 0x03);
        break;
      case LSM9DS1_CTRL_REG7_XL:
        //sensor_settings[3 * FILTER_SETTINGS + ACCELEROMETER] &= 0x05;//first, zero out the bit that will be changing
        acc_settings[FILTER_SETTINGS] &= 0x05;
        //sensor_settings[3 * FILTER_SETTINGS + ACCELEROMETER] |= ((value & 0x80) >> 6); //then add the new bit (right shift by 6 to get it to the 2LSB position)
        acc_settings[FILTER_SETTINGS] |= ((value & 0x80) >> 6);
        //sensor_settings[3 * HPFILTER_FREQ + ACCELEROMETER] = ((value & 0x60) >> 5);
        acc_settings[HPFILTER_FREQ] = ((value & 0x60) >> 5);
        //sensor_settings[3 * FILTER_SETTINGS + ACCELEROMETER] &= 0x06;//first, zero out the bit that will be changing
        acc_settings[FILTER_SETTINGS] &= 0x06;
        //sensor_settings[3 * FILTER_SETTINGS + ACCELEROMETER] |= ((value & 0x04) >> 2); //then add the new bit (right shift by 2 to get it to the 1LSB position)
        acc_settings[FILTER_SETTINGS] |= ((value & 0x04) >> 2);
        break;
    }
  }
  else if (slaveAddress == LSM9DS1_ADDRESS_M)
  {
    switch(address)
    {
      case LSM9DS1_CTRL_REG1_M:
        //sensor_settings[3 * POWER_MODE + MAGNETOMETER] &= 0xCF; //first, zero out MSB3 and MSB4
        mag_settings[POWER_MODE] &= 0xCF;
        //sensor_settings[3 * POWER_MODE + MAGNETOMETER] |= ((value & 0x60) >> 1 );//then add the two new bits
        mag_settings[POWER_MODE] |= ((value & 0x60) >> 1);
        //sensor_settings[3 * ODR + MAGNETOMETER] = ((value & 0x1E) >> 1); //this represents both the ODR and if FastODR mode is enabled
        mag_settings[ODR] = ((value & 0x1E) >> 1);
        break;
      case LSM9DS1_CTRL_REG2_M:
        //sensor_settings[3 * FULLSCALE_RANGE + MAGNETOMETER] = ((value & 0x60) >> 5);
        mag_settings[FULLSCALE_RANGE] = ((value & 0x60) >> 5);
        break;
      case LSM9DS1_CTRL_REG3_M:
        //sensor_settings[3 * POWER_MODE + MAGNETOMETER] &= 0xF3; //first, zero out bits LSB3 and LSB4
        mag_settings[POWER_MODE] &= 0xF3;
        //sensor_settings[3 * POWER_MODE + MAGNETOMETER] |= ((value & 0x03) << 2 );//then add the two new bits to the right location
        mag_settings[POWER_MODE] |= ((value & 0x03) << 2); 
        break;
      case LSM9DS1_CTRL_REG4_M:
        //sensor_settings[3 * POWER_MODE + MAGNETOMETER] &= 0xFC; //first, zero out the 2LSB
        mag_settings[POWER_MODE] &= 0xFC;
        //sensor_settings[3 * POWER_MODE + MAGNETOMETER] |= ((value & 0x0C) << 2 );//then add the two new bits to the right location
        mag_settings[POWER_MODE] |= ((value & 0x0C) << 2);
        break;
    }
  }

  //return succesfully from the function
  return 1;
}

#ifdef ARDUINO_ARDUINO_NANO33BLE
LSM9DS1Class IMU(Wire1);
#else
LSM9DS1Class IMU(Wire);
#endif
