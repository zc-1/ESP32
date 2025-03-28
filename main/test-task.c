#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"

typedef struct
{
    int data;
} queue_data;

#define uxQueueLength 20
#define uxItemSize sizeof(queue_data)

QueueHandle_t Queue1;

void taskSend(void *param)
{
    queue_data dd = {0};
    while (1)
    {
        if (!gpio_get_level(0))
        {
            if (xQueueSendToBack(Queue1, &dd, pdMS_TO_TICKS(10)))
            {
                ESP_LOGI("Send", "%d", dd.data++);
                vTaskDelay(pdMS_TO_TICKS(300));
            }
            else
            {
                ESP_LOGI("SendFail", "");
            };
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void taskRec(void *param)
{
    queue_data dd, *pdd = &dd;

    while (1)
    {
        if (xQueueReceive(Queue1,
                          pdd,
                          pdMS_TO_TICKS(pdMS_TO_TICKS(100000))))
        {
            ESP_LOGI("RecSuccess", "%d", dd.data);
        }
        else
        {
            ESP_LOGI("RecFail", "");
        }
        // vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void configure_but(void)
{
    gpio_reset_pin(0);
    gpio_set_direction(0, GPIO_MODE_INPUT);
    gpio_set_pull_mode(0, GPIO_PULLUP_ONLY);
}

void app_main(void)
{
    Queue1 = xQueueCreate(uxQueueLength, uxItemSize);

    configure_but();
    xTaskCreatePinnedToCore(taskSend, "Send", 2048, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(taskRec, "Rec", 2048, NULL, 4, NULL, 1);
}
