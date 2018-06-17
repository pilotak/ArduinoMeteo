#if defined(SENSOR_BME280_HUMIDITY) || defined(SENSOR_BME280_PRESSURE) || defined(SENSOR_BME280_TEMPERATURE)
    #include <Adafruit_Sensor.h>
    #include <Adafruit_BME280.h>

    Adafruit_BME280 bme;
#endif

#if defined(SENSOR_MCP9808)
    #include "Adafruit_MCP9808.h"
    Adafruit_MCP9808 tempsensor;
#endif

#include "MovingAverage.h"
MovingAverage <int16_t, 8> tempFilter;
MovingAverage <int16_t, 8> humidityFilter;
MovingAverage <int32_t, 16> pressureFilter;

void sensorSetup() {
#if defined(SENSOR_BME280_HUMIDITY) || defined(SENSOR_BME280_PRESSURE) || defined(SENSOR_BME280_TEMPERATURE)

    if (!bme.begin(BME_I2C_ADDRESS, &I2C)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Couldn't find BME280 sensor, check wiring!");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset
    }

    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X1,   // temperature
                    Adafruit_BME280::SAMPLING_X16,   // pressure
                    Adafruit_BME280::SAMPLING_X2, // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5 );

    // Fake reading to settle up readings and it's oversampling, min repeat should be as high as highest oversampling set
    /*    for (uint8_t i = 0; i < 10; ++i) {
    #if defined(SENSOR_BME280_TEMPERATURE)
            bme.readTempC();
    #endif
    #if defined(SENSOR_BME280_HUMIDITY)
            bme.readFloatHumidity();
    #endif
    #if defined(SENSOR_BME280_PRESSURE)
            bme.readFloatPressure();
    #endif

            delay(200);  // give time to "burn up" sensor
        }*/

#endif

#if defined(SENSOR_MCP9808)

    if (!tempsensor.begin(MCP9808_I2C_ADDRESS, &I2C)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Couldn't find MCP9808 sensor, check wiring!");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset
    }

    tempsensor.setResolution(3);
    tempsensor.shutdown_wake(0);
#endif
}

void readSensors() {
#if defined(SENSOR_BME280_HUMIDITY)
    humidityFilter.add(static_cast<int16_t>(bme.readHumidity() * 100));
#endif

#if defined(SENSOR_BME280_PRESSURE)
    pressureFilter.add(static_cast<int32_t>(bme.readPressure()));
#endif

#if defined(SENSOR_BME280_TEMPERATURE)
    tempFilter.add(static_cast<int16_t>(bme.readTemperature() * 100));
#elif defined(SENSOR_MCP9808)
    tempFilter.add(static_cast<int16_t>(tempsensor.readTempC() * 100));
#endif
}

void sendSensors() {
    char cstr[8];
    digitalWrite(INFO_LED, HIGH);

#if defined(SENSOR_BME280_TEMPERATURE) || defined(SENSOR_MCP9808)
    itoa(tempFilter.get(), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("\nTemperature: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_TEMP, cstr);
#endif

#if defined(SENSOR_BME280_HUMIDITY)
    itoa(humidityFilter.get(), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Humidity: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_HUMIDITY, cstr);
#endif

#if defined(SENSOR_BME280_PRESSURE)
    // Get relative pressure
    float pressure = (pressureFilter.get() / pow(1.0 - (DEF_ALTITUDE / 44330.0), 5.255));
    itoa(pressure, cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Pressure: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_PRESSURE, cstr);
#endif
    digitalWrite(INFO_LED, LOW);
}
