#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN D0
#define DHTTYPE DHT22

char* ssid;
char* password;
char* mqtt_server;

WiFiClient espClient;
PubSubClient client(espClient);

int lastTime1 = 0;
int lastTime2 = 0;
boolean state = true;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Ustawienie adresu ukladu na 0x27

byte tempChar[] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01110,
  B11111,
  B11111,
  B01110
};

byte humChar[] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110
};

void setup() {
  importPrivateData();

  dht.begin();
  lcd.begin(16, 2);
  setBacklight();
  lcd.createChar(0, tempChar);
  lcd.createChar(1, humChar);

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  long now = millis();

  if (!client.connected()) {
    reconnect();
  } else {
    client.loop();

    if (now - lastTime1 > 5000) {
      lastTime1 = now;
      publishStatus();
      ESP.wdtFeed();
    }
  }
  if (now - lastTime2 > 5000) {
    lastTime2 = now;
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.print("Temp: ");
    lcd.print(readTemp());
    lcd.print("\337C");
    lcd.setCursor(0, 1);
    lcd.write(1);
    lcd.print("Wilg: ");
    lcd.print(readHum());
    lcd.print("%");
  }
}

void publishStatus() {
  String msg = "thermometer," + String(state) + "," + String(readTemp()) + "," + String(readHum());
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("thermometer", msg.c_str());
}
