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


![](http://aliyunzixunbucket.oss-cn-beijing.aliyuncs.com/jpg/84aa1a58b0cbad6d25aa7c22487341cd.jpg?x-oss-process=image/resize,p_100/auto-orient,1/quality,q_90/format,jpg/watermark,image_eXVuY2VzaGk=,t_100,g_se,x_0,y_0)


![](http://aliyunzixunbucket.oss-cn-beijing.aliyuncs.com/jpg/a6c0f0e783499e378c50dee76e8df91f.jpg?x-oss-process=image/resize,p_100/auto-orient,1/quality,q_90/format,jpg/watermark,image_eXVuY2VzaGk=,t_100,g_se,x_0,y_0)


#### 空气质量 MQ-135
![](https://cloud-image-xu.oss-cn-hangzhou.aliyuncs.com/屏幕快照%202018-08-28%20下午8.23.07.png)

MQ135模块使用5V进行驱动，D0输出数字信号，A0输出模拟型号。
D0输出就相当一个开关电源，到了设定值进行跳转，基本没啥用处。
除非你要做一个气体上限报警装置那就用D0引脚吧。
我们这里使用的A0引脚进行模拟信号的输入和输出

**连线**：

+ 5V+ 接开发板正极5V
+ 5V- 接开发板负极GND
+ D0 接板载要开启ADC的引脚

开始使用它之前，你需要将它通电，预热12-24个小时，之后，在20°C/35%空气温度的环境中执行下列程序，读取RZERO的值。

可以通过获取温度和湿度，来获取正确的值。温度和湿度使用DHT11

```
#define DHT_TYPE     DHT11
#define DHT_PIN      2  //pin d4

#define ANALOGPIN    A0

DHT dht(DHT_PIN, DHT_TYPE);

MQ135 gasSensor = MQ135(ANALOGPIN);

void setup() {
  Serial.begin(9600);
  dht.begin(); 

}

void loop() {
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t))
    {
      Serial.println("Failed to read from DHT sensor!");
      delay(1000);
      return;
    }
    Serial.print("hum=");
    Serial.print(h); // this to display the rzero value continuously, uncomment this to get ppm value
    Serial.println("%");
    Serial.print("temp=");
    Serial.print(t); // this to display the rzero value continuously, uncomment this to get ppm value
    Serial.println("C");
    
    rzero = gasSensor.getRZero(); //this to get the rzero value, uncomment this to get ppm value
    Serial.print("RZero=");
    Serial.println(rzero); // this to display the rzero value continuously, uncomment this to get ppm value
     
    ppm = gasSensor.getPPM(); // this to get ppm value, uncomment this to get rzero value
    Serial.print("PPM=");
    Serial.println(ppm); // this to display the ppm value continuously, uncomment this to get rzero value
    
    ppmbalanced = gasSensor.getCorrectedPPM(t, h); // this to get ppm value, uncomment this to get rzero value
    Serial.print("PPM Corrected=");
    Serial.println(ppmbalanced); // this to display the ppm value continuously, uncomment this to get rzero value
} 
```

##### 空气质量对照



![](https://cloud-image-xu.oss-cn-hangzhou.aliyuncs.com/屏幕快照%202018-08-28%20下午9.09.30.png)


HomeBridge-Mqtt 消息

**Topic:**

test/to/set

**Payload:**

{"name": "AirQualitySensor", "service_name": "AirQualitySensor", "characteristic": "AirQuality", "value": 6}

value:1 空气质量非常好
value:6 空气质量非常差

![](https://cloud-image-xu.oss-cn-hangzhou.aliyuncs.com/IMG_2576.PNG)


