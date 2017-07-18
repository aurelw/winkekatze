/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include "Arduino.h"

//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "FastLED.h"
#include "Servo.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

extern "C" {
#include "user_interface.h"
}

#include "wifi_credentials.h"

/* pin out */
#define LED_DATA_PIN 3
#define SERVO_DATA_PIN 13 

/* led animation timer */
os_timer_t timer0;
int timerCounter = 0;
const int timerInterval = 5;
/***********************/

/* for a simple wave */
const int homePos = 80;
const int maxPos = 140;
const int minPos = 40;
int globPos = homePos;
Servo pawservo;
/*********************/

/* LEDS */
#define NUM_LEDS 16
#define BODY_LED_START 0
#define BODY_LED_END 7
#define PAW_LED_START 8
#define PAW_LED_END 15
CRGB leds [NUM_LEDS];

enum LightMode {
    STATIC_LIGHT,
    RED_FLASH,
    GREEN_FLASH,
    BLUE_FLASH,
    PURPLE_FLASH,
    STROBO,
    NUM_LIGHT_MODES
};
const char* LightModeStrings[] = {
    "STATIC_LIGHT",
    "RED_FLASH",
    "GREEN_FLASH",
    "BLUE_FLASH",
    "PURPLE_FLASH",
    "STROBO",
};
LightMode currentLightMode = RED_FLASH;
/*********************/


/* Network */
ESP8266WiFiMulti wifiMulti;
IPAddress MQTTserver(158, 255, 212, 248);
WiFiClient wclient;
PubSubClient client(MQTTserver, 1883, wclient);
/***********/


void waveOnce(int wait) {
    pawservo.attach(SERVO_DATA_PIN);
    delay(wait);

    int startPos = globPos;
    int pos;

    for (pos = startPos; pos <= maxPos; pos += 1) {
        pawservo.write(pos);             
        delay(wait);                       
    }
    for (pos = maxPos; pos > minPos; pos -= 1) {
        pawservo.write(pos);             
        delay(wait);                      
    }
    for (pos = minPos; pos <= homePos; pos += 1) {
        pawservo.write(pos);             
        delay(wait);   
    }

    delay(wait);
    globPos = pos;

    pawservo.detach();
}


void resetAllLeds() {
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
}


void setPawColor(CRGB color) {
    for (int i=PAW_LED_START; i<=PAW_LED_END; i++) {
        leds[i] = color;
    }
    FastLED.show();
}


void setBodyColor(CRGB color) {
    for (int i=BODY_LED_START; i<=BODY_LED_END; i++) {
        leds[i] = color;
    }
    FastLED.show();
}


void runningFlash(unsigned int wait=50, CRGB color=CRGB::Red) {
    static unsigned int cpos = 0;
    static unsigned long lastUpdate = millis();

    int cmillis = millis();
    if (lastUpdate + wait < cmillis) {
        lastUpdate = cmillis;

        /* do animation frame */
        resetAllLeds();
        leds[(cpos) % NUM_LEDS] = color;
        leds[(cpos) % NUM_LEDS].fadeLightBy(230);
        leds[(cpos + 1) % NUM_LEDS] = color;
        leds[(cpos + 1) % NUM_LEDS].fadeLightBy(200);
        leds[(cpos + 2) % NUM_LEDS] = color;
        leds[(cpos + 2) % NUM_LEDS].fadeLightBy(100);
        leds[(cpos + 3) % NUM_LEDS] = color;
        //leds[((int)(counter*0.5)) % NUM_LEDS] = CRGB::Red;
        FastLED.show();

        cpos++;
    }
}


void strobo(unsigned int wait=100) {
    if ((millis() / wait) % 2 == 1) {
        setBodyColor(CRGB::White);
        setPawColor(CRGB::White);
    } else {
        setBodyColor(CRGB::Black);
        setPawColor(CRGB::Black);
    }
}


/* inefficient but i see no better way atm */
inline String byteArrayToString(byte* array, unsigned int length) {
    char chars[length+1];
    for (int i=0; i<length; i++) {
        chars[i] =  (char) array[i];
    }
    chars[length] = '\0';
    return chars;
}


void timerCallback(void *pArg) {
    if (currentLightMode == RED_FLASH) {
        runningFlash(50);
    }
    else if (currentLightMode == GREEN_FLASH) {
        runningFlash(50, CRGB::Green);
    }
    else if (currentLightMode == BLUE_FLASH) {
        runningFlash(50, CRGB::Blue);
    }
    else if (currentLightMode == PURPLE_FLASH) {
        runningFlash(50, CRGB::Purple);
    }
    else if (currentLightMode == STROBO) {
        strobo();
    }
}


void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  
  String msg = byteArrayToString(payload, length);
  Serial.print("msg length: ");
  //Serial.println(length);
  Serial.print("string length: ");
  //Serial.println(msg.length());
  Serial.print("msg received: ");
  //Serial.println("_" + msg + "_");
  //Serial.println(msg.substring(0, 10));
  
  if (msg == "WINK") {
    waveOnce(15);
  }
  else if (msg == "WINK3") {
    waveOnce(10);
    waveOnce(10);
    waveOnce(10);
  }
  else if (msg == "WINKFAST") {
    waveOnce(7);
    waveOnce(7);
    waveOnce(7);
  }

  else if (msg.substring(0, 11) == "LIGHT_MODE:") {
      for (int lightMode=0; lightMode<NUM_LIGHT_MODES; lightMode++) {
          if (msg.substring(11) == LightModeStrings[lightMode]) {
              currentLightMode = (LightMode) lightMode;
          }
      }
  }

  else if (msg == "PAW_RED") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setPawColor(CRGB::Red);
  }
  else if (msg == "PAW_GREEN") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setPawColor(CRGB::Green);
  }
  else if (msg == "PAW_BLUE") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setPawColor(CRGB::Blue);
  }
  else if (msg == "PAW_BLACK") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setPawColor(CRGB::Black);
  }
  else if (msg == "PAW_WHITE") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setPawColor(CRGB::White);
  }
  else if (msg == "BODY_RED") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setBodyColor(CRGB::Red);
  }
  else if (msg == "BODY_GREEN") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setBodyColor(CRGB::Green);
  }
  else if (msg == "BODY_BLUE") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setBodyColor(CRGB::Blue);
  }
  else if (msg == "BODY_BLACK") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setBodyColor(CRGB::Black);
  }
  else if (msg == "BODY_WHITE") {
      currentLightMode = STATIC_LIGHT;
      delay(timerInterval * 2);
      setBodyColor(CRGB::White);
  }

  
}


void setup()
{
    //Serial.println("setup");
    // initialize LED digital pin as an output.
    FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);

    //Serial.begin(115200);
    delay(1000);

    for (int i=0; i<NUM_WIFI_CREDENTIALS; i++) {
        wifiMulti.addAP(WIFI_CREDENTIALS[i][0], WIFI_CREDENTIALS[i][1]);
    }

    if(wifiMulti.run() == WL_CONNECTED) {
        //Serial.println("Wifi connected.");
    } else {
        //Serial.println("Wifi not connected!");
    }

    /* set up LED animation timer */
    os_timer_setfn(&timer0, timerCallback, &timerCounter);
    os_timer_arm(&timer0, timerInterval, true);
    
    setBodyColor(CRGB::Red);
}
    

void loop()
{
    static unsigned int counter = 0;
    static bool wifiConnected = false;

    //Serial.println("loop");
    //Serial.println(counter);
    delay(100);
#if 1
    /* reconnect wifi */
    if(wifiMulti.run() == WL_CONNECTED) {
        if (!wifiConnected) {
            //Serial.println("Wifi connected.");
            wifiConnected = true;
        }
    } else {
        if (wifiConnected) {
            //Serial.println("Wifi not connected!");
            wifiConnected = false;
        }
    }


    /* MQTT */
    if (client.connected()) {
        client.loop();
    } else {
        if (client.connect("cadskatze", "devlol/winkekatze/cadskatze/online", 0, true, "false")) {
            client.publish("devlol/winkekatze/cadskatze/online", "true", true);
            //Serial.println("MQTT connected");
            client.setCallback(mqtt_callback);
            client.subscribe("devlol/winkekatze/cadskatze/cmd");
        }
    }
#endif
    yield();

    //Serial.println("endloop");

}

