#include <Arduino.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

#include "WiFiConfig.h"

const char* ntpServer =  "ntp.jst.mfeed.ad.jp"; //日本のNTPサーバー選択
const long  gmtOffset_sec = 9 * 3600;           //9時間の時差を入れる
const int   daylightOffset_sec = 0;             //夏時間はないのでゼロ

const char* url = "http://s3-ap-northeast-1.amazonaws.com/ktansai/TrainDia/Tsurumiono_weekday.csv";

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

void loop()
{
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    Serial.printf("Response: %d", httpCode);
    Serial.println();
    if (httpCode == HTTP_CODE_OK) {
        String body = http.getString();
        // Serial.print("Response Body: ");
        // Serial.println(body);
    }
    delay(5000);
    printLocalTime();
}