#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_adc/adc_continuous.h"

// #define PWM GPIO_NUM_2
// #define LED GPIO_NUM_0

int duty = 0;

void pwm_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT, // resolução 10 bits (2^10 = 1024)
        .freq_hz = 5000,                      // frequência máxima para a resolução de 13 bits
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer); // chama a função acima "ledc_timer" para confirurar.

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = 2, // saída do PWM no GPIO 2
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel); // chama a função acima "ledc_channel" para confirurar.
}

void task_adc(void *params)
{
    while (true)
    {
        duty = adc1_get_raw(ADC1_CHANNEL_4); // Faz a leitura do ADC0_Chanel4

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty); // seta duty (resolução de 10 bits 2^10 = 1024)
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);    // Aplica o valor do duty cycle
        vTaskDelay(10 / portTICK_PERIOD_MS);
        printf("Duty Sycle: %d \n", duty);
    }
}

void app_main(void)
{

    // Inicia PWM
    pwm_init();

    // Configura os GPIO
    // esp_rom_gpio_pad_select_gpio(PWM);
    // gpio_set_direction(PWM, GPIO_MODE_OUTPUT);
    // esp_rom_gpio_pad_select_gpio(LED);
    // gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // Configura ADC 1 Chanel 4 GPIO 32
    adc1_config_width(ADC_WIDTH_BIT_10);                        // configura resolução de 9 bits (12 bits é o máximo)
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_12); // Configura o ADC1 para leitura no canal 4 com uma atenuação de 11dB

    xTaskCreate(&task_adc, "Task ADC", 2048, NULL, 1, NULL);
}
