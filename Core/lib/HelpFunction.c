#include "HelpFunction.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void BuffCLC(uint8_t* buff, uint8_t lenght )
{
    for(uint8_t i = 0; i < lenght; i++)
    {
        *(buff+i) = 0;
    }
}

void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms ; i++) {
        for (volatile uint32_t j = 0; j < 1000; j++) {
            __NOP();
        }
    }
}

void OperatingMode(UseInterface* _interface)
{
    char* command = (char*)_interface->RxBuffer + 3; // Skip the device ID and space
    
    if (strncmp(command, "1", 1) == 0)
    {
        _interface->status = STATUS_MESSURE; 
    }
		
    else if (strncmp(command, "2", 1) == 0)
    {
        _interface->status = STATUS_OLD_DATA;
    }
		
    else if (strncmp(command, "3", 1) == 0)
    {
        _interface->status = STATUS_SETTING;
        char* settings = command + 2; // Skip the command and space
        int lower, upper, interval_ds, interval_mq;
        
				if (sscanf(settings, "%d %d %d %d", &lower, &upper, &interval_ds ,&interval_mq) == 4)
        {
            _interface->temp_lower = lower;
            _interface->temp_upper = upper;
            if (interval_ds > 0)
            { 
							_interface->time_ds = interval_ds;
							
            }
						
						if (interval_mq > 0)
            { 
							_interface->time_heating = interval_mq;
							
            }
						
        }
        else
        {
            _interface ->status = STATUS_ERROR;
        }
    }
		
		else if (strncmp(command, "4", 1) == 0)
		{
			NVIC_SystemReset();
		}
		
    else 
    {
        _interface->status = STATUS_ERROR;
    }
    
    
}    
