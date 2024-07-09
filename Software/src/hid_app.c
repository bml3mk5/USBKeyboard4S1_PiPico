/**
 * @file hid_app.c
 * @author Sasaji
 * @brief 
 * @version 0.1
 * @date 2023-12-16
 * 
 * @copyright Copyright (c) Sasaji 2023
 * 
 */

#ifdef USE_LOCAL_TINYUSB
#include "bsp/board_api.h"
#else
#include "bsp/board.h"
#endif
#include "tusb.h"
#include <hardware/gpio.h>
#include "main.h"
#include "s1_led.h"

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

// for Japanese keyboard
// F9 -> Break key
static const uint8_t key2scancodeTable[256] = {
//  0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
//0                         A     B     C     D     E     F     G     H     I     J     K     L
    0xff, 0xff, 0xff, 0xff, 0x38, 0x49, 0x4b, 0x3b, 0x2b, 0x31, 0x39, 0x32, 0x23, 0x30, 0x33, 0x3c,
//1 M     N     O     P     Q     R     S     T     U     V     W     X     Y     Z     1!    2"
    0x40, 0x42, 0x2c, 0x24, 0x28, 0x21, 0x3a, 0x29, 0x20, 0x41, 0x2a, 0x4a, 0x22, 0x48, 0x1a, 0x1b,
//2 3#    4$    5%    6&    7'    8(    9)    0     ENTER ESC   BS    TAB   SPACE -=    ^~    @`
    0x17, 0x11, 0x19, 0x12, 0x10, 0x13, 0x1c, 0x14, 0x2f, 0x0c, 0x1e, 0x6c, 0x00, 0x16, 0x15, 0x26,
//3 [     \\    ]     ;+    :*    Zen   ,<    .>    /?    CAPS  F1    F2    F3    F4    F5    F6
    0x25, 0x1f, 0x35, 0x34, 0x36, 0xff, 0x43, 0x4c, 0x44, 0x09, 0x50, 0x51, 0x52, 0x53, 0x54, 0x67,
//4 F7    F8    F9    F10   F11   F12   PRINT SCROL PAUSE INSER HOME  PAGEU DELET END   PAGED RIGHT
    0x0b, 0x0a, 0x80, 0xff, 0x46, 0x68, 0x68, 0xff, 0xff, 0x65, 0x2e, 0xff, 0x6f, 0x02, 0x6d, 0x05,
//5 LEFT  DOWN  UP    NUMLO num/  num*  num-  num+  numEN num1  num2  num3  num4  num5  num6  num7
    0x03, 0x04, 0x01, 0xff, 0x45, 0x0f, 0x3f, 0x4f, 0x2f, 0x47, 0x4d, 0x4e, 0x37, 0x3d, 0x3e, 0x1d,
//6 num8  num9  num0  num.  \\|   APP   POWER EQSIZ F13   F14   F15   F16   F17   F18   F19   F20
    0x0d, 0x0e, 0x27, 0x2d, 0x1f, 0xff, 0xff, 0xff, 0x65, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//7 F21   F22   F23   F24   EXEC  HELP  MENU  SELEC STOP  AGAIN UNDO  CUT   COPY  PASTE FIND  MUTE
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//8 VOLUP VOLDW LCAPS LNUM  LSCRO num,  numEQ (_)   Kata  (\\|) Henka Muhen Inte6 Inte7 Inte8 Inte9
    0xff, 0xff, 0xff, 0xff, 0xff, 0x6d, 0xff, 0x46, 0x0a, 0x1f, 0x67, 0x0b, 0xff, 0xff, 0xff, 0xff,
//9 KanaA EisuA Lang3 Lang4 Lang5 Lang6 Lang7 Lang8 Lang9 ALTES SYSRE CALCE CLEAR PRIOR RETUR SEPAR
    0x0a, 0x0b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2f, 0xff,
//A OUT   OPER  CLEAA CRSEL EXSEL
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//B num00 num00 THOUS DECI  CURR  CURRS KEYOP CLOSP OPENC CLOSC numTA numBS numA  numB  numC  numD
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x6c, 0x1e, 0xff, 0xff, 0xff, 0xff,
//C numE  numF  numXO numCA numPE num<  num>  num&  num&& num|  num|| num:  numHA numSP num@  num!
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//D 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//E LCTRL LSHIF LALT  LWIN  RCTRL RSHIF RALT  RWIN
    0x06, 0x07, 0xff, 0x0b, 0x06, 0x07, 0x46, 0x0a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// If your host terminal support ansi escape code such as TeraTerm
// it can be use to simulate mouse cursor movement within terminal
#define USE_ANSI_ESCAPE   0

#define MAX_INFOS   2
#define MAX_REPORT  4

//static uint8_t const keycode2ascii[128][2] =  { HID_KEYCODE_TO_ASCII };

// Each HID instance can has multiple reports
typedef struct st_hid_report
{
  uint8_t count;
  tuh_hid_report_info_t info[MAX_REPORT];
} hid_report_t;

typedef struct st_info
{
  uint8_t dev_addr;
  uint8_t main_protocol;
  uint8_t main_instance;
  hid_report_t reports[CFG_TUH_HID];
} hid_info_t;

static hid_info_t hid_info[MAX_INFOS];

static uint8_t kbd_led_report_index = 0;
static uint8_t kbd_led_report = 0;

static const uint32_t interval_ms = 100;
static uint32_t start_ms = 0;

//--------------------------------------------------------------------+

static void process_kbd_report(hid_keyboard_report_t const *report);
//static void process_mouse_report(hid_mouse_report_t const * report);
static void process_generic_report(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);

static void hid_app_set_report(void);

//--------------------------------------------------------------------+
//--------------------------------------------------------------------+

//
void hid_app_init(void)
{
  for(uint8_t i=0; i<MAX_INFOS; i++) {
    memset(&hid_info[i], 0, sizeof(hid_info[i]));
  }
}

//
void hid_app_task(void)
{
  // Blink every interval ms
  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  hid_app_set_report();
}

//--------------------------------------------------------------------+

uint8_t hid_info_find_by_dev_addr(uint8_t dev_addr) 
{
  for(uint8_t i=0; i<MAX_INFOS; i++) {
    if (hid_info[i].dev_addr == dev_addr) {
      return i;
    }
  }
  return (uint8_t)-1;
}

uint8_t hid_info_find_by_protocol(uint8_t protocol) 
{
  for(uint8_t i=0; i<MAX_INFOS; i++) {
    if (hid_info[i].dev_addr > 0 && hid_info[i].main_protocol == protocol) {
      return i;
    }
  }
  return (uint8_t)-1;
}

uint8_t hid_info_clear(uint8_t dev_addr) 
{
  uint8_t idx = hid_info_find_by_dev_addr(dev_addr);
  if (idx != (uint8_t)-1) {
    hid_info[idx].dev_addr = 0;
  }
  return idx;
}

uint8_t hid_info_set(uint8_t dev_addr) 
{
  uint8_t idx = hid_info_find_by_dev_addr(dev_addr);
  if (idx != (uint8_t)-1) {
    return idx;
  }
  for(idx=0; idx<MAX_INFOS; idx++) {
    if (hid_info[idx].dev_addr == 0) {
      hid_info[idx].dev_addr = dev_addr;
      return idx;
    }
  }
  return (uint8_t)-1;
}

uint8_t hid_app_parse_report(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  if (instance >= CFG_TUH_HID) {
    return (uint8_t)-1;
  }

  uint8_t idx = hid_info_set(dev_addr);
  if (idx == (uint8_t)-1) {
    // error
    return idx;
  }
  hid_info[idx].reports[instance].count = tuh_hid_parse_report_descriptor(hid_info[idx].reports[instance].info, MAX_REPORT, desc_report, desc_len);
  return idx;
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  DEBUG_PRINTF("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);

  // Interface protocol (hid_interface_protocol_enum_t)
  const char* protocol_str[] = { "None", "Keyboard", "Mouse" };
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  DEBUG_PRINTF("HID Interface Protocol = %s\r\n", protocol_str[itf_protocol]);

  s1_led_lock();

  uint8_t info_idx = hid_app_parse_report(dev_addr, instance, desc_report, desc_len);

  switch( itf_protocol ) {
    case HID_ITF_PROTOCOL_KEYBOARD:
      hid_info[info_idx].main_protocol = itf_protocol;
      hid_info[info_idx].main_instance = instance;
      s1_led_start_blink();
      break;

  // By default host stack will use activate boot protocol on supported interface.
  // Therefore for this simple example, we only need to parse generic report descriptor (with built-in parser)
    default:
      break; 

  }
#ifdef DEBUG
  hid_report_t *report = &hid_info[info_idx].reports[instance];
  for(int i=0; i<report->count; i++) {
    tuh_hid_report_info_t *info = &report->info[i];
    printf("  REPORT_ID:%u  USAGE:%u  USAGE_PAGE:%u\r\n", info->report_id, info->usage, info->usage_page);
  }
#endif

  // request to receive report
  // tuh_hid_report_received_cb() will be invoked when report is available
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    DEBUG_PRINTF("Error: cannot request to receive report\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  DEBUG_PRINTF("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  hid_info_clear(dev_addr);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  switch (itf_protocol)
  {
    case HID_ITF_PROTOCOL_KEYBOARD:
      TU_LOG2("HID receive boot keyboard report\r\n");
      process_kbd_report( (hid_keyboard_report_t const*) report );
    break;

    case HID_ITF_PROTOCOL_MOUSE:
      TU_LOG2("HID receive boot mouse report\r\n");
//      process_mouse_report( (hid_mouse_report_t const*) report );
    break;

    default:
      // Generic report requires matching ReportID and contents with previous parsed report info
      process_generic_report(dev_addr, instance, report, len);
    break;
  }
  // next report
  tuh_hid_receive_report(dev_addr, instance);
}

void hid_app_set_report_led(void)
{
  for(; kbd_led_report_index < MAX_INFOS; kbd_led_report_index++) {
    // set_report to keyboard to update led lamps.
    uint8_t dev_addr = hid_info[kbd_led_report_index].dev_addr;
    if (dev_addr == 0 || hid_info[kbd_led_report_index].main_protocol != HID_ITF_PROTOCOL_KEYBOARD) {
      continue;
    }
    uint8_t instance = hid_info[kbd_led_report_index].main_instance;

#ifdef USE_LOCAL_TINYUSB
    if (!tuh_hid_send_ready(dev_addr, instance)) {
      continue;
    }
#endif

    // Send report using interrupt endpoint
    // If report_id > 0 (composite), it will be sent as 1st byte, then report contents. Otherwise only report content is sent.
    if ( !tuh_hid_set_report(dev_addr, instance, 0, HID_REPORT_TYPE_OUTPUT, &kbd_led_report, 1) )
    {
      DEBUG_PRINTF("Error: cannot request to set report\r\n");
      continue;
    }
    break;
  }
}

void hid_app_set_report(void)
{
  if (!s1_led_need_update()) {
    return;
  }

  uint8_t status = s1_led_get_status();
  
  s1_led_clear_update();
  kbd_led_report = 0;
  if ((status & LED_STATUS_KATA_MASK) != 0) kbd_led_report |= KEYBOARD_LED_NUMLOCK;
  if ((status & LED_STATUS_HIRA_MASK) != 0) kbd_led_report |= KEYBOARD_LED_SCROLLLOCK;
  if ((status & LED_STATUS_CAPS_MASK) == 0) kbd_led_report |= KEYBOARD_LED_CAPSLOCK;

  kbd_led_report_index = 0;
  hid_app_set_report_led();
}

TU_ATTR_WEAK void tuh_hid_set_report_complete_cb(uint8_t dev_addr, uint8_t idx, uint8_t report_id, uint8_t report_type, uint16_t len)
{
  kbd_led_report_index++;
  hid_app_set_report_led();
}

//--------------------------------------------------------------------+
// Keyboard
//--------------------------------------------------------------------+

#if 0
// look up new key in previous keys
static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode)
{
  for(uint8_t i=0; i<6; i++)
  {
    if (report->keycode[i] == keycode)  return true;
  }

  return false;
}
#endif

static void process_kbd_report(hid_keyboard_report_t const *report)
{
  int i;
  uint8_t key;
  uint8_t new_key_onoff_flags[20];

  memset(new_key_onoff_flags, 0, sizeof(new_key_onoff_flags));
  new_key_onoff_flags[0x71 >> 3]=(1 << (0x71 & 7));

  for (i = 0; i < 8; i++) {
    if (report->modifier & (1 << i)) {
      key = key2scancodeTable[i + HID_KEY_CONTROL_LEFT];  
      if (key != 0xff) {
        new_key_onoff_flags[key >> 3] |= (1 << (key & 7));

        DEBUG_PRINTF("%02x\n", key);
      }
    }
  }

  for (i = 0; i < 6; i++) {
    key = key2scancodeTable[report->keycode[i]];
    if (key != 0xff) {
      new_key_onoff_flags[key >> 3] |= (1 << (key & 7));

      DEBUG_PRINTF("%02x\n", key);

      if (key == 0x09) {
        if (test_mode) {
          s1_led_set_status(s1_led_status ^ LED_STATUS_CAPS_MASK);
          s1_led_update();
        }
      }
    }
  }
  memcpy(key_onoff_flags, new_key_onoff_flags, sizeof(key_onoff_flags));
}

//--------------------------------------------------------------------+
// Mouse
//--------------------------------------------------------------------+

#if 0
void cursor_movement(int8_t x, int8_t y, int8_t wheel)
{
#if USE_ANSI_ESCAPE
  // Move X using ansi escape
  if ( x < 0)
  {
    printf(ANSI_CURSOR_BACKWARD(%d), (-x)); // move left
  }else if ( x > 0)
  {
    printf(ANSI_CURSOR_FORWARD(%d), x); // move right
  }

  // Move Y using ansi escape
  if ( y < 0)
  {
    printf(ANSI_CURSOR_UP(%d), (-y)); // move up
  }else if ( y > 0)
  {
    printf(ANSI_CURSOR_DOWN(%d), y); // move down
  }

  // Scroll using ansi escape
  if (wheel < 0)
  {
    printf(ANSI_SCROLL_UP(%d), (-wheel)); // scroll up
  }else if (wheel > 0)
  {
    printf(ANSI_SCROLL_DOWN(%d), wheel); // scroll down
  }

  printf("\r\n");
#else
  printf("(%d %d %d)\r\n", x, y, wheel);
#endif
}

static void process_mouse_report(hid_mouse_report_t const * report)
{
  static hid_mouse_report_t prev_report = { 0 };

  //------------- button state  -------------//
  uint8_t button_changed_mask = report->buttons ^ prev_report.buttons;
  if ( button_changed_mask & report->buttons)
  {
    printf(" %c%c%c ",
       report->buttons & MOUSE_BUTTON_LEFT   ? 'L' : '-',
       report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-',
       report->buttons & MOUSE_BUTTON_RIGHT  ? 'R' : '-');
  }

  //------------- cursor movement -------------//
  cursor_movement(report->x, report->y, report->wheel);
}
#endif

//--------------------------------------------------------------------+
// Generic Report
//--------------------------------------------------------------------+
static void process_generic_report(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  uint8_t info_idx = hid_info_find_by_dev_addr(dev_addr);

  uint8_t const rpt_count = hid_info[info_idx].reports[instance].count;
  tuh_hid_report_info_t* rpt_info_arr = hid_info[info_idx].reports[instance].info;
  tuh_hid_report_info_t* rpt_info = NULL;

  if ( rpt_count == 1 && rpt_info_arr[0].report_id == 0)
  {
    // Simple report without report ID as 1st byte
    rpt_info = &rpt_info_arr[0];
  }else
  {
    // Composite report, 1st byte is report ID, data starts from 2nd byte
    uint8_t const rpt_id = report[0];

    // Find report id in the array
    for(uint8_t i=0; i<rpt_count; i++)
    {
      if (rpt_id == rpt_info_arr[i].report_id )
      {
        rpt_info = &rpt_info_arr[i];
        break;
      }
    }

    report++;
    len--;
  }

  if (!rpt_info)
  {
    DEBUG_PRINTF("Couldn't find the report info for this report !\r\n");
    return;
  }

  // For complete list of Usage Page & Usage checkout src/class/hid/hid.h. For examples:
  // - Keyboard                     : Desktop, Keyboard
  // - Mouse                        : Desktop, Mouse
  // - Gamepad                      : Desktop, Gamepad
  // - Consumer Control (Media Key) : Consumer, Consumer Control
  // - System Control (Power key)   : Desktop, System Control
  // - Generic (vendor)             : 0xFFxx, xx
  if ( rpt_info->usage_page == HID_USAGE_PAGE_DESKTOP )
  {
    switch (rpt_info->usage)
    {
      case HID_USAGE_DESKTOP_KEYBOARD:
        TU_LOG1("HID receive keyboard usage in generic report\r\n");
        // Assume keyboard follow boot report layout
        process_kbd_report( (hid_keyboard_report_t const*) report );
      break;

      case HID_USAGE_DESKTOP_MOUSE:
        TU_LOG1("HID receive mouse usage in generic report\r\n");
        // Assume mouse follow boot report layout
//        process_mouse_report( (hid_mouse_report_t const*) report );
      break;

      default: break;
    }
  }
}
