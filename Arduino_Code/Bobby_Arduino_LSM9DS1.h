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

#include <Arduino.h>
#include <Wire.h>

class LSM9DS1Class {
  public:
    LSM9DS1Class(TwoWire& wire);
    virtual ~LSM9DS1Class();

    int begin();
    void end();

    //Setting functions
    uint8_t* getAccelerometerSettings();
    uint8_t* getGyroscopeSettings();
    uint8_t* getMagnetometerSettings();

    // Controls whether a FIFO is continuously filled, or a single reading is stored.
    // Defaults to one-shot.
    void setContinuousMode();
    void setOneShotMode();

    // Accelerometer
    virtual int readAcceleration(float& x, float& y, float& z); // Results are in G (earth gravity).
    virtual int accelerationAvailable(); // Number of samples in the FIFO.
    virtual float accelerationSampleRate(); // Sampling rate of the sensor.

    // Gyroscope
    virtual int readGyroscope(float& x, float& y, float& z); // Results are in degrees/second.
    virtual int gyroscopeAvailable(); // Number of samples in the FIFO.
    virtual float gyroscopeSampleRate(); // Sampling rate of the sensor.

    // Magnetometer
    virtual int readMagneticField(float& x, float& y, float& z); // Results are in uT (micro Tesla).
    virtual int magneticFieldAvailable(); // Number of samples in the FIFO.
    virtual float magneticFieldSampleRate(); // Sampling rate of the sensor.

    //All Data
    bool readRawData(int16_t *data); //used for getting raw binary data for Acc., Gyr. and Mag.
  private:
    bool continuousMode;
    int readRegister(uint8_t slaveAddress, uint8_t address);
    int readRegisters(uint8_t slaveAddress, uint8_t address, uint8_t* data, size_t length);
    int writeRegister(uint8_t slaveAddress, uint8_t address, uint8_t value);
    void getCurrentSettings();

  private:
    TwoWire* _wire;
    //uint8_t sensor_settings[18]; //TODO: Delete at some point
    uint8_t acc_settings[18];
    uint8_t gyr_settings[18];
    uint8_t mag_settings[18];

    /*
     * Some notes on the sensor settings array. The GYROSCOPE_FILTER_SETTINGS variable is a combination of a few different registers. Its 8 bits look
     * like this: [00000ABB] where the A bit represents if the hi-pass filter is enabled and BB bits represent which filters get used. The 
     * ACCELEROMETER_FILTER_SETTINGS variable is also a combination of a few different registers. Its 8 bits look like this: [00000ABC] where the A
     * bit represents if the anti-aliasing filter is set to custom or auto, the B bit represents if the LPF2 is on or off and the C bit represents which
     * filter(s) get utilized for the final output value. The MAGNETOMETER_ODR variable is a combination of both the ODR and whether or not FastODR mode is 
     * enabled. Its 8 bits look like this: [0000AAAB] where the AAA bits represents the ODR and B represents FastODR mode. Finally, the MAGNETOMETER_POWER_MODE
     * variable is a combination of a few registers. Its 8 bits
     * look like this [00AABBCC] where the AA bits represent the power mode for the X and Y axes, the BB bits represent the power mode for the Z axis, and the CC
     * bits represent whether the magnetometer is in continous, single, or off mode.
     */
};

extern LSM9DS1Class IMU;
