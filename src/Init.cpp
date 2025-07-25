//
// Created by mbara on 7/24/2025.
//

#include "Init.h"

#include <WifiUtils.h>
#include <MqttUtils.h>
#include <definitions.h>
#include <Debug.h>

void configureRadio(SX1262 *radio)
{
    // Set frequency
    radio->setFrequency(LORA_FREQ);

    // CRITICAL: Both devices must have identical settings
    radio->setBandwidth(125.0); // kHz
    radio->setSpreadingFactor(11); // 7-12
    radio->setCodingRate(5); // 5-8 (4/5 to 4/8)
    radio->setSyncWord(0x12); // Must match on both devices
    radio->setOutputPower(22); // dBm
    radio->setPreambleLength(8); // symbols

    // Enable CRC for error detection
    radio->setCRC(true);

    DEBUG_PRINTLN("Radio configured with matching parameters");
}

// might need to add polarity
void initLed(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}



void initRadio(SX1262 *radio) {
    int state = radio->begin();

    if (state == RADIOLIB_ERR_NONE)
    {
        DEBUG_PRINTLN(F("success!"));
    }
    else
    {
        DEBUG_PRINT(F("failed, code "));
        DEBUG_PRINTLN(state);
        // while (true); // halt
    }

    configureRadio(radio);
}



#ifdef BASE_MODE
bool setupWifi()
{
    try
    {
        WiFi.disconnect();
        delay(5000);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        DEBUG_PRINT("Connecting to Wi-Fi");

        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            printWiFiStatus(WiFi.status());
            delay(1000);
        }

        printWifiData();
        printCurrentNet();

        return true;
    }
    catch (const char* error)
    {
        return false;
    }
}

void initMqtt(MQTTClient *mqtt, NetworkClient *net)
{
    mqtt->begin(MQTT_SERVER, MQTT_PORT, *net);
    connectMqtt(mqtt);
}




void initC6Antenna()
{
    pinMode(WIFI_ENABLE, OUTPUT); // pinMode(3, OUTPUT);
    digitalWrite(WIFI_ENABLE, LOW); // digitalWrite(3, LOW); // Activate RF switch control

    delay(100);

    pinMode(WIFI_ANT_CONFIG, OUTPUT); // pinMode(14, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH); // digitalWrite(14, HIGH); // Use external antenna
}
#endif