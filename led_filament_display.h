#pragma once

#include <Adafruit_SHT31.h>
#include <displayManager.h>

#include "display_task_handler.h"


class LedFilamentDisplayTaskHandler : public DisplayTaskHandler
{
private:
    static const uint8_t DISPLAY_TASK_PRIORITY = 10; // higher priority than sensor task
    static const uint8_t SENSOR_TASK_PRIORITY = 5;
    static const int LATCH_PIN = 2;
    static const int EN_PIN = 13;
    static const int SENSOR_READ_INTERVAL = 5000;

    DisplayManager _displayManager = DisplayManager(LATCH_PIN);
    Adafruit_SHT31 _sht31 = Adafruit_SHT31();

    unsigned long _lastReadingMillis = 0;
    int _sensorReadings[2] = {0, 0};
    uint8_t _brightness = 100;

public:
    LedFilamentDisplayTaskHandler() {}
    bool createTask() override;

private:
    void task(void *parameters) override;
    void displayTask(void *pvParameters);
    void sensorTask(void *pvParameters);

    static void sensorTaskWrapper(void *parameters)
    {
        LedFilamentDisplayTaskHandler *handler = static_cast<LedFilamentDisplayTaskHandler *>(parameters);
        handler->sensorTask(parameters);
    }
};

bool LedFilamentDisplayTaskHandler::createTask()
{
    log_d("Starting LED filament display setup");

    if (_taskHandle != NULL)
    {
        log_w("Task already started");
        return false;
    }

    if (!_sht31.begin(SHT31_DEFAULT_ADDR))
    {
        log_e("SHT31 sensor not found!");
        return false;
    }

    _displayManager.begin();
    _displayManager.clearDisplays(1);

    // EN_PIN PWM for brightness control
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, _display);
    analogWrite(EN_PIN, _brightness);

    xTaskCreate(taskWrapper, "DisplayTask", 4096, this, DISPLAY_TASK_PRIORITY, &_taskHandle); // pin to core 1?
    xTaskCreate(sensorTaskWrapper, "SensorTask", 4096, this, SENSOR_TASK_PRIORITY, NULL);

    log_d("LedFilamentDisplay setup complete");
    return true;
}

void LedFilamentDisplayTaskHandler::task(void *parameters)
{
    displayTask(parameters);
}

void LedFilamentDisplayTaskHandler::displayTask(void *pvParameters)
{
    log_d("Starting display task");

    while (1)
    {
        if (!_display)
        {
            analogWrite(EN_PIN, 0);
            _displayManager.clearDisplays(2);
            continue;
        }

        analogWrite(EN_PIN, _brightness);
        _displayManager.displayNumbers(_sensorReadings, 2);

        vTaskDelay(1);
    }
}


void LedFilamentDisplayTaskHandler::sensorTask(void *pvParameters)
{
    log_d("Starting sensor task");

    while (1)
    {
        float temp = _sht31.readTemperature();
        float humidity = _sht31.readHumidity();

        // convert to Fahrenheit
        // temp = temp * 1.8 + 32;

        _lastReadingMillis = millis();
        _sensorReadings[0] = temp;
        _sensorReadings[1] = humidity;

        delay(SENSOR_READ_INTERVAL);
    }
}

