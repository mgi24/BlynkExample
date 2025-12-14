#define BLYNK_PRINT Serial
#define BLYNK_AUTH_TOKEN   "device_auth_token" //dapatkan di devices
#define BLYNK_TEMPLATE_ID "" //dapatkan di developer zone > mytemplates
#define BLYNK_TEMPLATE_NAME "" //bebas
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "wifi ssid";
char pass[] = "wifi password";

//map GPIO
const int PIN_LAMP = 5;
const int PIN_DOOR = 4;

//timer variables
int minutes = 0;
int seconds = 0;
bool isUVon = false;

//sync virtual pin to lamp
BLYNK_WRITE(V1) {
  isUVon = param.asInt();
  digitalWrite(PIN_LAMP, isUVon);
}
//sync virtual pin to timer variables
BLYNK_WRITE(V3) {
  minutes = param.asInt();
}

BLYNK_WRITE(V4) {
  seconds = param.asInt();
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(PIN_DOOR, INPUT_PULLDOWN); 
  pinMode(PIN_LAMP, OUTPUT);
  digitalWrite(PIN_LAMP, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

int lastInputState;
void loop()
{
  Blynk.run();
  int currentState = digitalRead(PIN_DOOR);
  if (currentState != lastInputState) {
    lastInputState = currentState;
    Blynk.virtualWrite(V2, currentState);
    Serial.print("GPIO4 changed to: ");
    Serial.println(currentState);
  }
  static unsigned long lastTick = 0;
  if (isUVon && (minutes > 0 || seconds > 0)) {
    unsigned long now = millis();
    if (now - lastTick >= 1000) {
      lastTick = now;
      if (seconds == 0) {
        if (minutes > 0) {
          minutes--;
          seconds = 59;
        }
      } else {
        seconds--;
      }
      Blynk.virtualWrite(V3, minutes);
      Blynk.virtualWrite(V4, seconds);
      if (minutes == 0 && seconds == 0) {
        isUVon = false;
        digitalWrite(PIN_LAMP, LOW);
        Blynk.virtualWrite(V1, 0);
      }
    }
  } else {
    lastTick = millis();
  }
   delay(50);
}

