#include <math.h>

// 方向
enum GoDir {
  eLF = 0x2F,
  eLB = 0x1F,
  eLS = 0x0F,
  eRF = 0xF2, 
  eRB = 0xF1,
  eRS = 0xF0, 
};

long LeftCount = 0;
long RightCount = 0;

long LeftDistance = 0;
long RightDistance = 0;

// pin 脚定义
int LFPin=5;
int LBPin=4;
int RFPin=6;
int RBPin=7;

int LTrigPin=14;
int LEchoPin=15;
int RTrigPin=16;
int REchoPin=17;

int LStepPin=8;
int RStepPin=12;

// 左轮前
#define LF digitalWrite(LFPin, HIGH);\
           digitalWrite(LBPin, LOW); 
// 右轮前        
#define RF digitalWrite(RFPin, HIGH);\
           digitalWrite(RBPin, LOW); 
// 左轮退
#define LB digitalWrite(LFPin, LOW); \
           digitalWrite(LBPin, HIGH); 
// 右轮退
#define RB digitalWrite(RFPin, LOW); \
           digitalWrite(RBPin, HIGH); 
// 左轮停
#define LS digitalWrite(LFPin, LOW);\
           digitalWrite(LBPin, LOW); 
// 右轮停     
#define RS digitalWrite(RFPin, LOW);\
           digitalWrite(RBPin, LOW); 


long checkDistance(int trig,int echo)
{
   long cm;
   pinMode(trig, OUTPUT);
   digitalWrite(trig, LOW);
   delayMicroseconds(2);
   digitalWrite(trig, HIGH);
   delayMicroseconds(10);
   digitalWrite(trig, LOW);
   pinMode(echo, INPUT);
   cm  = pulseIn(echo, HIGH) / 29 / 2;
   return cm;
}


void setup() {
  
  Serial.begin(9600); 
  // put your setup code here, to run once:
  pinMode(LFPin, OUTPUT);
  pinMode(LBPin, OUTPUT);
  pinMode(RFPin, OUTPUT);
  pinMode(RBPin, OUTPUT);

  pinMode(LTrigPin, OUTPUT);
  pinMode(RTrigPin, OUTPUT);
  pinMode(LEchoPin, INPUT);
  pinMode(REchoPin, INPUT);

  pinMode(LStepPin, INPUT);
  pinMode(RStepPin, INPUT);
}


void checkDistanceTask()
{
   LeftDistance = checkDistance(LTrigPin,LEchoPin);
   RightDistance = checkDistance(RTrigPin,REchoPin);
}

void setStep(int left_step,int right_step)
{
    LeftCount = left_step;
    RightCount = right_step;
}

void checkStep()
{
  static int LeftOn = 0;
  static int RightOn = 0;
  int new_Left = digitalRead(LStepPin);
  int new_Right = digitalRead(RStepPin);
  
  // 向左走LeftCount
  if (LeftCount > 0)
  {
    if (new_Left != LeftOn) 
    {
      LeftCount--;
      if (LeftCount <= 0)  LS
    }
  }
  // 向右走RightCount
  if (RightCount > 0)
  {
    if (new_Right != RightOn) 
    {
      RightCount--;
      if (RightCount <= 0) RS
    }

  }

}

// speed : 0-255
void LFSpeed(unsigned char speed)
{
  if (speed == 255)
  {
    LF
  }
  analogWrite(LFPin, (int)speed);
  digitalWrite(LBPin, LOW); 
}

// speed : 0-255
void RFSpeed(unsigned char speed)
{
  if (speed == 255)
  {
    RF
  }
  else
  {
    analogWrite(RFPin, (int)speed);
    digitalWrite(RBPin, LOW);
  }
  
}

void loop() {


}

