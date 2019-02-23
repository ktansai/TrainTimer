#include <Arduino.h>
#include <ArduinoJson.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

#include "WiFiConfig.h"

const char* ntpServer =  "ntp.jst.mfeed.ad.jp"; //日本のNTPサーバー選択
const long  gmtOffset_sec = 9 * 3600;           //9時間の時差を入れる
const int   daylightOffset_sec = 0;             //夏時間はないのでゼロ

const char* url = "https://f568o9ukoc.execute-api.us-east-1.amazonaws.com/default/trainDiaLambda";

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%Y %m %d %a %H:%M:%S");    //日本人にわかりやすい表記へ変更
}






void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    M5.begin();
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

int value = 0;

void renderRemainingTime(const char* text){
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(5);
    M5.Lcd.drawCentreString(text, M5.Lcd.width()/2, M5.Lcd.height()/2 - 5*5, 1);
    // M5.Lcd.printf(text);
}

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
        
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(body);
        const char* time1 = root["train"][0];
        Serial.println(time1);
        renderRemainingTime(time1);
    }
    delay(5000);
    printLocalTime();
}