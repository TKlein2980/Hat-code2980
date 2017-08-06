#include <LIDARLite.h>
#include <Wire.h>
//#include <Ultrasonic.h>

int interval;

unsigned long previousMillis = millis();
int pulseState = LOW;

int group1[4] = {7,8,12,13};
int group2[3] = {2,7,8};
int group3[2] = {12,13};
int group4[2] = {2,7};
int group5[4] = {2,7,8,12};
int wholeGroup[5] = {2,7,8,12,13};

float aScale;
int prevButState = 0;
float vol;
int sigVal;

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

LIDARLite myLidarLite; 

void setState(int arr[], int state){
  for(int i = 0; i < 5; i++){
    digitalWrite(arr[i], state);
  }

  return;
}

void normalMode(double dist){

  sigVal = analogRead(A0);
  aScale = fscale(0,1023,0.5,2,sigVal,0);
  vol = dist * aScale;

    if (vol == 0) {
      return setState(wholeGroup, LOW);
    }
    else if (0 < vol <= 50) {
      return setState(wholeGroup, pulseState);
    }
    else if (50 < vol <= 100) {
       return setState(wholeGroup, pulseState);
    }
    else if (100 < vol <= 160) {
       return setState(wholeGroup, pulseState);
    }
    else if (160 < vol <= 200) {
      return setState(wholeGroup, pulseState);
    }
     else
    {
      return setState(wholeGroup, LOW);
    }

}

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  //pinMode(3, INPUT);
  Serial.begin(115200);
  myLidarLite.begin(0, true);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
  readings[thisReading] = 0;
  }
}

void loop() {

  unsigned int currentMillis = millis();
 
  int lidarDistance = myLidarLite.distance(false);

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = lidarDistance;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  delay(1);        // delay in between reads for stability
  
  sigVal = analogRead(A0);
  aScale = fscale(0,1023,0.5,2,sigVal,0);
  vol = average * aScale;
  
  interval = average; //pulses faster  a lot faster...trying to see if this is easier to feel

  
    if(vol == 0) 
    {
      pulseState == LOW;
    }
    else if(vol > 300) 
    {
      pulseState == LOW;
      setState(wholeGroup, pulseState);
    }
    if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;        
        if(pulseState == LOW){
          pulseState = HIGH;
          setState(wholeGroup, pulseState);
        }
        else {
          pulseState = LOW;
          setState(wholeGroup, pulseState);
        }
    }
    
  
  
  Serial.println(vol);

}


float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float
  

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}
