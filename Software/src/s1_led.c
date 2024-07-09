/**
 * @file s1_led.c
 * @author Sasaji
 * @brief 
 * @version 0.1
 * @date 2023-12-16
 * 
 * @copyright Copyright (c) Sasaji 2023
 * 
 */

#include <stdlib.h>
#ifdef USE_LOCAL_TINYUSB
#include "bsp/board_api.h"
#else
#include "bsp/board.h"
#endif
#include "hardware/gpio.h"
#include "main.h"
#include "s1_led.h"

const uint32_t interval_ms = 500;
static uint32_t start_ms = 0;

uint8_t s1_led_updated;
enum en_s1_led_flags {
    S1_LED_UPDATE = 0x01,
    S1_LED_LOCK = 0x02,
};
uint8_t s1_led_status;
static uint8_t s1_led_count;
static uint8_t s1_led_hira_inv;
static uint8_t s1_led_status_prev;

void s1_led_init(void)
{
  s1_led_updated = 0;
  s1_led_status = 0;
  s1_led_hira_inv = 0;
  s1_led_count = 0;
}

//--------------------------------------------------------------------+
// Blinking Task
//--------------------------------------------------------------------+
void s1_led_task(void)
{
  // Blink every interval ms
  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  s1_led_hira_inv = 0;
  if (s1_led_count) {
    s1_led_count--;
    if (s1_led_count & 1) s1_led_hira_inv = LED_STATUS_HIRA_MASK;
  }

  s1_led_update();
}

void s1_led_update(void)
{
  uint8_t status = (s1_led_status ^ s1_led_hira_inv);
  if (status != s1_led_status_prev) {
    UPDATE_KATA_LED(status);
    UPDATE_HIRA_LED(status);
    UPDATE_CAPS_LED(status);
    s1_led_status_prev = status; 
    s1_led_updated |= S1_LED_UPDATE;

//    DEBUG_PRINTF("LS: %u\r\n", led_status);
  }
}

uint8_t s1_led_get_status(void)
{
    return (s1_led_status ^ s1_led_hira_inv);
}

void s1_led_set_status(uint8_t status)
{
    s1_led_status = status;
}

void s1_led_lock(void)
{
    s1_led_updated |= S1_LED_LOCK;
}

void s1_led_unlock(void)
{
    s1_led_updated &= ~S1_LED_LOCK;
}

uint8_t s1_led_need_update(void)
{
    return (s1_led_updated == S1_LED_UPDATE) ? 1 : 0;
}

void s1_led_clear_update(void)
{
    s1_led_updated &= ~S1_LED_UPDATE;
}

void s1_led_start_blink(void)
{
    s1_led_count = 6;
}
