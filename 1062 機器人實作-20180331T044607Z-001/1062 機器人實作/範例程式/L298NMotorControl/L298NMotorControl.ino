//   
//  L298N Motor Control
//

#define MotorA_I1     8   // IN1
#define MotorA_I2     9   // IN2
#define MotorB_I3     10  // IN3
#define MotorB_I4     11  // IN4
#define MotorA_PWNA   5   // ENA (For PWM)
#define MotorB_PWNB   6   // ENB (For PWM)

void setup() {
    pinMode(MotorA_I1,OUTPUT);
    pinMode(MotorA_I2,OUTPUT);
    pinMode(MotorB_I3,OUTPUT);
    pinMode(MotorB_I4,OUTPUT);
    pinMode(MotorA_PWNA,OUTPUT);
    pinMode(MotorB_PWNB,OUTPUT);
}

void loop() {
    // Fast Forward
    analogWrite(MotorA_PWNA,255);   // Set the speed, 0 ~ 255
    analogWrite(MotorB_PWNB,255);
    digitalWrite(MotorA_I1,HIGH);
    digitalWrite(MotorA_I2,LOW);
    digitalWrite(MotorB_I3,HIGH);
    digitalWrite(MotorB_I4,LOW);
    delay(2000);

    // Slow Forward
    analogWrite(MotorA_PWNA,80);   // Set the speed, 0 ~ 255
    analogWrite(MotorB_PWNB,80);
    digitalWrite(MotorA_I1,HIGH);
    digitalWrite(MotorA_I2,LOW);
    digitalWrite(MotorB_I3,HIGH);
    digitalWrite(MotorB_I4,LOW);
    delay(2000);

    // Stop
    digitalWrite(MotorA_I1,HIGH);
    digitalWrite(MotorA_I2,HIGH);
    digitalWrite(MotorB_I3,HIGH);
    digitalWrite(MotorB_I4,HIGH);
    delay(2000);
}
