//#define DEBUG

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <DoubleResetDetect.h>

#define DRD_TIMEOUT 2.0
#define DRD_ADDRESS 0x00

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);
WiFiManager wifiManager;

//flag for saving data
bool shouldSaveConfig = false;

char menu_buff[1];
int menu_pin = 0;
int brightness = 100;

// R G B values used for the Output, 0...1023
int R = 1023;
int G = 1023;
int B = 1023;

// Outputpins
#define R_LED D1
#define G_LED D2
#define B_LED D3

void push() {
  analogWrite(R_LED, R * brightness / 100);
  analogWrite(G_LED, G * brightness / 100);
  analogWrite(B_LED, B * brightness / 100);
  Blynk.run();
#ifdef DEBUG
  Serial.printf("\nRGB-Values: %d %d %d | %d % Brightness", R, G, B, brightness);
#endif
}

void setupSpiffs() {
  //clean FS, for testing
  // SPIFFS.format();
  //read configuration from FS json
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config1.json")) {
      //file exists, reading and loading
      File configFile = SPIFFS.open("/config1.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
#ifdef DEBUG
          Serial.println("\nparsed json");
#endif
          // copy json data in variable
          strcpy(menu_buff, json["menu_pin"]);
          menu_pin = atoi(menu_buff);
          Serial.println(menu_pin);
        }
      }
    }
  } 
  else {
#ifdef DEBUG
    Serial.println("failed to mount FS");
#endif
  }
}

//Colorfunction Police
void police() {
  //red
  R = 1023; G = 0; B = 0;
  push();
  delay(150);

  R = 0; G = 0; B = 0;
  push();
  delay(40);

  R = 1023; G = 0; B = 0;
  push();
  delay(100);

  R = 0; G = 0; B = 0;
  push();
  delay(40);

  R = 1023; G = 0; B = 0;
  push();
  delay(200);

  R = 0; G = 0; B = 0;
  push();
  delay(40);


  // blue
  R = 0; G = 0; B = 1023;
  push();
  delay(150);

  R = 0; G = 0; B = 0;
  push();
  delay(40);

  R = 0; G = 0; B = 1023;
  push();
  delay(100);

  R = 0; G = 0; B = 0;
  push();
  delay(40);

  R = 0; G = 0; B = 1023;
  push();
  delay(200);

  R = 0; G = 0; B = 0;
  push();
  delay(40);
}

//Colorfunction Regenbogen
void rainbow(int del) {
  R = 1023; G = 0; B = 0;
  for (int i = 0; i < 1023; i += 3) {
    G += 3;
    push();
    delay(del);
  }

  for (int i = 1023; i > 0; i -= 3) {
    R -= 3;
    push();
    delay(del);
  }
    
  for (int i = 0; i < 1023; i += 3) {
    B += 3;
    push();
    delay(del);
  }

  for (int i = 1023; i > 0; i -= 3) {
    G -= 3;
    push();
    delay(del);
  }

  for (int i = 0; i < 1023; i += 3) {
    R += 3;
    push();
    delay(del);
  }

  for (int i = 1023; i > 0; i -= 3) {
    B -= 3;
    push();
    delay(del);
  }
}

//Colorfunction Atmung
int breathing(int del)
{
  brightness = 100;
  push();
  //downwards
  while (brightness >= 0) {
    brightness--;
    push();
    delay(del);
  }
  //black
  push();
  delay(del * 15);
  //upwards
  while (brightness <= 100) {
    brightness++;
    push();
    delay(del);
  }
  //on
  delay(del * 15);
}

//Colorfunction epilepsy
void epilepsy(int del) {
  //white
  R = 1023; G = 1023; B = 1023;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);

  //red
  R = 1023; G = 0; B = 0;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);

  //green
  R = 0; G = 1023; B = 0;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);

  //blue
  R = 0; G = 0; B = 1023;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  pinMode(D4, OUTPUT);
  push();
  
  if (drd.detect()) {
#ifdef DEBUG
    Serial.println("Entered Double Reset Mode");
#endif
    digitalWrite(D4, LOW); //turns bultin led on

    setupSpiffs();
    shouldSaveConfig = true;
    //save the custom parameters to FS
    if (shouldSaveConfig) {
#ifdef DEBUG
      Serial.println("saving config");
#endif
      if (menu_pin < 4) {
        menu_pin++;
      }
      else if (menu_pin == 4) {
        menu_pin = 0;
      }
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["menu_pin"] = menu_pin;

      File configFile = SPIFFS.open("/config1.json", "w");
#ifdef DEBUG
      if (!configFile) {
        Serial.println("failed to open config file for writing");
      }
#endif
      json.prettyPrintTo(Serial);
      json.printTo(configFile);
      configFile.close();
      //end save
      shouldSaveConfig = false;
    }
    digitalWrite(D4, HIGH); //turns bultin led off
  } 
  else {
    setupSpiffs();
    digitalWrite(D4, HIGH); //turns bultin led off
  }
  
}

void loop() {
  if (menu_pin == 0) {
    R = 1023; G = 1023; B = 1023;
  }
  else if (menu_pin == 1) {
    epilepsy(26);
  }
  else if (menu_pin == 2) {
    rainbow(22);
  }
  else if (menu_pin == 3) {
    breathing(32);
  }
  else if (menu_pin == 4) {
    police();
  }
  push();
}
