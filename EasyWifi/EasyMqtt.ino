#include "EasyWifi-Globals.h"

String getServiceByName(String strName,int index)
{
    if(strName.equals(String("Ws2812b-Lightbulb")))
    {
        return String("Lightbulb");
    }
    if (strName.equals(String("DHT11")) && index == 0)
    {
        return String("HumiditySensor");
    }
    if (strName.equals(String("DHT11")) && index == 1)
    {
        return String("TemperatureSensor");
    }
    if (strName.equals(String("GreeAC")) && index == 0) 
    {
        return String("Thermostat");
    }

    return strName;
}

// 如果是自定义设备的化，设备的信息会从网上下下来

bool isCustomIRRemoteAccessory(Config &config)
{
    if (config.strServiceName.equals(String("IRremote")))
    {
        return true;
    }
    return false;
}

int getAccessoryNum(Config &config)
{
    // 如果一个esp设备要加两个传感器
    if (config.strServiceName.equals(String("DHT11")))
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

String getName(String strName,int index = 0)
{
    String strNewName;

    if (index > 0)
    {
        strNewName = String(strName + String(index));
    }
    else
    {
        strNewName = strName;
    }
    //LOG(strNewName);
    return strNewName;
}


void initAccessory(Config &config)
{
    if (config.strServiceName.equals(String("Switch")))
    {
        pinMode(SWITCH_PIN, OUTPUT);
    }
    if(config.strServiceName.equals(String("Ws2812b-Lightbulb")))
    {
        LOG("init Ws2812b Accessory");
        ws2812fx.init();
        ws2812fx.setBrightness(128);
        ws2812fx.setSegment(0,  0,  7, FX_MODE_BLINK, 0xFF0000, 1000, false); // segment 0 is leds 0 - 9
    }
    if (config.strServiceName.equals(String("DHT11")))
    {
        LOG("init DHT 11");
        dht.begin();
    }
    if (config.strServiceName.equals(String("IRremote")))
    {
        pinMode(IR_PIN,OUTPUT);
    }

}

void loadCustomAccessory(String &strJsonAccInfo)
{
    // 取得自定义Accessory的信息
    int code = restClient.get("/accessory",&strJsonAccInfo);

    if (code == 200)
    {   
        LOG("=================Download Success==================");
        writeAccessoryInfo(strJsonAccInfo);
    }
    else
    {
        LOG("=================Download Failed==================");
        
        strJsonAccInfo = "[{\"name\": \"TVBoxSwitch\", 	\"service_name\": \"TVPowerSwitch\", 	\"service\": \"Switch\" }]";
        //strJsonAccInfo = "[{\"name\":\"GreeAc\",\"service\":\"IRRemoteAC\",\"service_name\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
        writeAccessoryInfo(strJsonAccInfo);
        /*
        if (readAccessoryInfo(strJsonAccInfo) <0)
        {
            //strJsonAccInfo="[{\"name\":\"OtherAC\",\"service\":\"Thermostat\",\"service_name\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
           strJsonAccInfo = "[{\"name\":\"GreeAc\",\"service\":\"IRRemoteAC\",\"service_name\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
           //strJsonAccInfo="[{\"name\":\"GreeAC\",\"service\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
           writeAccessoryInfo(strJsonAccInfo);
        }
        */
    }
}


void loadIRDataBase(String &strJsonIRDataBase)
{
    // 取得自定义Accessory的信息
    int code = restClient.get("/database",&strJsonIRDataBase);

    if (code == 200)
    {   
        LOG("=================Download Success==================");
        writeAccessoryInfo(strJsonIRDataBase);
    }
    else
    {
        LOG("=================Download Failed==================");
        
        strJsonIRDataBase = "[{\"name\":\"TVPowerSwitch\",\"service_name\":\"PowerSwitch\",\"service\":\"Switch\"}]";
        //strJsonIRDataBase = "[{\"name\":\"GreeAc\",\"service\":\"IRRemoteAC\",\"service_name\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
        writeAccessoryInfo(strJsonIRDataBase);
        /*
        if (readAccessoryInfo(strJsonIRDataBase) <0)
        {
            //strJsonIRDataBase="[{\"name\":\"OtherAC\",\"service\":\"Thermostat\",\"service_name\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
           strJsonIRDataBase = "[{\"name\":\"GreeAc\",\"service\":\"IRRemoteAC\",\"service_name\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
           //strJsonIRDataBase="[{\"name\":\"GreeAC\",\"service\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
           writeAccessoryInfo(strJsonIRDataBase);
        }
        */
    }
}

void opAccessory(bool isAdd,Config & config)
{
    // 判断是否是自定义数据
    if (isCustomIRRemoteAccessory(config))
    {
        StaticJsonBuffer<1024> staticJsonBuffer;
        
        String strJsonAccInfo;
        loadCustomAccessory(strJsonAccInfo);

        String strJsonIRDB;
        loadIRDataBase(strJsonIRDB);

        JsonArray& arrayAccessory = staticJsonBuffer.parseArray(strJsonAccInfo.c_str());

        for (JsonObject& objAccessory : arrayAccessory) 
        {
            String data;
            String addTopic;

            if (isAdd) 
            {
                addTopic = String(config.strTopicPrefix + "/to/add");
            }
            else
            {
                addTopic = String(config.strTopicPrefix + "/to/remove");
            }
            
            const int capacity = JSON_OBJECT_SIZE(3);
            StaticJsonBuffer<capacity> jsonBuffer;
            JsonObject& json = jsonBuffer.createObject();
            
            json["name"] = objAccessory["name"].as<char*>();

            if (isAdd)
            {
                json["service_name"] = objAccessory["service_name"].as<char*>();
                json["service"] = objAccessory["service"].as<char*>();
                Serial.print("addAccessory: ");
            }
            else
            {
                Serial.print("removeAccessory: ");
            }
    
            json.printTo(data);

            LOG(data);
            MQTTclient.publish(addTopic.c_str(), data.c_str());
        }

    }
    else
    {
        // 判断ServcieName需要追加几个Accessory
        int num = getAccessoryNum(config);
        for (int index =0; index < num; index++)
        {
            String data;
            String addTopic;

            if (isAdd) 
            {
                addTopic = String(config.strTopicPrefix + "/to/add");
            }
            else
            {
                addTopic = String(config.strTopicPrefix + "/to/remove");
            }

            const int capacity = JSON_OBJECT_SIZE(4);
            StaticJsonBuffer<capacity> jsonBuffer;
            JsonObject& json = jsonBuffer.createObject();
            String name = getName(config.strDeviceName,index);
            json["name"] = name.c_str();

            if (isAdd)
            {
                json["service_name"] = config.strServiceName.c_str();
                String strService = getServiceByName(config.strServiceName ,index);
                json["service"] = strService.c_str();

                if (strService.equals(String("Lightbulb")))
                {
                    json["Brightness"] = "default";
                }

                Serial.print("addAccessory: ");
            }
            else
            {
                Serial.print("removeAccessory: ");
            }
            
            json.printTo(data);
            LOG(data);
            MQTTclient.publish(addTopic.c_str(), data.c_str());
        }
    }

    // 初始化pin 脚模式
    initAccessory(config); 
}
void addAccessory(Config &config)
{
    opAccessory(true,config);

    // 初始化pin 脚模式
    initAccessory(config);

}

void removeAccessory(Config &config)
{
    opAccessory(false,config);

}

void setSwitchValue2Homebridge(bool value)
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/set");
    String data;
    const int capacity = JSON_OBJECT_SIZE(4);
    StaticJsonBuffer<capacity> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = config.strDeviceName.c_str();
    json["service_name"] = config.strServiceName.c_str();
    json["characteristic"] = "On";
    json["value"] = value;
    json.printTo(data);

    Serial.print("public Switch status to Homebridge: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
}

void setDHT11HumidityValue2Homebridge(float fHumidity,JsonObject& in_json)
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/set");
    String data;
    const int capacity = JSON_OBJECT_SIZE(4);
    StaticJsonBuffer<capacity> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = in_json["name"].asString();
    json["service_name"] = in_json["service_name"].asString();
    json["characteristic"] = in_json["characteristic"].asString();
    json["value"] = fHumidity;
    json.printTo(data);

    Serial.print("public Humidity status to Homebridge: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
}

void setDHT11TemperatureValue2Homebridge(float fTemperature,JsonObject& in_json)
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/set");
    String data;
    const int capacity = JSON_OBJECT_SIZE(4);
    StaticJsonBuffer<capacity> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = in_json["name"].asString();
    json["service_name"] = in_json["service_name"].asString();
    json["characteristic"] = in_json["characteristic"].asString();
    json["value"] = fTemperature;
    json.printTo(data);

    Serial.print("public Temperature status to Homebridge: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
}

void recv(char* topic, byte* payload, unsigned int length)
{
    Config config;
    loadConfig(config);

    String strTopic = topic;
    String strJsondata = (char*)payload;
    strJsondata = strJsondata.substring(0,length);

    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(strJsondata);

    if (!json.success()) {
        
        Serial.print("Failed to parse recv json data:");
        LOG(strJsondata);
        return;
    } 
    
    if (strTopic == String(config.strTopicPrefix + "/from/get"))
    {
        Serial.print("homebrige ==>Get: ");
        LOG(strJsondata);
        if (String("DHT11").equals(json["service_name"].asString()))
        {
            getDHT11Sensor(json);
        }
    } 
    // homebrige设置值到其它设备
    else if (strTopic == String(config.strTopicPrefix + "/from/set"))
    {
        Serial.print("homebrige ==>Set: ");
        LOG(strJsondata);
        
        if (isCustomIRRemoteAccessory(config))
        {
            // 根据功能调用不同函数
            // 如果是空调
            if (String("IRRemoteAC").equals(json["service_name"].asString()))
            {
                sendAC_CMD(json["name"].asString(),json["characteristic"].asString(),json["value"].as<int>());
            }
            if (String("TVPowerSwitch").equals(json["service_name"].asString()))
            {
                sendTVPower_CMD(json);
                //sendTV_CMD(JsonObject& json);
            }
        }
        else
        {
            if (config.strDeviceName.equals(json["name"].asString()))
            {
                // 用Service Name 区分是什么功能
                if (String("Switch").equals(json["service_name"].asString()))
                {
                    setSwitch(json);
                }
                else if (String("Lightbulb").equals(json["service_name"].asString()))
                {
                    setLightbulb(json);
                }
                else if (String("Ws2812b-Lightbulb").equals(json["service_name"].asString()))
                {
                    setWs2812bLightbulb(json);
                }
                else if(String("Thermostat").equals(json["service_name"].asString()))
                {
                    setThermostat(json);
                }
                else
                {
                    LOG(json["service_name"].asString());
                }
            }
        }

    }
    // 收到homebrige的response
    else if(strTopic == String(config.strTopicPrefix + "/from/response"))
    {
        Serial.print("homebrige ==>response: ");
        LOG(strJsondata);
    }
    // 收到homebrige的response
    else if(strTopic == String(config.strTopicPrefix + "/from/IR/info"))
    {
        LOG(strJsondata);
        writeAccessoryInfo(strJsondata);
        LOG("homebrige ==>/from/IR: ");  
    }
    else
    {
        LOG(strTopic);
    }
    

}

void send(String topic,String data)
{   
    MQTTclient.publish(topic.c_str(), data.c_str());
}


/*
int searchRawData(String &strAccName,String &strFeature, JsonArray &rawData)
{
    String strJson;
    if (readAccessoryInfo(strJson) < 0)
    {
        strJson = "[{\"name\":\"OtherAC\",\"service\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[1,3,4,5,6,7,8,9,0,1],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]},{\"name\":\"GreeAC\",\"service\":\"Thermostat\",\"feature\":[{\"name\":\"mode_off\",\"rawData\":[2,3,4,5,6,7,2,4,5,6,7,3],\"len\":100},{\"name\":\"mode_heat\",\"rawData\":[],\"len\":100}]}]";
    }

    const int capacity = 2*JSON_ARRAY_SIZE(0) + 3*JSON_ARRAY_SIZE(2) + 2*JSON_ARRAY_SIZE(10) + 6*JSON_OBJECT_SIZE(3);
    StaticJsonBuffer<capacity> jsonBuffer;

    JsonArray& arrayAccessory = jsonBuffer.parseArray(strJson.c_str());

    if (!arrayAccessory.success()) 
    {
        Serial.println("parseObject() failed");
        return -1;
    }
    for (JsonObject& objAccessory : arrayAccessory) {
        if (strAccName.equals(String(objAccessory["name"].as<char*>())))
        {
            JsonArray & arrayFeature = objAccessory["feature"];

            for (JsonObject& objFeature : arrayFeature) 
            {
                Serial.println(objFeature["name"].as<char*>());
                //Serial.println(elem1["len"].as<int>());
                rawData = objFeature["rawData"];
                Serial.println(rawData.size());
                return 1;
            }           
        }
    }
    return 0;
}
*/