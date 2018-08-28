## homebridge-mqtt

[Link](https://www.npmjs.com/package/homebridge-mqtt)

### 配置文件
	
	/Users/xuqi/.homebridge

```
{
    "bridge": {
    "name": "Homebridge",
    "username": "B8:E8:56:17:E3:58",
    "port": 51825,
    "pin": "123-11-122"
    },
    
    "description": "This is an example configuration file with pilight plugin.",

    
  "platforms": [
    {
        "platform": "mqtt",
  	"name": "mqtt",
  	"url": "mqtt://lot-xu.top",
  	"port": "1883",
  	"topic_type": "multiple",
  	"topic_prefix": "in",
  	"username": "",
  	"password": "",
  	"qos": 1
    }]
}

```

