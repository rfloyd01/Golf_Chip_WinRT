#include <ArduinoBLE.h>
#include <Wire.h>
#include "Bobby_Adafruit_FXOS8700.h"
#include "Bobby_Adafruit_FXAS21002C.h"
#include "Bobby_Adafruit_Sensor.h"
#include "Bobby_Arduino_LSM9DS1.h"

/* Assign a unique ID to the sensors at the same time */
Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);
Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);

/* Create a timer just in case */
int32_t begin_time;

/* Create array to store all data*/
const int number_of_samples = 10;
int16_t data[9 * number_of_samples];
int data_size = 9 * number_of_samples;

/* Create new BLE Service and Characteristics */
/* The Arduino API only lets one service broadcast at a time, so only create a single service with multiple characteristics */
BLEService DataService("180C"); //holds raw data as well as setting information from all three sensors and notifies Central devices when updated

//Characteristic for holding raw sensor data (data from all three sensors is kept in a single array
BLECharacteristic RawDataCharacteristic("2A58", BLERead | BLENotify, 18 * number_of_samples); //There are 9 pieces of data each at 2 bytes, so characteristic size is 18 bytes * the number of samples desired

//Characteristics for holding information about the active sensors
BLECharacteristic SensorInformationCharacteristic("2A59", BLERead, 18);

//Characteristics for holding setting information for each sensor (one characteristic for each sensor)
//Each byte represents a different setting option (i.e. byte 0 = ODR, 0x00 = 119 Hz, 0x01 = 200 Hz, etc.) 
BLECharacteristic AccelerometerSettingsCharacteristic("2A5A", BLERead | BLEWrite, 18);
BLECharacteristic GyroscopeSettingsCharacteristic("2A5B", BLERead | BLEWrite, 18);
BLECharacteristic MagnetometerSettingsCharacteristic("2A5C", BLERead | BLEWrite, 18);

void setup(void)
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT); //Setup onboard LED to show when BLE connection is succesful

  /* Wait for the Serial Monitor */
  //while (!Serial)

  //Initialize the IMU
  Serial.println("Initializing the IMU!");
  if (!IMU.begin()) //initialize BLE and go to infinite loop if fail
  {
    Serial.println("Starting IMU Failed!");
    while(1);
  }
  
  //Initialise the BLE chip
  if (!BLE.begin()) //initialize BLE and go to infinite loop if fail
  {
    Serial.println("Starting BLE Failed!");
    while(1);
  }
  else Serial.println("Starting BLE Succeeded!");

  //Set any characteristic callback functions
  AccelerometerSettingsCharacteristic.setEventHandler(BLEWritten, configCharacteristicWritten);

  /* Initialize user made BLE services and characteristics */
  BLE.setLocalName("Golf Chip");
  BLE.setAdvertisedService(DataService);
  DataService.addCharacteristic(RawDataCharacteristic);
  DataService.addCharacteristic(SensorInformationCharacteristic);
  DataService.addCharacteristic(AccelerometerSettingsCharacteristic);
  DataService.addCharacteristic(GyroscopeSettingsCharacteristic);
  DataService.addCharacteristic(MagnetometerSettingsCharacteristic);

  BLE.addService(DataService);
  RawDataCharacteristic.writeValue(&data[0], 9 * number_of_samples);
  AccelerometerSettingsCharacteristic.writeValue(IMU.getAccelerometerSettings(), 18);
  GyroscopeSettingsCharacteristic.writeValue(IMU.getGyroscopeSettings(), 18);
  MagnetometerSettingsCharacteristic.writeValue(IMU.getMagnetometerSettings(), 18);

  /* Set desired connection interval for BLE connection */
  BLE.setConnectionInterval(0x0006, 0x000C); //sets the desired connection interval to be between 7.5ms and 15ms (6 * 1.25 = 7.5 and 12 * 1.25 = 15)
}

void configCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic)
{
  Serial.print("Characteristic has been written to, new value is ");
  Serial.print(AccelerometerSettingsCharacteristic.read());
}

void loop(void)
{
  Serial.println("Initializing the IMU!");
  BLE.advertise(); //start advertising
  bool maintain_connection = 1;
  while (maintain_connection)
  {
    Serial.println("Initializing the IMU!");
    BLEDevice central = BLE.central(); //Wait for BLE central to connect
    if (central)
    {
      Serial.println("The computer has connected to the BLE 33 Nano");

      //don't turn on light when connected to reduce current draw
      digitalWrite(LED_BUILTIN, HIGH); //turn on the yellow LED to indicate a connection has been made
      begin_time = millis();
      while (central.connected()) //keep looping while connected
      {
        int count = 0;
        int32_t current_time, new_timer = millis();
        while (count < number_of_samples)
        {
          IMU.readRawData(&data[count * 9]);
          count++;
        }
        RawDataCharacteristic.writeValue(data, number_of_samples * 18);
        Serial.println("Characteristic updated!");
      }
      
      //when the central disconnects, turn off the LED:
      Serial.println("The computer has disconnected from the BLE 33 Nano\n");
      digitalWrite(LED_BUILTIN, LOW);
      maintain_connection = 0;
    }
  }
}
