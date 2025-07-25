//
// Created by mbara on 7/24/2025.
//

#ifndef INIT_H
#define INIT_H

#include <RadioLib.h>
#include <MQTTClient.h>

#include <WiFi.h>


//
// Created by mbara on 7/24/2025.
//


void configureRadio(SX1262*);
void initLed(int);
void initRadio(SX1262*);
bool setupWifi();

void initMqtt(MQTTClient*, NetworkClient*);
void initC6Antenna();

#endif //INIT_H
