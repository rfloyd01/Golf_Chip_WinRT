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

uint8_t better_data[9 * number_of_samples];
int data_size = 9 * number_of_samples;

/* Create new BLE Service and Characteristic */
BLEService DataService("180C"); //holds raw data from all three sensors and notifies Central devices when updated
BLECharacteristic RawDataCharacteristic("2A58", BLERead | BLENotify, 18 * number_of_samples); //There are 9 pieces of data each at 2 bytes, so characteristic size is 18 bytes * the number of samples desired

BLEService SensorInformationService("180D"); //holds information on individual sensors, such as name and current settings
BLECharacteristic AccelerometerSettingsCharacteristic("2A59", BLERead | BLEWrite, 18);
BLECharacteristic AccelerometerNameCharacteristic("2A5A", BLERead, "LSM9DS1_ACC");

void displaySensorDetails(void)
{
  sensor_t accel, mag, gyr;
  accelmag.getSensor(&accel, &mag);
  gyro.getSensor(&gyr);
  Serial.println("------------------------------------");
  Serial.println("ACCELEROMETER");
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(accel.name);
  Serial.print("Driver Ver:   ");
  Serial.println(accel.version);
  Serial.print("Unique ID:    0x");
  Serial.println(accel.sensor_id, HEX);
  Serial.print("Min Delay:    ");
  Serial.print(accel.min_delay);
  Serial.println(" s");
  Serial.print("Max Value:    ");
  Serial.print(accel.max_value, 4);
  Serial.println(" m/s^2");
  Serial.print("Min Value:    ");
  Serial.print(accel.min_value, 4);
  Serial.println(" m/s^2");
  Serial.print("Resolution:   ");
  Serial.print(accel.resolution, 8);
  Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");

  Serial.println("------------------------------------");
  Serial.println("GYROSCOPE");
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(gyr.name);
  Serial.print("Driver Ver:   ");
  Serial.println(gyr.version);
  Serial.print("Unique ID:    0x");
  Serial.println(gyr.sensor_id, HEX);
  Serial.print("Max Value:    ");
  Serial.print(gyr.max_value);
  Serial.println(" rad/s");
  Serial.print("Min Value:    ");
  Serial.print(gyr.min_value);
  Serial.println(" rad/s");
  Serial.print("Resolution:   ");
  Serial.print(gyr.resolution);
  Serial.println(" rad/s");
  Serial.println("------------------------------------");
  Serial.println("");
  
  Serial.println("------------------------------------");
  Serial.println("MAGNETOMETER");
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(mag.name);
  Serial.print("Driver Ver:   ");
  Serial.println(mag.version);
  Serial.print("Unique ID:    0x");
  Serial.println(mag.sensor_id, HEX);
  Serial.print("Min Delay:    ");
  Serial.print(accel.min_delay);
  Serial.println(" s");
  Serial.print("Max Value:    ");
  Serial.print(mag.max_value);
  Serial.println(" uT");
  Serial.print("Min Value:    ");
  Serial.print(mag.min_value);
  Serial.println(" uT");
  Serial.print("Resolution:   ");
  Serial.print(mag.resolution);
  Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

uint8_t oldBatteryLevel = 0;

void setup(void)
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT); //Setup onboard LED to show when BLE connection is succesful
  //digitalWrite(LED_PWR, LOW); //turn off light to reduce current consumption
  //digitalWrite(LED_BUILTIN, LOW); //turn off light to reduce current consumption

  /* Wait for the Serial Monitor */
  //while (!Serial) delay(1);

  Serial.println("FXOS8700 Test");
  Serial.println("");

  /*
  //Initialise the accelerometer and magnetometer
  if (!accelmag.begin(ACCEL_RANGE_4G))
  {
    //There was a problem detecting the FXOS8700 ... check your connections
    Serial.println("Ooops, no FXOS8700 detected ... Check your wiring!");
    while (1);
  }

  //Initialise the gyroscope
  if (!gyro.begin(GYRO_RANGE_500DPS))
  {
    //There was a problem detecting the FXAS21002C ... check your connections
    Serial.println("Ooops, no FXAS21002C detected ... Check your wiring!");
    while (1);
  }
  */
  //Initialize the IMU
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

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /*Set up const pointer to data location */
  uint8_t* const data_location = &better_data[0];
  const uint8_t* other_data_location = &better_data[0];

  //Set any characteristic callback functions
  AccelerometerSettingsCharacteristic.setEventHandler(BLEWritten, configCharacteristicWritten);

  /* Initialize user made BLE services and characteristics */
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(DataService);
  DataService.addCharacteristic(RawDataCharacteristic);
  DataService.addCharacteristic(AccelerometerSettingsCharacteristic);
  DataService.addCharacteristic(AccelerometerNameCharacteristic);
  BLE.addService(DataService);
  RawDataCharacteristic.writeValue(&data[0], 9 * number_of_samples);
  AccelerometerSettingsCharacteristic.writeValue(IMU.getSensorSettings(), 18); //each of the 18 standard settings gets its own byte
  //RawDataCharacteristic.writeValue(better_data, data_size);
  //RawDataCharacteristic.writeValue(data_location, data_size);
  //RawDataCharacteristic.writeValue(oldBatteryLevel);

  /* Set desired connection interval for BLE connection */
  BLE.setConnectionInterval(0x0006, 0x000C); //sets the desired connection interval to be between 7.5ms and 15ms (6 * 1.25 = 7.5 and 12 * 1.25 = 15)
}

void configCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic)
{
  Serial.print("Characteristic has been written to, new value is ");
  Serial.print(AccelerometerSettingsCharacteristic.read());
}

void getSensorSettings(uint8_t *data_buffer)
{
  
}

void loop(void)
{
  BLE.advertise(); //start advertising
  bool maintain_connection = 1;
  while (maintain_connection)
  {
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
          //accelmag.getRawData(&data[count * 9]); //uncomment when new FXOS chip is installed
          //gyro.getRawData(&data[count * 9]); //uncomment when new FXAS chip is installed
          IMU.readRawData(&data[count * 9]);
          count++;
        }
        RawDataCharacteristic.writeValue(data, number_of_samples * 18);
        Serial.println("Characteristic updated!");
        //Serial.print(millis() - new_timer);
        //Serial.println(" milliseconds elapsed.");
      }
      
      //when the central disconnects, turn off the LED:
      Serial.println("The computer has disconnected from the BLE 33 Nano\n");
      digitalWrite(LED_BUILTIN, LOW);
      maintain_connection = 0;
    }
  }
}

/*
 Print information to be used if necessary
      //With an ODR of 400 it should take roughly 3ms to get each new reading
      Serial.print("A ");
      Serial.print("X: ");
      Serial.print(data[0] * 0.000488 * 9.80665);
      Serial.print("  ");
      Serial.print("Y: ");
      Serial.print(data[1] * 0.000488 * 9.80665);
      Serial.print("  ");
      Serial.print("Z: ");
      Serial.print(data[2] * 0.000488 * 9.80665);
      Serial.print("  ");
      Serial.println("deg/s ");
      Serial.print("Took ");
      Serial.print(millis() - timestamp);
      Serial.println(" milliseconds to read data.");
      Serial.println("");
 */
