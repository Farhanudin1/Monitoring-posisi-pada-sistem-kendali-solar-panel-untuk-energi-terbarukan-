#include <Servo.h> // include Servo library 

#define enA 3
#define enB 5
#define in1 2
#define in2 4
#define ldr1 A0 //LDR top left - BOTTOM LEFT    <--- BDG
#define ldr2 A1 //LDR top rigt - BOTTOM RIGHT 
#define ldr3 A2 //LDR down left - TOP LEFT
#define ldr4 A3 //ldr down rigt - TOP RIGHT

// Servo horizontal;
// int servox = 90;
// int servoxLimitHigh = 180;
// int servoxLimitLow = 65;

// 65 degrees MAX
Servo vertical;   // vertical servo 
int servoy = 90;    //   90;     // stand vertical servo
int servoyLimitHigh = 120;
int servoyLimitLow = 15;

int Stop = 100;
int MotorB1 = 0;
int MotorB2 = 0;

void setup()
{
  Serial.begin(9600);
// Deklarasiin Sensor LDR
  pinMode(ldr1, INPUT);
  pinMode(ldr2, INPUT);
  pinMode(ldr3, INPUT);
  pinMode(ldr4, INPUT);

// Deklarasiin Motor DC
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT); 

// // servo x
//   horizontal.attach(6);
//   horizontal.write(180);
  
// servo y
  vertical.attach(10);
  vertical.write(45);

  MotorB1 = 100; //0-255
  MotorB2 = 100; //0-255
  analogWrite(enA, MotorB1);
  analogWrite(enB, MotorB2);    
  
  delay(3000);
    
}

void loop() 
{  

  int TL = analogRead(ldr4); // top left
  int TR = analogRead(ldr3); // top right
  int DL = analogRead(ldr2); // down left
  int DR = analogRead(ldr1); // down rigt
  Serial.print(TL);
  Serial.print(" ");
  Serial.print(TR);
  Serial.print(" ");
  Serial.print(DL);
  Serial.print(" ");
  Serial.print(DR);
  Serial.print("   ");
  
  int avgT = (TL + TR) / 2; // average value top
  int avgD = (DL + DR) / 2; // average value down
  int avgL = (TL + DL) / 2; // average value left
  int avgR = (TR + DR) / 2; // average value right

  int dvert = abs(avgT - avgD); // check the diffirence of up and down
  int dhoriz = abs(avgL - avgR);// check the diffirence og left and rigt
  
  // Serial.print(avgT);
  // Serial.print(" ");
  // Serial.print(avgD);
  // Serial.print(" ");
  // Serial.print(avgL);
  // Serial.print(" ");
  // Serial.print(avgR);
  // Serial.print("   ");
  // // Serial.print(dtime);
  // // Serial.print("   ");
  // Serial.print(Stop);
  // Serial.println(" ");

// Horizontal Angle (Sumbu X)
  if (dhoriz < Stop){
    if (avgL > avgR){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      delay(500);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }     
    else if (avgL < avgR){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      delay(500);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    } 
  }   

// // Horizontal Angle (Sumbu X)
//   if (-1*Stop > dhoriz || dhoriz > Stop) {// check if the diffirence is in the tolerance else change vertical angle
//     if (avgL > avgR){
//       servox = ++servox;
//       if (servox > servoxLimitHigh) { 
//         servox = servoxLimitHigh;
//       }
//     }
//     else if (avgL < avgR){
//       servox= --servox;
//       if (servox < servoxLimitLow){
//         servox = servoxLimitLow;
//       }
//     }
//     horizontal.write(servox);
//   }

// Verikal Angle (Sumbu Y)
  if (-1*Stop > dvert || dvert > Stop) {// check if the diffirence is in the tolerance else change vertical angle
    if (avgT > avgD){
      servoy = ++servoy;
      if (servoy > servoyLimitHigh) { 
        servoy = servoyLimitHigh;
      }
    }
    else if (avgT < avgD){
      servoy= --servoy;
      if (servoy < servoyLimitLow){
        servoy = servoyLimitLow;
      }
    }
    vertical.write(servoy);
  }
  // delay (1000);
}
