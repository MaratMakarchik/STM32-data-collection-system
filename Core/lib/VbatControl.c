#include "VbatControl.h"

VbatControl VbatControl_init(ADC_HandleTypeDef* hADCx){
	VbatControl _VbatControl;
	_VbatControl.hADCx = hADCx;
	_VbatControl.Vbat = 0.0;
	return _VbatControl;

}

void VbatControl_Measurement(VbatControl* _VbatControl){
		HAL_ADCEx_Calibration_Start(_VbatControl->hADCx);
		uint32_t sum = 0;
    
    for (int i = 0; i < ADC_SAMPL; i++) {
        HAL_ADC_Start(_VbatControl->hADCx);
        HAL_ADC_PollForConversion(_VbatControl->hADCx, 100);  // Таймаут 100 мс
        sum += HAL_ADC_GetValue(_VbatControl->hADCx);
        HAL_ADC_Stop(_VbatControl->hADCx);
    }
		HAL_ADC_Stop(_VbatControl->hADCx);
		float v_adc = ( ((float)sum/(float)ADC_SAMPL) / 4095.0) * 3.3f; // Предполагаем, что Vref = 3.3V
		_VbatControl->Vbat = v_adc * (((float)R1 + (float)R2) / (float)(R2)); // 
		 
}