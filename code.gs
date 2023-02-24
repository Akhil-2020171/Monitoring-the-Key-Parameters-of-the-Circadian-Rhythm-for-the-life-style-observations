var ss = SpreadsheetApp.openById('1VT9Ua-cor43qylF6DCIG5K7XY2gRKvoKauFtPCzoDDY');
var sheet = ss.getSheetByName("Form_Responses");
var spreadsheet = SpreadsheetApp.getActive();
var timezone = Session.getScriptTimeZone();

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

function createNewSheet(name) {
  var new_sheet = SpreadsheetApp.getActiveSpreadsheet().insertSheet();
  new_sheet.setName(name);
  Utilities.sleep(500);
  new_sheet.getRange("A1").setValue("GSR").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("B1").setValue("CBT(degC)").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("C1").setValue("PPG").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("D1").setValue("ECG").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("E1").setValue("Sleep").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("F1").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
}

function random(name, number){
  var str = "";
  for(var i = 0; i<name.length; i=i+2) str += name[i];
  for(var i = 0; i<number.length; i=i+2) str += number[i];
  return str.toString();
}

function createAndSaveLineChart(ecgData=[20, 45, 32, 56, 21, 54, 32, 65, 45, 50],name="AHLSAM018434"){
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
  var folder = getFolders(name);

  var deployDate = new Date();
  var deployTime = Utilities.formatDate(deployDate, timezone, 'HH:mm:ss');

  folder.createFile(fileBlob.setName(deployTime));
  spareSheet.getRange(1, 1, spareSheet.getLastRow(), spareSheet.getLastColumn()).clear();
  Utilities.sleep(1000);

  return fileBlob;
}

function getFolders(folderName) {
  var folders = DriveApp.getFoldersByName(folderName); // get all the folders with the name "My Folder"
  
  while (folders.hasNext()) {
    var folder = folders.next(); // get the next folder in the collection
  }
  return folder;
}

function doPost(e){
  var parsedData;
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }

  if (parsedData !== undefined){
    var flag = parsedData.format;
    if (flag === undefined){
      flag = 0;
    }
    var dataArr = parsedData.values.split(","); // creates an array of the values to publish 
    var value1 = dataArr [0];                   // value1 from Arduino code, sheet name
    var value2 = dataArr [1];                   // value2 from Arduino code, sensorValue
    var value3 = dataArr [2];                   // value3 from Arduino code, cbt_Value
    var value4 = dataArr [3];                   // value4 from Arduino code, Acceleration 
    var value5 = dataArr [4];                   // value5 from Arduino code, Gyro
    var value6 = dataArr [5];                   // value7 from Arduino code, Beats/minute
    var value7 = dataArr [6];                   // value8 from Arduino code, Heart Rate Array
  }

  // var value1 = e.parameters.id; // value0 from Arduino code, id
  // var value2 = e.parameters.name; // value1 from Arduino code, name

  var sheet_name   = value1.toString();
  var gsr_value    = value2.toString();
  var cbt_value    = (+value3 + 4).toString();
  var acceleration = value4.toString();
  var gyro         = value5.toString();
  var beatsPminute = value6.toString();
  var heart_rate   = value7.toString();

  var ecgArray     = heart_rate.split(";");
  createAndSaveLineChart(ecgArray,sheet_name);
  Utilities.sleep(1000);

  var required_sheet = spreadsheet.getSheetByName(sheet_name);
  var new_Row        = +required_sheet.getLastRow()+1;
  required_sheet.getRange("A"+new_Row).setValue(gsr_value);
  required_sheet.getRange("B"+new_Row).setValue(cbt_value);
  required_sheet.getRange("C"+new_Row).setValue(beatsPminute);
  required_sheet.getRange("D"+new_Row).setValue(heart_rate);
  required_sheet.getRange("E"+new_Row).setValue("Acceleration = "+acceleration+" , "+"Gyro = "+gyro);
  
  return ContentService.createTextOutput("DONE");
}
