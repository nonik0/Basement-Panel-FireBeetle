#include <Adafruit_SHT31.h>
#include <Arduino.h>
#include <displayManager.h>

#include "services.hpp"

#define LATCH_PIN 2
#define EN_PIN 13

DisplayManager displayManager(LATCH_PIN);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

unsigned long lastReadingMillis = 0;
int sensorReadings[] = {0, 0};

bool display, lastDisplay = true;
uint8_t brightness, lastBrightness = 100;

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
  //temp = temp * 1.8 + 32;

  sensorReadings[0] = temp;
  sensorReadings[1] = humidity;

  lastReadingMillis = millis();

  // Serial.print("Temp: "); Serial.print(temp); Serial.print("C, ");
  // Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
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

  // EN_PIN PWM for brightness control
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH);
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
  if (millis() - lastReadingMillis > 1000)
    readSHT31();

  if (display != lastDisplay) {
    display = lastDisplay;
    digitalWrite(EN_PIN, display);
    analogWrite(EN_PIN, brightness);
  }

  if (brightness != lastBrightness) {
    brightness = lastBrightness;
    analogWrite(EN_PIN, brightness);
  }

  ArduinoOTA.handle();
  restServer.handleClient();
  checkWifiStatus();
  delay(10);
}