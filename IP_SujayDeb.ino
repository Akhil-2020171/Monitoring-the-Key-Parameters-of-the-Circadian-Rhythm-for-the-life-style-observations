#include "MAX30105.h"
#include "heartRate.h"

//#include <DFRobot_MLX90614.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Wire.h>
#include <HTTPSRedirect.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define WIFI_SSID "STUDENTS-N"
#define WIFI_PASSWORD "I@netsec"

// Do not edit at all 
const char* HOST = "script.google.com";
const int httpsPort = 443;
String sheet_url = "/macros/s/AKfycbzOTarW7qzhLlF5XHqyOzNnOzXDZ8kzSTndPs3c49gESFpJTXaOHcaefoUeGUgbdcgD/exec";
HTTPSRedirect* client = nullptr;
String payload_base =  "{\"values\": ";
String payload = "";

int sensorValue=0;

//DFRobot_MLX90614_I2C sensor;   // instantiate an object to drive our sensor
String cbt_value = "";

Adafruit_MPU6050 mpu;
String Acceleration = "";
String Gyro = "";

MAX30105 particleSensor;
const byte RATE_SIZE = 4;  //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];     //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;         //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

// AD8232
#define NUM_SAMPLES 256
int readings[NUM_SAMPLES];
String HeartRate = "";
String Time = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);

  //WiFi Connectivity
  Serial.println("Booting");
  Serial.println("Connecting to Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.print("Connected to WIFI!");
  Serial.println('\n');  

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = client->connect(HOST, httpsPort);
    if (retval == 1){
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(HOST);
    return;
  }  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(HOST);

  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object
  
  while(!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    delay(100);
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
  // Read ID_Name 
  Serial.println("Cell value: AHLSAM018434");
  delay(5000);

  flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr){
    if (!client->connected()){
      client->connect(HOST, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }

//  sensor.enterSleepMode();
//  delay(50);
//  sensor.enterSleepMode(false);
//  delay(200);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  particleSensor.setup();                   //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);  //Turn off Green LED
  particleSensor.enableDIETEMPRDY();         //Enable the temp ready interrupt. This is required.

  // AD8232
  pinMode(14, INPUT); // Setup for leads off detection LO +
  pinMode(12, INPUT); // Setup for leads off detection LO -

  for (int i=0; i<NUM_SAMPLES; i++) {
    readings[i] = 0;
  }
}

void loop() { 
  // put your main code here, to run repeatedly:
  int temp = 0;
  
//  GSR_Data();
  TemperaturePPG();
  Accelerometer_Data();
  while(temp<350){
    PPG_Data();
    temp++;
  }
  Heart_Rate();
  delay(500);
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + "AHLSAM018434" + "," + String(sensorValue) + "," + String(cbt_value) + "," + String(Acceleration) + "," + String(Gyro) + "," + String(beatAvg) + "," + String(HeartRate) + ","+ String(Time) + "\"}";
  // Publish data to Google Sheets
  
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(sheet_url, HOST, payload)){ 
    delay(1000);
    Serial.println("Successful Connection : Data has been transferred to Google Sheets.");
    delay(1000);
  }
  else{
    Serial.println("Error while connecting");
    delay(1000);
  }

  sensorValue = 0;
  cbt_value = "";
  Acceleration = "";
  Gyro = "";
  beatsPerMinute = 0;
  beatAvg = 0;
  HeartRate = "";
  Time = "";

  delay(2000);
}

//void GSR_Data(){ 
//  sensorValue = analogRead(A0);
//  delay(500);
//}

void Accelerometer_Data(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");
  Acceleration = String(a.acceleration.x)+"i "+String(a.acceleration.y)+"j "+String(a.acceleration.z)+"k  m/s^2";

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");
  Gyro = String(g.gyro.x)+"i "+String(g.gyro.y)+"j "+String(g.gyro.z)+"k  rad/s";

//  Serial.print("Temperature: ");
//  Serial.print(temp.temperature);
//  Serial.println(" degC");
//  Temp = String(temp.temperature)+" degC";

  Serial.println("");
  delay(500);
}

void PPG_Data(){
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true){
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20){
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
//  if (irValue < 50000) Serial.println(" No finger?");
}

void Heart_Rate() {
  if((digitalRead(12) == 1)||(digitalRead(14) == 1)){
    Serial.println('!');
  }
  else{
    // send the value of analog input A0
    float t1 = millis();
    for(int i = 0; i<NUM_SAMPLES; i++){
      int dataECG = analogRead(A0);
      readings[i] = dataECG;
      Serial.println(dataECG);
    }
    float t2 = millis();
    
    for (int i=0; i<NUM_SAMPLES; i++) {
      HeartRate += String(readings[i]) + ";";
    }
    HeartRate = HeartRate.substring(0, HeartRate.length()-1);
    Time      = String(t2-t1);
  }
}

void TemperaturePPG(){
  float temperature = particleSensor.readTemperature();
  cbt_value = String(temperature);
  delay(500);
}
