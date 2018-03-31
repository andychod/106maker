#include <Servo.h> 

// L298N 馬達驅動板
// 宣告 MotorA 為右邊
// 宣告 MotorB 為左邊

#define MotorA_I1     8  //定義 I1 接腳
#define MotorA_I2     9  //定義 I2 接腳
#define MotorB_I3    10  //定義 I3 接腳
#define MotorB_I4    11  //定義 I4 接腳
#define MotorA_PWMA    5  //定義 ENA (PWM調速) 接腳
#define MotorB_PWMB    6  //定義 ENB (PWM調速) 接腳

// HC-SR04 超音波測距模組
#define US_Trig  13  //定義超音波模組 Trig 腳位
#define US_Echo  12  //定義超音波模組 Echo 腳位

// 伺服馬達(舵機)
#define Servo_Pin      3  // 定義伺服馬達輸出腳位(PWM)
#define servo_delay  250  // 伺服馬達轉向後的穩定時間
Servo myservo;            // 宣告伺服馬達變數

// 定義小車移動方向
#define Fgo  8  // 前進
#define Rgo  6  // 右轉
#define Lgo  4  // 左轉
#define Bgo  2  // 倒車


void setup()
{
  Serial.begin(9600); 
  
  pinMode(MotorA_I1,OUTPUT);
  pinMode(MotorA_I2,OUTPUT);
  pinMode(MotorB_I3,OUTPUT);
  pinMode(MotorB_I4,OUTPUT);
  pinMode(MotorA_PWMA,OUTPUT);
  pinMode(MotorB_PWMB,OUTPUT);
  
  pinMode(US_Trig, OUTPUT);
  pinMode(US_Echo, INPUT);

  myservo.attach(Servo_Pin);
  
  analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
  analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
}

void advance(int a)    // 小車前進
{
    digitalWrite(MotorA_I1,HIGH);   //馬達（右）順時針轉動
    digitalWrite(MotorA_I2,LOW);
    digitalWrite(MotorB_I3,HIGH);   //馬達（左）逆時針轉動
    digitalWrite(MotorB_I4,LOW);
    delay(a * 100);
}

void turnR(int d)    //小車右轉
{
    digitalWrite(MotorA_I1,LOW);    //馬達（右）逆時針轉動
    digitalWrite(MotorA_I2,HIGH);
    digitalWrite(MotorB_I3,HIGH);   //馬達（左）逆時針轉動
    digitalWrite(MotorB_I4,LOW);
    delay(d * 100);
}

void turnL(int e)    //小車左轉
{
    digitalWrite(MotorA_I1,HIGH);   //馬達（右）順時針轉動
    digitalWrite(MotorA_I2,LOW);
    digitalWrite(MotorB_I3,LOW);    //馬達（左）順時針轉動
    digitalWrite(MotorB_I4,HIGH);
    delay(e * 100);
}    

void stopRL(int f)  //小車停止
{
    digitalWrite(MotorA_I1,HIGH);   //馬達（右）停止轉動
    digitalWrite(MotorA_I2,HIGH);
    digitalWrite(MotorB_I3,HIGH);   //馬達（左）停止轉動
    digitalWrite(MotorB_I4,HIGH);
    delay(f * 100);
}

void back(int g)    //小車後退
{
    digitalWrite(MotorA_I1,LOW);    //馬達（右）逆時針轉動
    digitalWrite(MotorA_I2,HIGH);
    digitalWrite(MotorB_I3,LOW);    //馬達（左）順時針轉動
    digitalWrite(MotorB_I4,HIGH);
    delay(g * 100);     
}
    
int detection()    // 超音波偵測函數 三個角度讀取距離數值
{
    int Car_Direction = 0;
    int F_Distance = 0;
    int R_Distance = 0;
    int L_Distance = 0;

    F_Distance = Ask_Distance(90);  // 讀取前方距離, 預設為90度,若超音波感測器轉至小車正前方後發現角度有些許偏差,您可以透過本處宣告其他角度進行調整。
    
    if(F_Distance < 10)  // 超音波感測器偵測後得到的距離低於 10公分
    {
         stopRL(1);
         back(2);
    }
 
    if(F_Distance < 25)  // 超音波感測器偵測前方的距離小於25公分
    {
        stopRL(1);

        R_Distance = Ask_Distance(0);    // 讀取右方距離, 請依照您組裝後的實際狀況調整舵機角度
        L_Distance = Ask_Distance(180);  // 讀取右方距離, 請依照您組裝後的實際狀況調整舵機角度

        if ((L_Distance < 10) && (R_Distance < 10))  //假如超音波感測器偵測到左邊距離和右邊距離皆小於10公分
        {
            Car_Direction = Bgo;  //向後走
        }
        else if(L_Distance > R_Distance)  //假如左邊距離大於右邊距離
        {
            Car_Direction = Lgo;  //向左走
        }
        else
        {
            Car_Direction = Rgo;  //向右走
        }         
    }
    else
    {
        Car_Direction = Fgo;  //向前走     
    }
    
    return Car_Direction;
}    

int Ask_Distance(int dir)  // 測量距離 
{
    myservo.write(dir);  // 調整超音波模組角度
    delay(servo_delay);  // 等待伺服馬達穩定
    
    digitalWrite(US_Trig, LOW);   // 讓超聲波發射低電壓2μs
    delayMicroseconds(2);
    digitalWrite(US_Trig, HIGH);  // 讓超聲波發射高電壓10μs，這裡至少是10μs
    delayMicroseconds(10);
    digitalWrite(US_Trig, LOW);   // 維持超聲波發射低電壓
    float distance = pulseIn(US_Echo, HIGH);  // 讀取時間差
    distance = distance / 5.8 / 10;  // 將時間轉為距離距离（單位：公分）
    Serial.print("Distance:"); //輸出距離（單位：公分）
    Serial.println(distance); //顯示距離
    Serial.print('\n');
    return distance;
}

void loop()
 {
    switch(detection())  // 偵測並判斷要往哪一方向移動
    {
    case Bgo:    // 倒車
        back(2);                   // 倒退(車)
        turnL(1);                  // 稍微向左方移動(防止卡在死巷裡)
        Serial.print(" Reverse ");  // 顯示方向(倒退)
        break;
 
    case Lgo:    // 左轉
        back(1);                  // 稍微倒退
        turnL(2);                 // 左轉
        Serial.print(" Left ");    // 顯示方向(左轉)  
        break;
        
    case Rgo:    // 右轉
        back(1);                   // 稍微倒退
        turnR(2);                  // 右轉
        Serial.print(" Right ");    // 顯示方向(右轉)
        break;
        
    case Fgo:    // 前進
        advance(1);                // 正常前進  
        Serial.print(" Advance ");  //顯示方向(前進)
        Serial.print("   ");
        break;
    }
 }
