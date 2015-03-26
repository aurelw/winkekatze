// Sweep
// by BARRAGAN <http://barraganstudio.com>u
// This example code is in the public domain.


#include <Servo.h>
#include <SoftwareSerial.h>

 
Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created
 
const int homePos = 20;
int pos = 0;    // variable to store the servo position
 
void setup()
{
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(14400);
  goHome();
}


void goHome() {
  myservo.write(homePos);
}


void waveOnce(int speed) {
  int startPos = pos;
  int waitms = 15;
 
  /*
  for (pos = startPos; pos<180; pos++) {
    myservo.write(pos);
    delay(waitms);
  }
  
  for (pos=180; pos>0; pos--) {
    myservo.write(pos);
    delay(waitms);
  }
  
  for (pos=0; pos<startPos; pos++) {
   myservo.write(pos);
   delay(waitms);
  }
  */
  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
  
}



void handleSerial() {
  if (Serial.available()) {
    String cbuffer;
    int cbuffer_len = 256;

    /* Get the command string */
    char c;
    for(int i=0; i<cbuffer_len; i++) {
      c = Serial.read();
      //FIXME \n is not detected
      if (c == ';' || c == '\n') {
        break;
      }
      cbuffer += c;
    }
    
    /* handle special reset command */
    if (cbuffer.equals("RESET")) {
      Serial.println("R");
      goHome();
    }
    
    if (cbuffer.equals("W")) {
      Serial.println("W");
      waveOnce(10);
    }
    
    if (cbuffer.equals("W3")) {
      Serial.println("W3");
      waveOnce(10);
      waveOnce(10);
      waveOnce(10);
    }
    
    char buffer_foo[256];
    cbuffer.toCharArray(buffer_foo, 256);
    Serial.println(buffer_foo);
    Serial.println("SERIAL");
    
  }

}

 
void loop()
{
  handleSerial();
}

