/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "HelpFunction.h"
#include "LoRa.h"
#include "ds18b20.h"
#include "MQ.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEVICE_ID "10"
#define DEFAULT_LOWER_TEMP (-25)
#define DEFAULT_UPPER_TEMP  35
#define DEFAULT_TIME_DS     60
#define DEFAULT_TIME_HEATING 70
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
LoRa myLoRa;
MQ7 myMQ7;
DS18B20 temperatureSensor;
UseInterface myInterface;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==DIO0_Pin){
        HAL_ResumeTick();
        LoRa_receive(&myLoRa,myInterface.RxBuffer,20);
        if(strncmp((const char*)myInterface.RxBuffer, DEVICE_ID, 2) == 0){
            myInterface.flag_is_i = 1;
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1) 
    {
        HAL_ResumeTick();
        myInterface.flag_tim = 1;
		}
	}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  //MX_ADC2_Init();
  MX_TIM1_Init();
  
  /* USER CODE BEGIN 2 */
  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
  
  // Struct Interface
	
  myInterface.flag_is_i = 0;
	myInterface.flag_tim = 0;
  myInterface.status = STATUS_EXPECT;    
  myInterface.temp_lower = DEFAULT_LOWER_TEMP;
  myInterface.temp_upper = DEFAULT_UPPER_TEMP;
	myInterface.time_heating = DEFAULT_TIME_HEATING;
	myInterface.time_ds = DEFAULT_TIME_DS;
	
	myInterface.data.mqData = 0;
	myInterface.data.temp = 0;
  
  // setting LoRa
  myLoRa = newLoRa();
  myLoRa.CS_port         = NSS_GPIO_Port;
  myLoRa.CS_pin          = NSS_Pin;
  myLoRa.reset_port      = RST_GPIO_Port;
  myLoRa.reset_pin       = RST_Pin;
  myLoRa.DIO0_port       = DIO0_GPIO_Port;
  myLoRa.DIO0_pin        = DIO0_Pin;
  myLoRa.hSPIx           = &hspi1;
    
  myLoRa.frequency             = 433;
  myLoRa.spredingFactor        = SF_7;
  myLoRa.bandWidth             = BW_125KHz;
  myLoRa.crcRate               = CR_4_5;
  myLoRa.power                 = POWER_20db;
  myLoRa.overCurrentProtection = 100;
  myLoRa.preamble              = 8;
  
  if (LoRa_init(&myLoRa) == LORA_OK)
  {
      delay_ms(200);
      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
      delay_ms(200);
      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
      delay_ms(200);
      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
      delay_ms(200);
      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
  }
  else
  {
      myInterface.status = (uint8_t)STATUS_ERROR;
			while(1)
			{
			delay_ms(1000);
      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
			}
  }
  
  LoRa_gotoMode(&myLoRa,RXCONTIN_MODE);
    
  // setting DS18B20
  DS18B20_Init(&temperatureSensor, &huart1);
  DS18B20_InitializationCommand(&temperatureSensor);
  DS18B20_ReadRom(&temperatureSensor);
  DS18B20_ReadScratchpad(&temperatureSensor);
    
  uint8_t settings[3];
  settings[0] = temperatureSensor.temperatureLimitHigh;
  settings[1] = temperatureSensor.temperatureLimitLow;
  settings[2] = DS18B20_12_BITS_CONFIG;
  DS18B20_InitializationCommand(&temperatureSensor);
  DS18B20_SkipRom(&temperatureSensor);
  DS18B20_WriteScratchpad(&temperatureSensor, settings);
  
	DS18B20_InitializationCommand(&temperatureSensor);
  DS18B20_SkipRom(&temperatureSensor);
  DS18B20_ConvertT(&temperatureSensor, DS18B20_DATA);  
  DS18B20_InitializationCommand(&temperatureSensor);
  DS18B20_SkipRom(&temperatureSensor);
  DS18B20_ReadScratchpad(&temperatureSensor);
	myInterface.data.temp = temperatureSensor.temperature;
	
	// setting MQ7
	myMQ7 = MQ7_init(MQ_KEY_GPIO_Port,MQ_KEY_Pin,&hadc1);
		
	__HAL_TIM_CLEAR_FLAG(&htim1, TIM_SR_UIF); 
  HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Stop_IT(&htim1);
	__HAL_TIM_SET_AUTORELOAD(&htim1,myInterface.time_ds*80);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	HAL_TIM_Base_Start_IT(&htim1);
	


	char tmpBuff[20];
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
		
		
    if (myInterface.flag_is_i == 1)
    {
			
			OperatingMode(&myInterface);
			snprintf(tmpBuff,3,DEVICE_ID);
			snprintf(tmpBuff+2,4," OK");
			LoRa_transmit(&myLoRa,(uint8_t*)tmpBuff, 20, 1000);
			myInterface.flag_is_i = 0;
			
			if(myInterface.status == (uint8_t)STATUS_MESSURE){
				MQ7_Start_heating(&myMQ7);
				
				DS18B20_InitializationCommand(&temperatureSensor);
				DS18B20_SkipRom(&temperatureSensor);
				DS18B20_ConvertT(&temperatureSensor, DS18B20_DATA);  
				DS18B20_InitializationCommand(&temperatureSensor);
				DS18B20_SkipRom(&temperatureSensor);
				DS18B20_ReadScratchpad(&temperatureSensor);
				myInterface.data.temp = temperatureSensor.temperature;	
				
				delay_ms(myInterface.time_heating*450);
				MQ7_Measurement(&myMQ7,&myInterface.data.mqData,1);
				MQ7_Stop_heating(&myMQ7);
				
				snprintf(tmpBuff,3,DEVICE_ID);
				snprintf(tmpBuff+2,6,"% .2f",myInterface.data.temp);
		    snprintf(tmpBuff+9,6," %d",myInterface.data.mqData);
				LoRa_transmit(&myLoRa,(uint8_t*)tmpBuff, 20, 1000);
			}
				
			else if(myInterface.status == (uint8_t)STATUS_OLD_DATA)
			{
				snprintf(tmpBuff,3,DEVICE_ID);
				snprintf(tmpBuff+2,6,"% .2f",myInterface.data.temp);
		    snprintf(tmpBuff+9,6," %d",myInterface.data.mqData);
				LoRa_transmit(&myLoRa,(uint8_t*)tmpBuff, 20, 1000);
			}
				
			else if(myInterface.status == (uint8_t)STATUS_SETTING)
			{
				
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_SR_UIF); 
			  HAL_TIM_Base_Start_IT(&htim1);
				HAL_TIM_Base_Stop_IT(&htim1);
				__HAL_TIM_SET_AUTORELOAD(&htim1,myInterface.time_ds*80);
				__HAL_TIM_SET_COUNTER(&htim1, 0);
				HAL_TIM_Base_Start_IT(&htim1);
				LoRa_transmit(&myLoRa,(uint8_t*)"OK", 2, 1000);
				
		   		
			}
			
			else if(myInterface.status == (uint8_t)STATUS_ERROR)
			{
				snprintf(tmpBuff,3,DEVICE_ID);
				snprintf(tmpBuff+3,5," ERR");
				LoRa_transmit(&myLoRa,(uint8_t*)tmpBuff, 9, 1000);
			}			
                  
    }
		
		if(myInterface.flag_tim == 1)
		{
			myInterface.flag_tim = 0;
			
      DS18B20_InitializationCommand(&temperatureSensor);
      DS18B20_SkipRom(&temperatureSensor);
      DS18B20_ConvertT(&temperatureSensor, DS18B20_DATA);  
      DS18B20_InitializationCommand(&temperatureSensor);
      DS18B20_SkipRom(&temperatureSensor);
      DS18B20_ReadScratchpad(&temperatureSensor);
        
      myInterface.data.temp = temperatureSensor.temperature;
        
      if (myInterface.data.temp > (float)myInterface.temp_upper ||
					myInterface.data.temp < (float)myInterface.temp_lower  )
        {
					snprintf(tmpBuff,2,DEVICE_ID);
					
          sprintf(tmpBuff+3,"dngt %.2f", myInterface.data.temp);
          LoRa_transmit(&myLoRa,(uint8_t*)tmpBuff, strlen(tmpBuff), 1000);
        }
        
      myInterface.status = (uint8_t)STATUS_EXPECT;
      LoRa_gotoMode(&myLoRa,RXCONTIN_MODE);
			HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		}
		LoRa_gotoMode(&myLoRa,RXCONTIN_MODE);
		myInterface.status = STATUS_EXPECT;
		BuffCLC((uint8_t*)tmpBuff,20);
		
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */