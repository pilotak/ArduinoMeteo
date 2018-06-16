#include <UIPEthernet.h>
#include "PubSubClient.h"

EthernetClient ethClient;
PubSubClient mqttClient;

uint8_t mac[6];

void commSetup() {
#if HW_VER == 1
    /*
     * if remaping, change manually NSS pin
     * add #define ENC28J60_CONTROL_CS PA10
     * after line #include "mempool.h"
     * in file Enc28J60Network.h
     * otherwise PA4 will be used as NSS
     */
    afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);  // release PB3 and PB5
    afio_remap(AFIO_REMAP_SPI1);  // remap SPI1

    gpio_set_mode(GPIOB, 3, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOB, 4, GPIO_INPUT_FLOATING);
    gpio_set_mode(GPIOB, 5, GPIO_AF_OUTPUT_PP);
#endif

    // build MAC
#if defined(STM32_MCU_SERIES)
#if defined(__STM32F1__)
    uint32_t word0 = *(uint32_t*)(0x1FFFF7E8U);
#else
#error "Please define UID address for MAC address"
#endif

    mac[0] = 0x00;  // STM default
    mac[1] = 0x80;  // STM default
    mac[2] = 0xe1;  // STM default
    mac[3] = (word0 & 0x00ff0000) >> 16;
    mac[4] = (word0 & 0x0000ff00) >> 8;
    mac[5] = (word0 & 0x000000ff);
#else
#error "Please define MAC address"
    /*mac[0] = ;
    mac[1] = ;
    mac[2] = ;
    mac[3] = ;
    mac[4] = ;
    mac[5] = ;*/
#endif

    if (Ethernet.begin(mac) == 0) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Ethernet configuration using DHCP failed");
#endif
        digitalWrite(INFO_LED, HIGH);

        while (1) {}  // wait for watchdog to reset

    } else {
#if defined(ENABLE_DEBUG)
        debugPort.println("DHCP ok");
#endif
    }

    // setup mqtt client
    mqttClient.setClient(ethClient);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.connect(MQTT_ID, MQTT_USER, MQTT_PASS, MQTT_TOPIC_WILL, 0, true, MQTT_STATE_OFF);

    sendData(MQTT_TOPIC_WILL, MQTT_STATE_ON, true);
}

void sendData(const char * topic, char * data, bool retain) {
    digitalWrite(INFO_LED, HIGH);

    if (!mqttClient.connected()) {
        mqttClient.connect(MQTT_ID, MQTT_USER, MQTT_PASS, MQTT_TOPIC_WILL, 0, true, MQTT_STATE_OFF);
        sendData(MQTT_TOPIC_WILL, MQTT_STATE_ON, true);
    }

    mqttClient.publish(topic, data, retain);
    digitalWrite(INFO_LED, LOW);
}
