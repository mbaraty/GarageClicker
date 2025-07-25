//
// Created by mbara on 7/24/2025.
//

#include "WifiUtils.h"
#include <Arduino.h>

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