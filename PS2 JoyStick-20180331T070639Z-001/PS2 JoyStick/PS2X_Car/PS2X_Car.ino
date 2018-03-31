/*
此程式庫不支援熱插拔，亦即，你必須在連接控制器後重新啟動Arduino，
或者在連接控制器之後再次呼叫config_gamepad()函數。
*/
#include <PS2X_lib.h>    // 匯入PS2函式庫
#include <Servo.h> 

// L298N 馬達驅動板
// 宣告 MotorA 為右邊
// 宣告 MotorB 為左邊

// L298N 馬達驅動板
#define MotorA_I1     8  //定義 I1 接腳
#define MotorA_I2     9  //定義 I2 接腳
#define MotorB_I3    10  //定義 I3 接腳
#define MotorB_I4    11  //定義 I4 接腳
#define MotorA_PWMA    5  //定義 ENA (PWM調速) 接腳
#define MotorB_PWMB    6  //定義 ENB (PWM調速) 接腳

// PS2 搖桿
#define PS2_DAT       A3  // 定義 DATA 接腳
#define PS2_CMD       A4  // 定義 COMMAND 接腳
#define PS2_ATT        7  // 定義 ATTENTION 接腳
#define PS2_CLK        4  // 定義 CLOCK 接腳

PS2X ps2x;    // 建立PS2控制器的類別實體

unsigned long ps2x_tick = 0;  // 宣告計時參數
int ps2x_error = 0;           // 宣告錯誤碼參數

#define MotorA_PWMA    5  //定義 ENA (PWM調速) 接腳
#define MotorB_PWMB    6  //定義 ENB (PWM調速) 接腳
// 伺服馬達(舵機)
#define Servo_Pin      3  // 定義伺服馬達輸出腳位(PWM)
#define servo_delay  250  // 伺服馬達轉向後的穩定時間
Servo myservo;            // 宣告伺服馬達變數

void setup()
{
  Serial.begin(9600);  // 開啟 Serial port, 通訊速率為 9600 bps 
 
  // 設定馬達控制接腳模式
  pinMode(MotorA_I1,OUTPUT);
  pinMode(MotorA_I2,OUTPUT);
  pinMode(MotorB_I3,OUTPUT);
  pinMode(MotorB_I4,OUTPUT);
  pinMode(MotorA_PWMA,OUTPUT);
  pinMode(MotorB_PWMB,OUTPUT);
  
  // PS2控制器接腳設置; config_gamepad(時脈腳位, 命令腳位, 選取腳位, 資料腳位, 是否支援類比按鍵, 是否支援震動) 
  ps2x_error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, false, false);
  
  switch(ps2x_error)
  {
    case 0:  // 如果控制器連接沒有問題，就顯示底下的訊息。
      Serial.print("Found Controller, configured successful");
      break;
    case 1:  // 找不到控制器，顯示底下的錯誤訊息。
      Serial.print("No controller found, check wiring, see reamde.txt to enable debug.");
      break;
    case 2:  // 發現控制器，但不接受命令，請參閱程式作者網站的除錯說明。
      Serial.print("Controller found but not accepting commands. see readme.txt to enable debug.");
      break;
    case 3:  // 控制器拒絕進入類比感測壓力模式，或許是此控制器不支援的緣故。
      Serial.print("Controller refusing to enter Pressures mode, may not support it. ");
      break;
  }
  
  myservo.attach(Servo_Pin);
  
  analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
  analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
}

void loop()
{
   analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
  analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
  if(ps2x_error == 1) return; // 如果沒發現任何控制器，則返回。
 
  if(millis() - ps2x_tick > 50)  // 每隔50ms讀取一次按鍵
  {
    ps2x_tick = millis();  // 紀錄目前時間
  
    ps2x.read_gamepad();  // 讀取控制器

    //--------------------------------------------------------------
    //  按鍵操控 : 「上」前進、「下」後退、「左」左轉、「右」右轉
    //--------------------------------------------------------------
    if(ps2x.ButtonPressed(PSB_RED))  // 若「RED」按鍵被按下
    {
     Ask_Distance(10);
      return;
    }
        if(ps2x.ButtonReleased(PSB_RED))  // 若「上」按鍵被放開
    {
      delay(1);  // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_RED) == false)  // 確認「上」按鍵被放開
      {
        Serial.print("\r\nPS2 button [UP] released");
       Ask_Distance(90);
      }      
      return;
    }
    //----------------
    if(ps2x.ButtonPressed(PSB_PINK))  // 若「RED」按鍵被按下
    {
     Ask_Distance(180);
      return;
    }
        if(ps2x.ButtonReleased(PSB_PINK))  // 若「上」按鍵被放開
    {
      delay(1);  // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PINK) == false)  // 確認「上」按鍵被放開
      {
        Serial.print("\r\nPS2 button [UP] released");
       Ask_Distance(90);
      }      
      return;
    }
//---------------
    
    if(ps2x.ButtonPressed(PSB_PAD_UP))  // 若「上」按鍵被按下
    {
      Serial.print("\r\nPS2 button [UP] pressed");
      Car_Advance(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_UP))  // 若「上」按鍵被放開
    {
      delay(1);  // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_UP) == false)  // 確認「上」按鍵被放開
      {
        Serial.print("\r\nPS2 button [UP] released");
        Car_Stop(0);
      }      
      return;
    }
    
    if(ps2x.ButtonPressed(PSB_PAD_DOWN))  // 若「下」按鍵被按下
    {
      Serial.print("\r\nPS2 button [DOWN] pressed");
      Car_Back(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_DOWN))  // 若「下」按鍵被放開
    {
      delay(1);   // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_DOWN) == false)  // 確認「下」按鍵被放開
      {
        Serial.print("\r\nPS2 button [DOWN] released");
        Car_Stop(0);
      }
      return;
    }
    
    if(ps2x.ButtonPressed(PSB_PAD_LEFT))  // 若「左」按鍵被按下
    {
      Serial.print("\r\nPS2 button [LEFT] pressed");
      Car_turnL(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_LEFT))  // 若「左」按鍵被放開
    {
      delay(1);   // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_LEFT) == false)  // 確認「左」按鍵被放開
      {
        Serial.print("\r\nPS2 button [LEFT] released");
        Car_Stop(0);
      }
      return;
    }
    
    if(ps2x.ButtonPressed(PSB_PAD_RIGHT))  // 若「右」按鍵被按下
    {
      Serial.print("\r\nPS2 button [RIGHT] pressed");
      Car_turnR(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_RIGHT))  // 若「右」按鍵被放開
    {
      delay(1);   // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_RIGHT) == false)  // 確認「右」按鍵被放開
      {
        Serial.print("\r\nPS2 button [RIGHT] released");
        Car_Stop(0);
      }
      return;
    }
    
    //-------------------------------------------------------
    //  搖桿操控 : 左右搖桿各別控制左右馬達
    //-------------------------------------------------------
    if(ps2x.Button(0xFFFF) == false)  // 如沒有按鍵, 則偵測搖桿
    {
      static byte stick_ly = 128;  // 左搖桿 Y 軸數值變數
      static byte stick_ry = 128;  // 右搖桿 Y 軸數值變數
      
      if(stick_ly != ps2x.Analog(PSS_LY))  // 若左搖桿 Y 軸有變化
      {
        if((ps2x.Analog(PSS_LY) < 10) || (ps2x.Analog(PSS_LY) > 250))
        {
          delay(1);   // 延遲1ms防止誤判
          ps2x.read_gamepad();  // 再次讀取控制器
        }
        
        if(stick_ly != ps2x.Analog(PSS_LY))  // 確認左搖桿 Y 軸有變化
        {          
          stick_ly = ps2x.Analog(PSS_LY);  // 紀錄左搖桿 Y 軸數值
          
          Serial.print("PS2 [LEFT] stick Y value : ");
          Serial.println(stick_ly, DEC); 
          
          if(stick_ly < 126)  // 左搖桿往前推
          {
            anticlockwiseL();
          }
          else if(stick_ly > 128)  // 左搖桿往後推
          {
            clockwiseL();
          }
          else  // 左搖桿置中
          {
            stopL();
          }
        }
      }
      
      if(stick_ry != ps2x.Analog(PSS_RY))  // 若右搖桿 Y 軸有變化
      {
        if((ps2x.Analog(PSS_RY) < 10) || (ps2x.Analog(PSS_RY) > 250))
        {
          delay(1);   // 延遲1ms防止誤判
          ps2x.read_gamepad();  // 再次讀取控制器
        }
        
        if(stick_ry != ps2x.Analog(PSS_RY))  // 確認右搖桿 Y 軸有變化
        {        
          stick_ry = ps2x.Analog(PSS_RY);  // 紀錄右搖桿 Y 軸數值
          
          Serial.print("PS2 [RIGHT] stick Y value : ");
          Serial.println(stick_ry, DEC);
          
          if(stick_ry < 126)  // 右搖桿往前推
          {
            clockwiseR();
          }
          else if(stick_ry > 128)  // 右搖桿往後推
          {
            anticlockwiseR();
          }
          else  // 右搖桿置中
          {
            stopR();
          }
        }
      }
    }
  }
}

// 小車前進
void Car_Advance(int t)
{
  clockwiseR();
  anticlockwiseL();
  delay(t * 100);
}

// 小車後退
void Car_Back(int t)
{
  anticlockwiseR();
  clockwiseL();
  delay(t * 100);  
}

// 小車左旋轉
void Car_turnL(int t)
{
  clockwiseR();
  clockwiseL();
  delay(t * 100);
}

// 小車右旋轉
void Car_turnR(int t)
{
  anticlockwiseR();
  anticlockwiseL();
  delay(t * 100);
}

// 小車停止
void Car_Stop(int t)
{
  stopR();
  stopL();
  delay(t * 100);
}

void anticlockwiseR()  // 右馬達順時針轉動
{
  digitalWrite(MotorA_I1,HIGH);
  digitalWrite(MotorA_I2,LOW);
}

void clockwiseR()  // 右馬達逆時針轉動
{
  digitalWrite(MotorA_I1,LOW);
  digitalWrite(MotorA_I2,HIGH);
}

void stopR()  //右馬達停止
{
    digitalWrite(MotorA_I1,HIGH);
    digitalWrite(MotorA_I2,HIGH);
}

void clockwiseL()  // 左馬達順時針轉動
{
  digitalWrite(MotorB_I3,LOW);
  digitalWrite(MotorB_I4,HIGH);
}

void anticlockwiseL()  // 左馬達逆時針轉動
{
  digitalWrite(MotorB_I3,HIGH);
  digitalWrite(MotorB_I4,LOW);
}

void stopL()  //左馬達停止
{
  digitalWrite(MotorB_I3,HIGH);
  digitalWrite(MotorB_I4,HIGH);
}

void Ask_Distance(int dir)  // 測量距離 
{
    myservo.write(dir);  // 調整超音波模組角度
    delay(servo_delay);  // 等待伺服馬達穩定
    
    /*digitalWrite(US_Trig, LOW);   // 讓超聲波發射低電壓2μs
    delayMicroseconds(2);
    digitalWrite(US_Trig, HIGH);  // 讓超聲波發射高電壓10μs，這裡至少是10μs
    delayMicroseconds(10);
    digitalWrite(US_Trig, LOW);   // 維持超聲波發射低電壓
    float distance = pulseIn(US_Echo, HIGH);  // 讀取時間差
    distance = distance / 5.8 / 10;  // 將時間轉為距離距离（單位：公分）
    Serial.print("Distance:"); //輸出距離（單位：公分）
    Serial.println(distance); //顯示距離
    Serial.print('\n');
    return distance;*/
}
