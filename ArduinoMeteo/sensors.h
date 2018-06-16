#include "SparkFunBME280.h"
#include "Adafruit_MCP9808.h"
#include "MovingAverage.h"

BME280 bme;
Adafruit_MCP9808 tempsensor;
MovingAverage <int16_t, 8> tempFilter;
MovingAverage <int16_t, 8> humidityFilter;
MovingAverage <int32_t, 16> pressureFilter;

void sensorSetup() {
    bme.setI2CAddress(BME_I2C_ADDRESS);

    if (!bme.beginI2C(I2C)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Couldn't find BME280 sensor, check wiring!");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset
    }

    bme.setMode(MODE_NORMAL);
    bme.setTempOverSample(8);
    bme.setHumidityOverSample(8);
    bme.setPressureOverSample(16);

    if (!tempsensor.begin(MCP9808_I2C_ADDRESS, &I2C)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Couldn't find MCP9808 sensor, check wiring!");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset
    }

    tempsensor.setResolution(3);
    tempsensor.shutdown_wake(0);

    delay(1000);  // give time to "burn up" sensors

    // Fake reading to settle up readings
    bme.readFloatPressure();
    bme.readFloatHumidity();
    bme.readTempC();
    tempsensor.readTempC();
}

void readSensors() {
    float pressure = (bme.readFloatPressure() / pow(1.0 - (DEF_ALTITUDE / 44330.0), 5.255));
    humidityFilter.add(static_cast<int16_t>(bme.readFloatHumidity() * 100));
    pressureFilter.add(static_cast<int32_t>(pressure));
    tempFilter.add(static_cast<int16_t>(tempsensor.readTempC() * 100));
}

void sendSensors() {
    char cstr[8];
    digitalWrite(INFO_LED, HIGH);

    itoa(tempFilter.get(), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("\nTemperature 0: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_TEMP0, cstr);


    itoa(static_cast<int>(bme.readTempC() * 100), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Temperature 1: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_TEMP1, cstr);
    itoa(humidityFilter.get(), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Humidity: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_HUMIDITY, cstr);


    itoa(pressureFilter.get(), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Pressure: ");
    debugPort.println(cstr);
#endif
    sendData(MQTT_TOPIC_PRESSURE, cstr);

    digitalWrite(INFO_LED, LOW);
}
