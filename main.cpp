#include <Adafruit_SHT31.h>
#include <Arduino.h>
// #include <ArduinoJson.h>
#include <displayManager.h>
// #include <HTTPClient.h>
// #include <WiFi.h>

#include "secrets.h"

#define LATCH_PIN 2
#define EN_PIN 13

DisplayManager displayManager(LATCH_PIN);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

unsigned long lastReadingMillis = 0;
int sensorReadings[] = {0, 0};

void displayReadings(void *pvParameters)
{
  while (1)
  {
    displayManager.displayNumbers(sensorReadings, 2);
    vTaskDelay(1);
  }
}

void readSHT31()
{
  float temp = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  // convert to Fahrenheit
  temp = temp * 1.8 + 32;

  sensorReadings[0] = temp;
  sensorReadings[1] = humidity;

  lastReadingMillis = millis();

  // Serial.print("Temp: "); Serial.print(temp); Serial.print("C, ");
  // Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
}

void setup()
{
  // Serial.begin(115200);
  // Serial.println("Starting...");

  if (!sht31.begin(0x44))
  {
    while (1)
      delay(1);
  }

  displayManager.begin();
  displayManager.clearDisplays(1);

  // EN_PIN PWM for brightness control
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH);
  analogWrite(EN_PIN, 100);

  // WiFi.begin(SSID_NAME, SSID_PASS);

  // create task for displayManager
  xTaskCreatePinnedToCore(
      &displayReadings,
      "displayManagerTask",
      10000,
      NULL,
      0,
      NULL,
      0);
}

void loop()
{
  if (millis() - lastReadingMillis > 1000)
    readSHT31();
}