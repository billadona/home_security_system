/*
  Knock Sensor

  This sketch reads a piezo element to detect a knocking sound.
  It reads an analog pin and compares the result to a set threshold.
  If the result is greater than the threshold, it writes "knock" to the serial
  port, and toggles the LED on pin 13.

  The circuit:
  - positive connection of the piezo attached to analog in 0
  - negative connection of the piezo attached to ground
  - 1 megohm resistor attached from analog in 0 to ground

  created 25 Mar 2007
  by David Cuartielles <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Knock
*/


#include <Servo.h>

Servo servo;

// these constants won't change:
const int ledPin = 13;      // LED connected to digital pin 13
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not
const int lockButtonPin = 2;
const int servoPin = 4;

// RGB LED pins
const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;


// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int ledState = LOW;         // variable used to store the last LED status, to toggle the light
long knockTimes[10];
long knockAttempt[10];
unsigned int i = 0;
unsigned int j = 0;
bool patternRecorded = false;
int lockButtonState = 0;
int angle = 0;


void setup() {
  pinMode(A0, INPUT);
  pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  pinMode(lockButtonPin, INPUT);
  servo.attach(servoPin);
  Serial.begin(9600);       // use the serial port
}

void loop() {
  if(i == 0)
  {
    // record initial knock password
    recordPattern(knockTimes, i);
    Serial.println("Knock pattern stored");
    for(int ind = 0; ind < i; ind++)
    {
      Serial.println(knockTimes[ind]);
    }    
  }
  else
  {
    // listen for knock pattern to unlock
    Serial.println("listening");

    // lock logic
    Serial.println("Reading lock button state");
    lockButtonState = digitalRead(lockButtonPin);
    // when the lock button is pressed, turn the servo to the locked position
    if (lockButtonState == 1) {
      Serial.println("Lock Button Pressed");
      for (angle = 0; angle < 300; ++angle) {
        servo.write(angle);
        delay(5);
      }
    } 

    lockButtonState = 0;
    
    recordPattern(knockAttempt, j);
    for(int ind = 0; ind < j; ++ind)
    {
      Serial.println(knockAttempt[ind]);
    }
    Serial.println();
    if(doesItMatch(knockTimes, knockAttempt, i))
    {
      Serial.println("Unlock!~");
      setColourRgb(0,255,0);

      // spin the motor to unlock
      for (angle = 300; angle > 0; --angle) {
        servo.write(angle);
        delay(5);
      }
    
      delay(1000);
      setColourRgb(0,0,0);
      for(int ind = 0; ind < j; ++ind)
      {
        knockAttempt[ind] = 0.0;
      }
      j = 0; 
    }
    else
    {
      setColourRgb(255,0,0);
      delay(100);
      setColourRgb(0,0,0);
      delay(100);
      setColourRgb(255,0,0);
      delay(100);
      setColourRgb(0,0,0);
      for(int ind = 0; ind < j; ++j)
      {
        knockAttempt[ind] = 0.0;
      }
      j = 0;
    }
  }
}

void recordPattern(long* arr, unsigned int& i) 
{
  unsigned long startTime = millis();
  unsigned long currentTime = 0.0;
  bool secondKnock = true;
  while(true)
  {
    sensorReading = analogRead(knockSensor);
    
    // lock logic
    //Serial.println("Reading lock button state in LONG PART");
    lockButtonState = digitalRead(lockButtonPin);
    // when the lock button is pressed, turn the servo to the locked position
    if (lockButtonState == 1) {
      Serial.println("Lock Button Pressed in LONG PART");
      for (angle = 0; angle < 100; ++angle) {
        servo.write(angle);
        delay(10);
      }
    } 
    lockButtonState = 0;
    
    if((millis() - startTime) > 5000)
    {
      break;
    }
    // if the sensor reading is greater than the threshold:
    if (sensorReading >= threshold) 
    {
      if(secondKnock == true && i == 0)                  // listen for very first knock
      {
        startTime = millis();                            // start timer
        Serial.println(i);
        secondKnock = false;                             // set the boolean
        Serial.println(analogRead(knockSensor));         
      }
      else if(secondKnock == true && i > 0)
      {
        currentTime = millis();
        arr[i] = currentTime - startTime;
        ++i;
        Serial.println(i);    
        secondKnock = false;
        Serial.println(analogRead(knockSensor));
      }
      else                                               // listen for second knock
      {
        currentTime = millis();                          // get current time
        arr[i] = currentTime - startTime;                // store time in between knocks
        ++i;
        startTime, currentTime = 0.0;
        startTime = millis();                            // restart the time for next knock
        Serial.println(i);        
        secondKnock = true;
        Serial.println(analogRead(knockSensor));
      }
      setColourRgb(255,165,0);
      delay(50);
      setColourRgb(0,0,0);
    } 
    sensorReading = 0; 
    delay(70);  
  }
  Serial.println("Pattern 1");
}

bool doesItMatch(const long* arr1, const long* arr2, const unsigned int i)
{
  long temp = 0.0;
  if(i != j)
  {
    i, j = 0; 
    Serial.println("false: i != j");
    
    return false;
  }
  Serial.println();
  for(int ind = 0; ind < i; ++ind)
  {
    temp = arr1[ind] - arr2[ind];
    if(temp < 400)
    {
      Serial.println(arr1[ind] - arr2[ind]);
    }
    if(temp > -400)
    {
      Serial.println(arr1[ind] - arr2[ind]);
    }
    else
    {
      Serial.println("false: broke threshold");
      return false;
    }
  }
  Serial.println("true");
  return true;
}

void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
 }

