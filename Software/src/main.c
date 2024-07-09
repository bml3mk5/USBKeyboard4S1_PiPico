/**
 * @file main.c
 * @author Sasaji
 * @brief 
 * @version 0.1
 * @date 2023-12-16
 * 
 * @copyright Copyright (c) Sasaji 2023
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <stdbool.h>

#ifdef USE_LOCAL_TINYUSB
#include "bsp/board_api.h"
#else
#include "bsp/board.h"
#endif
#include "tusb.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

#include "pico/binary_info.h"

#include "main.h"
#include "hid_app.h"
#include "s1_led.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
void s1_gpio_init(void);

// gpio_irq_callback_t
void irq_hpp_cb(uint gpio, uint32_t event_mask);

//

uint8_t key_onoff_flags[20];

static uint8_t hpp_counter;

uint8_t test_mode = 0;

/*------------- MAIN -------------*/
int main(void)
{
  bi_decl(bi_program_name("USBKeyboard4S1"));
  bi_decl(bi_program_version_string("0.1"));

  board_init();

  s1_gpio_init();

  test_mode = gpio_get(TEST_PIN) ? 0 : 1;

  DEBUG_PRINTF("USBKeyboard4S1\r\n");

  memset(key_onoff_flags, 0, sizeof(key_onoff_flags));
  key_onoff_flags[0x71 >> 3]=(1 << (0x71 & 7));

  s1_led_init();

  // init host stack on configured roothub port
  tuh_init(BOARD_TUH_RHPORT);

  hid_app_init();

  gpio_set_irq_enabled_with_callback(S1_HPP, (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE), true, irq_hpp_cb);

  while (1)
  {
    // tinyusb host task
    tuh_task();

    hid_app_task();

    s1_led_task();

    if (key_onoff_flags[16] & 1) {
        LAT_BREAK_N_SET;
    } else {
        LAT_BREAK_N_CLR;
    }
  }

  return 0;
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

void tuh_mount_cb(uint8_t dev_addr)
{
  // application set-up
  DEBUG_PRINTF("A device with address %d is mounted\r\n", dev_addr);
  s1_led_unlock();
}

void tuh_umount_cb(uint8_t dev_addr)
{
  // application tear-down
  DEBUG_PRINTF("A device with address %d is unmounted \r\n", dev_addr);
}

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+
void s1_gpio_init(void)
{
  gpio_init(S1_HPP);
  gpio_init(S1_KRES1);
  gpio_init(S1_KRES2);
  gpio_init(S1_LED);
  gpio_init(S1_Y);
  gpio_init(S1_BREAK);
  gpio_init(LED_HIRA);
  gpio_init(LED_KATA);
  gpio_init(LED_CAPS);
  gpio_init(TEST_PIN);

  gpio_set_dir(S1_HPP, GPIO_IN);
  gpio_set_dir(S1_KRES1, GPIO_IN);
  gpio_set_dir(S1_KRES2, GPIO_IN);
  gpio_set_dir(S1_LED, GPIO_IN);
  gpio_set_dir(S1_Y, GPIO_OUT);
  gpio_set_dir(S1_BREAK, GPIO_OUT);
  gpio_set_dir(LED_HIRA, GPIO_OUT);
  gpio_set_dir(LED_KATA, GPIO_OUT);
  gpio_set_dir(LED_CAPS, GPIO_OUT);
  gpio_set_dir(TEST_PIN, GPIO_IN);

  gpio_set_input_enabled(TEST_PIN, true);
  gpio_set_pulls(TEST_PIN, true, false);

  LAT_Y_N_CLR;
  LAT_BREAK_N_CLR;
  LAT_KATA_LED_CLR;
  LAT_HIRA_LED_CLR;
  LAT_CAPS_LED_SET;
}

void irq_hpp_cb(uint gpio, uint32_t event_mask)
{
  uint32_t pos;
  uint32_t bits;
  uint8_t *key;

  gpio_set_irq_enabled(S1_HPP, (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE), false);

  if (event_mask & GPIO_IRQ_EDGE_FALL) {
    // fall down
    LAT_Y_N_CLR;

    if (PORT_LED_IS_LOW) {
      s1_led_set_status(hpp_counter & 0xe);
      s1_led_update();
    }
  } else {
    // rise up
    hpp_counter++;

    // if reset signal settle, clear counter
    if (PORT_KRES1_IS_SET) {
        hpp_counter &= 0xf0;
    }
    if (PORT_KRES2_IS_SET) {
        hpp_counter &= 0x0f;
    }

    if ((hpp_counter & 1) == 0 && PORT_HPP_IS_HIGH) {
        // counter even
        pos = (hpp_counter >> 1);
        bits = (1 << (pos & 7));
        pos = ((pos >> 3) & 15);

        key = &key_onoff_flags[pos];
        
        if (*key & bits) {
            // key pressed
            LAT_Y_N_SET;
        } else {
            LAT_Y_N_CLR;
        }
    } else {
        // counter odd
        LAT_Y_N_CLR;
    }
  }

  gpio_set_irq_enabled(S1_HPP, (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE), true);
}


