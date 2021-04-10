//#define RGBSLIDERCODE
//#define DEBUG
#define remotecontrol

//theoretisch genial, praktisch egal

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

//define your default values here, if there are different values in config.json, they are overwritten.
char api_token[32];
char rgb_changed[10];
char ssid[40];
char pass[40];

//flag for saving data
bool shouldSaveConfig = false;

int menu_pin = 0;
int user_delay = 30;
int rainbow_state = 0;
int brightness = 100;
int manual_pin_state = 0;
int onoff = 1;

// R G B für den Output, 0...1023
int R = 0;
int G = 0;
int B = 0;

#ifdef RGBSLIDERCODE - auskommentieren um die Slider zu verwenden
int r_slider;
int g_slider;
int b_slider;
#endif

//definition der Pins für den Output
/*
#define R_LED D1
#define G_LED D2
#define B_LED D3
*/
uint8_t R_LED;   //D1
uint8_t G_LED;   //D2
uint8_t B_LED;   //D3


//definition der benutzerspezifischen delays für die Farbfunktionen
#define epilepsy_start 40
#define rainbow_start 50
#define breathing_start 25

void push(int bright_temp = brightness) {
  analogWrite(R_LED, R * bright_temp / 100 * onoff);
  analogWrite(G_LED, G * bright_temp / 100 * onoff);
  analogWrite(B_LED, B * bright_temp / 100 * onoff);
  Blynk.run();
#ifdef DEBUG
  Serial.printf("\nRGB-Wert: %d %d %d | %d % Helligkeit | %d ONOFF", R, G, B, bright_temp, onoff);
#endif
}

//read menu pin
BLYNK_WRITE(V0) {
  menu_pin = param.asInt();
#ifdef DEBUG
  Serial.println("Colorfunction has been changed in the menu.");
#endif
  manual_pin_state = 0;
  Blynk.virtualWrite(V2, LOW);
  if (menu_pin == 1) {
    // delay für das Farbpattern automatisch einstellen
    Blynk.virtualWrite(V1, epilepsy_start);
    user_delay = epilepsy_start;
    user_delay = map(user_delay, 1, 100, 40, 1);
  }
  else if (menu_pin == 2) {
    // delay für das Farbpattern automatisch einstellen
    Blynk.virtualWrite(V1, rainbow_start);
    user_delay = rainbow_start;
    user_delay = map(user_delay, 1, 100, 40, 1);
  }
  else if (menu_pin == 3) {
    // delay für das Farbpattern automatisch einstellen
    Blynk.virtualWrite(V1, breathing_start);
    user_delay = breathing_start;
    user_delay = map(user_delay, 1, 100, 40, 1);
  }
}

//read user delay
BLYNK_WRITE(V1) {
  user_delay = param.asInt();
  user_delay = map(user_delay, 1, 100, 40, 1);
#ifdef DEBUG
  Serial.println("Delay/Speed has been changed.\n");
#endif
}

#ifdef RGBSLIDERCODE
// Code für den button um die Sliderwerte zu übertragen
// Button V2
// Die Slider auf Werte zwischen 0 und 255 einstellen und als Pin "Virtual" verwenden
// ROT: V3    Grün: V4      Blau: V5
// "send on release" ist empfohlen, andere einstellungen sind optional und nicht getestet.

//read manual pin
BLYNK_WRITE(V2) {
  manual_pin_state = param.asInt();
  menu_pin = 0;
#ifdef DEBUG
  Serial.println("Slider Button has been pressed.");
#endif
}

//red slider
BLYNK_WRITE(V3) {
  r_slider = param.asInt() * 4;
#ifdef DEBUG
  Serial.println("Red Slider has been moved.");
#endif
}

//green slider
BLYNK_WRITE(V4) {
  g_slider = param.asInt() * 4;
#ifdef DEBUG
  Serial.println("Green Slider has been moved.");
#endif
}

//blue slider
BLYNK_WRITE(V5) {
  b_slider = param.asInt() * 4;
#ifdef DEBUG
  Serial.println("Blue Slider has been moved.");
#endif
}
#endif

//rgb zebra output to leds
BLYNK_WRITE(V7) {
  R = param[0].asInt() * 4;
  G = param[1].asInt() * 4;
  B = param[2].asInt() * 4;
  menu_pin = 0;
#ifdef RGBSLIDERCODE
  manual_pin_state = 0;
#endif
  Blynk.virtualWrite(V2, LOW);
  push();
#ifdef DEBUG
  Serial.println("Zebra RGB color has been changed.");
#endif
}

//brightness slider
BLYNK_WRITE(V9) {
  brightness = param.asInt();
#ifdef DEBUG
  Serial.println("Brightness has been changed.");
#endif
}

//on-off switch
BLYNK_WRITE(V10) {
  onoff = param.asInt();
#ifdef DEBUG
  Serial.printf("\nLeds are now %d", onoff);
#endif
}


#ifdef remotecontrol

//remote red
BLYNK_WRITE(V30) {
  menu_pin = 0;
  Blynk.virtualWrite(V10, HIGH);
  onoff = 1;
  R = param.asInt() * 4;
#ifdef RGBSLIDERCODE
  manual_pin_state = 0;
  Blynk.virtualWrite(V2, LOW);
#endif
#ifdef DEBUG
  Serial.println("Color has been remotely changed.");
#endif
}

//remote green
BLYNK_WRITE(V31) {
  menu_pin = 0;
  Blynk.virtualWrite(V10, HIGH);
  onoff = 1;
  G = param.asInt() * 4;
#ifdef RGBSLIDERCODE
  manual_pin_state = 0;
  Blynk.virtualWrite(V2, LOW);
#endif
#ifdef DEBUG
  Serial.println("Color has been remotely changed.");
#endif
}

//remote blue
BLYNK_WRITE(V32) {
  menu_pin = 0;
  Blynk.virtualWrite(V10, HIGH);
  onoff = 1;
  B = param.asInt() * 4;
#ifdef RGBSLIDERCODE
  manual_pin_state = 0;
  Blynk.virtualWrite(V2, LOW);
#endif
#ifdef DEBUG
  Serial.println("Color has been remotely changed.");
#endif
}

#endif


void setupSpiffs() {
  //clean FS, for testing
  // SPIFFS.format();

  //read configuration from FS json
#ifdef DEBUG
  Serial.println("mounting FS...");
#endif
  if (SPIFFS.begin()) {
#ifdef DEBUG
    Serial.println("mounted file system");
#endif
    if (SPIFFS.exists("/config1.json")) {
      //file exists, reading and loading
#ifdef DEBUG
      Serial.println("reading config file");
#endif
      File configFile = SPIFFS.open("/config1.json", "r");
      if (configFile) {
#ifdef DEBUG
        Serial.println("opened config file");
#endif
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
          strcpy(api_token, json["api_token"]);
          strcpy(rgb_changed, json["pins"]);
          Serial.println("punkt1");
          char *p1 = &rgb_changed[0];
          char *p2 = &rgb_changed[2];
          char *p3 = &rgb_changed[4];
          Serial.println("punkt2");
          R_LED = (uint8_t)atoi(p1);
          G_LED = (uint8_t)atoi(p2);
          B_LED = (uint8_t)atoi(p3);
          Serial.println("punkt3");
        }
        else {
#ifdef DEBUG
          Serial.println("failed to load json config");
#endif
        }
      }
    }
  } else {
#ifdef DEBUG
    Serial.println("failed to mount FS");
#endif
  }
  //end read
}

//callback notifying us of the need to save config
void saveConfigCallback () {
#ifdef DEBUG
  Serial.println("Should save config");
#endif
  shouldSaveConfig = true;
}

// Funktionen um zu überprüfen ob sich während einer Farbfunktion das Farbpattern, der Sliderbutton oder die Verzögerung verändert
bool rainbow_change (float speedo) {
  if (menu_pin != 2 or manual_pin_state != 0 or speedo != (user_delay)) {
    return true;
  }
}

bool breathing_change (float speedo) {
  if (menu_pin != 3 or manual_pin_state != 0 or speedo != user_delay) {
    return true;
  }
}

bool epilepsy_change (float del) {
  if (menu_pin != 1 or manual_pin_state != 0 or del != user_delay) {
    return true;
  }
}

//Farbfunktion Police
int police() {

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

  if (menu_pin != 4 or manual_pin_state == 1) {
    return 0;
  }

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

  return 0;
}

//Farbfunktion Regenbogen
int rainbow(int speedo) {
  if (rainbow_state == 0) {
    R = 1023; G = 0; B = 0;
    for (int i = 0; i < 1023; i += 3) {
      G += 3;
      push();
      if (rainbow_change(speedo)) {
        return 0;
      }
      delay(speedo);
    }
    rainbow_state = 1;
  }
  else if (rainbow_state == 1) {
    R = 1023; G = 1023; B = 0;
    for (int i = 1023; i > 0; i -= 3) {
      R -= 3;
      push();
      if (rainbow_change(speedo)) {
        return 0;
      }
      delay(speedo);
    }
    rainbow_state = 2;
  }
  else if (rainbow_state == 2) {
    R = 0; G = 1023; B = 0;
    for (int i = 0; i < 1023; i += 3) {
      B += 3;
      push();
      if (rainbow_change(speedo)) {
        return 0;
      }
      delay(speedo);
    }
    rainbow_state = 3;
  }
  else if (rainbow_state == 3) {
    R = 0; G = 1023; B = 1023;
    for (int i = 1023; i > 0; i -= 3) {
      G -= 3;
      push();
      if (rainbow_change(speedo)) {
        return 0;
      }
      delay(speedo);
    }
    rainbow_state = 4;
  }
  else if (rainbow_state == 4) {
    R = 0; G = 0; B = 1023;
    for (int i = 0; i < 1023; i += 3) {
      R += 3;
      push();
      if (rainbow_change(speedo)) {
        return 0;
      }
      delay(speedo);
    }
    rainbow_state = 5;
  }
  else if (rainbow_state == 5) {
    R = 1023; G = 0; B = 1023;
    for (int i = 1023; i > 0; i -= 3) {
      B -= 3;
      push();
      if (rainbow_change(speedo)) {
        return 0;
      }
      delay(speedo);
    }
    rainbow_state = 0;
  }
  return 0;
}

//Farbfunktion Atmung
int breathing(int speedo)
{
  brightness = 100;
  push();

  //downwards
  while (brightness >= 0) {
    brightness--;
    push();
    if (breathing_change(speedo)) {
      return 0;
    }
    delay(speedo);
  }

  //black
  push();
  delay(speedo * 15);

  //upwards
  while (brightness <= 100) {
    brightness++;
    push();
    if (breathing_change(speedo)) {
      return 0;
    }
    delay(speedo);
  }

  //on
  delay(speedo * 15);
}

int epilepsy(int del)
{
  //white
  R = 1023; G = 1023; B = 1023;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);

  if (epilepsy_change(del)) {
    return 0;
  }

  //red
  R = 1023; G = 0; B = 0;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);

  if (epilepsy_change(del)) {
    return 0;
  }

  //green
  R = 0; G = 1023; B = 0;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);

  if (epilepsy_change(del)) {
    return 0;
  }

  //blue
  R = 0; G = 0; B = 1023;
  push();
  delay(del);

  R = 0; G = 0; B = 0;
  push();
  delay(del);

  if (epilepsy_change(del)) {
    return 0;
  }
  return 0;
}

void setup() {
  R = 1023; G = 1023; B = 1023;
  brightness = 100;
  user_delay = 10;
  push();


#ifdef DEBUG
  Serial.begin(9600);
#endif

  WiFiManager wm;

  pinMode(D4, OUTPUT);
  if (drd.detect()) {
#ifdef DEBUG
    Serial.println("Entered Wifi setup");
#endif
    digitalWrite(D4, LOW); //turns bultin led on

    setupSpiffs();

    //set config save notify callback
    wm.setSaveConfigCallback(saveConfigCallback);

    // setup custom parameters
    WiFiManagerParameter custom_api_token("api", "auth token", "", 32);
    WiFiManagerParameter rgb_change_values("rgb", "R G B", "5 4 0", 10);

    //add all your parameters here
    wm.addParameter(&custom_api_token);
    wm.addParameter(&rgb_change_values);

    // start configportal for a little 300 seconds
    wm.setConfigPortalTimeout(300);
    wm.setClass("invert");

    //set custom ip for portal
    wm.setAPStaticIPConfig(IPAddress(172, 217, 28, 1), IPAddress(172, 217, 28, 1), IPAddress(255, 255, 255, 0));

    // start portal
    wm.startConfigPortal("RGB Controller");

    //if you get here you have connected to the WiFi
#ifdef DEBUG
    Serial.println("connected...yeey :)");
#endif
    //read updated parameters
    strcpy(api_token, custom_api_token.getValue());
    strcpy(rgb_changed, rgb_change_values.getValue());
    
    int ssid_str_len = wm.getWiFiSSID().length() + 1;
    char ssid_char_array[ssid_str_len];
    wm.getWiFiSSID().toCharArray(ssid_char_array, ssid_str_len);

    int pass_str_len = wm.getWiFiPass().length() + 1;
    char pass_char_array[pass_str_len];
    wm.getWiFiPass().toCharArray(pass_char_array, pass_str_len);

    strcpy(ssid, ssid_char_array);
    strcpy(pass, pass_char_array);

    //save the custom parameters to FS
    if (shouldSaveConfig) {
#ifdef DEBUG
      Serial.println("saving config");
#endif

      char *p1 = &rgb_changed[0];
      char *p2 = &rgb_changed[2];
      char *p3 = &rgb_changed[4];
      R_LED = (uint8_t)atoi(p1);
      G_LED = (uint8_t)atoi(p2);
      B_LED = (uint8_t)atoi(p3);
     
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["api_token"]   = api_token;
      json["ssid"]        = ssid;
      json["pass"]        = pass;
      json["pins"]        = rgb_changed;
      
      // json["ip"]          = WiFi.localIP().toString();
      // json["gateway"]     = WiFi.gatewayIP().toString();
      // json["subnet"]      = WiFi.subnetMask().toString();

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
#ifdef DEBUG
    Serial.println("\nlocal ip");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.subnetMask());

    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("PW: ");
    Serial.println(pass);
    Serial.print("auth: ");
    Serial.println(api_token);
#endif
    wm.stopConfigPortal();
    digitalWrite(D4, HIGH); //turns bultin led off
  } else {
#ifdef DEBUG
    Serial.println("Using existing Wifi");
#endif
    setupSpiffs();
    digitalWrite(D4, HIGH); //turns bultin led off
  }

  Blynk.begin(api_token, ssid, pass);

  Blynk.virtualWrite(V1, 10);
#ifdef RGBSLIDERCODE
  Blynk.virtualWrite(V2, LOW);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V4, 0);
  Blynk.virtualWrite(V5, 0);
#endif
  Blynk.virtualWrite(V7, 255);
  Blynk.virtualWrite(V9, 100);
  Blynk.virtualWrite(V10, 1);

  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
}

void loop() {
  if (manual_pin_state == 0) {
    if (menu_pin == 1) {
      epilepsy(user_delay);
    }
    else if (menu_pin == 2) {
      rainbow(user_delay);
    }
    else if (menu_pin == 3) {
      breathing(user_delay);
    }
    else if (menu_pin == 4) {
      police();
    }
  }

#ifdef RGBSLIDERCODE
  else if (manual_pin_state == 1)
  {
    R = r_slider;
    G = g_slider;
    B = b_slider;
  }
#endif
  push();
}
