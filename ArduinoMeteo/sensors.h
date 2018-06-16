#if defined(SENSOR_BME280_HUMIDITY) || defined(SENSOR_BME280_PRESSURE) || defined(SENSOR_BME280_TEMPERATURE)
    #include "SparkFunBME280.h"
    BME280 bme;
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
    bme.setI2CAddress(BME_I2C_ADDRESS);

    if (!bme.beginI2C(I2C)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Couldn't find BME280 sensor, check wiring!");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset
    }

    bme.setMode(MODE_NORMAL);
    bme.setStandbyTime(5);  // 1000ms
    bme.setFilter(1);
#if defined(SENSOR_BME280_TEMPERATURE)
    bme.setTempOverSample(8);
#endif
#if defined(SENSOR_BME280_HUMIDITY)
    bme.setHumidityOverSample(8);
#endif
#if defined(SENSOR_BME280_PRESSURE)
    bme.setPressureOverSample(8);
#endif

    // Fake reading to settle up readings and it's oversampling, min repeat should be as high as highest oversampling set
    for (uint8_t i = 0; i < 8; ++i) {
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
    }

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
    humidityFilter.add(static_cast<int16_t>(bme.readFloatHumidity() * 100));
#endif

#if defined(SENSOR_BME280_PRESSURE)
    float pressure = (bme.readFloatPressure() / pow(1.0 - (DEF_ALTITUDE / 44330.0), 5.255));
    pressureFilter.add(static_cast<int32_t>(pressure));
#endif

#if defined(SENSOR_BME280_TEMPERATURE)
    tempFilter.add(static_cast<int16_t>(bme.readTempC() * 100));
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
    itoa(pressureFilter.get(), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Pressure: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_PRESSURE, cstr);
#endif
    digitalWrite(INFO_LED, LOW);
}
