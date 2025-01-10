#include "arduino_secrets.h"
/*

Things to add:
-> Send connection data to arduino to display connection status
-> Maybe more sensors:
   -> Ultrasonic
   -> *Atmospheric Pressure
   -> *Humidity
   -> Gas sensor
   -> *GPS sensor
-> *Fitting / Good Idea

Possible Workflow:
   -> Arduino Collects & Sends data to ESP8266
   -> ESP8266 Sends Collected Data to Station
   -> HC-05 Recieves Movement Data from Station
   -> HC-05 Sends Movement Data to Arduino
   -> Arduino moves the rover
   -> ESP displays connection status on LED
   -> HC-05 displays connection status on LED

*/

#include "thingProperties.h"

float latitude = 0.00;
float longitude = 0.00;
float temperature = 0.00;
float pres = 0.00;

void setup() 
{
  pinMode(12, OUTPUT);
  
  Serial.begin(9600);
  
  delay(1500); 

  initProperties();
  
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
}

void loop() 
{
  ArduinoCloud.update();
  
  if (ArduinoCloud.connected() == 1)
  {
    Serial.print('c');
    Serial.print(';');
  }
  else
  {
    Serial.print('d');
    Serial.print(';');
  }
  
  if (Serial.available())
  {
    String message = Serial.readStringUntil(';');
    if (message.charAt(0) == 'l')
    {
      latitude = message.substring(1, message.indexOf(':')).toFloat();
      longitude = message.substring(message.indexOf(':')+1, message.indexOf(';')).toFloat();
    }
    else if (message.charAt(0) == 't')
    {
      temperature = message.substring(1, message.indexOf(';')).toFloat();
    }
    else if (message.charAt(0) == 'p')
    {
      pres = message.substring(1, message.indexOf(';')).toFloat();
    }
  }
  
  location = Location(latitude, longitude);
  temp = temperature;
  pressure = pres;
}
/*
  Since Pressure is READ_WRITE variable, onPressureChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onPressureChange()  {
  // Add your code here to act upon Pressure change
}