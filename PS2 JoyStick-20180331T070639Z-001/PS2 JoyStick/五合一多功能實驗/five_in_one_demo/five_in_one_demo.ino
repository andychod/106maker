//*******************************************************************************
//    Arduino 小車 5 合一多功能實驗 PS2 遙控整合
//   
//    開發環境：Arduino IDE 1.6.5 以上
//    測試使用開發板型號：Arduino UNO R3 / Arduino Genuino
//    台灣物聯科技有限公司：https://www.taiwaniot.com.tw
//    業務諮詢: 0971-209-389
//    客服專線: 06-2596159
//    傳真電話: 06-2803781
//*******************************************************************************
#include <Servo.h>  // 載入伺服舵機函數庫
#include <IRremote.h> // 載入紅外線解碼函數庫
#include <PS2X_lib.h> // 載入PS2搖桿解碼函數庫

// L298N 馬達驅動板
// 宣告 MotorA 為右邊
// 宣告 MotorB 為左邊

#define MotorA_I1     8  //宣告 I1 接腳
#define MotorA_I2     9  //宣告 I2 接腳
#define MotorB_I3    10  //宣告 I3 接腳
#define MotorB_I4    11  //宣告 I4 接腳
#define MotorA_PWMA    5  //宣告 ENA (PWM調速) 接腳
#define MotorB_PWMB    6  //宣告 ENB (PWM調速) 接腳

// IRremote 紅外線
#define IR_Recv      2   // 宣告紅外線接收接腳
IRrecv irrecv(IR_Recv);  // 宣告 IRrecv 物件來接收紅外線訊號　
decode_results results;  // 宣告解碼變數

// 抓取套件中紅外線遙控器所對應的 IR Code 請勿直接使用本範例中抓到的 IR Code
// 請務必先使用範例資料夾中 IRremote_Test 的範例碼燒入 Arduino 開發板後，使用套件內的紅外線遙控器實際去按下您所
// 要宣告的各按鈕，並透過序列埠監控視窗 Serial Monitor 所顯示對應的數值，實際抓出對應的 IR Code　再宣告下列指令參數

#define IR_Forwards    0x00511DBB  // 遙控器方向鍵 上, 前進
#define IR_Back        0xA3C8EDDB  // 遙控器方向鍵 下, 後退
#define IR_Stop        0xD7E84B1B  // 遙控器 OK 鍵, 停止
#define IR_Left        0x52A3D41F  // 遙控器方向鍵 左, 左轉
#define IR_Right       0x20FE4DBB  // 遙控器方向鍵 右, 右轉
#define IR_Tracking    0x32C6FDF7  // 遙控器 * 鍵, 循跡模式
#define IR_Ultrasonic  0x3EC3FC1B  // 遙控器 # 鍵, 超音波避障模式

// 循線模組
#define SensorLeft    A0  //宣告 左側感測器 輸入腳
#define SensorMiddle  A1  //宣告 中間感測器 輸入腳
#define SensorRight   A2  //宣告 右側感測器 輸入腳
int off_track = 0;        //宣告變數

// 超音波測距模組 HC-SR04 
#define US_Trig  13  //宣告超音波模組 Trig 腳位
#define US_Echo  12  //宣告超音波模組 Echo 腳位

// 伺服馬達(舵機)
#define Servo_Pin      3  // 宣告伺服馬達輸出腳位(PWM)
#define servo_delay  250  // 伺服馬達轉向後的穩定時間
Servo myservo;            // 宣告伺服馬達變數

// PS2 搖桿
#define PS2_DAT       A3  // 定義 DATA 接腳
#define PS2_CMD       A4  // 定義 COMMAND 接腳
#define PS2_ATT        7  // 定義 ATTENTION 接腳
#define PS2_CLK        4  // 定義 CLOCK 接腳
PS2X ps2x;                // 建立PS2控制器的類別實體
byte ps2x_error = 0;      // 宣告錯誤碼參數
unsigned long ps2x_tick = 0;  // 宣告計時參數


// 宣告小車移動方向
#define Fgo  8  // 前進
#define Rgo  6  // 右轉
#define Lgo  4  // 左轉
#define Bgo  2  // 倒車

// 宣告小車運作模式變數

#define Car_Normal       0  // 標準模式, 可 藍牙/紅外線/PS2 操控
#define Car_Tracking     1  // 循跡模式
#define Car_Avoidance    2  // 避障模式
byte Car_Mode = Car_Normal; // 宣告小車模式變數

void setup()
{
  Serial.begin(9600); 
  
  pinMode(MotorA_I1,OUTPUT);
  pinMode(MotorA_I2,OUTPUT);
  pinMode(MotorB_I3,OUTPUT);
  pinMode(MotorB_I4,OUTPUT);
  pinMode(MotorA_PWMA,OUTPUT);
  pinMode(MotorB_PWMB,OUTPUT);
  
  pinMode(SensorLeft,   INPUT); 
  pinMode(SensorMiddle, INPUT);
  pinMode(SensorRight,  INPUT);
  
  pinMode(US_Trig, OUTPUT);
  pinMode(US_Echo, INPUT);
  myservo.attach(Servo_Pin);

  irrecv.enableIRIn();  // 啟動 IR 解碼讀取
  
  // PS2控制器接腳設置; config_gamepad(時脈腳位, 命令腳位, 選取腳位, 資料腳位, 是否支援類比按鍵, 是否支援震動) 
  ps2x_error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, false, false);
  
  switch(ps2x_error)
  {
    case 0:  // 如果控制器連接沒有問題，就顯示底下的訊息。
      Serial.println("Found Controller, configured successful");
      break;
    case 1:  // 找不到控制器，顯示底下的錯誤訊息。
      Serial.println("No controller found, check wiring, see readme.txt to enable debug.");
      break;
    case 2:  // 發現控制器，但不接受命令，請參閱程式作者網站的除錯說明。
      Serial.println("Controller found but not accepting commands. see readme.txt to enable debug.");
      break;
    case 3:  // 控制器拒絕進入類比感測壓力模式，或許是此控制器不支援的緣故。
      Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
      break;
  }
  
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

void turnR(int d)    // 小車右轉
{
    digitalWrite(MotorA_I1,LOW);    //馬達（右）逆時針轉動
    digitalWrite(MotorA_I2,HIGH);
    digitalWrite(MotorB_I3,HIGH);   //馬達（左）逆時針轉動
    digitalWrite(MotorB_I4,LOW);
    delay(d * 100);
}

void turnL(int e)    // 小車左轉
{
    digitalWrite(MotorA_I1,HIGH);   //馬達（右）順時針轉動
    digitalWrite(MotorA_I2,LOW);
    digitalWrite(MotorB_I3,LOW);    //馬達（左）順時針轉動
    digitalWrite(MotorB_I4,HIGH);
    delay(e * 100);
}    

void stopRL(int f)  // 小車停止
{
    digitalWrite(MotorA_I1,HIGH);   //馬達（右）停止轉動
    digitalWrite(MotorA_I2,HIGH);
    digitalWrite(MotorB_I3,HIGH);   //馬達（左）停止轉動
    digitalWrite(MotorB_I4,HIGH);
    delay(f * 100);
}

void back(int g)    // 小車後退
{
    digitalWrite(MotorA_I1,LOW);    //馬達（右）逆時針轉動
    digitalWrite(MotorA_I2,HIGH);
    digitalWrite(MotorB_I3,LOW);    //馬達（左）順時針轉動
    digitalWrite(MotorB_I4,HIGH);
    delay(g * 100);     
}

void clockwiseR()  // 右馬達順時針轉動
{
  digitalWrite(MotorA_I1,HIGH);
  digitalWrite(MotorA_I2,LOW);
}

void anticlockwiseR()  // 右馬達逆時針轉動
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
int detection()    // 超音波偵測函數 三個角度讀取距離數值
{
    int Ultrasound_Direction = 0;
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
        L_Distance = Ask_Distance(180);  // 讀取左方距離, 請依照您組裝後的實際狀況調整舵機角度

        if ((L_Distance < 10) && (R_Distance < 10))  //假如超音波感測器偵測到左邊距離和右邊距離皆小於10公分
        {
            Ultrasound_Direction = Bgo;  //向後走
        }
        else if(L_Distance > R_Distance)  //假如左邊距離大於右邊距離
        {
            Ultrasound_Direction = Lgo;  //向左走
        }
        else
        {
            Ultrasound_Direction = Rgo;  //向右走
        }         
    }
    else
    {
        Ultrasound_Direction = Fgo;  //向前走     
    }
    
    return Ultrasound_Direction;
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

void bleCommand()
{
  int cmd = Serial.read();  // 讀取藍芽指令
    
  switch(cmd)  // 執行藍芽指令
  {
    case 'S':  // 倒車
      back(5);
      break;
 
    case 'A':  // 左轉
      turnL(5);
      break;
        
    case 'D':  // 右轉
      turnR(5);
      break;
      
    case 'W':  // 前進
      advance(5);
      break;
        
    case 'Q':  // 停止
        stopRL(5);
        break;
        
  ////////////////////////////////////////////////////////
  //  您可以額外再自行定義指令，搭配藍芽或 PC端遙控軟體
  //  新增的指令可以採用 case 來宣告特定接收字元運作，並
  //  使用 break; 來終止。
  ////////////////////////////////////////////////////////

    case 'X':  // 自行開發的動作或者啟用其他感測器的數值讀取
        break;
        
    case 'N':  // 超音波測距查詢
        Serial.print("Ultrasonic Distance:");                   // 輸出距離（單位：公分）
        Serial.println(Ask_Distance(90));                      // 顯示距離
        break;
  }
}

// 解析並執行 PS2控制器 指令
void ps2Command()
{
  if(ps2x_error == 1) return; // 如果沒發現任何控制器，則返回。
 
  if(millis() - ps2x_tick > 50)  // 每隔50ms讀取一次按鍵
  {
    ps2x_tick = millis();  // 紀錄目前時間
  
    ps2x.read_gamepad();  // 讀取控制器

    //--------------------------------------------------------------
    //  按鍵操控 : 「上」前進、「下」後退、「左」左轉、「右」右轉
    //--------------------------------------------------------------
    if(ps2x.ButtonPressed(PSB_SQUARE))  // 若「方形」按鍵被按下
    {
      Serial.println("Switching to Tracking mode");
      Car_Mode = Car_Tracking;  // 切換到循跡模式
      return;
    }
    
    if(ps2x.ButtonPressed(PSB_CIRCLE))  // 若「圓形」按鍵被按下
    {
      Serial.println("Switching to Avoidance mode");
      Car_Mode = Car_Avoidance;  // 切換到避障模式
      return;
    }
          
    if(ps2x.ButtonPressed(PSB_PAD_UP))  // 若「上」按鍵被按下
    {
      Serial.println("PS2 button [UP] pressed");
      advance(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_UP))  // 若「上」按鍵被放開
    {
      delay(1);  // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_UP) == false)  // 確認「上」按鍵被放開
      {
        Serial.println("PS2 button [UP] released");
        stopRL(0);
      }      
      return;
    }
    
    if(ps2x.ButtonPressed(PSB_PAD_DOWN))  // 若「下」按鍵被按下
    {
      Serial.println("PS2 button [DOWN] pressed");
      back(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_DOWN))  // 若「下」按鍵被放開
    {
      delay(1);   // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_DOWN) == false)  // 確認「下」按鍵被放開
      {
        Serial.println("PS2 button [DOWN] released");
        stopRL(0);
      }
      return;
    }
    
    if(ps2x.ButtonPressed(PSB_PAD_LEFT))  // 若「左」按鍵被按下
    {
      Serial.println("PS2 button [LEFT] pressed");
      turnL(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_LEFT))  // 若「左」按鍵被放開
    {
      delay(1);   // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_LEFT) == false)  // 確認「左」按鍵被放開
      {
        Serial.println("PS2 button [LEFT] released");
        stopRL(0);
      }
      return;
    }
    
    if(ps2x.ButtonPressed(PSB_PAD_RIGHT))  // 若「右」按鍵被按下
    {
      Serial.println("PS2 button [RIGHT] pressed");
      turnR(0);
      return;
    }
    
    if(ps2x.ButtonReleased(PSB_PAD_RIGHT))  // 若「右」按鍵被放開
    {
      delay(1);   // 延遲1ms防止誤判
      
      ps2x.read_gamepad();  // 再次讀取控制器
      
      if(ps2x.Button(PSB_PAD_RIGHT) == false)  // 確認「右」按鍵被放開
      {
        Serial.println("PS2 button [RIGHT] released");
        stopRL(0);
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

// 解析並執行 紅外線接收器 收到的指令
void ircommand()
{
  if(irrecv.decode(&results))  // 若解碼成功，收到一組紅外線訊號
  {
    // 顯示 IR Code
    Serial.print("IR Code: ");            
    Serial.print(results.value, HEX);

    switch(results.value)  // 解析並執行指令
    {
      case IR_Forwards:  // 前進
        Serial.println(" Advance");
        advance(5);
        break;      
      case IR_Back:  // 後退
        Serial.println(" Back");
        back(5);
        break;      
      case IR_Stop:  // 停止
        Serial.println(" Stop");
        stopRL(5);
        break;        
      case IR_Left:  // 左轉
        Serial.println(" Left");
        turnL(5);
        break;        
      case IR_Right:  // 右轉
        Serial.println(" Right");
        turnR(5);
        break;
      case IR_Tracking:
        Serial.println(" Tracking Mode");
        Car_Mode = Car_Tracking;
        break;      
      case IR_Ultrasonic:
        Serial.println(" Ultrasonic detector Mode");
        Car_Mode = Car_Avoidance;
        break;        
      default:
        Serial.println(" Unsupported");
        break;
    }

    irrecv.resume(); // 接收下一個數值
  }
}

void loop()
{

  switch(Car_Mode)
  {
   case Car_Normal:
      bleCommand();  // 解析並執行 UART/藍牙 指令
      ircommand();    // 解析並執行 紅外線接收器 指令
      ps2Command();   // 解析並執行 PS2控制器 指令
      break;
    case Car_Tracking:
      if(irrecv.decode(&results))
      {
        if(results.value == IR_Stop)  // 若遙控器按下「OK」則回到標準模式
        {
          Serial.println("Stop Tracking Mode");
          Car_Mode = Car_Normal;
        }
        
        irrecv.resume(); // 接收下一個數值
      }
      
      if(ps2x_error != 1) // 若有 PS2控制器
      { 
        if(millis() - ps2x_tick > 50)  // 每隔50ms讀取一次控制器
        {
          ps2x_tick = millis();  // 紀錄目前時間
          
          ps2x.read_gamepad();  // 讀取控制器
          
          if(ps2x.ButtonPressed(PSB_CROSS))  // 若「X」按鍵被按下
          {
            Serial.println("Stop Tracking Mode");
            Car_Mode = Car_Normal;
          }
        }
      }
      
      if(Car_Mode == Car_Normal)  // 若回到標準模式
      {
        stopRL(1);  // 停車
        analogWrite(MotorA_PWMA,200);  // 調回馬達 (右) 轉速
        analogWrite(MotorB_PWMB,200);  // 調回馬達 (左) 轉速
      }
      else
      {
        // 讀取感測器狀態
        boolean SL = digitalRead(SensorLeft);
        boolean SM = digitalRead(SensorMiddle);
        boolean SR = digitalRead(SensorRight);

        if((SM == LOW) && (SL == LOW) && (SR == LOW))  // 小車脫離黑線
        {
          analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
          analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
          
          // 小車有時會因為循線感測器誤判或黑線轉角太大, 認為脫離黑線了而停車
          // 加上尋回黑線機制, 避免小車誤動作
          // 您可以修改程式讓 循線/尋線 機制更完美
          if(off_track < 3)
          {            
            switch(off_track)
            {
              case 0:
                back(1);
                break;              
              case 1:
                turnR(1);
                break;                
              case 2:
                turnL(2);
                break;
            }
            
            off_track++;
          }
          else
          {
            stopRL(0);
          }
        }
        else
        {
          off_track = 0;
                         
          if(SM == HIGH)  //中感測器在黑色區域
          {
            if((SL == LOW) && (SR == HIGH))  // 左白右黑, 車體偏右校正
            {
              analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
              analogWrite(MotorB_PWMB, 80);    //設定馬達 (左) 轉速
              advance(0);
            } 
            else if((SL == HIGH) && (SR == LOW))  // 左黑右白, 車體偏左校正
            {
              analogWrite(MotorA_PWMA, 80);    //設定馬達 (右) 轉速
              analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
              advance(0);
            }
            else  // 其他, 直走
            {
              analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
              analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
              advance(0);
            }
          } 
          else // 中感測器在白色區域, 車體已大幅偏離黑線
          {
            if((SL == LOW) && (SR == HIGH))  // 左白右黑, 車體快速右轉
            {
              analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
              analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
              turnR(0);
            }
            else if((SL == HIGH) && (SR == LOW))  // 左黑右白, 車體快速左轉
            {
              analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
              analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
              turnL(0);
            }
          }
        }
      }
      break;
    case Car_Avoidance:
      if(irrecv.decode(&results))
      {
        if(results.value == IR_Stop)  // 若遙控器按下「OK」則回到標準模式
        {
          Serial.println("Stop Ultrasonic detector Mode");
          Car_Mode = Car_Normal;
        }
        
        irrecv.resume(); // 接收下一個數值
      }
      
      if(ps2x_error != 1) // 若有 PS2控制器
      { 
        if(millis() - ps2x_tick > 50)  // 每隔50ms讀取一次控制器
        {
          ps2x_tick = millis();  // 紀錄目前時間
          
          ps2x.read_gamepad();  // 讀取控制器
          
          if(ps2x.ButtonPressed(PSB_CROSS))  // 若「X」按鍵被按下
          {
            Serial.println("Stop Ultrasonic detector Mode");
            Car_Mode = Car_Normal;
          }
        }
      }
      
      if(Car_Mode == Car_Normal)  // 若回到標準模式
      {
        stopRL(5);  // 停車
        Ask_Distance(90);  // 超音波模組轉正
      }
      else
      {
        switch(detection())  // 偵測障礙物並判斷要往哪一方向移動
        {
          case Bgo:  // 倒車
            back(2);   // 倒退(車)
            turnL(1);  // 稍微向左方移動(防止卡在死巷裡)
            break;     
          case Lgo:  // 左轉
            back(1);   // 稍微倒退
            turnL(2);  // 左轉
            break;            
          case Rgo:  // 右轉
            back(1);   // 稍微倒退
            turnR(2);  // 右轉
            break;            
          case Fgo:  // 前進
            advance(1);  // 正常前進
            break;
        }
      }
      break;
  
  
  }
 }
