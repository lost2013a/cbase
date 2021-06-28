#include <stdio.h>
#include "plat_api.h"

void Plat_led0_set(uint8_t val) {
    static uint8_t i = 0;
    i = (i+1)%2;
    printf("led0 %s\n", i == 0 ? "off": "on");
}



