#include "EasyWifi-Globals.h"

void setSwitch(JsonObject& json)
{
    Serial.print("setSwitch: ");
    LOG(json["value"].as<bool>());

    if (json["value"].as<bool>())
    {
        digitalWrite(SWITCH_PIN,HIGH);
    }
    else
    {
        digitalWrite(SWITCH_PIN,LOW);
    }
}

void setLightbulb(JsonObject& json)
{
    
    if (String("On").equals(json["characteristic"].asString()))
    {   
        Serial.print("setLightbulbSwitch: ");
        LOG(json["value"].as<bool>());

        if (json["value"].as<bool>())
        {
            
        }
        else
        {

        }
    }

    if (String("Brightness").equals(json["characteristic"].asString()))
    {
        Serial.print("setLightbulbBrightness: ");
        LOG(json["value"].as<int>());
    }
}

void setWs2812bLightbulb(JsonObject& json)
{
    if (String("On").equals(json["characteristic"].asString()))
    {   
        Serial.print("setWs2812bLightbulb: ");
        LOG(json["value"].as<bool>());

        if (json["value"].as<bool>())
        {
            ws2812fx.setBrightness(128);
        }
        else
        {
            ws2812fx.setBrightness(0);
        }
    }

    if (String("Brightness").equals(json["characteristic"].asString()))
    {
        Serial.print("setWs2812bLightbulb: ");
        LOG(json["value"].as<int>());
    }
}


void setThermostat(JsonObject& json)
{
    sendAC_CMD(json["name"].asString(),json["characteristic"].asString(),json["value"].as<int>());
}

void getDHT11Sensor(JsonObject& json)
{
    // Check if any reads failed and exit early (to try again).
    float h = 0.0;
    float t = 0.0;
    do {
        h = dht.readHumidity();
        // Read temperature as Celsius (the default)
        t = dht.readTemperature();
        
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        delay(200);   
    } while(isnan(h) || isnan(t));

    // // Compute heat index in Celsius (isFahreheit = false)
    // float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");

    if (String("CurrentRelativeHumidity").equals(json["characteristic"].asString()))
    {   
        setDHT11HumidityValue2Homebridge(h,json);
    }

    if (String("CurrentTemperature").equals(json["characteristic"].asString()))
    {   
        setDHT11TemperatureValue2Homebridge(t,json);
    }   
}
