#include "EasyWifi-Globals.h"
/*
int searchDBRawData(String name,String feature, JsonArray &rawData)
{
    String strJson;
    if (readIRRemoteDB(strJson) < 0)
    {
        strJson = "[{\"name\":\"GreeAC\",\"feature\":\"mode_off\",\"rawdata\":[1,2,3,4,5,6,7,8],\"len\":8},{\"name\":\"GreeAC\",\"feature\":\"mode_heat\",\"rawdata\":[1,2,3,4,5,6,7,8],\"len\":8}]";
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

// AC
void sendAC_CMD(String name,String characteristic, int value)
{
    LOG("sendAC_CMD");

    if (name.equals(String("GreeAc")))
    {
        if (characteristic.equals(String("TargetHeatingCoolingState")))
        {
            Serial.print("TargetHeatingCoolingState:");
            LOG(value);
        }

       if (characteristic.equals(String("TargetTemperature")))
       {
            Serial.print("TargetTemperature:");
            LOG(value);
       }
    }
}

void sendTVPower_CMD(JsonObject& json)
{
    if (String("On").equals(json["characteristic"].asString()))
    {
        irsend.sendSAMSUNG(0xE0E040BF,32);
        delay(1000);
        irsend.sendNEC(0x80BF3BC4,32);
        delay(1000);
        LOG("sendTVPower_CMD");
    }
}

