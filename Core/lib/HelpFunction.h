#include "main.h"
#include "stdio.h"

#define STATUS_EXPECT 0   // ����� ��������
#define STATUS_MESSURE 1  // ������ �� ���������
#define STATUS_OLD_DATA 2 // ������ ������ ������
#define STATUS_SETTING 3  // ��������� ���������� 
#define STATUS_RESTART 4  // ������������ 
#define STATUS_ERROR 10   // ������ ���������
 



typedef struct DatchikData
{
	float temp;
	uint16_t mqData;
	float VCC;
	
} DatchikData;

typedef struct UseInterface
{
	uint8_t flag_is_i;//stm
	uint8_t status;//stm
	
	uint8_t RxBuffer[128];//lora
	
	uint8_t flag_tim;//ds
  uint32_t time_ds; //ds
	int8_t  temp_lower;//ds
	int8_t  temp_upper;//ds
	
	uint8_t time_heating;//mq second
	
	DatchikData data;
	
}UseInterface;

void BuffCLC(uint8_t* buff, uint8_t lenght );
void delay_ms(uint32_t ms);
void OperatingMode(UseInterface* _interface);