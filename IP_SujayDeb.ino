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
String sheet_url = "/macros/s/AKfycby0ibmYjSB9nQaq0AZ6KP30UiKJfgtZOMoOIi4qhkjgtT-_rjNpsga_w1uMkNk_oUmv/exec";
HTTPSRedirect* client = nullptr;
String payload_base =  "{\"values\": ";
String payload = "";

const int GSR=A0;
int sensorValue=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);

  //WiFi Connectivity
  Serial.println("Booting");
  Serial.print("Connecting to Wi-Fi");
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
       Serial.println("Connected");
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
  
  // Read ID_Name 
  Serial.println("Cell value: AHLSAM018434");
  delay(2000);

  GSR_Data();
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  //  server.handleClient();
}

void GSR_Data(){
  // GSR data
  static bool flag = false;
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
  
  for(int i = 0; i<100; i++){
    sensorValue = analogRead(GSR);
    delay(1000);
    // send the sensorValue to the googlesheet // 
    // Create json object string to send to Google Sheets
    
    payload = payload_base + "\"" + "AHLSAM018434" + "," + sensorValue + "\"}";
    // Publish data to Google Sheets
    
    Serial.println("Publishing data...");
    Serial.println(payload);
    if(client->POST(sheet_url, HOST, payload)){ 
      delay(500);
      Serial.println("Successful Connection : Data has been transferred to Google Sheets.");
      delay(500);
    }
    else{
      Serial.println("Error while connecting");
      delay(100);
    }
    delay(500);
  }
}


// Get Mac Address of NodeMCU

//#ifdef ESP32
//  #include <WiFi.h>
//#else
//  #include <ESP8266WiFi.h>
//#endif
//
//void setup(){
//  Serial.begin(9600);
//  Serial.println();
//  Serial.print("ESP Board MAC Address:  ");
//  Serial.println(WiFi.macAddress());
//}
// 
//void loop(){
//
//}


// Get IP-Address for NodeMCU
//#include <ESP8266WiFi.h>        // Include the Wi-Fi library
//
//const char *ssid = "realme"; // The name of the Wi-Fi network that will be created
//const char *password = "1234@1234";   // The password required to connect to it, leave blank for an open network
//
//void setup() {
//  Serial.begin(9600);
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.println("Connecting to WiFi...");
//  }
//  Serial.println("Connected to WiFi");
//  Serial.println("IP address: " + WiFi.localIP().toString());
//}
//
//void loop() { }
