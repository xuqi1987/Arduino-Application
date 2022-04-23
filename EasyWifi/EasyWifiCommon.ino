#include "EasyWifi-Globals.h"

void initPin()
{
  // initialize the LED pin as an output:
  pinMode(LED_PIN, OUTPUT);
  // initialize the pushbutton pin as an input:
  #ifndef ESP01

  pinMode(BTN_PIN, INPUT);
  #endif
}

bool checkResetBtnPress()
{
#ifndef ESP01
  if (HIGH == digitalRead(BTN_PIN) )
  {
    return true;
  }
  else
  {
    return false;
  }
#else
  return false;
#endif


}

void setWifiMode(WiFiMode_t wifimode) {
  if (WiFi.getMode() == wifimode) {
    return;
  }
  WiFi.mode(wifimode);
  delay(30); // Must allow for some time to init.
}

void scanWifi(ScanList& list)
{
  WiFi.disconnect();
  delay(100);
  
  // WiFi.scanNetworks will return the number of networks found
  while (1) {
    LOG("scan Start");
    int n = WiFi.scanNetworks();
    LOG("scan End");

    if (n == 0)
    {
      LOG("no networks found");
      delay(5000);
      // Wait a bit before scanning again
    }
    else
    {
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        list.push_back(WiFi.SSID(i));
      }
      break;
    }
    
  }

}

void printScanList(ScanList& list)
{
  LOG("===================== printScanList Start=========================");  
  for (ScanList::iterator it=list.begin(); it != list.end(); ++it)
  {
      LOG(*it);
  }
  LOG("===================== printScanList End=========================");  
}

void rootRouter()
{
  Config config;
  loadConfig(config);
  char buffer[1024];
  int n = sprintf(buffer,"/setting?%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s",
    DEV_NAME,config.strDeviceName.c_str(),
    DEV_AP_SSID,config.strApName.c_str(),
    DEV_CONNECT_SSID,config.strConnectSsid.c_str(),
    DEV_CONNECT_PWD,config.strConnectPwd.c_str(),
    MQTT_SERVICE_ADDRESS,config.strMqttServiceAddress.c_str(),
    TOPIC_PREFIX,config.strTopicPrefix.c_str(),
    SERVICE_NAME,config.strServiceName.c_str());
  Serial.print("rootRouter ==> ");
  LOG(buffer);

  String message ="<body><script language='javascript'>document.location = '";
  message += buffer;
  message += "'</script></body>";

  WebServer.send(200, "text/html", message);

}
void settingRouter() 
{
  if (WebServer.method() == HTTP_GET)
  {
    File file = SPIFFS.open("/setting.html", "r");
    WebServer.streamFile(file, "text/html");
    file.close();
  }
  else if (WebServer.method() == HTTP_POST)
  {
    Config config;
    for (uint8_t i=0; i<WebServer.args(); i++)
    {
      if (WebServer.argName(i) == DEV_NAME)
      {
         config.strDeviceName = WebServer.arg(i);
      }
      if (WebServer.argName(i) == DEV_AP_SSID)
      {
          config.strApName = WebServer.arg(i);
      }
      if (WebServer.argName(i) == DEV_CONNECT_SSID)
      {
          config.strConnectSsid = WebServer.arg(i);
      }
      if (WebServer.argName(i) == DEV_CONNECT_PWD)
      {
          config.strConnectPwd = WebServer.arg(i);
      }
      if (WebServer.argName(i) == MQTT_SERVICE_ADDRESS)
      {
          config.strMqttServiceAddress = WebServer.arg(i);
      }
      if (WebServer.argName(i) == TOPIC_PREFIX)
      {
          config.strTopicPrefix = WebServer.arg(i);
      }
      if (WebServer.argName(i) == SERVICE_NAME)
      {
          config.strServiceName = WebServer.arg(i);
      }
      
    }
    config.eState = Station_state;
    saveConfig(config);
    String message ="<body><script language='javascript'>window.setInterval(function(){document.location ='/';},3000); </script>";    
    message += "Set successfully,Please reboot device...";
    message += "</body>";
    
    WebServer.send(200, "text/html", message);

  }
  else
  {

  }

}
void scanListRouter()
{
  Config config;
  loadConfig(config);
  String message = "<html><head><meta charset='UTF-8'><title>Easy WiFi Setting</title><link rel='stylesheet' href='css/style.css' media='screen' type='text/css' /></head>";
  message+= "<body><div class='container'> <div id='contact'><h3>Easy WiFi 设置</h3><h4>请选择以下热点:</h4><ul>";

  for (ScanList::iterator it=g_arrayList.begin(); it != g_arrayList.end(); ++it)
  {
    char url[2048];
    int n = sprintf(url,"/setting?%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s&%s=%s",
    DEV_NAME,config.strDeviceName.c_str(),
    DEV_AP_SSID,config.strApName.c_str(),
    DEV_CONNECT_SSID,String(*it).c_str(),
    DEV_CONNECT_PWD,config.strConnectPwd.c_str(),
    MQTT_SERVICE_ADDRESS,config.strMqttServiceAddress.c_str(),
    TOPIC_PREFIX,config.strTopicPrefix.c_str(),
    SERVICE_NAME,config.strServiceName.c_str());

    message += "<li><a href='";
    message += url;
    message += "'>";
    message += *it;
    message +="</a></li>";
  }

  message += "</ul></div></div></body>";
  WebServer.send(200, "text/html", message);
}

void cssRouter()
{
  File file = SPIFFS.open("/css/style.css", "r");
  WebServer.streamFile(file, "text/css");
  file.close();
}


void setWifiApServer(String strApName) {
  
  WiFi.softAP(strApName.c_str());
	IPAddress myIP = WiFi.softAPIP();

  if (!MDNS.begin(HOST, myIP)) {
    LOG("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  MDNS.begin(HOST);

	LOG(myIP);

  WebServer.on("/", rootRouter);
  WebServer.on("/index.html", rootRouter);
  WebServer.on("/setting", settingRouter);
  WebServer.on("/scanList.html", scanListRouter);
  WebServer.on("/css/style.css", cssRouter);

  WebServer.onNotFound([](){
    WebServer.send(404, "text/plain", "FileNotFound");
  });

	WebServer.begin();

  MDNS.setInstanceName("XuQi's ESP8266");
  MDNS.addService("http", "tcp", 80);

	LOG("HTTP server started");
}

void connectStation(String strSsid,String strPWD)
{
  LOG("===================== connectStation Start=========================");
    bool bState = false;
    int count = 0;
    int delayTime = 500;
    WiFi.begin(strSsid.c_str(), strPWD.c_str());
  
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(delayTime);
      Serial.print(".");

      if (bState)
      {
        digitalWrite(LED_PIN, HIGH); 
        bState = false;
      }
      else
      {
        digitalWrite(LED_PIN, LOW); 
        bState = true;
      }
      count++;

      if (count > 20)
      {
         Config config;
         loadConfig(config);
         config.eState = Ap_state;
         saveConfig(config);
         delayTime = 3000;
         count = 0;
      }
    }
    Serial.print("\nWiFi connected,Get IP:");
    LOG(WiFi.localIP());
    
    digitalWrite(LED_PIN,HIGH);
    LOG("===================== connectStation End=========================");
}

void connectMqttServer(String strAddress)
{
  LOG("===================== connectMqttServer Start=========================");
  MQTTclient.setServer(strAddress.c_str(), 1883);
  MQTTclient.setCallback(recv);
  LOG("===================== connectMqttServer End=========================");
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String strTopic = topic;
  String strData = (char*)payload;
  LOG(strTopic);
  LOG(strData);
}

void blinkLed(int i)
{
  switch(i)
  {
    case 1:
    {
      digitalWrite(LED_PIN,LOW);
      delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
      delay(300);
      digitalWrite(LED_PIN,HIGH);
      delay(300);
      digitalWrite(LED_PIN,LOW);
      delay(1000);
      digitalWrite(LED_PIN,HIGH);
      break;
    }
    default:
    break;
  }
}
void reconnect() {
  Config config;
  loadConfig(config);
  
  // Loop until we're reconnected
  while (!MQTTclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (MQTTclient.connect("ESP8266Client")) {
      
      LOG("connected");
      // Once connected, publish an announcement...
      //MQTTclient.publish(config.strPubTopic.c_str(), "hello world");
     // LOG(config.strPubTopic.c_str());
      // ... and resubscribe
      blinkLed(1);
      LOG("Add accessory to HomeKit..");
      removeAccessory(config);
      delay(500);
      MQTTclient.subscribe(String(config.strTopicPrefix + "/from/#").c_str());
      delay(500);
      
      addAccessory(config);
      
      //snprintf (subTopic, 40, "%s#",inTopic.c_str());
      Serial.print("SubScribe:");
      LOG(String(config.strTopicPrefix + "/from/#").c_str());

      MQTTclient.publish("DB/IR/get","");

    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTTclient.state());
      LOG(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
