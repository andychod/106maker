//
//  Infrared signals
//

#include <IRremote.h>

// IRremote
#define IR_Recv       2           // Infrared receiving pin
IRrecv irrecv(IR_Recv);
decode_results results;

//
// IR Code
// Use IRremote_Test to get your IR Code, and then define the following macros
//
#define IR_Forward    0x00FF629D  // up
#define IR_Backward   0x00FFA857  // down

void setup() {
    Serial.begin(9600);           // Open serial port, baud rate 9600 bps
  
    irrecv.enableIRIn();          // enable infrared receiving
}

void loop() {
    if(irrecv.decode(&results)) {
        Serial.print("\r\nirCode: ");            
        Serial.print(results.value, HEX);
  
        switch(results.value) {
            case IR_Forward:
                Serial.print(" Forward");
                break;
              
            case IR_Backward:
                Serial.print(" Backward");
                break;
            
            default:
                Serial.print(" Unsupported");
        }
        delay(500);
        irrecv.resume(); // Receive the next value
    }
}
