#include <SPI.h>
#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

TinyGPSPlus gps;
SoftwareSerial GPS(9, 10); //TX, RX

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "67585";

struct package1
{
  int x = 0;
  int y = 0;
  char state = 'i';
};

struct package1 data1;

int motor1_dir_1 = 2;
int motor1_dir_2 = 3;

int motor2_dir_1 = 4;
int motor2_dir_2 = 5;

float temperature = 0;
float pressure = 0;

int ledIndicatorPinGreen = A1;

int headLightPin = A3;
bool headLightOn = true;
char oldState = '2';

void setup() 
{
  Serial.begin(9600);

  bmp.begin(0x76, 0x60);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);

  GPS.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.startListening();

  pinMode(motor1_dir_1, OUTPUT);
  pinMode(motor1_dir_2, OUTPUT);

  pinMode(motor2_dir_1, OUTPUT);
  pinMode(motor2_dir_2, OUTPUT);

  pinMode(ledIndicatorPinGreen, OUTPUT);
  pinMode(headLightPin, OUTPUT);
}

void loop() 
{
  if (radio.available()) 
  {
    radio.read(&data1, sizeof(data1));

    headLightOn = (data1.state == '0');

    if (data1.x > 150)
    {
      digitalWrite(motor1_dir_1, HIGH);
      digitalWrite(motor1_dir_2, LOW);

      digitalWrite(motor2_dir_1, LOW);
      digitalWrite(motor2_dir_2, HIGH);
    }
    else if (data1.x < 80)
    {
      digitalWrite(motor1_dir_1, LOW);
      digitalWrite(motor1_dir_2, HIGH);

      digitalWrite(motor2_dir_1, HIGH);
      digitalWrite(motor2_dir_2, LOW);
    }
    else
    {
      if (data1.y > 150)
      {
        digitalWrite(motor1_dir_1, HIGH);
        digitalWrite(motor1_dir_2, LOW);

        digitalWrite(motor2_dir_1, HIGH);
        digitalWrite(motor2_dir_2, LOW);
      }
      else if (data1.y < 80)
      {
        digitalWrite(motor1_dir_1, LOW);
        digitalWrite(motor1_dir_2, HIGH);

        digitalWrite(motor2_dir_1, LOW);
        digitalWrite(motor2_dir_2, HIGH);
      }
      else
      {
        digitalWrite(motor1_dir_1, LOW);
        digitalWrite(motor1_dir_2, LOW);

        digitalWrite(motor2_dir_1, LOW);
        digitalWrite(motor2_dir_2, LOW); 
      }
    }
  }

  analogWrite(headLightPin, (headLightOn) ? 255 : 0);

  if (Serial.available())
  {
    String message = Serial.readStringUntil(';');

    if (message.charAt(0) == 'c')
    {
      analogWrite(ledIndicatorPinGreen, 255);
    }
    else if (message.charAt(0) == 'd')
    {
      analogWrite(ledIndicatorPinGreen, 0);
    }
  }

  if (GPS.available() > 0)
  {
    gps.encode(GPS.read());

    if (gps.location.isValid())
    {
      Serial.print("l"); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(":"); 
      Serial.print(gps.location.lng(), 6);
      Serial.print(";");
    }
  }

  temperature = (bmp.readTemperature() * 9 / 5) + 32;

  Serial.print("t");
  Serial.print(temperature);
  Serial.print(";");

  pressure = bmp.readPressure();

  Serial.print("p");
  Serial.print(pressure);
  Serial.print(";");
}
