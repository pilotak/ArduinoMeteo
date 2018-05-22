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
#include "watchdog.h"

#if defined(ENABLE_DEBUG)
    #define debugPort Serial2
#endif

#if HW_VER == 1
    TwoWire I2C(2, I2C_FAST_MODE); // use second I2C

#elif HW_VER == 2
    TwoWire I2C(1, I2C_FAST_MODE);

#elif defined(__AVR__)
    #define I2C Wire
#endif

void sendData(const char * topic, char * data, bool retain = false);  // compiler workaround

#include "sensors.h"
#include "meters.h"

#if !defined(WIFI)
    #include "ethernet.h"
#else
    #include "wifi.h"
#endif

long previousMillis, previousWillMillis;

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
    setupMeters();
    commSetup();

    previousMillis = previousWillMillis = millis();
}

void loop() {
    if (millis() - previousMillis >= SENSORS_SEND_INTERVAL) {
        previousMillis = millis();
        readSensors();

        iwdg_feed();
    }

    if (millis() - previousWillMillis >= MQTT_WILL_SEND_INTERVAL) {
        previousWillMillis = millis();
        sendData(MQTT_TOPIC_WILL, "1", true);

        iwdg_feed();
    }

    if (got_meters_data) {
        got_meters_data = false;
        readMetersData();
    }

    iwdg_feed();
    mqttClient.loop();
}
