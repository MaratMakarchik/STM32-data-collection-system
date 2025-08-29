#include "HelpFunction.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms ; i++) {
        for (volatile uint32_t j = 0; j < 1000; j++) {
            __NOP();
        }
    }
}
