#define BME_I2C_ADDRESS 0x76
#define MCP9808_I2C_ADDRESS 0x18

#define DEF_ALTITUDE 408.0  // floating point here

#define INFO_LED LED_BUILTIN

#define SENSORS_SEND_INTERVAL 120000  // ms
#define SENSORS_READ_INTERVAL 1000  // ms
#define METERS_SEND_INTERVAL 256  // sec
#define MQTT_STATUS_SEND_INTERVAL 45000  // ms

#define MQTT_ID       "meteo"
#define MQTT_HOST     "192.168.0.100"
#define MQTT_USER     "xxx"
#define MQTT_PASS     "xxx"
#define MQTT_PORT     1883

#define MQTT_BASE_TOPIC "meteo"
#define MQTT_TOPIC_TEMP0 MQTT_BASE_TOPIC"/temp/0"
#define MQTT_TOPIC_TEMP1 MQTT_BASE_TOPIC"/temp/1"
#define MQTT_TOPIC_HUMIDITY MQTT_BASE_TOPIC"/humidity"
#define MQTT_TOPIC_PRESSURE MQTT_BASE_TOPIC"/pressure"
#define MQTT_WIND_DEGREES MQTT_BASE_TOPIC"/wind_degrees"
#define MQTT_WIND_SPEED MQTT_BASE_TOPIC"/wind_speed"
#define MQTT_RAIN_GAUGE MQTT_BASE_TOPIC"/rain_gauge"
#define MQTT_TOPIC_WILL MQTT_BASE_TOPIC"/status"
#define MQTT_STATE_ON "1"
#define MQTT_STATE_OFF "0"

#if HW_VER == 1
    #define anemometer_pin PB12
    #define windvane_pin PB1
    #define raingauge_pin PB14
#endif
