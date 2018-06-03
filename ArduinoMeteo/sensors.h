#include <Adafruit_BME280.h>
#include "Adafruit_MCP9808.h"

Adafruit_BME280 bme;
Adafruit_MCP9808 tempsensor;

void sensorSetup() {
    if (!bme.begin(BME_I2C_ADDRESS, &I2C)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Couldn't find BME280 sensor, check wiring!");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset
    }

    if (!tempsensor.begin(MCP9808_I2C_ADDRESS, &I2C)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Couldn't find MCP9808 sensor, check wiring!");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset
    }

    tempsensor.setResolution(3);
    tempsensor.shutdown_wake(0);

    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,  // temperature
                    Adafruit_BME280::SAMPLING_X1,  // pressure
                    Adafruit_BME280::SAMPLING_X1,  // humidity
                    Adafruit_BME280::FILTER_OFF);
}

void readSensors() {
    float tmp;
    char cstr[8];
    digitalWrite(INFO_LED, LOW);

    bme.takeForcedMeasurement();

    tmp = bme.readHumidity();
    itoa(static_cast<int>(tmp * 100), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("\nhumidity: ");
    debugPort.print(cstr);
    debugPort.print("=");
    debugPort.println(tmp);
#endif
    sendData(MQTT_TOPIC_HUMIDITY, cstr);

    tmp = bme.readPressure();
    itoa(static_cast<int>(tmp), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("pressure: ");
    debugPort.print(cstr);
    debugPort.print("=");
    debugPort.println(tmp / 100);
#endif
    sendData(MQTT_TOPIC_PRESSURE, cstr);

    tmp = tempsensor.readTempC();
    itoa(static_cast<int>(tmp * 100), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Temperature: ");
    debugPort.print(cstr);
    debugPort.print("=");
    debugPort.print(tmp);
#endif
    sendData(MQTT_TOPIC_TEMP, cstr);

    digitalWrite(INFO_LED, HIGH);
}
