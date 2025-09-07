#ifndef MQ7_H
#define MQ7_H



/* Includes ------------------------------------------------------------------*/

#include "main.h"


/* Declarations and definitions ----------------------------------------------*/
#define MQ7_OK 0
#define MQ7_ERR 1
#define ADC_SAMPLES 10


typedef struct MQ7
{
	
	GPIO_TypeDef*		Key_port;
	uint16_t		Key_pin;
	ADC_HandleTypeDef* hADCx;
	uint8_t is_heating;
	uint16_t res_adc;
	

} MQ7;


MQ7 MQ7_init(GPIO_TypeDef*		Key_port,uint16_t		Key_pin, ADC_HandleTypeDef* hADCx);
void MQ7_Start_heating( MQ7* _MQ7);
void MQ7_Stop_heating( MQ7* _MQ7);
uint8_t MQ7_Measurement(MQ7* _MQ7);

#endif // #ifndef MQ7_H