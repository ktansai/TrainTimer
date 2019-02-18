#include <Arduino.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "WiFiConfig.h"


const char* url = "http://s3-ap-northeast-1.amazonaws.com/ktansai/TrainDia/Tsurumiono_weekday.csv";

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

int value = 0;

void loop()
{
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    Serial.printf("Response: %d", httpCode);
    Serial.println();
    if (httpCode == HTTP_CODE_OK) {
        String body = http.getString();
        Serial.print("Response Body: ");
        Serial.println(body);
    }
    delay(5000);
    }
