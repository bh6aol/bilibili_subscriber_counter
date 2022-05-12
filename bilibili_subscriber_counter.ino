
/**
 * 订阅者计数器v0.0.1
 * 作者:BH6AOL
 * 日期:2022-05-11
 * 单片机:NodeMCU（ESP8266）
 * IDE:Arduino IDE
 * 数码管:HS420361K-32
 */
#include <Arduino.h>
#include <ArduinoJson.h>


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>

ESP8266WiFiMulti WiFiMulti;

/**
 * ssid: wifi名称
 * password：wifi密码
 * vmid：up的uid
 */
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* vmid = "1174548714";


char url_prefix[100] = "https://api.bilibili.com/x/relation/stat?jsonp=jsonp&vmid=";
char *url = strcat(url_prefix,vmid);


const int n_0[8] = {1,1,1,1,1,1,0,0};                // 0~9 数字(共阴)
const int n_1[8] = {0,1,1,0,0,0,0,0};
const int n_2[8] = {1,1,0,1,1,0,1,0};
const int n_3[8] = {1,1,1,1,0,0,1,0};
const int n_4[8] = {0,1,1,0,0,1,1,0};
const int n_5[8] = {1,0,1,1,0,1,1,0};
const int n_6[8] = {1,0,1,1,1,1,1,0};
const int n_7[8] = {1,1,1,0,0,0,0,0};
const int n_8[8] = {1,1,1,1,1,1,1,0};
const int n_9[8] = {1,1,1,1,0,1,1,0};
const int n_err[8] = {0,0,0,0,0,0,0,1};

const int pin_w[4] = {2,0,4,5};                      // 四位位选(低电平有效)
const int pin_n[8] = {14,12,13,15,3,1,16,17};           // 八位数字产生(高电平有效)


/**
 * 输出 一位数字，该方法由print_number()调用
 */
void print_a_number(int n)  
{
    switch(n)
    {
      case 0:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_0[i] );break;
      case 1:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_1[i] );break;
      case 2:    
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_2[i] );break;
      case 3:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_3[i] );break;
      case 4:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_4[i] );break;
      case 5:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_5[i] );break;
      case 6:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_6[i] );break;
      case 7:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_7[i] );break;
      case 8:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_8[i] );break;
      case 9:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_9[i] );break;
      case -1:
          for(int i = 0; i < 8; i++)
              digitalWrite(pin_n[i],n_err[i] );break;
    }
}

/**
 * 向数码管输出数字（最多四位）
 */
void print_number(int n)
{
  int num[4];
  for(int i = 3; i >= 0; i--)
  {
      num[i] = n % 10;
      n  /= 10; 
  }
  for(int i = 0; i < 4; i++)
  {
      digitalWrite(pin_w[i], LOW);
      for(int k = 0; k < 4; k++)
      {
        if(k != i)
            digitalWrite(pin_w[k], HIGH);
      }
      print_a_number(num[i]);
      delay(1);    
  }
}

/**
 * 请求bilibili api，返回订阅者总数
 */
int request(){
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    
    HTTPClient https;
    
    StaticJsonDocument<512> doc;

    https.begin(*client, url);
    int httpCode = https.GET();

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();

      DeserializationError error = deserializeJson(doc, payload);
      if(error)return -1;
      long code = doc["code"];
      if(code != 0)return -1;
      https.end();
      return doc["data"]["follower"];
    }
    return -1;
  }
}


void setup() 
{ 
  //初始化放前面，不然会有奇怪的BUG
  for(int i = 0; i < 4; i++)
      pinMode(pin_w[i], OUTPUT);
  for(int i = 0; i < 7; i++)
      pinMode(pin_n[i], OUTPUT);
  
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

}

void loop()
{
  unsigned int i = 20000; // 大约80s更新一次
  for(int i = 0; i < 4; i++)digitalWrite(pin_w[i], HIGH);// 关闭显示

  // 请求订阅者个数
  int follower = request();
  
  if(follower > -1){
    while(i--)print_number(follower);
  }else{
    while(i--)print_number(-1111);
  }
  
}
