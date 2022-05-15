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
    uint8_t acc_settings[18]; //See info on each byte below
    uint8_t gyr_settings[18]; //See info on each byte below
    uint8_t mag_settings[18]; //See info on each byte below

    /*
     * ACC Settings array options
     * Byte 0 (ODR Settings): 0x000 (off), 0x001 (10 Hz or 14.9Hz if gryo on), 0x010 (50 Hz or 59.5 Hz if gyro on), 0x011 (119 Hz), 0x100 (238 Hz), 0x101 (476 Hz), 0x110 (952 Hz)
     * Byte 1 (Fullscale Range Settings): 0x00 (+/- 2G), 0x01 (+/- 16G), 0x10 (+/- 4G), 0x11 (+/- 8G)
     * Byte 2 (Filter Settings): 0x000 (Auto mode, no extra filters), 0x001 (Auto mode, HPF on), 0x010 (Auto mode, LPF2 on), 0x100 (Manual mode, no extra filters), 0x101 (Manual mode, HPF on), 0x110 (Manual mode, LPF2 on)
     * Byte 3 (High Pass Filter Frequency): 0x00 (ODR/50), 0x01 (ODR/100), 0x10 (ODR/9), 0x11 (ODR/400)
     * Byte 4 (Low Pass Filter Frequency): 0x00 (408 Hz), 0x01 (211 Hz), 0x10 (105 Hz), 0x11 (50 Hz)
     * Byte 5 (Power/Mode Settings): 0x0 (Accelerometer only), 0x1 (Accelerometer and Gyroscope active)
     * Bytes 6-17 are currently not used and set at 0x00
     */

     /*
     * GYR Settings array options
     * Byte 0 (ODR Settings): 0x000 (off), 0x001 (14.9Hz), 0x010 (59.5 Hz), 0x011 (119 Hz), 0x100 (238 Hz), 0x101 (476 Hz), 0x110 (952 Hz)
     * Byte 1 (Fullscale Range Settings): 0x00 (+/- 245 deg/s), 0x01 (+/- 500 deg/s), 0x11 (+/- 2000 deg/s)
     * Byte 2 (Filter Settings [These are confusing in the documentation and should be verified]): 0x000 (LPF1 Only), 0x010 (LPF1 and LPF2), 0x101 (LPF1 and HPF), 0x111 (LPF1, LPF2 and HPF)
     * Byte 3 (High Pass Filter Frequency): 0x0000 (1Hz/4Hz/8Hz/15Hz/30Hz/57Hz ascending with ODR),
     *                                      0x0001 (.5Hz/2Hz/4Hz/8Hz/15Hz/30Hz ascending with ODR),
     *                                      0x0010 (.2Hz/1Hz/2Hz/4Hz/8Hz/15Hz ascending with ODR),
     *                                      0x0011 (.1Hz/.5Hz/1Hz/2Hz/4Hz/8Hz ascending with ODR),
     *                                      0x0100 (.05Hz/.2Hz/.5Hz/1Hz/2Hz/4Hz ascending with ODR),
     *                                      0x0101 (.02Hz/.1Hz/.2Hz/.5Hz/1Hz/2Hz ascending with ODR),
     *                                      0x0110 (.01Hz/.05Hz/.1Hz/.2Hz/.5Hz/1Hz ascending with ODR),
     *                                      0x0111 (.005Hz/.02Hz/.05Hz/.1Hz/.2Hz/.5Hz ascending with ODR),
     *                                      0x1000 (.002Hz/.01Hz/.02Hz/.05Hz/.1Hz/.2Hz ascending with ODR),
     *                                      0x1001 (.001Hz/.005Hz/.01Hz/.02Hz/.05Hz/.1Hz ascending with ODR)
     * Byte 4 (Low Pass Filter Frequency): 0x00 (0Hz/16Hz/14Hz/14Hz/21Hz/33Hz ascending with ODR),
     *                                     0x01 (0Hz/16Hz/31Hz/29Hz/28Hz/40Hz ascending with ODR),
     *                                     0x10 (0Hz/16Hz/31Hz/63Hz/57Hz/58Hz ascending with ODR),
     *                                     0x11 (0Hz/16Hz/31Hz/78Hz/100Hz/100Hz ascending with ODR)
     * Byte 5 (Power/Mode Settings): 0x0 (Normal Operating Mode), 0x1 (Low Power Mode)
     * Bytes 6-17 are currently not used and set at 0x00
     */

     /*
     * MAG Settings array options
     * Byte 0 (ODR Settings): 0x0000 (0.625 Hz), 0x0001 (1.25 Hz), 0x0010 (2.5 Hz), 0x0011 (5 Hz), 0x0100 (10 Hz), 0x0101 (20 Hz), 0x0110 (40 Hz), 0x0111 (80 Hz), 0x1000 (Fast ODR Mode, not sure what this entails, need to test)
     * Byte 1 (Fullscale Range Settings): 0x00 (+/- 4 Gauss), 0x01 (+/- 8 Gauss), 0x10 (+/- 12 Gauss), 0x11 (+/- 16 Gauss)
     * Byte 2 (Filter Settings): 0x0 (Currently no filter settings to adjust)
     * Byte 3 (High Pass Filter Frequency): 0x0 (Currently no filter settings to adjust)
     * Byte 4 (Low Pass Filter Frequency): 0x0 (Currently no filter settings to adjust)
     * Byte 5 (Power/Mode Settings): 0x000000 (Low Power XYZ, Continuous Conversion), 0x000100 (Low Power XY, Medium Performance Z, Continuous Conversion), 0x001000 (Low Power XY, High Performance Z, Continuous Conversion), 0x001100 (Low Power XY, Ultra Performance Z, Continuous Conversion),
     *                               0x000001 (Low Power XYZ, Single Conversion), 0x000101 (Low Power XY, Medium Performance Z, Single Conversion), 0x001001 (Low Power XY, High Performance Z, Single Conversion), 0x001101 (Low Power XY, Ultra Performance Z, Single Conversion),
     *                               0x010000 (Medium Performance XY, Low Power Z, Continuous Conversion), 0x010100 (Medium Performance XY, Medium Performance Z, Continuous Conversion), 0x011000 (Medium Performance XY, High Performance Z, Continuous Conversion), 0x011100 (Medium Performance XY, Ultra Performance Z, Continuous Conversion),
     *                               0x010001 (Medium Performance XY, Low Power Z, Single Conversion), 0x010101 (Medium Performance XY, Medium Performance Z, Single Conversion), 0x011001 (Medium Performance XY, High Performance Z, Single Conversion), 0x011101 (Medium Performance XY, Ultra Performance Z, Single Conversion),
     *                               0x100000 (High Performance XY, Low Power Z, Continuous Conversion), 0x100100 (High Performance XY, Medium Performance Z, Continuous Conversion), 0x101000 (High Performance XY, High Performance Z, Continuous Conversion), 0x101100 (High Performance XY, Ultra Performance Z, Continuous Conversion),
     *                               0x100001 (High Performance XY, Low Power Z, Single Conversion), 0x100101 (High Performance XY, Medium Performance Z, Single Conversion), 0x101001 (High Performance XY, High Performance Z, Single Conversion), 0x101101 (High Performance XY, Ultra Performance Z, Single Conversion),
     *                               0x110000 (Ultra Performance  XY, Low Power Z, Continuous Conversion), 0x110100 (Ultra Performance  XY, Medium Performance Z, Continuous Conversion), 0x111000 (Ultra Performance  XY, High Performance Z, Continuous Conversion), 0x111100 (Ultra Performance  XY, Ultra Performance Z, Continuous Conversion),
     *                               0x110001 (Ultra Performance  XY, Low Power Z, Single Conversion), 0x110101 (Ultra Performance  XY, Medium Performance Z, Single Conversion), 0x111001 (Ultra Performance  XY, High Performance Z, Single Conversion), 0x111101 (Ultra Performance  XY, Ultra Performance Z, Single Conversion),
     * Bytes 6-17 are currently not used and set at 0x00
     */

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
