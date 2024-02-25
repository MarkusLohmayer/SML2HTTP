#include "EEPROM.h"

#include "Sensor.h"
#include "config.h"
#include "debug.h"

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <IotWebConf.h>
#include <ArduinoJson.h>
#include <list>
#include <sml/sml_file.h>

// Sensor is a state machine
Sensor *sensor;

// WiFi
WiFiClient net;

// Web server
DNSServer dnsServer;
WebServer server(80);

// Callback functions for IotWebConf
void wifiConnected();
void configSaved();

IotWebConf iotWebConf(WIFI_AP_SSID, &dnsServer, &server,
                      WIFI_AP_DEFAULT_PASSWORD, CONFIG_VERSION);

// Implicit state machine
boolean needReset = false;

// Active power consumed from grid
double power = 0;

// Callback that handles a  sensor reading
void process_message(byte *buffer, size_t len, Sensor *sensor) {
  sml_file *file = sml_file_parse(buffer + 8, len - 16);

  for (int i = 0; i < file->messages_len; i++)
  {
    sml_message *message = file->messages[i];
    if (*message->message_body->tag == SML_MESSAGE_GET_LIST_RESPONSE)
    {
      sml_list *entry;
      sml_get_list_response *body;
      body = (sml_get_list_response *)message->message_body->data;
      for (entry = body->val_list; entry != NULL; entry = entry->next)
      {
        // don't crash on empty value
        if (!entry->value) { continue; }
        // filter relevant readings
        if (entry->obj_name->str[0] != 1) { continue; }
        if (entry->obj_name->str[1] != 0) { continue; }
        if (entry->obj_name->str[4] != 0) { continue; }

        double value = sml_value_to_double(entry->value);
        int scaler = (entry->scaler) ? *entry->scaler : 0;
        int prec = -scaler;
        if (prec < 0)
          prec = 0;
        value = value * pow(10, scaler);

        power = value;
      }
    }
  }

	// free the malloc'd memory
	sml_file_free(file);
}


void webResponse() {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["power"] = power;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  server.send(200, "application/json", jsonString);
}


// Arduino setup()
void setup() {
  sensor = new Sensor(&SENSOR_CONFIG, process_message);

  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);

  // WiFi.onStationModeDisconnected(
  //     [](const WiFiEventStationModeDisconnected &event) {
  //       publisher.disconnect();
  //     });

  boolean validConfig = iotWebConf.init();
  if (!validConfig) {
    DEBUG("Missing or invalid config.");
  }

  server.on("/", []() { webResponse(); });
  server.on("/setup", []() { iotWebConf.handleConfig(); });
  server.on("/reset", []() { needReset = true; });
  server.onNotFound([]() { iotWebConf.handleNotFound(); });

  DEBUG("Setup done.");
}

// Arduino loop()
void loop() {
  if (needReset) {
    // Doing a chip reset caused by config changes
    DEBUG("Rebooting after 1 second.");
    delay(1000);
    ESP.restart();
  }

  // Execute sensor state machines
  sensor->loop();
  iotWebConf.doLoop();
  yield();
}

// IotWebConf config saved callback
void configSaved() {
  DEBUG("Configuration was updated.");
  needReset = true;
}

// IotWebConf config WiFi connected callback
void wifiConnected() {
  DEBUG("WiFi connection established.");
}
