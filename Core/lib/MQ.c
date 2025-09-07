#include "MQ.h"

MQ7 MQ7_init(GPIO_TypeDef*		Key_port,uint16_t		Key_pin, ADC_HandleTypeDef* hADCx)
{
	MQ7 new_MQ7;
	new_MQ7.hADCx = hADCx;
	new_MQ7.Key_pin = Key_pin;
	new_MQ7.Key_port = Key_port;
	new_MQ7.is_heating = 0;
	return new_MQ7;
}

void MQ7_Start_heating( MQ7* _MQ7)
{
	HAL_GPIO_WritePin( _MQ7->Key_port,_MQ7->Key_pin,GPIO_PIN_SET);
	_MQ7->is_heating = 1;
}

void MQ7_Stop_heating( MQ7* _MQ7)
{
	HAL_GPIO_WritePin( _MQ7->Key_port,_MQ7->Key_pin,GPIO_PIN_RESET);
	_MQ7->is_heating = 0;
}

uint8_t MQ7_Measurement(MQ7* _MQ7)
{
	HAL_ADCEx_Calibration_Start(_MQ7->hADCx);
	if (_MQ7 ->is_heating == 0) return MQ7_ERR;
    uint32_t sum = 0;
    
    for (int i = 0; i < ADC_SAMPLES; i++) {
        HAL_ADC_Start(_MQ7->hADCx);
        HAL_ADC_PollForConversion(_MQ7->hADCx, 100);  // Таймаут 100 мс
        sum += HAL_ADC_GetValue(_MQ7->hADCx);
        HAL_ADC_Stop(_MQ7->hADCx);
    }
		_MQ7->res_adc  = sum/ADC_SAMPLES;
		HAL_ADC_Stop(_MQ7->hADCx); 
		return MQ7_OK;
}