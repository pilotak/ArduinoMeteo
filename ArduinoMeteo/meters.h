#if defined(STM32_MCU_SERIES)
    #include <RTClock.h>

    RTClock rtclock(RTCSEL_LSE);
#elif defined(__AVR__)
    #include "TimerOne.h"
#endif
#include "WeatherMeters.h"

WeatherMeters meters(windvane_pin, METERS_SEND_INTERVAL);

volatile bool got_meters_data = false;

void intAnemometer() {
    meters.intAnemometer();
}

void intRaingauge() {
    meters.intRaingauge();
}

void secondCount() {
    meters.secondCount();
}

void readDone(void) {
    got_meters_data = true;
}

void setupMeters() {
#if defined(STM32_MCU_SERIES)
    rtclock.attachSecondsInterrupt(secondCount);
#elif defined(__AVR__)
    Timer1.initialize(1000000);  // initialize timer1, and set a 1 second period
    Timer1.attachInterrupt(secondCount);
#endif

    attachInterrupt(digitalPinToInterrupt(anemometer_pin), intAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), intRaingauge, FALLING);
    meters.attach(readDone);

#if defined(ENABLE_DEBUG)
    meters.init(&debugPort);
#else
    meters.init();
#endif
}

void readMetersData() {
    float tmp;
    char cstr[8];

    tmp = meters.getWindVane();
    itoa(static_cast<int>(tmp * 10), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Wind degrees: ");
    debugPort.print(cstr);
    debugPort.print("=");
    debugPort.println(tmp);
#endif
    sendData(MQTT_WIND_DEGREES, cstr);

    tmp = meters.getWindSpeed();
    itoa(static_cast<int>(tmp * 10), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Wind speed: ");
    debugPort.print(cstr);
    debugPort.print("=");
    debugPort.print(tmp);
    debugPort.println("km/h");
#endif
    sendData(MQTT_WIND_SPEED, cstr);

    tmp = meters.getRainGauge();
    itoa(static_cast<int>(tmp * 10000), cstr, 10);

#if defined(ENABLE_DEBUG)
    debugPort.print("Rain gauge: ");
    debugPort.print(cstr);
    debugPort.print("=");
    debugPort.print(tmp);
    debugPort.println("mm");
#endif
    sendData(MQTT_RAIN_GAUGE, cstr);
}
