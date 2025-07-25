//
// Created by mbara on 7/24/2025.
//

#include "MqttUtils.h"
#include <Arduino.h>
#include <definitions.h>


void connectMqtt(MQTTClient *mqtt)
{
    if (!mqtt->connected())
    {
        while (!mqtt->connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)){
            Serial.print("ESP32 - Connecting to MQTT broker");
            Serial.print(".");
            delay(1000);
        }
        Serial.println("MQttconnectsuccess");
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