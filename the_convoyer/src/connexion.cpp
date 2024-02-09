#include <Arduino.h>
#include <WiFi.h>
#include <M5Stack.h>
#include "connexion.h"

const char* ssid = "Pixel_1850";
const char* password = "ainaaina";

void initWifi() {
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);

    M5.Lcd.print("Waiting for wifi ");
    while(WiFi.status() != WL_CONNECTED){
        M5.Lcd.print(". ");
        delay(200);
    }

    M5.Lcd.clear();
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("Wifi is ready!");
}