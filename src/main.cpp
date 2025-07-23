#include <RadioLib.h>
#include <Arduino.h>
#include "definitions.h"
#include <queue>
#include <RollingCode.h>


int gstate = SEED;


SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

void configureRadio()
{
    // Set frequency
    radio.setFrequency(LORA_FREQ);

    // CRITICAL: Both devices must have identical settings
    radio.setBandwidth(125.0); // kHz
    radio.setSpreadingFactor(10); // 7-12
    radio.setCodingRate(5); // 5-8 (4/5 to 4/8)
    radio.setSyncWord(0x12); // Must match on both devices
    radio.setOutputPower(15); // dBm
    radio.setPreambleLength(8); // symbols

    // Enable CRC for error detection
    radio.setCRC(true);

    Serial.println("Radio configured with matching parameters");
}

#ifdef BASE_MODE
#include <WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

void populateQueue();
void printCurrentNet();
void printWifiData();
bool setupWifi();
void connectMqtt();
void sendToMqtt(int);
void initMqtt();
bool validateAndTruncate(int);
int responseToInt(char []);
void initC6Antenna();

void printWiFiStatus(wl_status_t status)
{
    switch (status)
    {
    case WL_IDLE_STATUS:
        Serial.println("WiFi Status: Idle");
        break;
    case WL_NO_SSID_AVAIL:
        Serial.println("WiFi Status: No SSID Available");
        break;
    case WL_SCAN_COMPLETED:
        Serial.println("WiFi Status: Scan Completed");
        break;
    case WL_CONNECTED:
        Serial.println("WiFi Status: Connected");
        break;
    case WL_CONNECT_FAILED:
        Serial.println("WiFi Status: Connect Failed");
        break;
    case WL_CONNECTION_LOST:
        Serial.println("WiFi Status: Connection Lost");
        break;
    case WL_DISCONNECTED:
        Serial.println("WiFi Status: Disconnected");
        break;
    default:
        Serial.printf("WiFi Status: Unknown (%d)\n", status);
    }
}

WiFiClient net;
MQTTClient mqtt(256);
std::queue<int> q;

void setup()
{
#ifdef C6
    initC6Antenna();
#endif

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    Serial.begin(115200);

    // Initialize custom SPI bus
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    // Initialize the radio
    Serial.print(F("[SX1262] Initializing ... "));
    int state = radio.begin();

    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        // while (true); // halt
    }

    configureRadio();

    // You can now send or receive packets...
    Serial.println("Sending test packet...");
    state = radio.transmit("Hello LoRa!");
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

    initMqtt();
}


void loop()
{
    if (!mqtt.connected())
    {
        connectMqtt();
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
                    sendToMqtt(code);
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
//
// int count = 0;
// void loop()
// {
//     String msg = "Message " + String(count);
//     int state = radio.transmit(msg.c_str());
//     Serial.println("Transmitted " + String(count));
//     count++;
//
//     if (state == RADIOLIB_ERR_NONE) {
//         Serial.println("TX success!");
//     } else {
//         Serial.print("TX failed: ");
//         Serial.println(state);
//     }
//     delay(4000);
// }


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

void printCurrentNet()
{
    // print the SSID of the network you're attached to:

    Serial.print("SSID: ");

    Serial.println(WiFi.SSID());

    // print the MAC address of the router you're attached to:

    byte bssid[6];

    WiFi.BSSID(*bssid);

    Serial.print("BSSID: ");

    Serial.print(bssid[5], HEX);

    Serial.print(":");

    Serial.print(bssid[4], HEX);

    Serial.print(":");

    Serial.print(bssid[3], HEX);

    Serial.print(":");

    Serial.print(bssid[2], HEX);

    Serial.print(":");

    Serial.print(bssid[1], HEX);

    Serial.print(":");

    Serial.println(bssid[0], HEX);

    // print the received signal strength:

    long rssi = WiFi.RSSI();

    Serial.print("signal strength (RSSI):");

    Serial.println(rssi);

    Serial.println();
}

void printWifiData()
{
    // print your WiFi shield's IP address:

    IPAddress ip = WiFi.localIP();

    Serial.print("IP Address: ");

    Serial.println(ip);

    Serial.println(ip);

    // print your MAC address:

    byte mac[6];

    WiFi.macAddress(mac);

    Serial.print("MAC address: ");

    Serial.print(mac[5], HEX);

    Serial.print(":");

    Serial.print(mac[4], HEX);

    Serial.print(":");

    Serial.print(mac[3], HEX);

    Serial.print(":");

    Serial.print(mac[2], HEX);

    Serial.print(":");

    Serial.print(mac[1], HEX);

    Serial.print(":");

    Serial.println(mac[0], HEX);
}

bool setupWifi()
{
    try
    {
        WiFi.disconnect();
        delay(5000);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.print("Connecting to Wi-Fi");

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

void initMqtt()
{
    mqtt.begin(MQTT_SERVER, MQTT_PORT, net);
    connectMqtt();
}

void connectMqtt()
{
    if (!mqtt.connected())
    {
        while (!mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)){
            Serial.print("ESP32 - Connecting to MQTT broker");
        Serial.print(".");
        delay(1000);
        }
        Serial.println("MQttconnectsuccess");
    }
}

void sendToMqtt(int state)
{
    StaticJsonDocument<200> message;
    message["state"] = String(state);
    char messageBuffer[512];
    serializeJson(message, messageBuffer);
    mqtt.publish(MQTT_PUBLISH_TOPIC, messageBuffer);
}

int responseToInt(char* response)
{
    // Get the length of the code from the first 2 characters
    char lengthStr[3] = {response[0], response[1], '\0'};
    int codeLength = atoi(lengthStr);

    // Extract the code based on the codeLength
    char codeStr[codeLength + 1]; // +1 for null terminator
    strncpy(codeStr, response + 2, codeLength);
    codeStr[codeLength] = '\0'; // Null-terminate

    // Convert the code string to an integer
    return atoi(codeStr);
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

#ifdef TRIGGER_MODE
#define BUTTON_PIN 1

int state = 1;

void intToResponse(int, char []);

void setup()
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    pinMode(9, OUTPUT);
    digitalWrite(9, HIGH);
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);

    pinMode(BUTTON_PIN, INPUT);

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

    configureRadio();
    // You can now send or receive packets...
}

void loop()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        char msg[64];
        int code = generateNextInt(&gstate);
        intToResponse(code, msg);
        Serial.printf("Sending msg -> %s\n", msg);
        state = radio.transmit((uint8_t*)msg, strlen(msg));
        if (state == RADIOLIB_ERR_NONE)
        {
            // After successful transmit
            Serial.println("Packet sent successfully! Waiting for ACK...");
            digitalWrite(8,LOW);
            delay(100);
            digitalWrite(8,HIGH);

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
                digitalWrite(9, LOW);
                delay(500);
                digitalWrite(9, HIGH);
            }
            else
            {
                digitalWrite(10, LOW);
                delay(500);
                digitalWrite(10, HIGH);
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

// int count = 0;
// void loop()
// {
//     int state = radio.startReceive();
//     String buff;
//     if (state == RADIOLIB_ERR_NONE)
//     {
//         // Check if packet is available
//         if (radio.getPacketLength() > 0)
//         {
//             state = radio.receive(buff, sizeof(buff));
//
//             if (state == RADIOLIB_ERR_NONE)
//             {
//                 digitalWrite(LED_PIN, LOW);
//                 Serial.print("Received: ");
//                 Serial.println(buff);
//                 delay(100);
//                 digitalWrite(LED_PIN, HIGH);
//
//
//             }
//         }
//         delay(100);
//     }
// }

void intToResponse(int value, char response[])
{
    // Convert int to string
    char valueStr[12]; // Enough for a 32-bit int
    sprintf(valueStr, "%d", value);

    int length = strlen(valueStr);

    // Format final response: 2-digit length + value string
    sprintf(response, "%02d%s", length, valueStr);
}
#endif
