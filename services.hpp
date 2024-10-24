#pragma once

#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>

#include "secrets.h"

WebServer restServer(80);
extern bool display;
extern uint8_t brightness;

void wifiSetup()
{
    Serial.println("Wifi setting up...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    Serial.print("Wifi ready, IP address: ");
    Serial.println(WiFi.localIP());
}

volatile long wifiStatusDelayMs = 0;
int wifiDisconnects = 0;
void checkWifiStatus()
{
    if (wifiStatusDelayMs < 0)
    {
        try
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                Serial.println("Reconnecting to WiFi...");
                WiFi.disconnect();
                WiFi.reconnect();
                wifiDisconnects++;
                Serial.println("Reconnected to WiFi");
            }
        }
        catch (const std::exception &e)
        {
            Serial.println("Wifi error:" + String(e.what()));
            wifiStatusDelayMs = 10 * 60 * 1000; // 10 minutes
        }

        wifiStatusDelayMs = 60 * 1000; // 1 minute
    }
}

void otaSetup()
{
    Serial.println("OTA setting up...");

    // ArduinoOTA.setPort(3232);
    ArduinoOTA.setHostname("Basement-Panel-FireBeetle");

    ArduinoOTA
        .onStart([]()
                 {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else  // U_SPIFFS
          type = "filesystem";

        Serial.println("Start updating " + type); })
        .onEnd([]()
               { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed"); });
    ArduinoOTA.begin();

    Serial.println("OTA setup complete.");
}

void mDnsSetup()
{
    if (!MDNS.begin("basement-panel-firebeetle"))
    {
        Serial.println("Error setting up MDNS responder!");

        return;
    }
    Serial.println("mDNS responder started");
}

void restIndex()
{
    Serial.println("Serving index.html");
    restServer.send(200, "text/plain", "test");
    Serial.println("Served index.html");
}

void restDisplay()
{
  if (restServer.hasArg("plain")) {
    String body = restServer.arg("plain");
    body.toLowerCase();

    if (body == "off") {
      Serial.println("Turning display off");
      display = false;
    } else if (body == "on") {
      Serial.println("Turning display on");
      display = true;
    } else {
      restServer.send(400, "text/plain", body);
      return;
    }
  }

  restServer.send(200, "text/plain", display ? "on" : "off");
}

void restBrightness()
{
  if (restServer.hasArg("plain")) {
    String body = restServer.arg("plain");

    int newBrightness = body.toInt();
    if (newBrightness < 0 || newBrightness > 255) {  
        restServer.send(400, "text/plain", body);
        return;
    }
    else {
      Serial.println("Setting brightness to " + String(newBrightness));
      brightness = newBrightness;
    }
  }

  restServer.send(200, "text/plain", String(brightness));
}

void restSetup()
{
    restServer.on("/", restIndex);
    restServer.on("/display", restDisplay);
    restServer.on("/brightness", restBrightness);
    restServer.begin();

    Serial.println("REST server running");
}