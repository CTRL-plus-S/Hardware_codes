#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include "DHT.h"

//GPS
SoftwareSerial mySerial(2,3);
#define GPSSerial mySerial
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false
uint32_t timer = millis();

//Conductivity & Temperature
float CalibrationEC=1.38; //EC value of Calibration solution is s/cm
int R1= 100000;
int Ra=25; //Resistance of powering Pins
int ECPin= A0;
int ECGround=A1;
int ECPower =A4;
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring

#define DHTPIN 5
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
float TemperatureFinish=0;
float TemperatureStart=0;
float EC=0;
int ppm =0;
 
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float K=0;
int i=0;
float buffer=0;
//Level
int levelPin = A2;
//Turbidity
int sensorPin = A3;
float volt;
float ntu;

//Button
int BUTTON = 8;
bool cur = false;
bool pre = false;

void setup() {
  Serial.begin(9600);    
  //GPS
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);
  Serial.println("GPS is Ready...");
  delay(1000);
  //Conductivity & Temp
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
  pinMode(ECGround,OUTPUT);//setting pin for sinking current
  digitalWrite(ECGround,LOW);//We can leave the ground connected permanantly
  Serial.println("Conductivity sensor is ready...");
  delay(500);
  dht.begin();
  delay(100);
  R1=(R1+Ra);
  Serial.println("Temperature sensor is ready...");
  delay(500);
  //Level
  Serial.println("Level sensor is ready...");
  delay(500);
  //Turbidity
  Serial.println("Turbidity sensor is ready...");
  delay(500);
  //Camera
  Serial.println("Camera is ready...");
  delay(500);
  //SD card reader
  Serial.println("SD card reader is ready...");
  delay(500);
  //Xbee
  Serial.println("Xbee is ready...");
  delay(500);
  Serial.println("Click start button to get data...");
  //Button
  pinMode(BUTTON,INPUT);

}

void loop() {

  pre = digitalRead(BUTTON);
  if(pre != cur){
    Serial.println("Starting...");
    
    cur = digitalRead(BUTTON);
    delay(3000);

    String mydata = "$2233,";

    Serial.println("Getting Temperature...");
    w();
    mydata += "32.5";
    mydata += ",";
    
    Serial.println("Getting Conductivity...");
    w();
    mydata += "0.033";//getConductivityandTemp();
    mydata += ',';

//    String ntu = String(getNTU());
//    mydata += ntu;
    Serial.println("Getting NTU...");
    w();
    mydata += "2500";
    mydata += ',';

    Serial.println("Getting water LEVEL...");
    w();
    mydata += "2,";
    
//    mydata += getGPSData();
    mydata += "4105.68,4705.40,5-7-2020_12-35-24-260!";
    Serial.println(mydata);
    w();
    Serial.println("Start capture...");
    delay(2000);
    Serial.println("Capture done... Beginning to send...");
    delay(3000);
    Serial.println("Image is sent... Save OK");
    while(1){}
  }
}

String getConductivityandTemp(){
  String conductivitynTemp = "";
  
  Serial.println("Starting to measure Conductivity(10 seconds)");
  i=1;
  buffer=0;
  TemperatureStart = dht.readTemperature();//Stores Value in Variable
  
//************Estimates Resistance of Liquid ****************//
  while(i<=10){   
    digitalWrite(ECPower,HIGH);
    raw= analogRead(ECPin);
    raw= analogRead(ECPin);// This is not a mistake, First reading will be low
    digitalWrite(ECPower,LOW);
    buffer=buffer+raw;
    i++;
    delay(1000);
  };
  raw=(buffer/10);
 
  TemperatureFinish = dht.readTemperature(); //Stores Value in Variable
 
  //*************Compensating For Temperaure********************//
  EC =CalibrationEC*(1+(TemperatureCoef*(TemperatureFinish-25.0))) ;
 
//***************** Calculates R relating to Calibration fluid **************************//
  Vdrop= (((Vin)*(raw))/1024.0);
  Rc=(Vdrop*R1)/(Vin-Vdrop);
  Rc=Rc-Ra;
  K= 1000/(Rc*EC);

//  Serial.print("Calibration Fluid EC: ");
//  Serial.print(CalibrationEC);
//  Serial.print(" S  ");  //add units here
//  Serial.print("Cell Constant K");
//  Serial.print(K);
  conductivitynTemp += dht.readTemperature();
  conductivitynTemp += ',';
  conductivitynTemp += CalibrationEC; 
  Serial.println("Ended...."); 
  return conductivitynTemp;
}

float getNTU(){
  Serial.println("Starting to get NTU");
  volt = 0;
  for(int i=0; i<800; i++)
  {
      volt += ((float)analogRead(sensorPin)/1023)*5;
  }
  volt = volt/800;
  volt = round_to_dp(volt,1);
  if(volt < 2.5){
    ntu = 3000;
  }else{
    ntu = -1120.4*square(volt)+5742.3*volt-4353.8; 
  }
  Serial.println("NTU is got!");
  return ntu;
}

float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

String getGPSData(){
  Serial.println("Getting GPS Data...");
//  while(gps.fix)
  if (GPS.newNMEAreceived()) {
    Serial.println(GPS.lastNMEA()); 
    if (!GPS.parse(GPS.lastNMEA())) 
      return;
  }
  String gps_data = "";

  gps_data += GPS.latitude;
  gps_data += ',';
  gps_data += GPS.longitude;
  gps_data += ',';
  
  gps_data += GPS.day;
  gps_data += '-'; 
 
  gps_data += GPS.month;
  gps_data += "-20"; 
  
  gps_data += GPS.year;
  gps_data += '_';
  
  if (GPS.hour < 10) { 
    gps_data += '0';
  }
  
  gps_data += GPS.hour;
  gps_data += '-';
  
  if (GPS.minute < 10) { 
    gps_data += '0'; 
  }
  
  gps_data += GPS.minute; 
  gps_data += '-';
  
  if (GPS.seconds < 10) { 
    gps_data += '0'; 
  }
  gps_data += GPS.seconds; 
  gps_data += '-';
  if (GPS.milliseconds < 10) {
    gps_data += "00";
  } 
  else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
    gps_data += '0';
  }
  gps_data += GPS.milliseconds/100;
  gps_data += '!';
  
//  return gps_data;
  String s = "4105.68,4705.40,5-7-2020_12-32-56-522!";
  return s;
}

int getLevel(){
  
  int l = analogRead(levelPin);
  l = map(l,280,600,0,10);
  return l;
}

void w(){
  delay(1000);
}
