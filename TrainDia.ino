#include <Arduino.h>
#include <ArduinoJson.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

#include "WiFiConfig.h"

#define MODE_DEBUG 1

int mode = 0;

char httpResponceBuff[64];

const char* ntpServer =  "ntp.jst.mfeed.ad.jp"; //日本のNTPサーバー選択
const long  gmtOffset_sec = 9 * 3600;           //9時間の時差を入れる
const int   daylightOffset_sec = 0;             //夏時間はないのでゼロ

const char* url = "https://f568o9ukoc.execute-api.us-east-1.amazonaws.com/default/trainDiaLambda";

const int trainCount = 3;

struct tm g_time_remaining;

class TrainDia{
    public: 
        struct tm timeinfo[trainCount];
        
        TrainDia(){
            timeinfo[0].tm_sec = 99;
            size_of_train_info = 0;
            getNewDia();
        };

        void getNewDia(){
            HTTPClient http;
            http.begin(url);
            int httpCode = http.GET();
            if (httpCode == HTTP_CODE_OK) {
                String body = http.getString();
                body.toCharArray(httpResponceBuff,64);
                Serial.print("Response Body: ");
                Serial.println(body);
                
                StaticJsonBuffer<200> jsonBuffer;
                JsonObject& root = jsonBuffer.parseObject(body);

                size_of_train_info = root["train"].size();

                for(int i = 0; i < root["train"].size(); i++){
                    const char* time1 = root["train"][i];
                    int hours, minutes ;
                    sscanf(time1, "%2d:%2d", &hours, &minutes);
                    timeinfo[i].tm_hour = hours;
                    timeinfo[i].tm_min  = minutes;
                    timeinfo[i].tm_sec  = 0;
                }
            }
            return;
        };

        //不正かどうか確認
        bool shouldFetch(){
            if (timeinfo[0].tm_sec == 99){
                return true;
            }
            if (isPast()) {
                return true;
            }
            return false;

        };

        // 過ぎてるか判定
        bool isPast(){
            struct tm time_now;
            if (!getLocalTime(&time_now)) {
                Serial.println("Failed to obtain time");
                return false;
            }

            if (time_now.tm_hour * 60 + time_now.tm_min >= timeinfo[0].tm_hour * 60 + timeinfo[0].tm_min) {
                Serial.println("past");
                return true;
            }
            else{
                return false;
            }
        };

        bool isLast(){
            if(size_of_train_info == 1){
                return true;
            }
            else{
                return false;
            }   
        };

        bool wasLast(){
            if(size_of_train_info == 0){
                return true;
            }
            if(g_time_remaining.tm_hour > 0){
                return true;
            }

            else{
                return false;
            }
        }

    private:
        int size_of_train_info;
};



TrainDia trainDia = TrainDia();


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
    M5.Lcd.drawJpgFile(SD, "/image_initializing.jpg");

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

void renderLastTrain(){
    M5.Lcd.drawJpgFile(SD, "/image_last_train.jpg");
}

void renderAfterLastTrain(){
    M5.Lcd.drawJpgFile(SD, "/image_after_last.jpg");
}

void renderRemainingTime(){
    struct tm time_now;
    struct tm time_remaining;

    if (!getLocalTime(&time_now)) {
        M5.Lcd.println("Failed to obtain time");
        return;
    }

    int remain_seconds = 0;
    // remain_seconds += (trainDia.timeinfo[0].tm_hour - time_now.tm_hour) * (60 * 60) ; 
    // remain_seconds += (trainDia.timeinfo[0].tm_min  - time_now.tm_min)  * (60) ;
    // remain_seconds += (trainDia.timeinfo[0].tm_sec  - time_now.tm_sec);

    time_remaining = diffTime(trainDia.timeinfo[0], time_now);
    g_time_remaining = time_remaining;

    char str[20];
    M5.Lcd.drawJpgFile(SD, "/image_remaining_time.jpg");
    M5.Lcd.setTextColor(BLACK);
    
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(115,67);
    sprintf(str,"%02d", time_remaining.tm_min);
    M5.Lcd.print(str);
    
    M5.Lcd.setCursor(204,67);
    sprintf(str,"%02d", time_remaining.tm_sec);
    M5.Lcd.print(str);

    M5.Lcd.setCursor(140,188);
    sprintf(str,"%02d:%02d", trainDia.timeinfo[0].tm_hour, trainDia.timeinfo[0].tm_min);
    M5.Lcd.print(str);
    // M5.Lcd.drawCentreString(str, M5.Lcd.width()/2, M5.Lcd.height()/2 - 5*5, 4);
    // M5.Lcd.printf(text);
}

struct tm diffTime(struct tm time1 ,struct tm time2){
    struct tm result;
    int remain_seconds = 0;
    remain_seconds += (time1.tm_hour - time2.tm_hour) * (60*60);
    remain_seconds += (time1.tm_min - time2.tm_min)   * (60)   ;
    remain_seconds += (time1.tm_sec - time2.tm_sec)            ;

    result.tm_hour =  remain_seconds / (60 * 60); 
    remain_seconds -= result.tm_hour * (60 * 60);
    result.tm_min  =  remain_seconds / (60); 
    remain_seconds -= result.tm_min  * (60);
    result.tm_sec  = remain_seconds;

    return result;
}

void renderDebugConsole(){
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        M5.Lcd.println("Failed to obtain time");
        return;
    }
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0 ,0);

    M5.Lcd.println("Date: ");
    M5.Lcd.println(&timeinfo, "%Y %m %d %a %H:%M:%S");

    M5.Lcd.println("Response: ");
    M5.Lcd.println(httpResponceBuff);
}

void loop()
{
    M5.update();

    if(trainDia.shouldFetch()){
        trainDia.getNewDia();
    }

    if(M5.BtnC.isPressed()){
        mode++;
        mode %= 2;
    }

    if(mode == MODE_DEBUG){
        renderDebugConsole();
    }    
    else if(trainDia.isLast()){
        // renderLastTrain();
        renderRemainingTime();
    }
    else if(trainDia.wasLast()){
        renderAfterLastTrain();
    }else{
        renderRemainingTime();
    }



    Serial.print("mode :");
    Serial.println(mode);
    delay(1000);
    printLocalTime();
}