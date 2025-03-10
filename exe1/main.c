#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int timer_estado = 0;

bool timer_callback(repeating_timer_t *rt)
{
    timer_estado = 1;
    return true;
}

void btn_callback(uint gpio, uint32_t events)
{
    if (events == 0x4)
    { // fall edge
        flag_f_r = !flag_f_r;
    }
    else if (events == 0x8)
    { // rise edge
    }
}

int main()
{
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    int timer_freq_hz = 2;        // p/ 500ms de intervalo
    repeating_timer_t timer_info; // armazena infos do timer

    int ta_ligado = 0;

    while (true)
    {
        if (flag_f_r)
        {
            // inicia o timer só se ele não estiver rodando já
            if (!add_repeating_timer_us(1000000 / timer_freq_hz, timer_callback, NULL, &timer_info))
            {
                printf("Falha ao iniciar o timer\n");
            }
            while (flag_f_r)
            {
                if (timer_estado)
                {
                    // printf("Hello timer!!!!!!!!!!!!\n");
                    ta_ligado = !ta_ligado;
                    gpio_put(LED_PIN_R, !ta_ligado);
                    timer_estado = 0;
                }
            }
            cancel_repeating_timer(&timer_info);
            gpio_put(LED_PIN_R, 0);
        }
    }
}
