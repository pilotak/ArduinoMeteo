#define BME_I2C_ADDRESS 0x76
#define MCP9808_I2C_ADDRESS 0x18

#define INFO_LED LED_BUILTIN

#define MQTT_ID       "meteo"
#define MQTT_HOST     "192.168.0.100"
#define MQTT_USER     "admin"
#define MQTT_PASS     "admin"
#define MQTT_PORT     1883
#define SENSORS_SEND_INTERVAL 60000  // ms
#define METERS_SEND_INTERVAL 256  // sec

#define MQTT_BASE_TOPIC "meteo"
#define MQTT_TOPIC_TEMP MQTT_BASE_TOPIC"/temp"
#define MQTT_TOPIC_HUMIDITY MQTT_BASE_TOPIC"/humidity"
#define MQTT_TOPIC_PRESSURE MQTT_BASE_TOPIC"/pressure"
#define MQTT_WIND_DEGREES MQTT_BASE_TOPIC"/wind_degrees"
#define MQTT_WIND_SPEED MQTT_BASE_TOPIC"/wind_speed"
#define MQTT_RAIN_GAUGE MQTT_BASE_TOPIC"/rain_gauge"
#define MQTT_TOPIC_WILL MQTT_BASE_TOPIC"/status"
#define MQTT_WILL_SEND_INTERVAL 15000  // ms

#if HW_VER == 1
    #define anemometer_pin PB12
    #define windvane_pin PB1
    #define raingauge_pin PB14
#elif HW_VER == 2
    #define anemometer_pin PB7
    #define windvane_pin PB9
    #define raingauge_pin PB8
#endif

/* Watchdog setting example
 *
 * |  time  | prescaler | reload |
 * | ------ | --------- | ------ |
 * |   8s   |    256    |  1250  |
 * |   4s   |    256    |  625   |
 * |   2s   |    128    |  625   |
 * |   1s   |    64     |  625   |
 * |  500ms |    32     |  625   |
 * |  250ms |    16     |  625   |
 * |  100ms |    32     |  125   |
 * |   1ms  |     8     |   5    |
 *
 */

// reload = ((40000 / 1000) * 100) / 32;  (40kHz watchdog clock /divided by 1000ms *100 miliseconds desired timeout / 32 prescaler)

#define WDT_PRESCALER IWDG_PRE_256 // IWDG_PRE_4, IWDG_PRE_8, IWDG_PRE_16, IWDG_PRE_32, IWDG_PRE_64, IWDG_PRE_128, IWDG_PRE_256
#define WDT_RELOAD 1250
