//
//  紅外線控制智慧小車
//

#include <IRremote.h>

// L298N 馬達驅動板
#define MotorR_I1     8  //定義 I1 接腳
#define MotorR_I2     9  //定義 I2 接腳
#define MotorL_I3    10  //定義 I3 接腳
#define MotorL_I4    11  //定義 I4 接腳
#define MotorR_ENA    5  //定義 ENA (PWM調速) 接腳
#define MotorL_ENB    6  //定義 ENB (PWM調速) 接腳

// IRremote
#define IR_Recv      2   // 定義紅外線接收接腳
IRrecv irrecv(IR_Recv);  // 宣告 IRrecv 物件來接收紅外線訊號　
decode_results results;  // 宣告解碼變數

//
// IR Code
// 請務必先使用　IRremote_Test 抓出實際 IR Code　再定義下列指令參數
//
#define IR_Advence  0x00FF629D  // 遙控器方向鍵 上, 前進
#define IR_Back     0x00FFA857  // 遙控器方向鍵 下, 後退
#define IR_Stop     0x00FF02FD  // 遙控器 OK 鍵, 停止
#define IR_Left     0x00FF22DD  // 遙控器方向鍵 左, 左轉
#define IR_Right    0x00FFC23D  // 遙控器方向鍵 右, 右轉


void setup()
{
  Serial.begin(9600);  // 開啟 Serial port, 通訊速率為 9600 bps 
  
  pinMode(MotorR_I1,OUTPUT);
  pinMode(MotorR_I2,OUTPUT);
  pinMode(MotorL_I3,OUTPUT);
  pinMode(MotorL_I4,OUTPUT);
  pinMode(MotorR_ENA,OUTPUT);
  pinMode(MotorL_ENB,OUTPUT);
  
  irrecv.enableIRIn();  // 啟動紅外線解碼
  
  analogWrite(MotorR_ENA,200);    //設定馬達 (右) 轉速
  analogWrite(MotorL_ENB,200);    //設定馬達 (左) 轉速
}

void advance(int a)    // 前進
{
    digitalWrite(MotorR_I1,HIGH);   //馬達（右）順時針轉動
    digitalWrite(MotorR_I2,LOW);
    digitalWrite(MotorL_I3,HIGH);   //馬達（左）逆時針轉動
    digitalWrite(MotorL_I4,LOW);
    delay(a * 100);
}

void turnR(int d)    //右轉
{
    digitalWrite(MotorR_I1,LOW);    //馬達（右）逆時針轉動
    digitalWrite(MotorR_I2,HIGH);
    digitalWrite(MotorL_I3,HIGH);   //馬達（左）逆時針轉動
    digitalWrite(MotorL_I4,LOW);
    delay(d * 100);
}

void turnL(int e)    //左轉
{
    digitalWrite(MotorR_I1,HIGH);   //馬達（右）順時針轉動
    digitalWrite(MotorR_I2,LOW);
    digitalWrite(MotorL_I3,LOW);    //馬達（左）順時針轉動
    digitalWrite(MotorL_I4,HIGH);
    delay(e * 100);
}    

void stopRL(int f)  //停止
{
    digitalWrite(MotorR_I1,HIGH);   //馬達（右）停止轉動
    digitalWrite(MotorR_I2,HIGH);
    digitalWrite(MotorL_I3,HIGH);   //馬達（左）停止轉動
    digitalWrite(MotorL_I4,HIGH);
    delay(f * 100);
}

void back(int g)    //後退
{
    digitalWrite(MotorR_I1,LOW);    //馬達（右）逆時針轉動
    digitalWrite(MotorR_I2,HIGH);
    digitalWrite(MotorL_I3,LOW);    //馬達（左）順時針轉動
    digitalWrite(MotorL_I4,HIGH);
    delay(g * 100);     
}

void loop() 
{
    if(irrecv.decode(&results)) 
    {
        Serial.print("\r\nirCode: ");            
        Serial.print(results.value, HEX);    // 紅外線編碼
  
        switch(results.value)
        {
          case IR_Advence:
            Serial.print(" Advance");
            advance(1);
            break;
            
          case IR_Back:
            Serial.print(" Back");
            back(1);
            break;
            
          case IR_Stop:
            Serial.print(" Stop");
            stopRL(1);
            break;
            
          case IR_Left:
            Serial.print(" Left");
            turnL(1);
            break;
            
          case IR_Right:
            Serial.print(" Right");
            turnR(1);
            break;
            
          default:
          Serial.print(" Unsupported");
        }
    
        irrecv.resume(); // Receive the next value
    }
}
