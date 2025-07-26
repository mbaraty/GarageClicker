//
// Created by mbara on 7/24/2025.
//

#include "WifiUtils.h"
#include <Arduino.h>
#include "definitions.h"
#include <Debug.h>


void printCurrentNet()
{
    // print the SSID of the network you're attached to:
    DEBUG_PRINT("SSID: ");
    DEBUG_PRINTLN(WiFi.SSID());
    // print the MAC address of the router you're attached to:
    byte bssid[6];
    WiFi.BSSID(*bssid);
    DEBUG_PRINT("BSSID: ");
    DEBUG_PRINT(bssid[5], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(bssid[4], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(bssid[3], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(bssid[2], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(bssid[1], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(bssid[0], HEX);
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    DEBUG_PRINT("signal strength (RSSI):");
    DEBUG_PRINTLN(rssi);
    DEBUG_PRINTLN();
}

void printWifiData()
{
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    DEBUG_PRINT("IP Address: ");
    DEBUG_PRINTLN(ip);
    DEBUG_PRINTLN(ip);
    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    DEBUG_PRINT("MAC address: ");
    DEBUG_PRINT(mac[5], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(mac[4], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(mac[3], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(mac[2], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINT(mac[1], HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(mac[0], HEX);
}


void printWiFiStatus(wl_status_t status)
{
    switch (status)
    {
    case WL_IDLE_STATUS:
        DEBUG_PRINTLN("WiFi Status: Idle");
        break;
    case WL_NO_SSID_AVAIL:
        DEBUG_PRINTLN("WiFi Status: No SSID Available");
        break;
    case WL_SCAN_COMPLETED:
        DEBUG_PRINTLN("WiFi Status: Scan Completed");
        break;
    case WL_CONNECTED:
        DEBUG_PRINTLN("WiFi Status: Connected");
        break;
    case WL_CONNECT_FAILED:
        DEBUG_PRINTLN("WiFi Status: Connect Failed");
        break;
    case WL_CONNECTION_LOST:
        DEBUG_PRINTLN("WiFi Status: Connection Lost");
        break;
    case WL_DISCONNECTED:
        DEBUG_PRINTLN("WiFi Status: Disconnected");
        break;
    default:
        DEBUG_PRINTF("WiFi Status: Unknown (%d)\n", status);
    }
}