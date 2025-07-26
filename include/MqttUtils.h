//
// Created by mbara on 7/24/2025.
//

#ifdef BASE_MODE
#ifndef MQTTUTILS_H
#define MQTTUTILS_H

#include <MQTTClient.h>
#include <ArduinoJson.h>


void connectMqtt(MQTTClient*);


void sendToMqtt(MQTTClient*, int);

#endif //MQTTUTILS_H
#endif