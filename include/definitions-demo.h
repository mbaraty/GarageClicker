//
// Created by mbara on 7/23/2025.
//

#ifndef DEFINITIONS_DEMO_H
#define DEFINITIONS_DEMO_H
#ifndef DEFS_H_
#define DEFS_H_

#define SEED 1234
#define QUEUE_LEN 128

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define MQTT_PORT 1883
#define MQTT_SERVER ""
#define MQTT_PUBLISH_TOPIC ""
#define MQTT_USER ""
#define MQTT_PASS ""
#define MQTT_CLIENT_ID ""

#ifdef C3
#define LORA_SCK 4
#define LORA_MISO 5
#define LORA_MOSI 6
#define LORA_CS   7
#define LORA_RST  21
#define LORA_BUSY 0      // not used
#define LORA_DIO1 20
#define LED_PIN 8

#ifdef TRIGGER_MODE
#define LED_R
#endif

#elif defined(C6)
#define LORA_SCK  19
#define LORA_MISO 20
#define LORA_MOSI 18
#define LORA_CS   17
#define LORA_RST  16
#define LORA_BUSY 23      // not used
#define LORA_DIO1 22
#define LED_PIN LED_BUILTIN
#else
#define LORA_SCK  -1
#define LORA_MISO -1
#define LORA_MOSI -1
#define LORA_CS   -1
#define LORA_RST  -1
#define LORA_BUSY -1      // not used
#define LORA_DIO1 -1
#define LED_PIN LED_BUILTIN
#endif

#define ACK_TIMEOUT_MS 3000

#define LORA_FREQ 915

#define ACK_ATTEMPTS 1
#define ACK_RETRY_DELAY_MS 10
#endif
#endif //DEFINITIONS_DEMO_H
