/**
 * @file s1_led.h
 * @author Sasaji
 * @brief 
 * @version 0.1
 * @date 2023-12-16
 * 
 * @copyright Copyright (c) Sasaji 2023
 * 
 */

#ifndef S1_LED_H
#define S1_LED_H

#include <stdint.h>

extern uint8_t s1_led_status;
enum en_s1_led_status_flag {
    LED_STATUS_KATA_MASK = 0x02,
    LED_STATUS_HIRA_MASK = 0x04,
    LED_STATUS_CAPS_MASK = 0x08,
};

void s1_led_init(void);

void s1_led_task(void);

void s1_led_update(void);

uint8_t s1_led_get_status(void);
void s1_led_set_status(uint8_t status);

void s1_led_lock(void);
void s1_led_unlock(void);

uint8_t s1_led_need_update(void);
void s1_led_clear_update(void);

void s1_led_start_blink(void);

#endif /* S1_LED_H */
