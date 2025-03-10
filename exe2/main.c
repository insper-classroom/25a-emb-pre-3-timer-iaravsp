#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int timer_r_estado = 0;
volatile int timer_g_estado = 0;

void btn_callback(uint gpio, uint32_t events)
{
    if (events == 0x4)
    {
        if (gpio == BTN_PIN_R)
            flag_r = !flag_r;
        else if (gpio == BTN_PIN_G)
            flag_g = !flag_g;
    }
}
bool timer_r_callback(repeating_timer_t *rt)
{
    timer_r_estado = 1;
    return true;
}
bool timer_g_callback(repeating_timer_t *rt)
{
    timer_g_estado = 1;
    return true;
}

int main()
{
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_r_freq_hz = 2; // p/ 500ms de intervalo
    int timer_g_freq_hz = 4; // p/ 250ms de intervalo

    // armazena infos do timer
    repeating_timer_t timer_r_info;
    repeating_timer_t timer_g_info;

    int ta_ligado_r = 0;
    int ta_ligado_g = 0;

    add_repeating_timer_us(1000000 / timer_r_freq_hz, timer_r_callback, NULL, &timer_r_info);
    add_repeating_timer_us(1000000 / timer_g_freq_hz, timer_g_callback, NULL, &timer_g_info);

    while (true)
    {
        if (flag_r)
        {
            if (timer_r_estado)
            {
                ta_ligado_r = !ta_ligado_r;
                gpio_put(LED_PIN_R, ta_ligado_r);
                timer_r_estado = 0;
            }
        }
        // cancel_repeating_timer(&timer_r_info);
        else
        {
            gpio_put(LED_PIN_R, 0);
        }
        if (flag_g)
        {
            if (timer_g_estado)
            {
                ta_ligado_g = !ta_ligado_g;
                gpio_put(LED_PIN_G, ta_ligado_g);
                timer_g_estado = 0;
            }
        }
        // cancel_repeating_timer(&timer_g_info);
        else
        {
            gpio_put(LED_PIN_G, 0);
        }
    }
}
