#include <Servo.h>

Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created
 
/* for a simple wave */
const int homePos = 70;
const int maxPos = 110;
const int minPos = 30;
/*********************/

const int ledPin = 13;
const int servoPin = 16;
const int neoPixelPin_body = 16;
const int neoPixelPin_paw = 17;
//Adafruit_NeoPixel pixels_paw = Adafruit_NeoPixel(1, neoPixelPin_paw, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel pixels_body = Adafruit_NeoPixel(8, neoPixelPin_body, NEO_GRB + NEO_KHZ800);

int globPos = homePos;    // variable to store the servo position
 
void setup()
{
  //pixels_paw.begin();
  //pixels_body.begin();
  //pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  //Serial.begin(19200);
  goHome();
}


void goToPos(int pos) {
  if (pos < 0 || pos > 1028) return;
  
  myservo.attach(servoPin);
  myservo.write(pos);
  delay(250);
  myservo.detach();
  globPos = pos;
}


void goHome() {
  goToPos(homePos);
}


void waveOnce(int wait) {
  myservo.attach(servoPin);
  
  int startPos = globPos;
  int pos;
  
  for (pos = startPos; pos > minPos; pos -= 1)
  {                                 
    myservo.write(pos);             
    delay(wait);                      
  }
  for (pos = minPos; pos <= maxPos; pos += 1)     
  {                                
    myservo.write(pos);             
    delay(wait);                       
  }
  for (pos = maxPos; pos >= homePos; pos -= 1) {
    myservo.write(pos);             
    delay(wait);   
  }
  delay(wait);

  globPos = pos;

  myservo.detach();
}


void waveFurious(int wait) {
  setPawColor(255,0,0);
  setBodyColor(255,255,0);
  waveOnce(wait);
  setPawColor(255,255,0);
  setBodyColor(0,255,255);
  waveOnce(wait);
  setPawColor(0,255,255);
  setBodyColor(255,0,255);
  waveOnce(wait);
  setPawColor(0,0,255);
  waveOnce(wait);
  setPawColor(255,255,255);
  setBodyColor(255,255,255);
  waveOnce(wait);
}


void setPawColor(uint8_t r, uint8_t g, uint8_t b) {
  //pixels_paw.setPixelColor(0, pixels_paw.Color(g,r,b));
  //pixels_paw.show();
}


void setBodyColor(int i, uint8_t r, uint8_t g, uint8_t b) {
  //pixels_body.setPixelColor(i, pixels_body.Color(r,g,b));
  //pixels_body.show();
}


void setBodyColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i=0; i<8; i++) {
    setBodyColor(i,r,g,b);
  }
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
    
    if (cbuffer.startsWith("POS")) {
      cbuffer.remove(0, 4);
      int toPos = cbuffer.toInt();
      goToPos(toPos);
    }

    /* handle special reset command */
    if (cbuffer.equals("RESET")) {
      goHome();
    }
    
    if (cbuffer.equals("WINK")) {
      waveOnce(10);
    }
    
    if (cbuffer.equals("WINK3")) {
      waveOnce(10);
      waveOnce(10);
      waveOnce(10);
    }

    if (cbuffer.equals("WINKFAST")) {
      waveOnce(3);
      waveOnce(3);
      waveOnce(3);
    }

    if (cbuffer.equals("WINKFURIOUS")) {
      waveFurious(3); 
    }

    if (cbuffer.equals("LIGHT_ON")) {
      Serial.println("light on 2");
      digitalWrite(ledPin, HIGH);
      setPawColor(255,255,255);
      setBodyColor(255,255,255);
    }

    if (cbuffer.equals("LIGHT_OFF")) {
      Serial.println("light off 2");
      digitalWrite(ledPin, LOW);
      setPawColor(0,0,0);
      setBodyColor(0,0,0);
    }

    if (cbuffer.equals("PAW_RED")) {
      setPawColor(255,0,0);
    }

    if (cbuffer.equals("PAW_GREEN")) {
      setPawColor(0,255,0);
    }
 
    if (cbuffer.equals("PAW_BLUE")) {
      setPawColor(0,0,255);
    }

    if (cbuffer.equals("PAW_WHITE")) {
      setPawColor(255,255,255);
    }
    
    if (cbuffer.equals("BODY_RED")) {
      setBodyColor(255,0,0);
    }

    if (cbuffer.equals("BODY_GREEN")) {
      setBodyColor(0,255,0);
    }

    if (cbuffer.equals("BODY_BLUE")) {
      setBodyColor(0,0,255);
    }
    
    if (cbuffer.equals("BODY_WHITE")) {
      setBodyColor(255,255,255);
    }

    if (cbuffer.equals("BODY_ROSE")) {
      setBodyColor(255,80,80);
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

