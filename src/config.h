#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"
#include "Sensor.h"

const char *VERSION = "2.3.0";

// Modifying the config version will probably cause a loss of the existig configuration.
// Be careful!
const char *CONFIG_VERSION = "1.0.2";

const char *WIFI_AP_SSID = "SML2HTTP";
const char *WIFI_AP_DEFAULT_PASSWORD = "";

static const SensorConfig SENSOR_CONFIG = {
    .pin = D7,
    .name = "1",
    .numeric_only = true,
    .status_led_enabled = false,
    .status_led_inverted = true,
    .status_led_pin = LED_BUILTIN,
    .interval = 0 // no throttling of messages
};

#endif
