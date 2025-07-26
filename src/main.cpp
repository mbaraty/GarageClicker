#include <RadioLib.h>
#include <Arduino.h>
#include "definitions.h"
#include <queue>
#include <../include/RollingCode.h>
#include <WifiUtils.h>
#include <Init.h>
#include <MqttUtils.h>

#include "Debug.h"
#include "FramUtils.h"

#define DEBUG


SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

#ifdef BASE_MODE
#include <WiFi.h>
#include <MQTTClient.h>

void populateQueue();
bool validateAndTruncate(int);


uint32_t gstate = SEED;
WiFiClient net;
MQTTClient mqtt(256);
std::queue<int> q;

void setup()
{
#ifdef C6
    initC6Antenna();
#endif


    Serial.begin(115200);
    initLed(LED_PIN);

    // Initialize custom SPI bus
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    // Initialize the radio
    Serial.print(F("[SX1262] Initializing ... "));
    initRadio(&radio);
    // You can now send or receive packets...
    Serial.println("Sending test packet...");
    int state = radio.transmit("Hello LoRa!");
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println("Packet sent successfully!");
    }
    else
    {
        Serial.print("Failed, code ");
        Serial.println(state);
    }


    Serial.printf("Setting up wifi\n");
    if (setupWifi())
    {
        Serial.printf("Wifi Success!\n");
    }
    else
    {
        Serial.printf("Wifi failed!\n");
    }

    initMqtt(&mqtt, &net);
}


void loop()
{
    if (!mqtt.connected())
    {
        connectMqtt(&mqtt);
    }
        uint8_t buff[64];
    populateQueue();
    int state = radio.startReceive();

    if (state == RADIOLIB_ERR_NONE)
    {
        // Check if packet is available
        if (radio.getPacketLength() > 0)
        {
            state = radio.receive(buff, sizeof(buff));

            if (state == RADIOLIB_ERR_NONE)
            {
                digitalWrite(LED_PIN, LOW);
                Serial.print("Received: ");
                Serial.println((char*)buff);
                delay(100);
                digitalWrite(LED_PIN, HIGH);

                int code = responseToInt((char*)buff);

                if (validateAndTruncate(code))
                {
                    Serial.printf("Accepted response! \n");
                    sendToMqtt(&mqtt, code);
                    delay(1000);

                    for (int i = 0; i < ACK_ATTEMPTS; i++)
                    {
                        Serial.println("Sending ack");
                        state = radio.transmit("1\0"); // Remove the length parameter
                        if (state == RADIOLIB_ERR_NONE) {
                            Serial.println("ACK sent successfully");
                        } else {
                            Serial.printf("ACK failed, code: %d\n", state);
                        }
                        delay(ACK_TIMEOUT_MS);
                    }
                }

            }
        }
    }

    delay(100);
}

void populateQueue()
{
    while (q.size() <= QUEUE_LEN)
    {
        q.push(generateNextInt(&gstate));
    }
}

bool validateAndTruncate(const int x)
{
    const std::queue<int> t;
    while (!q.empty())
    {
        const int temp = q.front();
        q.pop();
        if (temp == x)
        {
            return true;
        }
    }
    q = t;
    return false;
}


#endif

#ifdef TRIGGER_MODE
#include <FRAM.h>

int state = 1;
uint32_t gstate;

void updateGstate(uint32_t);

FRAM fram;

void setup()
{
    Serial.begin(115200);
    while (!Serial) {}

    Wire.begin(MEM_SDA, MEM_SCL);
    int rv = fram.begin(0x50);
    if (rv != 0)
    {
        DEBUG_PRINT("FRAM INIT ERROR: ");
        DEBUG_PRINTLN(rv);
    }
    else
    {
        DEBUG_PRINTLN("FRAM INIT OK");
    }

    initLed(BLUE_LED);
    initLed(RED_LED);
    initLed(GREEN_LED);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Initialize custom SPI bus
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    // Initialize the radio
    Serial.print(F("[SX1262] Initializing ... "));
    state = radio.begin();

    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true); // halt
    }

    configureRadio(&radio);
    // You can now send or receive packets...

        //updateGstate(SEED);

        gstate = readGstate(&fram);

    DEBUG_PRINTF("GSTATE: %d\n", gstate);
}

void loop()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        DEBUG_PRINTF("Prev gstate: %d\n", gstate);
        char msg[64];
        const int code = generateNextInt(&gstate);
        writeGstate(&fram, gstate);
        DEBUG_PRINTF("new gstate: %d\n", gstate);
        intToResponse(code, msg);
        Serial.printf("Sending msg -> %s\n", msg);
        state = radio.transmit((uint8_t*)msg, strlen(msg));
        if (state == RADIOLIB_ERR_NONE)
        {
            // After successful transmit
            digitalWrite(BLUE_LED,HIGH);
            delay(100);
            digitalWrite(BLUE_LED,LOW);
            Serial.println("Packet sent successfully! Waiting for ACK...");

            // Start receive mode
            radio.startReceive();
            delay(50); // Give radio time to switch modes

            unsigned long start = millis();
            String ackBuff;
            bool ackReceived = false;

            while (millis() - start < ACK_TIMEOUT_MS)
            {
                if (radio.getPacketLength() > 0)
                {
                    state = radio.receive(ackBuff, sizeof(ackBuff));
                    if (state == RADIOLIB_ERR_NONE)
                    {
                        Serial.print("Received ACK: ");
                        Serial.println(ackBuff);
                        if (ackBuff.startsWith("1"))
                        {
                            ackReceived = true;
                            break;
                        }
                    }
                }
                delay(10); // Small delay to prevent overwhelming the radio
            }

            if (ackReceived)
            {
                digitalWrite(GREEN_LED, HIGH);
                delay(500);
                digitalWrite(GREEN_LED, LOW);
            }
            else
            {
                digitalWrite(RED_LED, HIGH);
                delay(500);
                digitalWrite(RED_LED, LOW);
            }
        }
        else
        {
            Serial.print("Failed, code ");
            Serial.println(state);
        }
    }

    delay(100);
}

void updateGstate(uint32_t val)
{
    gstate = val;
    writeGstate(&fram, val);
}


#endif
