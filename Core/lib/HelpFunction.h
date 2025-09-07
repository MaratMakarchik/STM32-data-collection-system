#include "main.h"
#include "stdio.h"


typedef struct DatchikData
{
	float temp;
	uint16_t mqData;
	float Vbat;
	
} DatchikData;

typedef struct UseInterface
{
	uint8_t RxBuffer[128]; 	//lora
	uint8_t TxBuffer[128];
	uint8_t flag_work;			//ds
	uint32_t TDMA_time;
	DatchikData data;		
	
}UseInterface;

void delay_ms(uint32_t ms);
