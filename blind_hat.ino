int interval = 1000;

unsigned long previousMillis = millis();
int pulseState = LOW;
//double CM_FEET = 30.48;
int group1[] = {5,7,8,12,13};
int group2[] = {4,2,7,8};
int group3[] = {3,12,13};
int group4[] = {3,2,7};
int group5[] = {5,2,7,8,12};
int wholeGroup[] = {6,2,7,8,12,13};
int mode = 0;
int buttonState = 0;
int prevButState = 0;

#include <LIDARLite.h>
#include <Wire.h>
//#include <Ultrasonic.h>

LIDARLite myLidarLite;
//Ultrasonic u(6,5); 

/**
 * changes the mode if button is pressed
 */
void buttonPressed(){
  mode = (mode + 1) % 3;
}
/**
 * sets an array of pins to given state
 */
void setState(int* arr, int state){
  for(int i = 1; i < arr[0]; i++){
    digitalWrite(arr[i], state);
  }

  return;
}
/**
 * checks if x is in [a,b]
 */
boolean inRange(double x, int a, int b){
  return a <= x and b >= x;
}

void normalMode(double dist){


    if(mode == 1){
      dist *= 2.05;
    }
    else if(mode == 2){
      dist *= 5;
    }

    
    if (dist < 5) {
      digitalWrite(2, pulseState);
    }
    if (inRange(dist, 3,4)) {
       setState(group4, pulseState);
    }
    else
    {
      setState(group1, LOW);
    }
     if (inRange(dist,2,3)) {
        setState(group2, pulseState);
    }
     else
    {
      setState(group1, LOW);
    }
    if (inRange(dist,1,2)) {
      setState(group5, pulseState);
    }
     else
    {
      setState(group1, LOW);
    }
    if (dist < 1) {
      setState(wholeGroup, pulseState);
    }
     else
    {
      setState(group1, LOW);;
    }
  
  
  if(dist < 0.75){
    setState(group3, HIGH);
  }

 /**if(dist > 75 and dist <= 100){
     setState(group3, HIGH);
 }  */
}

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(3, INPUT);
  Serial.begin(115200);
  myLidarLite.begin(0, true);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(3);
  if(buttonState != prevButState){
    buttonPressed();
    prevButState = buttonState;
  }
  
  unsigned int currentMillis = millis();
  
  Serial.println(myLidarLite.distance(false));
  //int uDist = u.Ranging(CM);
  //Serial.print(uDist + ":::::::::::::::::::::::");
  int lidarDistance = myLidarLite.distance(false);
  interval = (lidarDistance); //pulses faster  a lot faster...trying to see if this is easier to feel
  //Serial.print(lidarDistance);

  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;

    if(pulseState == LOW){
      pulseState = HIGH;
    }
    else {
      pulseState = LOW;
    }
  }
  //converts the distance into meters
  double dist = (lidarDistance ) / 100;
  normalMode(dist);

}

