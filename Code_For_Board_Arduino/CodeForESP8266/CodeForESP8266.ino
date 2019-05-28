/*      
 *      Khoa Luan Tot Nghiep - He Thong Trong Rau Thuy Canh Ung Dung IoT
 *      5/2019
 *      Code For Wemos D1 R1
 *      Written by TheAnhPham
 *      theanhphamit@gmail.com
 */
#include <SoftwareSerial.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <DHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //https://github.com/Links2004/arduinoWebSockets
#include <SerialCommand.h> // Thêm vào sketch thư viện Serial Command

SerialCommand sCmd; // Khai báo biến sử dụng thư viện Serial Command

WebSocketsClient webSocket;
const char* ssid = "HTThuyCanhIoT";
const char* password = "12345678";

//========= Hàm xử lý các gói tin nhận từ websocket server ==========

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
    {
      Serial.printf("[WSc] Connected to url: %s\n", payload);
    }
      break;
    case WStype_TEXT:
      //Serial.printf("[WSc] get text: %s\n", payload);
      if(strcmp((char*)payload, "LED_ON") == 0) {
         Serial.println("LED 1");
      } else if(strcmp((char*)payload, "LED_OFF") == 0) {
        Serial.println("LED 0");
      }else if(strcmp((char*)payload, "FAN_ON") == 0) {
        Serial.println("FAN 1");
      }else if(strcmp((char*)payload, "FAN_OFF") == 0) {
        Serial.println("FAN 0");
      }else if(strcmp((char*)payload, "PUMP_ON") == 0) {
        Serial.println("PUMP 1");
      }else if(strcmp((char*)payload, "PUMP_OFF") == 0) {
        Serial.println("PUMP 0");
      }else if(strcmp((char*)payload, "control") == 0) {
        Serial.println("control");
      }else if(strcmp((char*)payload, "destroy") == 0) {
        Serial.println("destroy");
      }
      break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 Websocket Client");
  //Cấu hình wifiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP","12345678");
  
  webSocket.begin("httrongrauthuycanhiot.herokuapp.com", 80);
  webSocket.onEvent(webSocketEvent);
  sCmd.addCommand("temp",sendTempData);
  sCmd.addCommand("humi",sendHumiData);
  sCmd.addCommand("lux", sendLuxData);
  sCmd.addCommand("onQuat", sendOnQuatData);
  sCmd.addCommand("onDen", sendOnDenData);
  sCmd.addCommand("tgBom", sendTgBom);
  sCmd.addCommand("tgNghi", sendTgNghi);
  sCmd.addCommand("resetWifi", resetWifi);
}

void loop() {
  sCmd.readSerial();
  webSocket.loop();
  
}
//========== Các hàm nhận lệnh tương ứng với tin nhắn từ Mega 2650 ==========
void resetWifi(){
  ESP.reset();  
  delay(3000);
}

void sendTempData(){
  sendData("temp");
}
void sendHumiData(){
  sendData("humi");
}
void sendLuxData(){
  sendData("lux");
}
void sendOnQuatData(){
  sendData("onQuat");
}
void sendOnDenData(){
  sendData("onDen");
}
void sendTgNghi(){
  sendData("tgNghi");
}
void sendTgBom(){
  sendData("tgBom");
}

void sendData(String dataType){
  char *arg;
  arg = sCmd.next();
  float value = atof(arg);
  String Data = dataType+"="+String(value,1);
  webSocket.sendTXT(Data);
}
