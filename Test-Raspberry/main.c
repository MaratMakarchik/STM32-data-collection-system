/*
sudo gcc -o lora_terminal main.c -lwiringPi -lpthread
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include "LoRa.h"

#define BUFFER_SIZE 255

LoRa myLoRa; // Глобальная переменная для модуля LoRa

// Функция для проверки формата команды
int parse_command(const char* buffer, unsigned int* beacon_time) {
    char st[3], fn[3];
    return sscanf(buffer, "%2s %u %2s", st, beacon_time, fn) == 3 &&
           strcmp(st, "st") == 0 && strcmp(fn, "fn") == 0;
}

void* receive_lora_data(void* arg) {
    uint8_t RxBuffer[BUFFER_SIZE];
    uint8_t bytesReceived;

    while (1) {
        if (digitalRead(myLoRa.DIO0_pin) == HIGH) {
            bytesReceived = LoRa_receive(&myLoRa, RxBuffer, BUFFER_SIZE);
            if (bytesReceived > 0) {
                RxBuffer[bytesReceived] = '\0'; // Добавляем нулевой терминатор
                printf("Received LoRa message: %s\n", RxBuffer);
            }
        }
        usleep(1000); // Короткая задержка для уменьшения нагрузки на CPU
    }
    return NULL;
}

int main() {
    // Инициализация wiringPi
    if (wiringPiSetupGpio() == -1) {
        printf("wiringPi initialization failed\n");
        return 1;
    }

    // Инициализация LoRa
    myLoRa = newLoRa();
    myLoRa.CS_pin = 8;
    myLoRa.reset_pin = 25;
    myLoRa.DIO0_pin = 17;
    myLoRa.SPI_channel = 0;

    if (LoRa_init(&myLoRa) != LORA_OK) {
        printf("LoRa initialization failed\n");
        return 1;
    }

    LoRa_startReceiving(&myLoRa);
    printf("LoRa initialized successfully. Type commands in format 'st <beacon_time> fn'\n");
    printf("Type 'exit' to quit.\n");

    // Создаем поток для приема данных от LoRa
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_lora_data, NULL) != 0) {
        perror("Failed to create receive thread");
        return 1;
    }

    // Главный цикл для отправки команд
    char buffer[BUFFER_SIZE];
    while (1) {
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Убираем символ новой строки
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            len--;
        }

        // Проверяем команду выхода
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Парсим и проверяем команду
        unsigned int beacon_time;
        if (parse_command(buffer, &beacon_time)) {
            // Отправляем команду через LoRa
            LoRa_transmit(&myLoRa, (uint8_t*)buffer, len, 1000);
            printf("Sent: %s\n", buffer);
        } else {
            printf("Error: invalid format. Expected: st <beacon_time> fn\n");
            printf("Example: st 12345 fn\n");
        }
    }

    // Завершение программы
    printf("Exiting...\n");
    pthread_cancel(receive_thread); // Завершаем поток приема
    pthread_join(receive_thread, NULL);
    
    return 0;
}