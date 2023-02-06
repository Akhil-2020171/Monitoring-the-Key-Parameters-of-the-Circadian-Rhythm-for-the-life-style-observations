var ss = SpreadsheetApp.openById('1VT9Ua-cor43qylF6DCIG5K7XY2gRKvoKauFtPCzoDDY');
var sheet = ss.getSheetByName("Form_Responses");
var spreadsheet = SpreadsheetApp.getActive();

function myFunction() {
  Utilities.sleep(1000);
  var nextRow   = sheet.getLastRow();
  var name      = sheet.getRange("B"+nextRow).getValue().toString();
  var number    = sheet.getRange("D"+nextRow).getValue().toString();
  var id        = random(name,number);
  Utilities.sleep(500);
  sheet.getRange("G"+nextRow).setValue(id);
  // create new sheet for a particular ID
  createNewSheet(id);
}

function createNewSheet(name) {
  var new_sheet = SpreadsheetApp.getActiveSpreadsheet().insertSheet();
  new_sheet.setName(name);
  Utilities.sleep(500);
  new_sheet.getRange("A1").setValue("GSR").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("B1").setValue("CBT").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("C1").setValue("BP").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("D1").setValue("HRV").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
  new_sheet.getRange("E1").setValue("Sleep").setBackground("#00ff00").setFontWeight("bold").setHorizontalAlignment("center");
}

function random(name, number){
  var str = "";
  for(var i = 0; i<name.length; i=i+2) str += name[i];
  for(var i = 0; i<number.length; i=i+2) str += number[i];
  return str.toString();
}

function doGet(e){
  return doPost(e);
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
  }

  // var value1 = e.parameters.id; // value0 from Arduino code, id
  // var value2 = e.parameters.name; // value1 from Arduino code, name

  var sheet_name = value1.toString();
  var gsr_value  = value2.toString();

  var required_sheet = spreadsheet.getSheetByName(sheet_name);

  var new_Row   = +required_sheet.getLastRow()+1;
  required_sheet.getRange("A"+new_Row).setValue(gsr_value);
  // Logger.log(new_Row);

  return ContentService.createTextOutput("DONE");
}
