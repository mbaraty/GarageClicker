//
// Created by mbara on 7/24/2025.
//
#ifdef BASE_MODE
#include "MqttUtils.h"
#include <Arduino.h>
#include <definitions.h>
#include <Debug.h>


void connectMqtt(MQTTClient *mqtt)
{
    if (!mqtt->connected())
    {
        while (!mqtt->connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)){
            DEBUG_PRINT("ESP32 - Connecting to MQTT broker");
            DEBUG_PRINTLN(".");
            delay(1000);
        }
        DEBUG_PRINTLN("MQttconnectsuccess");
    }
}

void sendToMqtt(MQTTClient *mqtt, int state)
{
    StaticJsonDocument<200> message;
    message["state"] = String(state);
    char messageBuffer[512];
    serializeJson(message, messageBuffer);
    mqtt->publish(MQTT_PUBLISH_TOPIC, messageBuffer);
}

#endif