#include <BLiveDMClient.h>
#include <string>
#include <map>
#include <tuple>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <HTTPClient.h>

BLiveDMClient* client;
#include "myconfig.h"

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(400);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("WiFi Connected");

  client = (new BLiveDMClient(24643640));
  // 24643640 miriya
  client->print_log = true;
  client->handler = [](Operation op,std::string& message)
  {
    if (op==Operation::SEND_MSG_REPLY)
    {
      DynamicJsonDocument doc(2*message.size());
      deserializeJson(doc,message);
      auto json = doc.as<JsonObject>() ;
      if( DanmakuMessage::Check(json) )
      {
        DanmakuMessage dm(json);
        Serial.printf("发送弹幕 [%s]%s:%s\r\n",String(dm.medal_room_id),dm.uname.c_str(),dm.msg.c_str());
      }
      else if ( json["cmd"].as<String>().indexOf("INTERACT_WORD") > -1 )
      {
        Serial.printf("进入房间: %s %s\r\n",json["data"]["fans_medal"]["medal_name"].as<String>(),json["data"]["uname"].as<const char*>());
        // Serial.println(message.c_str());      
      }
      else
      {
        Serial.printf("cmd:%s\r\n",json["cmd"].as<const char*>());
      }
    }    
    else
    {
      Serial.println(message.c_str());
    }
  };
  client->Start();
  Serial.println("init finish: "+String(client->room_id));
}

void loop()
{
  client->Loop();
}
