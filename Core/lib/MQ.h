#ifndef MQ7_H
#define MQ7_H



/* Includes ------------------------------------------------------------------*/

#include "main.h"


/* Declarations and definitions ----------------------------------------------*/
#define MQ7_OK 0
#define MQ7_ERR 1



typedef struct MQ7
{
	
	GPIO_TypeDef*		Key_port;
	uint16_t		Key_pin;
	ADC_HandleTypeDef* hADCx;
	uint8_t is_heating;

} MQ7;


MQ7 MQ7_init(GPIO_TypeDef*		Key_port,uint16_t		Key_pin, ADC_HandleTypeDef* hADCx);
void MQ7_Start_heating( MQ7* _MQ7);
void MQ7_Stop_heating( MQ7* _MQ7);
uint8_t MQ7_Measurement(MQ7* _MQ7, uint16_t* array_data, uint8_t len_array);

#endif // #ifndef MQ7_H