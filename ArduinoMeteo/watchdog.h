#if defined(STM32_MCU_SERIES)
    #include <libmaple/iwdg.h>
#endif

void wdtSetup() {
#if defined(STM32_MCU_SERIES)
    iwdg_init(WDT_PRESCALER, WDT_RELOAD);  // init WDT timer
#endif
}

#if defined(__AVR__)
void iwdg_feed() {
    #error "TODO"
}
#endif
