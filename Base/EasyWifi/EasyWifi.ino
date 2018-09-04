/*
 *  This sketch demonstrates how to scan WiFi networks. 
 *  The API is almost the same as with the WiFi Shield library, 
 *  the most obvious difference being the different file you need to include:
 */


#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <ESP8266HTTPClient.h>

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <qrcode.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif


// pin define
#define IR_LED D3 // ESP8266 GPIO pin to use. Recommended: 4 (D2).

// iic 
#define LCD_SCL D1
#define LCD_SDA D2

// const value
#define MAX_AP_NUM 20 // max ap num for scan result list

#define EEPROM_MODE_OFFSET 0
#define EEPROM_SSID_OFFSET 1
#define EEPROM_PASS_OFFSET 32
#define EEPROM_DEVICE_NAME_OFFSET 64
#define EEPROM_SERVICE_NAME_OFFSET 96

#define AP_NAME "EasyWifi"
#define MQTT_SERVER_ADDR "lot-xu.top"
#define MQTT_OUT_TOPIC "out/"
#define MQTT_IN_TOPIC  "in/"

// global value
static String  g_strApList[MAX_AP_NUM]; // scan result,ap list
static uint8_t g_u8ApNum = 0;           // ap list size
static byte g_bMode = 0;
static String g_devName;

ESP8266WebServer webServer(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.
HTTPClient http;
SSD1306Wire  display(0x3c, LCD_SDA, LCD_SCL);
QRcode qrcode (&display);

// type define
typedef struct EEPROM_Data
{
  char strSSID[30];
  char strPassWord[30];
  char strClientName[30];
  char strMqttServerAddress[30];
}EEPROM_DATA;

// function
byte getCurrentMode();              // 0:app 1:station
void changeCurrentMode(byte mode);   // 0:app 1:station

void writeEEPROM(EEPROM_DATA *data);
void readEEPROM(EEPROM_DATA *data);

// web server
void scanApList();
void startApServerSetup();

void handleRoot();
void handleNotFound();
void handleStationSetup();
void handleSaveSetupInfo();

// mqtt client
void mqttSetup();
void mqttReconnect();
void callback(char* topic, byte* payload, unsigned int length) ;

void TvCallback(JsonObject& root);
void TvBoxCallback(JsonObject& root);
void TvHuashuCallback(JsonObject& root);
void DisplayCallback(JsonObject& root);

void changeConfCallback(JsonObject& root);

void responseMqtt(String topic,JsonObject &root);

void restartSystem();

String httpGet(String url);

void setupDisplay();
void lcdDisplay(char *data);
void lcdDisplayQRcode(char *data);
/*
void setupMqttServer();
*/


/* 
 * Function
 */

byte getCurrentMode()
{   
  EEPROM.begin(512);
  byte value = EEPROM.read(EEPROM_MODE_OFFSET);
  EEPROM.end();

  Serial.print("getCurrentMode:");
  Serial.println(value, DEC);

  return value; 
}

void changeCurrentMode(byte mode)
{
  Serial.print("changeCurrentMode:");
  Serial.println(mode, DEC);

  EEPROM.begin(512);
  EEPROM.write(EEPROM_MODE_OFFSET, mode);
  EEPROM.commit();
  delay(100);
  EEPROM.end();
}

void writeEEPROM(EEPROM_DATA *data)
{
  if (NULL == data) return;

  EEPROM.begin(512);

  for (uint8_t i =0; i <30; i++)
  {
    EEPROM.write(EEPROM_SSID_OFFSET + i,data->strSSID[i]);
    EEPROM.write(EEPROM_PASS_OFFSET + i,data->strPassWord[i]);
    EEPROM.write(EEPROM_DEVICE_NAME_OFFSET + i,data->strClientName[i]);
    EEPROM.write(EEPROM_SERVICE_NAME_OFFSET + i,data->strMqttServerAddress[i]);
  }

  EEPROM.write(EEPROM_SSID_OFFSET + 30,'\0');
  EEPROM.write(EEPROM_PASS_OFFSET +30,'\0');
  EEPROM.write(EEPROM_DEVICE_NAME_OFFSET +30,'\0');
  EEPROM.write(EEPROM_SERVICE_NAME_OFFSET +30,'\0');
  EEPROM.commit();
  
  Serial.println("writeEEPROM:");
  Serial.print(data->strSSID);
  Serial.print(data->strPassWord);
  Serial.print(data->strClientName);
  Serial.print(data->strMqttServerAddress);

}

void readEEPROM(EEPROM_DATA *data)
{
  if (NULL == data) return;
 
  EEPROM.begin(512);

  for(uint8_t i = 0; i < 30; i++)
  {
    data->strSSID[i] = (char)EEPROM.read(EEPROM_SSID_OFFSET + i);
    data->strPassWord[i]  = (char)EEPROM.read(EEPROM_PASS_OFFSET + i);
    data->strClientName[i] = (char)EEPROM.read(EEPROM_DEVICE_NAME_OFFSET + i);
    data->strMqttServerAddress[i] = (char)EEPROM.read(EEPROM_SERVICE_NAME_OFFSET + i);
  }

  EEPROM.end();
}

void scanApList()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("scan start");
  
  while(g_u8ApNum == 0)
  { 
     g_u8ApNum = WiFi.scanNetworks();
     Serial.println("scan done");
     delay(1000);
  }
  
  if (g_u8ApNum >= MAX_AP_NUM) g_u8ApNum = MAX_AP_NUM;
  
  for (int i = 0; i < g_u8ApNum; ++i)
  {
    // Print SSID and RSSI for each network found
    g_strApList[i] = WiFi.SSID(i);
    delay(10);
  }
  
  delay(100); 
}

void startApServerSetup()
{
   /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(AP_NAME);

  char strOut[30] = {0};
  IPAddress myIP = WiFi.softAPIP();
  snprintf(strOut,30,"Please connect %s",AP_NAME);
  lcdDisplay(strOut);
  Serial.print("startApServerSetup IP:");
  Serial.println(myIP);
  

  webServer.on("/", handleRoot);
  webServer.on("/station", handleStationSetup);
  webServer.on("/saveconf", handleSaveSetupInfo);
  webServer.on("/restart",restartSystem);
  
  webServer.onNotFound(handleNotFound);
  
  webServer.begin();
}

void handleRoot()
{
  String message = "<h2>Step1:</h2>Choose Ap:\n<ul>";
  for(uint8_t i=0;i < g_u8ApNum; i++)
  {
        message += "<li><a href='/station?ap=";
        message += g_strApList[i];
        message += "'>";
        message += g_strApList[i];
        message +="</a></li>";
  }
  message += "</ul>";
  webServer.send(200, "text/html", message);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i=0; i<webServer.args(); i++){
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
}

void handleStationSetup()
{
  String apName;
  String message = "<h2>Step2:</h2>Please Input password:</br><form action='saveconf'><table><tr>";
  for (uint8_t i=0; i<webServer.args(); i++){
    if (webServer.argName(i) == "ap")
    {
      apName = webServer.arg(i);
    }
  }

  message += "<tr><td>SSID:</td><td><input type='text' name='ssid' value='";
  message += apName;
  message += "'></td></tr>";
  message += "<tr><td>PWD:</td><td><input type='text' name='password' value=''></td></tr>";
  message += "<tr><td>DevName:</td><td><input type='text' name='clientname' value='Dev1'></td></tr>";
  message += "<tr><td>Server:</td><td><input type='text' name='server' value='lot-xu.top'></td><tr>";
  message += "</table><input type='submit' value='Submit'></form>";
  
  webServer.send(200, "text/html", message);
}

void handleSaveSetupInfo()
{
  EEPROM_DATA data;
  
  String message = "<h2>Step3:</h2>Save Config</br> <table>";
  for (uint8_t i=0; i<webServer.args(); i++){
    if (webServer.argName(i) == "ssid")
    {
      //data.strSSID = webServer.arg(i);
      strncpy(data.strSSID,webServer.arg(i).c_str(),webServer.arg(i).length() + 1);
      message += "<tr><td>SSID:</td><td>";
      message += data.strSSID;
      message += "</td></tr>";
    }
    if(webServer.argName(i) == "password")
    {
      //data.strPassWord = webServer.arg(i);
      strncpy(data.strPassWord,webServer.arg(i).c_str(),webServer.arg(i).length() + 1);
      message += "<tr><td>PWD:</td><td>";
      message += data.strPassWord;
      message += "</td></tr>";
    }
    if(webServer.argName(i) == "clientname")
    {
      //data.strClientName = webServer.arg(i);
      strncpy(data.strClientName,webServer.arg(i).c_str(),webServer.arg(i).length() + 1);
      message += "<tr><td>Client:</td><td>";
      message += data.strClientName;
      message += "</td></tr>";
    }
    if(webServer.argName(i) == "server")
    {
     //data.strMqttServerAddress = webServer.arg(i);
      strncpy(data.strMqttServerAddress,webServer.arg(i).c_str(),webServer.arg(i).length() + 1);
      message += "<tr><td>Server:</td><td>";
      message += data.strMqttServerAddress;
      message += "</td></tr>";
    }
  }
  
  message +="</table></br>please reboot device!</br><a href='/restart'>Reboot</a>";
  changeCurrentMode(1);
  writeEEPROM(&data);
  webServer.send(200, "text/html", message);
}

void mqttSetup(){

  int count = 200;
  EEPROM_DATA data;
  
  readEEPROM(&data);
  
  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ssid:");
  Serial.print(data.strSSID);
  Serial.print("password:");
  Serial.println(data.strPassWord);

  g_devName = data.strClientName;
  WiFi.begin(data.strSSID, data.strPassWord);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    count--;
    if (count <= 0)
    {
      changeCurrentMode(0);
      
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  mqttClient.setServer(data.strMqttServerAddress, 1883);
  mqttClient.setCallback(callback);
}

void mqttReconnect() {

  char subTopic[40];
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_OUT_TOPIC, "hello world");
      // ... and resubscribe
      delay(3000);
     
     snprintf (subTopic, 40, "%s#",MQTT_IN_TOPIC);
     mqttClient.subscribe(subTopic);
     
     Serial.print("SubScribe:");
     Serial.println(subTopic);

 
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  String strTopic = topic;
  String strData = (char*)payload;
  Serial.println(strTopic);
  Serial.println(strData);
  
  if (strTopic == "in/from/set")
  {
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(strData);
  
      if (!root.success()) {
          Serial.println("parseObject() failed");
          return;
      }

      if (root["service_name"] == "IR")
      {
          httpGet("http://dev-xu.top:8000");
      }

      if (root["name"] == String(g_devName + "TV"))
      {
        TvCallback(root);
      }

      if (root["name"] == String(g_devName + "TVBox"))
      {
        TvBoxCallback(root);
      }

      if (root["name"] == String(g_devName + "HuashuBox"))
      {
        TvHuashuCallback(root);
      }

      if (root["name"] == String(g_devName + "Display"))
      {
        DisplayCallback(root);
      }

      if(root["name"] == String(g_devName))
      {
        changeConfCallback(root);
      }
  }
}


void TvCallback(JsonObject& root)
{
  uint32_t tvRawData[205] = {4533,4532,557,1696,538,1714,532,1721,557,570,555,571,556,570,557,569,531,596,557,1695,555,1696,558,1696,555,572,532,594,557,569,557,569,533,594,556,570,531,1733,542,573,530,596,556,570,532,595,556,570,559,567,557,1695,557,570,532,1720,529,1723,554,1698,556,1695,556,1696,532,1721,558,47073,4535,4529,530,1723,557,1694,533,1721,530,596,556,570,528,598,557,569,556,570,532,1721,530,1721,557,1696,557,569,558,569,555,571,560,578,545,569,531,596,531,1721,556,570,532,595,555,570,558,569,556,570,557,569,556,1696,558,569,554,1697,532,1720,555,1697,559,1693,559,1694,531,1721,529,47097,4533,4527,533,1719,532,1720,555,1696,532,595,529,597,557,569,529,596,558,569,556,1695,530,1721,559,1693,558,568,530,597,531,595,556,570,530,595,555,571,557,1694,557,568,558,570,532,594,530,595,556,571,556,569,559,1693,555,571,542,1708,559,1694,557,1695,557,1695,557,1695,555,1697,556};
  irsend.sendRaw2(tvRawData,205,38);
}

void TvBoxCallback(JsonObject& root)
{
  uint32_t rawData[77] = {9037,4497,586,1691,586,554,587,552,587,553,588,552,585,555,584,555,586,554,588,1688,589,552,586,1690,572,1705,588,1688,589,1688,560,1716,589,1689,587,553,586,554,585,1693,586,1690,561,1716,587,553,585,1692,589,1688,589,1689,586,1691,585,555,562,578,586,553,561,1716,588,552,561,575,585,39504,9037,2242,587,96269,9035,2245,559};
  irsend.sendRaw2(rawData,77,38);
}

void TvHuashuCallback(JsonObject& root)
{
  uint32_t rawData[77] = {8997,4436,606,522,581,546,606,520,606,1642,607,520,606,520,606,1642,608,529,595,1642,604,1643,607,1649,595,524,606,1640,580,1668,606,1642,606,521,581,546,607,1641,605,520,605,1643,606,522,579,548,608,519,582,555,595,1651,596,522,605,1643,604,523,605,1650,597,1641,606,1642,582,1666,607,39567,8996,2206,605,95896,8969,2233,577};
  irsend.sendRaw2(rawData,77,38);
}

void DisplayCallback(JsonObject& root)
{
  
}

void changeConfCallback(JsonObject& root)
{
  EEPROM_DATA data;
  readEEPROM(&data);
  strncpy(data.strSSID, root["ssid"],30);
  strncpy(data.strPassWord , root["password"],30);
  strncpy(data.strMqttServerAddress, root["mqtt"],30);
  strncpy(data.strClientName , root["device"],30);
  writeEEPROM(&data);
  
  mqttClient.publish(root["topic"],"{\"result\":\"OK\"}");
}

void responseMqtt(String topic,JsonObject& root)
{ 
    mqttClient.publish(topic.c_str(), "hello world");
}

void restartSystem()
{
  String message = "<h2>请重启！</h2>";
  webServer.send(200, "text/html", message);
}

String httpGet(char* url)
{
  http.begin(url);
  int httpCode = http.GET();
  String payload;
  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == 200) {
          payload = http.getString();
          Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  return payload;
}

void setupDisplay()
{
    // Initialising the UI will init the display too.
  display.init();
  qrcode.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
}
void lcdDisplay(char *data)
{
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawStringMaxWidth(0, 0, 128,data);
    display.display();
}

void lcdDisplayQRcode(char *data)
{
  qrcode.create(data);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  setupDisplay();
  g_bMode = getCurrentMode();
  //g_bMode = 0;
  if (0 == g_bMode)
  {
      lcdDisplay("Scan Ap");
      scanApList();
      delay(100);
      startApServerSetup();
      lcdDisplayQRcode("http://192.168.4.1");
  }
  else
  {
      mqttSetup();
  }

}



void loop() {
  
  if (0 == g_bMode)
  {
      webServer.handleClient();
  }
  else
  {
    lcdDisplay("Wait Client Connected");
    if (!mqttClient.connected()) {
        mqttReconnect();
      }
      //digitalWrite(STATE_LED, LOW); 
      mqttClient.loop();
  }
}

