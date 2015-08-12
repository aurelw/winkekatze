#include <Servo.h>
 
Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created
 
/* for a simple wave */
const int homePos = 130;
const int maxPos = 200;
const int minPos = 100;
/*********************/

const int ledPin = 13;
const int servoPin = 9;

int pos = homePos;    // variable to store the servo position
 
void setup()
{
  pinMode(ledPin, OUTPUT);      
  Serial.begin(9600);
  //Serial.begin(19200);
  goHome();
}


void goToPos(int pos) {
  if (pos < 0 || pos > 1028) return;
  
  myservo.attach(servoPin);
  myservo.write(homePos);
  myservo.detach();
}


void goHome() {
  goToPos(homePos);
}


void waveOnce(int wait) {
  myservo.attach(servoPin);
  
  int startPos = pos;
 
  for(pos = startPos; pos < maxPos; pos += 3)  // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(3*wait);                       // waits 15ms for the servo to reach the position
  }
  for(pos = maxPos; pos >= minPos; pos-=3)     // goes from 180 degrees to 0 degrees
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(3*wait);                       // waits 15ms for the servo to reach the position
  }
  delay(wait);

  myservo.detach();
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

    if (cbuffer.equals("WINKFAST")) {
      waveOnce(3);
      waveOnce(3);
      waveOnce(3);
    }

    if (cbuffer.equals("LIGHT_ON")) {
      Serial.println("light on new");
      digitalWrite(ledPin, HIGH);
    }

    if (cbuffer.equals("LIGHT_OFF")) {
      Serial.println("light off 2");
      digitalWrite(ledPin, LOW);
    }

    if (cbuffer.startsWith("POS")) {
      cbuffer.remove(0, 4);
      int toPos = cbuffer.toInt();
      goToPos(toPos);
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

