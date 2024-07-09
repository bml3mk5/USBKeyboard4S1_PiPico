/**
 * @file main.h
 * @author Sasaji
 * @brief 
 * @version 0.1
 * @date 2023-12-16
 * @note 
 * 
 * @copyright Copyright (c) Sasaji 2023
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

extern uint8_t key_onoff_flags[20];

extern uint8_t test_mode;

// GPIO number
#define S1_HPP      (2)
#define S1_KRES1    (6)
#define S1_KRES2    (7)
#define S1_LED      (3)
#define S1_Y        (8)
#define S1_BREAK    (9)
#define LED_KATA    (22)
#define LED_HIRA    (21)
#define LED_CAPS    (20)
#define TEST_PIN    (26)

#define PORT_HPP_IS_HIGH    (gpio_get(S1_HPP))

#define PORT_KRES1_IS_SET   (!gpio_get(S1_KRES1))
#define PORT_KRES2_IS_SET   (!gpio_get(S1_KRES2))

#define PORT_LED_IS_LOW     (!gpio_get(S1_LED))

#define LAT_Y_N_SET         gpio_put(S1_Y, false)
#define LAT_Y_N_CLR         gpio_put(S1_Y, true)

#define LAT_BREAK_N_SET     gpio_put(S1_BREAK, false)
#define LAT_BREAK_N_CLR     gpio_put(S1_BREAK, true)

#define LAT_KATA_LED_SET    gpio_put(LED_KATA, true)
#define LAT_KATA_LED_CLR    gpio_put(LED_KATA, false)

#define LAT_HIRA_LED_SET    gpio_put(LED_HIRA, true)
#define LAT_HIRA_LED_CLR    gpio_put(LED_HIRA, false)

#define LAT_CAPS_LED_SET    gpio_put(LED_CAPS, true)
#define LAT_CAPS_LED_CLR    gpio_put(LED_CAPS, false)

#define UPDATE_KATA_LED(status) gpio_put(LED_KATA, ((status) & LED_STATUS_KATA_MASK) != 0)
#define UPDATE_HIRA_LED(status) gpio_put(LED_HIRA, ((status) & LED_STATUS_HIRA_MASK) != 0)
#define UPDATE_CAPS_LED(status) gpio_put(LED_CAPS, ((status) & LED_STATUS_CAPS_MASK) == 0)

#define DEBUG_PRINTF if (test_mode) printf

#endif /* MAIN_H */
