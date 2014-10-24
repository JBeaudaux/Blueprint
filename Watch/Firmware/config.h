// !!!! DO NOT EDIT !!!, use: make config

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define CONFIG_FREQUENCY 868
#define OPTION_TIME_DISPLAY 0
#define CONFIG_METRIC_ONLY
#ifndef THIS_DEVICE_ADDRESS
#define THIS_DEVICE_ADDRESS {0xed,0xc0,0xbb,0x25}
#endif // THIS_DEVICE_ADDRESS
#define USE_WATCHDOG
// DEBUG is not set
#define CONFIG_TEST
#define CONFIG_BLUE
// CONFIG_INFOMEM is not set
#define CONFIG_ACCEL
// CONFIG_ALARM is not set
// #define CONFIG_BATTERY
#define CONFIG_CLOCK
#define CONFIG_DATE
#define CONFIG_RFBSL
//#define CONFIG_STOP_WATCH
#define CONFIG_TEMP
// CONFIG_USEPPT is not set
// CONFIG_USE_SYNC_TOSET_TIME is not set
// CONFIG_USE_DISCRET_RFBSL is not set
// CONFIG_CW_TIME is not set

#endif // _CONFIG_H_
