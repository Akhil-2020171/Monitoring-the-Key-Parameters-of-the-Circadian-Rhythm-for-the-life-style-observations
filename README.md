# Monitoring the key parameters of the Circadian Rhythm for the lifestyle observations

## Introduction
A 24-hour cycle that governs numerous physiological processes in living creatures, including humans, is known as the circadian rhythm. The human circadian rhythm regulates many bodily functions, including sleep-wake cycles, hormone secretion, and metabolism.

As a result, there is a rising necessity for monitoring and analyzing the essential parameters of the circadian rhythm to comprehend an individual's health state. This is where our initiative comes in. Our research aims to develop a hardware module with sensors to monitor the important parameters of the circadian rhythm. The ECG, PPG, Accelerometer, and GSR sensors regularly collect data from the subject's body. Machine learning algorithms analyze the data and develop a health index reflecting the subject's health. The accelerometer sensor data is also used to measure sleep activity, which provides extra information about the subject's sleep quality.

## Hardware
The wearable hardware module with the required sensors interfaced with the central microcontroller unit. The module is powered up by a rechargeable 3.7 volts battery(integrated with the module). The module also includes a buzzer in its power supply segment, which triggers and alarms when the input voltage gets reduced below its minimum requirement. The  sensors are compatible with recording ECG, PPG, Accelerometer, and GSR pulses, tapped suitably at a predefined spot in the body. The left hand is chosen explicitly because the ECG sensor electrodes can be conveniently taken to the  chest nodes for obtaining the ECG pulses as per the medical standards. There will be no direct touch to the sensors and any other components that ensures the safety of the hardware and the arm. The developed prototype can be further fine-tuned as we gradually progress and can be made commercially viable. The overall cost of the hardware is meager concerning the commercially available health monitoring devices. 

![Hardware](https://drive.google.com/uc?export=view&id=1t7MnMIZefCYRyWCjZScdPi3oN5hAkZW7)

![Hardware](https://drive.google.com/uc?export=view&id=1Iix3cNzEFkXi1TJagr9xI6ZDKw-pDQwT)

## Software
The software is developed in Python and C++ programming languages. The software is divided into two parts: data acquisition and data analysis. The data acquisition part is developed in C++ and the data analysis part is developed in Python. The data acquisition part is responsible for collecting data from the sensors and storing them in the Google Sheet. The data analysis part is responsible for analyzing the data and generating the health index. The data analysis part is further divided into two parts: sleep analysis and health index generation. The sleep analysis part is responsible for analyzing the accelerometer data and generating the sleep activity index. The health index generation part is responsible for analyzing the ECG, PPG, and GSR data and generating the health index. The health index is generated using machine learning algorithms. 

## Data Acquisition
The data acquisition part is developed in C++ programming language. The data acquisition part is responsible for collecting data from the sensors and storing them in the Google Sheet. The data acquisition part is further divided into two parts: data collection and data transmission. The data collection part is responsible for collecting data from the sensors. The data transmission part is responsible for transmitting the collected data to the Google Sheet. The data transmission part is further divided into two parts: data formatting and data transmission. The data formatting part is responsible for formatting the collected data. The data transmission part is responsible for transmitting the formatted data to the Google Sheet. 

Collect Subject's details using the required google form and generate a seperate sheet and drive folder for it to store data collections and visualizations.

![Google Form](https://drive.google.com/uc?export=view&id=1Pk6z-zbjHi5dFPwL1g9oxpR5IOh2wouQ)

```
function myFunction() {
  Utilities.sleep(1000);
  var nextRow   = sheet.getLastRow();
  var name      = sheet.getRange("B"+nextRow).getValue().toString();
  var number    = sheet.getRange("D"+nextRow).getValue().toString();
  var id        = random(name,number);
  Utilities.sleep(500);
  sheet.getRange("G"+nextRow).setValue(id);
  createNewSheet(id);
  DriveApp.createFolder(id);
}

function createNewSheet(name) { // Create a new sheet for the subject
  var new_sheet = SpreadsheetApp.getActiveSpreadsheet().insertSheet();
  new_sheet.setName(name);
  Utilities.sleep(500);
  new_sheet.getRange("A1").setValue("GSR").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("B1").setValue("CBT(degC)").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("C1").setValue("PPG").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("D1").setValue("ECG").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("E1").setValue("Sleep").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("F1").setValue("Time").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
}

function random(name, number){ // Create a random ID for the subject
  var str = "";
  for(var i = 0; i<name.length; i=i+2) str += name[i];
  for(var i = 0; i<number.length; i=i+2) str += number[i];
  return str.toString();
}
```

### Data Collection
The data collection part is developed in C++ programming language. The data collection part is responsible for collecting data from the sensors. The data collection part is further divided into four parts: ECG data collection, PPG data collection, GSR data collection, and accelerometer data collection. The ECG data collection part is responsible for collecting ECG data from the ECG sensor. The PPG data collection part is responsible for collecting PPG data from the PPG sensor. The GSR data collection part is responsible for collecting GSR data from the GSR sensor. The accelerometer data collection part is responsible for collecting accelerometer data from the accelerometer sensor.

#### ECG Data Collection
```
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
```

#### PPG Data Collection
```
void PPG_Data(){
  for(int i = 0; i<PPG_NUM_SAMPLES; i++){
    long redValue = particleSensor.getRed();
    PPG_data = String(redValue) + ":" + PPG_data;
    delay(10);
  }
  PPG_data = PPG_data.substring(0,PPG_data.length()-1);
  delay(500);
}
```

#### GSR Data Collection
```
void GSR_Data(){ 
  // Channel 11
  digitalWrite(MUXPinS0,HIGH);digitalWrite(MUXPinS1,HIGH);digitalWrite(MUXPinS2,LOW);digitalWrite(MUXPinS3,HIGH);
  delay(500);
  sensorValue = analogRead(A0);
  delay(500);
  digitalWrite(MUXPinS0,LOW);digitalWrite(MUXPinS1,LOW);digitalWrite(MUXPinS2,LOW);digitalWrite(MUXPinS3,LOW);
}
```

#### Accelerometer Data Collection
```
void Accelerometer_Data(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Acceleration = String(a.acceleration.x)+"i "+String(a.acceleration.y)+"j "+String(a.acceleration.z)+"k  m/s^2";
  Gyro = String(g.gyro.x)+"i "+String(g.gyro.y)+"j "+String(g.gyro.z)+"k  rad/s";

  // Time at which the data is collected
  timeClient.update();
  Time = timeClient.getFormattedTime();
  Serial.println("");
  delay(500);
}
```

### Data Transmission
The data acquisition part is developed in C++ programming language. The data transmission part is responsible for transmitting the formatted data to the Google Sheet.
```
// Do not edit at all 
const char* HOST = "script.google.com";
const int httpsPort = 443;
const String sheet_url = "/macros/s/"+"webapp_deployed_ID"+"/exec";
HTTPSRedirect* client = nullptr;
String payload_base =  "{\"values\": ";
String payload = "";

// Create json object string to send to Google Sheets
payload = payload_base + "\"" + "Subject_ID" + "," + "Stored_Data" + "\"}";
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
```

## Data Storage & Formatting
The data storage part is developed in google appsscript. The data storage part is responsible for storing the collected data in the Google Sheet.

![Data](https://drive.google.com/uc?export=view&id=16WimBDYTmiOJtyM5uhLWctX25QWJlrR3)


```
function doPost(e){
  var parsedData;
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }

  if (parsedData !== undefined){
    var dataArr = parsedData.values.split(","); // creates an array of the values to publish 
    if(dataArr.length == 3){
      var value1 = dataArr [0];                   // value1 from Arduino code, sheet name
      var value2 = dataArr [1];                   // value2 from Arduino code, PPG Array
      var value3 = dataArr [2];                   // value3 from Arduino code, ECG Array

      var sheet_name = value1.toString();
      var ppg_data   = value2.toString();
      var ecg_data   = value3.toString();

      var required_sheet = spreadsheet.getSheetByName(sheet_name);
      var new_Row        = +required_sheet.getLastRow();
      required_sheet.getRange("C"+new_Row).setValue(ppg_data);
      required_sheet.getRange("D"+new_Row).setValue(ecg_data);
    }
    else{
      var value1 = dataArr [0];                   // value1 from Arduino code, sheet name
      var value2 = dataArr [1];                   // value2 from Arduino code, sensorValue
      var value3 = dataArr [2];                   // value3 from Arduino code, cbt_Value
      var value4 = dataArr [3];                   // value4 from Arduino code, Acceleration 
      var value5 = dataArr [4];                   // value5 from Arduino code, Gyro
      var value6 = dataArr [5];                   // value6 from Arduino code, Time stamp (MPU6050)

      var sheet_name   = value1.toString();
      var gsr_value    = value2.toString();
      var cbt_value    = (+value3+4).toString();
      var acceleration = value4.toString();
      var gyro         = value5.toString();
      var time         = value6.toString();

      var required_sheet = spreadsheet.getSheetByName(sheet_name);
      var new_Row        = +required_sheet.getLastRow()+1;
      required_sheet.getRange("A"+new_Row).setValue(gsr_value);
      required_sheet.getRange("B"+new_Row).setValue(cbt_value);
      required_sheet.getRange("E"+new_Row).setValue("Acceleration = "+acceleration+" , "+"Gyro = "+gyro);
      required_sheet.getRange("F"+new_Row).setValue(time);
    }
  }
  
  return ContentService.createTextOutput("DONE");
}
```

## Data Visualization
The data visualization part is developed in google appsscript. The data visualization part is responsible for visualizing the collected data.
```
// Similarly for PPG data visualization
function createAndSaveLineChart(ecgData=[20, 45, 32, 56, 21, 54, 32, 65, 45, 50],name="Subject_ID",fileName=deployTime){
   var yData = ecgData;
  
   var spareSheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Spare");
   var range = spareSheet.getRange(1, 1, yData.length, 1);
   var data = [];
   for (var i = 0; i < yData.length; i++) {
     data.push([yData[i]]);
   }
   range.setValues(data);

   // Create the chart data
   var chartBuilder = spareSheet.newChart();
  
   chartBuilder.addRange(range)
     .setChartType(Charts.ChartType.LINE)
     .setPosition(2, 2, 0, 0)
     .setOption('title', 'My Line Chart!');
    
   var chart = chartBuilder.build();
   var fileBlob = chart.getAs("image/png");
   var folder = getFolders("ECG"+name); // Instead of ECG , used PPG for PPG data

   folder.createFile(fileBlob.setName(fileName));
   spareSheet.getRange(1, 1, spareSheet.getLastRow(), spareSheet.getLastColumn()).clear();
   Utilities.sleep(1000);

   return fileBlob;
}
```

## Future Work
The health index is a number between 0 and 1. The health index is 0 when the subject is in a healthy state and 1 when the subject is in an unhealthy state. The health index is generated every 5 minutes. The health index is stored in a file along with the data collected from the sensors.
