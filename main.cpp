#include "led_filament_display.h"
#include "wifi_services.h"

#define EXT1_PIN 14
#define EXT2_PIN 15

LedFilamentDisplayTaskHandler ledFilamentDisplay;
WifiServices wifiServices;

void setup()
{
  Serial.begin(115200);
  delay(5000);
  log_d("Starting setup...");

  // external pins used for external device control
  pinMode(EXT1_PIN, OUTPUT);
  pinMode(EXT2_PIN, OUTPUT);
  digitalWrite(EXT1_PIN, true);
  digitalWrite(EXT2_PIN, true);

  wifiServices.setup(DEVICE_NAME);

  ledFilamentDisplay.createTask();
  wifiServices.createTask();

  wifiServices.registerSetDisplayCallback([&](bool displayState)
                                          { ledFilamentDisplay.setDisplay(displayState); });
  wifiServices.registerSetDisplayCallback([&](bool state)
                                          { digitalWrite(EXT1_PIN, state);
                                            digitalWrite(EXT2_PIN, state); });

  log_d("Setup complete");
}

void loop()
{
}