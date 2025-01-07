#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "67585";

struct package1
{
  int x = 0;
  int y = 0;
  char state = '0';
};

struct package1 data1;

char oldState = '0';
int count = 0;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.stopListening();

  pinMode(4, INPUT_PULLUP);
}

void loop() 
{
  int yAxis = analogRead(A0);
  int xAxis = analogRead(A1);

  xAxis = map(xAxis, 0, 674, 0, 180);
  yAxis = map(yAxis, 0, 674, 0, 180);
  
  char currentState = digitalRead(4) ? '0' : '1';

  if (oldState != currentState)
  {
    oldState = currentState;
    count++;
  }

  if (count == 1 || count == 2) data1.state = '0';
  else if (count == 3 || count == 4) data1.state = '1';
  else count = 1;

  data1.x = xAxis;
  data1.y = yAxis;
  radio.write(&data1, sizeof(data1));

  Serial.print("x: ");
  Serial.print(data1.x);
  Serial.print(" y: ");
  Serial.print(data1.y);
  Serial.print(" state: ");
  Serial.println(data1.state);

  delay(1);
}

