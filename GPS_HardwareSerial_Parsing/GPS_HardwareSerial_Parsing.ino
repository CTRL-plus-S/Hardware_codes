// Test code for Ultimate GPS Using Hardware Serial (e.g. GPS Flora or FeatherWing)
//
// This code shows how to listen to the GPS module via polling. Best used with
// Feathers or Flora where you have hardware Serial and no interrupt
//
// Tested and works great with the Adafruit GPS FeatherWing
// ------> https://www.adafruit.com/products/3133
// or Flora GPS
// ------> https://www.adafruit.com/products/1059
// but also works with the shield, breakout
// ------> https://www.adafruit.com/products/1272
// ------> https://www.adafruit.com/products/746
//
// Pick one up today at the Adafruit electronics shop
// and help support open source hardware & software! -ada

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2,3);
// what's the name of the hardware serial port?
#define GPSSerial mySerial

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();


void setup()
{
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(9600);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

void loop() // run over and over again
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
/*  if (GPSECHO)
    if (c) Serial.print(c);*/
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    String gps_data = getGPSData();
    Serial.println(gps_data);
  }
}

String getGPSData(){
  String gps_data = "";

  gps_data += GPS.latitude;
  gps_data += ',';
  gps_data += GPS.longitude;
  gps_data += ',';
  
  Serial.print("Date: ");
  Serial.print(GPS.day, DEC);
  gps_data += GPS.day;
  gps_data += '-'; 
  Serial.print('/');
  Serial.print(GPS.month, DEC);
  gps_data += GPS.month;
  gps_data += "-20"; 
  Serial.print("/20");
  Serial.println(GPS.year, DEC);
  gps_data += GPS.year;
  gps_data += '_';
  
  Serial.print("\nTime: ");
  if (GPS.hour < 10) { 
    Serial.print('0'); 
    gps_data += '0';
  }
  Serial.print(GPS.hour, DEC); 
  Serial.print(':');
  gps_data += GPS.hour;
  gps_data += '-';
  if (GPS.minute < 10) { 
    Serial.print('0');
    gps_data += '0'; 
  }
  Serial.print(GPS.minute, DEC);
  gps_data += GPS.minute; 
  Serial.print(':');
  gps_data += '-';
  if (GPS.seconds < 10) { 
    Serial.print('0');
    gps_data += '0'; 
  }
  Serial.print(GPS.seconds, DEC);
  gps_data += GPS.seconds; 
  Serial.print('.');
  gps_data += '-';
  if (GPS.milliseconds < 10) {
    Serial.print("00");
    gps_data += "00";
  } 
  else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
    Serial.print("0");
    gps_data += '0';
  }
  gps_data += GPS.milliseconds/100;
  gps_data += '!';
  Serial.println(GPS.milliseconds);
  
  Serial.print("Fix: "); Serial.print((int)GPS.fix);
  Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
  if (GPS.fix) {
    Serial.print("Location: ");
    Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
    Serial.print(", ");
    Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
    Serial.print("Speed (knots): "); Serial.println(GPS.speed);
    Serial.print("Angle: "); Serial.println(GPS.angle);
    Serial.print("Altitude: "); Serial.println(GPS.altitude);
    Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
  }
  return gps_data;
}
