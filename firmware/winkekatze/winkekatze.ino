#include <Servo.h>
 
Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created
 
const int homePos = 20;
const int maxPos = 90;

int pos = 0;    // variable to store the servo position
 
void setup()
{
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(19200);
  goHome();
}


void goHome() {
  myservo.write(homePos);
}


void waveOnce(int wait) {
  int startPos = pos;
 
  for(pos = 0; pos < maxPos; pos += 3)  // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(3*wait);                       // waits 15ms for the servo to reach the position
  }
  for(pos = maxPos; pos>=1; pos-=3)     // goes from 180 degrees to 0 degrees
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(3*wait);                       // waits 15ms for the servo to reach the position
  }
  delay(wait);
}


void handleSerial() {
  if (Serial.available()) {
    String cbuffer;
    int cbuffer_len = 128;

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
//      Serial.println("R");
      goHome();
    }
    
    if (cbuffer.equals("WINK")) {
 //     Serial.println("WINK");
      waveOnce(10);
    }
    
    if (cbuffer.equals("WINK3")) {
 //     Serial.println("WINK3");
      waveOnce(10);
      waveOnce(10);
      waveOnce(10);
    }
    /*
    char buffer_foo[256];
    cbuffer.toCharArray(buffer_foo, 128);
    Serial.println(buffer_foo);
    Serial.println("SERIAL");
    */
  }

}

 
void loop()
{
  handleSerial();
  delay(100);
}

