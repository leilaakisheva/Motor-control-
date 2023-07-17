#include <SoftwareSerial.h>
#include <AccelStepper.h>
#include <Wire.h>
#define SLAVE_ADDRESS 0x08

const int pinStep_1 = 12;
const int pinDirection_1 = 13;
const int pinStep_2 = 4;
const int pinDirection_2 = 5;
const int pinStep_3 = 10;
const int pinDirection_3 = 11;
volatile boolean receiveFlag = false;
char temp[32];
String command;
int x;
float x_per;
int y;
float y_per;
float ang;
int steps; // Number of steps
bool minus = false;
int sspeed = 1000;
int maxspeed = 2000;

AccelStepper motor1(1, pinStep_1, pinDirection_1);
AccelStepper motor2(1, pinStep_2, pinDirection_2);
AccelStepper motor3(1, pinStep_3, pinDirection_3);

void receiveEvent(int howMany) {

  for (int i = 0; i < howMany; i++) {
    temp[i] = Wire.read();
    temp[i + 1] = '\0'; //add null after ea. char
  }

  //RPi first byte is cmd byte so shift everything to the left 1 pos so temp contains our string
  for (int i = 0; i < howMany; ++i)
  {
    temp[i] = temp[i + 1];
  }
  int i = 0;
  int j = 0;
  x = 0;
  y = 0;
  while (temp[j] != ',')
  {
    j++;
  }
  for (i = 0; i < j; i++)
  {
    if (temp[i] == '-')
    {
      i++;
      minus = true;
    }
    x = 10 * x + (temp[i] - '0');
  }
  if (minus == true)
  {
    x = x * (-1);
    minus = false;
  }
  for (i = j + 1; i < strlen(temp); i++)
  {
    if (temp[i] == '-')
    {
      i++;
      minus = true;
    }
    y = 10 * y + (temp[i] - '0');
  }
  if (minus == true)
  {
    y = y * (-1);
    minus = false;
  }
  Serial.print("x =");
  Serial.println(x);
  Serial.print("y =");
  Serial.println(y);
  receiveFlag = true;
  x_per = x / 100.0;
  y_per = y / 100.0;
  ang = atan(y_per / x_per) * 180.0 / 3.1415926535;
  Serial.println(ang);
}

void setup() {
  motor1.setMaxSpeed(maxspeed);
  //  motor2.setMaxSpeed(maxspeed);/
  motor3.setMaxSpeed(maxspeed);
  pinMode(8, OUTPUT);
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
}

void loop() {

  steps = ang / 1.8; // Number of steps

  motor1.setSpeed(1200 * y_per);
  // motor2.setSpeed(1200);

  motor3.moveTo(steps);
  //motor3.setSpeed(-100);
  motor1.runSpeed();
  // motor2.runSpeed();
  motor3.run();
}
