/*
sudo gcc -o lora_app main.c LoRa.c -lwiringPi
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "LoRa.h"

#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 255

void* received_data_from_LoRa(void* arg)
{   
    uint8_t RxBuffer[BUFFER_SIZE];
    uint8_t bytesReceived;
    while(1)
    {
        if (digitalRead(arg->DIO0_pin) == HIGH)
            {
                bytesReceived = LoRa_receive(&myLoRa, RxBuffer, BUFFER_SIZE);
                if (bytesReceived > 0)
                {
                    printf("Message %d bytes : %s\n",bytesReceived, RxBuffer);
                }
            }
        sleep(1);
    }
    return NULL;
}

void* transmit_data_to_LoRa(void* arg)
{
    char buffer[BUFFER_SIZE];

    while(1)
    {
        scanf("%d[^\n]",&buffer);
        LoRa_transmit(&myLoRa,(uint8_t*)buffer,strlen(buffer),1000);
        sleep(1);
    }
    return NULL;
}

int main()
{
    // --- 1. Initialize LoRa ---
    LoRa myLoRa = newLoRa();
    myLoRa.CS_pin = 8;
    myLoRa.reset_pin = 25;
    myLoRa.DIO0_pin = 17; // This should be the GPIO number, not the physical pin
    myLoRa.SPI_channel = 0;

    if (LoRa_init(&myLoRa) == LORA_OK)
    {
        printf("--- Lora init ---\n");
    }
    else
    {
        printf("--- Lora fail ---\n");
        return 1;
	}
    
    // --- 2. Main Loop ---
    
    LoRa_startReceiving(&myLoRa);
    pthread_t transmit_data_to_LoRa_thread, received_data_from_LoRa_thread;

    if (pthread_create(&received_data_from_LoRa_thread, NULL, received_data_from_LoRa, &myLoRa) != 0) {
        perror("Error creating the Recive thread");
        return 1;
    }

    if (pthread_create(&transmit_data_to_LoRa_thread, NULL, transmit_data_to_LoRa_thread, &myLoRa) != 0) {
        perror("Error creating the transmit thread");
        return 1;
    }
    pthread_join(transmit_data_to_LoRa_thread, NULL);
    pthread_join(recived_data_from_LoRa_thread, NULL);

    return 0;
} 