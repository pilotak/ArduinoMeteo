#if defined(STM32_MCU_SERIES)
#include <libmaple/iwdg.h>

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

void wdtSetup() {
    iwdg_init(WDT_PRESCALER, WDT_RELOAD);  // init WDT timer
}
#endif

#if defined(ARDUINO_ARCH_ESP32)
#include "esp_system.h"

const int wdtTimeout = 3000;  // time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
    ets_printf("reboot\n");
    esp_restart_noos();
}

void wdtSetup() {
    timer = timerBegin(0, 80, true);                  // timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);  // attach callback
    timerAlarmWrite(timer, 8000000 false);            // set time in us
    timerAlarmEnable(timer);                          // enable interrupt
}

void iwdg_feed() {
    timerWrite(timer, 0);  // reset timer (feed watchdog)
}
#endif

#if defined(__AVR__)
void wdtSetup() {
#error "TODO"
}

void iwdg_feed() {
#error "TODO"
}
#endif
