/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "MAX30105.h"
#include "heartRate.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Wire.h>
#include <HTTPSRedirect.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#define WIFI_SSID "LAPTOP-S"
#define WIFI_PASSWORD "I@netsec"

// Do not edit at all 
const char* HOST = "script.google.com";
const int httpsPort = 443;
const String sheet_url = "/macros/s/AKfycbzYlaG37AZU9LiI3S-19ZYVhU3qozZ1y_vkeDtWOIlMJ2ofh0MbtmJTd87NBuKmuNJ2/exec";
HTTPSRedirect* client = nullptr;
String payload_base =  "{\"values\": ";
String payload = "";

int sensorValue=0;
String cbt_value = "";

Adafruit_MPU6050 mpu;
String Acceleration = "";
String Gyro = "";
String Time = "";

MAX30105 particleSensor;
#define PPG_NUM_SAMPLES 100
String PPG_data = "";

// AD8232
#define ECG_NUM_SAMPLES 200
String ECG_data = "";

int MUXPinS0 = D8;
int MUXPinS1 = D7;
int MUXPinS2 = D4;
int MUXPinS3 = D3;

//const float minimum_voltage = 3.3; // Set the minimum voltage of your battery
//const float maximum_voltage = 4.2; // Set the maximum voltage of your battery
//#define BUZZER_PIN D0

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
  Serial.print("Connecting to ");
  Serial.println(HOST);
  
  while(!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    delay(100);
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  // Serial.print("Accelerometer range set to: ");
  // switch (mpu.getAccelerometerRange()) {
  // case MPU6050_RANGE_2_G:
  //   Serial.println("+-2G");
  //   break;
  // case MPU6050_RANGE_4_G:
  //   Serial.println("+-4G");
  //   break;
  // case MPU6050_RANGE_8_G:
  //   Serial.println("+-8G");
  //   break;
  // case MPU6050_RANGE_16_G:
  //   Serial.println("+-16G");
  //   break;
  // }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  // Serial.print("Gyro range set to: ");
  // switch (mpu.getGyroRange()) {
  // case MPU6050_RANGE_250_DEG:
  //   Serial.println("+- 250 deg/s");
  //   break;
  // case MPU6050_RANGE_500_DEG:
  //   Serial.println("+- 500 deg/s");
  //   break;
  // case MPU6050_RANGE_1000_DEG:
  //   Serial.println("+- 1000 deg/s");
  //   break;
  // case MPU6050_RANGE_2000_DEG:
  //   Serial.println("+- 2000 deg/s");
  //   break;
  // }
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  // Serial.print("Filter bandwidth set to: ");
  // switch (mpu.getFilterBandwidth()) {
  // case MPU6050_BAND_260_HZ:
  //   Serial.println("260 Hz");
  //   break;
  // case MPU6050_BAND_184_HZ:
  //   Serial.println("184 Hz");
  //   break;
  // case MPU6050_BAND_94_HZ:
  //   Serial.println("94 Hz");
  //   break;
  // case MPU6050_BAND_44_HZ:
  //   Serial.println("44 Hz");
  //   break;
  // case MPU6050_BAND_21_HZ:
  //   Serial.println("21 Hz");
  //   break;
  // case MPU6050_BAND_10_HZ:
  //   Serial.println("10 Hz");
  //   break;
  // case MPU6050_BAND_5_HZ:
  //   Serial.println("5 Hz");
  //   break;
  // }

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
//  Serial.println("Place your index finger on the sensor with steady pressure.");
  particleSensor.setup();                   //Configure sensor with default settings
  // particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  // particleSensor.setPulseAmplitudeGreen(0);  //Turn off Green LED

  // AD8232
  pinMode(14, INPUT); // Setup for leads off detection LO +
  pinMode(12, INPUT); // Setup for leads off detection LO -

  // Analog Multiplexer
  pinMode(MUXPinS0, OUTPUT);
  pinMode(MUXPinS1, OUTPUT);
  pinMode(MUXPinS2, OUTPUT);
  pinMode(MUXPinS3, OUTPUT);

//  pinMode(BUZZER_PIN, OUTPUT); // Set the buzzer pin as an output

  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(19800);
}

void loop() { 
  // put your main code here, to run repeatedly:
//  Battery();
  GSR_Data();
  TemperaturePPG();
  Accelerometer_Data();
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + "AHLSAM018434" + "," + String(sensorValue) + "," + String(cbt_value) + "," + String(Acceleration) + "," + String(Gyro) + ","+ String(Time) + "\"}";
  // Publish data to Google Sheets
  
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(sheet_url, HOST, payload)){ 
    delay(1000);
    Serial.println("Successful Connection : Data has been transferred to Google Sheets.");
  }
  else{
    Serial.println("Error while connecting");
  }

  sensorValue = 0;
  cbt_value = "";
  Acceleration = "";
  Gyro = "";
  Time = "";

  delay(500);
  PPG_Data();
  ECG();
  delay(500);

  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + "AHLSAM018434" + "," + String(PPG_data) + "," + String(ECG_data) + "\"}";
  // Publish data to Google Sheets
  
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(sheet_url, HOST, payload)){ 
    delay(1000);
    Serial.println("Successful Connection : Data has been transferred to Google Sheets.");
  }
  else{
    Serial.println("Error while connecting");
    delay(1000);
  }

  PPG_data = "";
  ECG_data = "";
}

void GSR_Data(){ 
  // Channel 11
  digitalWrite(MUXPinS0,HIGH);digitalWrite(MUXPinS1,HIGH);digitalWrite(MUXPinS2,LOW);digitalWrite(MUXPinS3,HIGH);
  delay(500);
  sensorValue = analogRead(A0);
//  Serial.println(sensorValue);
  delay(500);
  digitalWrite(MUXPinS0,LOW);digitalWrite(MUXPinS1,LOW);digitalWrite(MUXPinS2,LOW);digitalWrite(MUXPinS3,LOW);
}

void Accelerometer_Data(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  // Serial.print("Acceleration X: ");
  // Serial.print(a.acceleration.x);
  // Serial.print(", Y: ");
  // Serial.print(a.acceleration.y);
  // Serial.print(", Z: ");
  // Serial.print(a.acceleration.z);
  // Serial.println(" m/s^2");
  Acceleration = String(a.acceleration.x)+"i "+String(a.acceleration.y)+"j "+String(a.acceleration.z)+"k  m/s^2";

  // Serial.print("Rotation X: ");
  // Serial.print(g.gyro.x);
  // Serial.print(", Y: ");
  // Serial.print(g.gyro.y);
  // Serial.print(", Z: ");
  // Serial.print(g.gyro.z);
  // Serial.println(" rad/s");
  Gyro = String(g.gyro.x)+"i "+String(g.gyro.y)+"j "+String(g.gyro.z)+"k  rad/s";

  timeClient.update();
  Time = timeClient.getFormattedTime();
  Serial.println("");
  delay(500);
}

void PPG_Data(){
  for(int i = 0; i<PPG_NUM_SAMPLES; i++){
    long redValue = particleSensor.getRed();
    PPG_data = String(redValue) + ":" + PPG_data;
    delay(10);
  }
  PPG_data = PPG_data.substring(0,PPG_data.length()-1);
  delay(500);
}

void ECG() {
  if((digitalRead(12) == 1)||(digitalRead(14) == 1)){
    Serial.println('!');
  }
  else{
    // send the value of analog input A0    
    digitalWrite(MUXPinS0,HIGH);digitalWrite(MUXPinS1,LOW);digitalWrite(MUXPinS2,HIGH);digitalWrite(MUXPinS3,HIGH);
    delay(500);
    for(int i = 0; i<ECG_NUM_SAMPLES; i++){
      int dataECG = analogRead(A0);
      delay(10);
      ECG_data += String(dataECG) + ":";
    }
    ECG_data = ECG_data.substring(0, ECG_data.length()-1);

    digitalWrite(MUXPinS0,LOW);digitalWrite(MUXPinS1,LOW);digitalWrite(MUXPinS2,LOW);digitalWrite(MUXPinS3,LOW);
    delay(500);
  }
}

void TemperaturePPG(){
  particleSensor.enableDIETEMPRDY();         //Enable the temp ready interrupt. This is required.
  float temperature = particleSensor.readTemperature();
  cbt_value = String(temperature);
  delay(500);
  particleSensor.disableDIETEMPRDY();   
  delay(500);
}

//void buzzer() {
//  digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
//  delay(1000); // Wait for one second
//  digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
//  delay(1000); // Wait for one second
//}
//
//void Battery() {
//  // Channel 10
//  digitalWrite(MUXPinS0,LOW);digitalWrite(MUXPinS1,HIGH);digitalWrite(MUXPinS2,LOW);digitalWrite(MUXPinS3,HIGH);
//  int raw_value = analogRead(A0); // Read the raw analog value
//  float voltage = raw_value / 1023.0 * 3.3; // Convert the raw value to voltage
//  int battery_percentage = (voltage - minimum_voltage) / (maximum_voltage - minimum_voltage) * 100; // Calculate the battery percentage
//  if( battery_percentage < 10) buzzer();
//  delay(500);
//}
