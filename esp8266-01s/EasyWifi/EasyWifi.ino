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

// define struct
typedef struct Ap_Info
{
  char ssid[30];
}AP_INFO;

enum MODE {
  MODE_AP_INPUT =0,
  MODE_STA = 1,
};

#define MAX_AP_NUM 20
#define MODE_ADDR 0
#define SSID_ADDR 1 
#define PWD_ADDR  32
#define DEV_NAME_ADDR  64

#define _SSID "ESPap"
#define _PASSWORD "12345678"
#define _MQTT_SERVER "devlot.club"


#define _ssid  "TP-LINK_xuqi"

#define _password  "xgm10503"

AP_INFO ap_list[MAX_AP_NUM];
uint8_t g_ApListNum = 0;
long lastMsg = 0;
char msg[50];
int value = 0;
byte g_mode = 0;
String outTopic = "out/";
String inTopic = "in/";

ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

// EEPROM Data Operation

byte checkMode()
{
 
  EEPROM.begin(512);
  byte value;
  value = EEPROM.read(MODE_ADDR);
  Serial.print("Read Mode:");
  Serial.println(value, DEC);
  EEPROM.end();

  if (value > 1)
  {
    EEPROM.begin(512);
    // write a 0 to all 512 bytes of the EEPROM
    for (int i = 0; i < 512; i++)
      EEPROM.write(i, 0);
    EEPROM.end();
    return 0;
  }
  return value;  
}

void switchMode(enum MODE mode)
{
  Serial.print("switchMode:");
  Serial.println(mode, DEC);
  EEPROM.begin(512);
  if (MODE_AP_INPUT == mode)
  {
    EEPROM.write(MODE_ADDR, 0);
  }
  else if (MODE_STA == mode)
  {
     EEPROM.write(MODE_ADDR, 1);
  }
  else
  {
    
  }
  EEPROM.commit();
  delay(100);
  EEPROM.end();
}

void writeSSID(String strSsid,String strPwd,String strClient)
{
  EEPROM.begin(512);
  Serial.print("writeSSID SSID:");
  Serial.println(strSsid.c_str());

  Serial.print("writeSSID PWD:");
  Serial.println(strPwd.c_str());

  Serial.print("writeSSID Client:");
  Serial.println(strClient.c_str());
  
  for (uint8_t i =0; i <30; i++)
  {
    EEPROM.write(SSID_ADDR + i,strSsid.charAt(i));
    EEPROM.write(PWD_ADDR + i,strPwd.charAt(i));
    EEPROM.write(DEV_NAME_ADDR + i,strClient.charAt(i));
   
  }
  EEPROM.write(SSID_ADDR + 30,'\0');
  EEPROM.write(PWD_ADDR +30,'\0');
  EEPROM.write(DEV_NAME_ADDR +30,'\0');
  EEPROM.commit();
  
}

void readSSID(String &strSsid,String &strPwd,String &strClient)
{
  EEPROM.begin(512);

  for(uint8_t i = 0; i < 30; i++)
  {
    strSsid +=(char)EEPROM.read(SSID_ADDR+i);
    strPwd += (char)EEPROM.read(PWD_ADDR+i);
    strClient += (char)EEPROM.read(DEV_NAME_ADDR+i);
  }
  Serial.print("readSSID SSID:");
  Serial.println(strSsid);

  Serial.print("readSSID PWD:");
  Serial.println(strPwd);

  Serial.print("readSSID PWD:");
  Serial.println(strClient);
  EEPROM.end();
}

void Scan_Ap_List()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("scan start");
  
  while(g_ApListNum == 0)
  { 
     g_ApListNum = WiFi.scanNetworks();
     Serial.println("scan done");
     delay(5000);
  }
  
  if (g_ApListNum >= MAX_AP_NUM)
  {
    g_ApListNum = MAX_AP_NUM;
  }
  
  
  for (int i = 0; i < g_ApListNum; ++i)
  {
    // Print SSID and RSSI for each network found
    strcpy(ap_list[i].ssid,WiFi.SSID(i).c_str());
    delay(10);
  }
 
  Serial.println("");
  delay(100);
}

void handleRoot() {

  String message = "<h2>Step1:</h2>Choose Ap:\n<ul>";
  for(uint8_t i=0;i < g_ApListNum; i++)
  {
        message += "<li><a href='/ap?ap=";
        message += ap_list[i].ssid;
        message += "'>";
        message += ap_list[i].ssid;
        message +="</a></li>";
  }
  message += "</ul>";
  server.send(200, "text/html", message);
  
}
void handleNotFound(){
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}

void handleAp()
{
  String apName;
  String message = "<h2>Step2:</h2>Please Input password:\n";
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i) == "ap")
    {
      apName = server.arg(i);
    }
  }

  message += "<form action='saveconf'>SSID:<input type='text' name='ssid' value='";
  message += apName;
  message += "'></br>PWD:";
  message += "<input type='text' name='password' value=''></br>Client:<input type='text' name='clientname' value='Dev1'></br><input type='submit' value='Submit'></form>";
  
  server.send(200, "text/html", message);
}

void handleSaveConf()
{
  String apName;
  String apPassWord;
  String devName;

  String strSSID;
  String strPWD;
  String strDevName;
  
  String message = "<h2>Step3:</h2>Save Config</br>";
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i) == "ssid")
    {
      apName = server.arg(i);
      message += "SSID:";
      message += apName;
    }
    if(server.argName(i) == "password")
    {
      apPassWord = server.arg(i);
      message += "</br>PWD:";
      message += apPassWord;
    }
    if(server.argName(i) == "clientname")
    {
      devName = server.arg(i);
      message += "</br>Client:";
      message += devName;
    }
  }
  
  message +="</br>please reboot device!";
  switchMode(MODE_STA);
  writeSSID(apName,apPassWord,devName);
  readSSID(strSSID,strPWD,strDevName);
  
  server.send(200, "text/html", message);

}

void Conf_Ap_Server()
{
    /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(_SSID);

  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/ap", handleAp);
  server.on("/saveconf", handleSaveConf);
  server.onNotFound(handleNotFound);
  
  server.begin();
}


void callback(char* topic, byte* payload, unsigned int length) {
  String strTopic = topic;
  Serial.println(strTopic);

  int lastIndex = strTopic.lastIndexOf("/")+1;
  Serial.println(strTopic.substring(lastIndex));
  if (strTopic.substring(lastIndex) == "gpio0")
  {
      if ((char)payload[0] == '1')
      {
          digitalWrite(0, HIGH);
          Serial.println("GPIO0 1");
      }
      else
      {
          digitalWrite(0, LOW);
          Serial.println("GPIO0 0");
      }

  }
  if (strTopic.substring(lastIndex) == "gpio2")
  {
      if ((char)payload[0] == '1')
      {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("GPIO2 1");
      }
      else
      {
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("GPIO2 0");
      }
  }
  

}

void config_mqtt()
{
  String strSSID;
  String strPWD;
  String strClientName;
  int count = 0;
  readSSID(strSSID,strPWD,strClientName);
  outTopic += strClientName;
  inTopic += strClientName;
  if (strSSID != "TP-LINK_xuqi" || strPWD != "xgm10503")
  {
    Serial.println("SSID not TP-LINK_xuqi / xgm10503");
  }
  
  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ssid:");
  Serial.print(strSSID.c_str());
  Serial.print("password:");
  Serial.println(strPWD.c_str());

  WiFi.begin(strSSID.c_str(), strPWD.c_str());
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    count = (count+ 1)%202;
    if (count > 200)
    {
      switchMode(MODE_AP_INPUT);
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(_MQTT_SERVER, 1883);
  client.setCallback(callback);
}


void reconnect() {
  char subTopic[40];
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic.c_str(), "hello world");
      // ... and resubscribe
      delay(3000);
     
     snprintf (subTopic, 40, "%s/#",inTopic.c_str());
     client.subscribe(subTopic);
     Serial.print("SubScribe:");
     Serial.println(subTopic);
 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  delay(5000);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(0, OUTPUT);
  
  g_mode = checkMode();
  
  if (MODE_AP_INPUT == g_mode)
  {
      Scan_Ap_List();
      delay(1000);
      Conf_Ap_Server(); 
  }
  else
  {
    config_mqtt();
  } 
}


void loop() {
  if (MODE_AP_INPUT == g_mode)
  { 
        server.handleClient();
  }
  else
  {
      
      if (!client.connected()) {
        digitalWrite(LED_BUILTIN, LOW);
        reconnect();
        digitalWrite(LED_BUILTIN, HIGH);
      }
      
      client.loop();
    
      long now = millis();
  
      if (now - lastMsg > 5000) {
        lastMsg = now;
        ++value;
        snprintf (msg, 75, "live #%ld", value);
        Serial.println(msg);
        client.publish(outTopic.c_str(), msg);
      }
  }
}
