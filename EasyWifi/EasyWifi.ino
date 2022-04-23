#include "EasyWifi-Globals.h"

void setup() {
  initSerial();
  delay(500);
  initPin();
  delay(500);
  initFS();
  delay(1000);

  if (checkResetBtnPress())
  {
      LOG("===============Reset Button Pressed===========");
  }
  Config config;
  initConfig(config);

  if (config.eState == Ap_state)
  {
      setWifiMode(WIFI_STA);
      scanWifi(g_arrayList);
      printScanList(g_arrayList);

      setWifiApServer(config.strApName);
  }
  else if (config.eState == Station_state)
  {
      setWifiMode(WIFI_OFF);
      delay(1000);
      setWifiMode(WIFI_STA);

      connectStation(config.strConnectSsid,config.strConnectPwd);
      connectMqttServer(config.strMqttServiceAddress);
      
  }
  else
  {

  }

}

void loop() {
  static bool bLoadFinish = false;
  static Config config;

  if (!bLoadFinish)
  {
      loadConfig(config);
      bLoadFinish = true;
  }

  if (config.eState == Ap_state)
  {
    WebServer.handleClient();
  }
  else if (config.eState == Station_state)
  {
    if (!MQTTclient.connected()) 
    {
      //digitalWrite(LED_BUILTIN, LOW);
      reconnect();
      //digitalWrite(LED_BUILTIN, HIGH);
    }
    
    MQTTclient.loop();

    LoopAccessoryLoop(config);

    if (checkResetBtnPress() )
    {
        resetConfig();
    }
  }
}


void resetConfig()
{   
  Config config;
  loadConfig(config);
  LOG("===============Reset Config===========");
  SPIFFS.remove(CONFIG_FILE);
  config.eState = Ap_state;
  setWifiMode(WIFI_STA);
  scanWifi(g_arrayList);
  printScanList(g_arrayList);
  setWifiApServer(config.strApName);
}
void LoopAccessoryLoop(Config &config)
{
  if (config.strServiceName.equals(String("Ws2812b-Lightbulb")))
  {
      //LOG("LoopAccessoryLoop Ws2812b");
      ws2812fx.service();
  }
}

// void serialEvent() {
//   if (Serial.available()) {
//     // get the new byte:
//     char inChar = (char)Serial.read();

//     if (inChar == '0') {
//       resetConfig();
//     }
//   }
// }
