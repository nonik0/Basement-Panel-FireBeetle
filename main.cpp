#include <Adafruit_SHT31.h>
#include <Arduino.h>
#include <displayManager.h>

#include "services.hpp"

#define LATCH_PIN 2
#define EN_PIN 13

#define EXT_PIN 14

DisplayManager displayManager(LATCH_PIN);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

unsigned long lastReadingMillis = 0;
int sensorReadings[] = {0, 0};

bool display = true;
uint8_t brightness = 100;

void displayReadings(void *pvParameters)
{
  int currentBrightness = brightness;
  while (1)
  {
    if (display)
    {
      if (currentBrightness != brightness) {
        currentBrightness = brightness;
        analogWrite(EN_PIN, brightness);
      }
      displayManager.displayNumbers(sensorReadings, 2);
    }
    else
    {
      currentBrightness = 0;
      digitalWrite(EN_PIN, 0);
      displayManager.clearDisplays(2);
    }
    
    vTaskDelay(1);
  }
}

void readSHT31()
{
  float temp = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  // convert to Fahrenheit
  // temp = temp * 1.8 + 32;

  sensorReadings[0] = temp;
  sensorReadings[1] = humidity;

  lastReadingMillis = millis();
}

void setup()
{
  delay(5000);

  Serial.begin(115200);
  Serial.println("Starting setup...");

  if (!sht31.begin(0x44))
  {
    while (1)
      delay(1);
  }

  wifiSetup();
  mDnsSetup();
  otaSetup();
  restSetup();

  displayManager.begin();
  displayManager.clearDisplays(1);

  // EXT_PIN for external device control
  pinMode(EXT_PIN, OUTPUT);
  digitalWrite(EXT_PIN, display);

  // EN_PIN PWM for brightness control
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, display);
  analogWrite(EN_PIN, brightness);

  // create task for displayManager
  xTaskCreatePinnedToCore(
      &displayReadings,
      "displayManagerTask",
      10000,
      NULL,
      0,
      NULL,
      1);
}

void loop()
{
  digitalWrite(EXT_PIN, display);

  if (millis() - lastReadingMillis > 1000)
    readSHT31();

  ArduinoOTA.handle();
  restServer.handleClient();
  checkWifiStatus();
  delay(10);
}