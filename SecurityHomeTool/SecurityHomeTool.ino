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


// these constants won't change:
const int ledPin = 13;      // LED connected to digital pin 13
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 50;  // threshold value to decide when the detected sound is a knock or not


// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int ledState = LOW;         // variable used to store the last LED status, to toggle the light
int counter = 0;
unsigned long startTime = 0.0;
unsigned long currentTime = 0.0;
unsigned long knockTimes[10];
unsigned long knockAttempt[10];
unsigned int i = 0;
bool secondKnock = true;

void setup() {
  pinMode(A0, INPUT);
  pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  Serial.begin(9600);       // use the serial port
}

void loop() {
  if(i == 0)
  {
    // record initial knock password
    recordPattern(knockTimes);
  }
  else
  {
    // listen for knock pattern to unlock
    while(!determinePattern())
    {
      
    }
  }
  delay(100);  // delay to avoid overloading the serial port buffer
}

void recordPattern(unsigned long* arr) 
{
  while(true)
  {
    sensorReading = analogRead(knockSensor);
    if((millis() - startTime) > 5000)
    {
      Serial.println("Knock pattern stored");
      // listen for the knock pattern
      for(int ind = 0; ind < i; ++ind)
      {
        Serial.println(knockTimes[ind]);
        return;
      }
    }
  
    // if the sensor reading is greater than the threshold:
    if (sensorReading >= threshold) 
    {
      if(secondKnock == true && i == 0)                  // listen for very first knock
      {
        startTime = millis();                            // start timer
        Serial.print("SecondKnock true ");
        Serial.println(secondKnock);
        secondKnock = false;                             // set the boolean
      }
      else if(secondKnock == true && i > 0)
      {
        currentTime = millis();
        arr[i] = currentTime - startTime;
        ++i;
        Serial.print("SecondKnock true ");
        Serial.println(secondKnock);      
        secondKnock = false;
      }
      else                                               // listen for second knock
      {
        currentTime = millis();                          // get current time
        arr[i] = currentTime - startTime;                // store time in between knocks
        ++i;
        startTime, currentTime = 0.0;
        startTime = millis();                            // restart the time for next knock
        Serial.print("SecondKnock false ");
        Serial.println(secondKnock);
        secondKnock = true;
      }
      // toggle the status of the ledPin:
      ledState = !ledState;
      // update the LED pin itself:
      digitalWrite(ledPin, ledState);
      // send the string "Knock!" back to the computer, followed by newline
      Serial.println(analogRead(knockSensor));
    }    
  }
}

boolean determinePattern()
{
  recordPattern(knockAttempt);
  return true;
}

