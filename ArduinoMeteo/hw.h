#if defined(BLUEPILL_METEO_V1)
#define anemometer_pin PB12
#define windvane_pin PB1
#define raingauge_pin PB14
#define INFO_LED LED_BUILTIN

#if defined(ENABLE_DEBUG)
    #define debugPort Serial2
#endif

#include "ENC28J60_ethernet.h"

TwoWire I2C(2, I2C_FAST_MODE);  // use second I2C

// sensors used
#define SENSOR_BME280_HUMIDITY
#define SENSOR_BME280_PRESSURE
// #define SENSOR_BME280_TEMPERATURE  // only one temperature can be used at time
#define SENSOR_MCP9808
#define SENSOR_WEATHER_METERS

#elif defined(__AVR__)
#define I2C Wire
#else
#error "unknown board"
#endif
