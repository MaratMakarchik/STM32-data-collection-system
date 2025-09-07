#include "main.h"

#define R1 100 //kOm
#define R2 33 //kOm
#define ADC_SAMPL 10

typedef struct VbatControl{

	ADC_HandleTypeDef* hADCx;
	float Vbat;
	
}VbatControl;

VbatControl VbatControl_init(ADC_HandleTypeDef* hADCx);
void VbatControl_Measurement(VbatControl* _VbatControl);

/*
				bat 18650
        Vbat (4.2V max)
          |
          R1 
          |
          +---> GPIO_STM32 (ADC2_IN1 PA1)
          |
          R2 
          |
         GND
*/