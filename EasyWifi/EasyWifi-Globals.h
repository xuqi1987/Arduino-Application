#ifndef EASYWIFI_GLOBALS_H_
#define EASYWIFI_GLOBALS_H_

//#define ESP01

const char* CONFIG_FILE = "/config.js";
const char* IR_DB = "/IR.db";
const char* ACC_LIST = "/ACC_LIST.db";
const char* HOST = "EasyWiFi";

#ifdef ESP01
const int LED_PIN =  2;      // the number of the LED pin
const int DHT_PIN = 2; 
const int WS2812_PIN = 0;
const int SWITCH_PIN = 0;

#else
const int BTN_PIN = D8;     // the number of the pushbutton pin
const int LED_PIN =  2;      // the number of the LED pin
const int DHT_PIN = 2; 
const int IR_LED = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

const int SWITCH_PIN = D2;
const int IR_PIN = D5;
const int WS2812_PIN = 0;
#endif




// --- Basic Config Settings ------------------------------------------------------------------------
#include <list>

#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer WebServer(80);

#include <PubSubClient.h>
// MQTT client
WiFiClient mqtt;
PubSubClient MQTTclient(mqtt);

#include <ArduinoJson.h>
#include "FS.h"

#define DEV_STATE "State"
#define DEV_NAME "dev_name"
#define DEV_AP_SSID "ap_name"
#define DEV_CONNECT_SSID "wifi_name"
#define DEV_CONNECT_PWD  "wifi_pass"
#define MQTT_SERVICE_ADDRESS "server_name"
#define PUB_TOPIC "pub_topic"
#define SUB_TOPIC "sub_topic"
#define TOPIC_PREFIX "topic_prefix"
#define SERVICE_NAME "service_name"


#include <WS2812FX.h>

WS2812FX ws2812fx = WS2812FX(8, WS2812_PIN, NEO_GRB + NEO_KHZ800);


#include "DHT.h"

DHT dht(DHT_PIN, DHT11);

#include "RestClient.h"

RestClient restClient = RestClient("127.0.0.1", 5000);


#include <IRsend.h>

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.


#define LOG Serial.println

typedef std::list<String> ScanList;

typedef enum State{
    Init_state = 0,
    Ap_state,
    Station_state,
};

typedef struct _Config
{
    State eState;
    String strDeviceName;
    String strApName;
    String strConnectSsid;
    String strConnectPwd;
    String strMqttServiceAddress;
    String strTopicPrefix;
    String strServiceName;
    // String strPubTopic;
    // String strSubTopic;

}Config;


ScanList g_arrayList;





#endif
