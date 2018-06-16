/*
MIT License
Copyright (c) 2018 Pavel Slama
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Wire.h>
#include "settings.h"
#include "hw.h"
#include "watchdog.h"

#include "sensors.h"

#if defined(SENSOR_WEATHER_METERS)
    #include "meters.h"
#endif

uint32_t previousSendMillis = 0;
uint32_t previousWillMillis = 0;
uint32_t previousReadMillis = 0;

void setup() {
    wdtSetup();

#if defined(ENABLE_DEBUG)
    debugPort.begin(115200);
#endif

    pinMode(INFO_LED, OUTPUT);
    digitalWrite(INFO_LED, LOW);

#if defined(ENABLE_DEBUG)
    debugPort.println("Starting");
#endif

    sensorSetup();

#if defined(SENSOR_WEATHER_METERS)
    setupMeters();
#endif

    commSetup();

    previousSendMillis = previousWillMillis = millis();
}

void loop() {
#if defined(SENSOR_WEATHER_METERS)

    if (got_meters_data) {
        got_meters_data = false;
        readMetersData();
    }

#endif

    if (millis() - previousReadMillis >= SENSORS_READ_INTERVAL) {
        previousReadMillis = millis();
        readSensors();

        iwdg_feed();
    }

    if (millis() - previousWillMillis >= MQTT_STATUS_SEND_INTERVAL) {
        previousWillMillis = millis();
        sendData(MQTT_TOPIC_WILL, MQTT_STATE_ON, true);

        iwdg_feed();
    }

    if (millis() - previousSendMillis >= SENSORS_SEND_INTERVAL) {
        previousSendMillis = millis();
        sendSensors();

        iwdg_feed();
    }

    iwdg_feed();
    mqttClient.loop();
}
