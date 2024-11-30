#include <ESP32_Servo.h>

#define ldr1 35 //LDR top left - BOTTOM LEFT    <--- BDG
#define ldr2 34 //LDR top rigt - BOTTOM RIGHT 
#define ldr3 39 //LDR down left - TOP LEFT
#define ldr4 36 //ldr down rigt - TOP RIGHT

Servo horizontal;
int servox = 90;
int servoxLimitHigh = 180;
int servoxLimitLow = 0;

Servo vertical;   // vertical servo 
int servoy = 45;    //   90;     // stand vertical servo
int servoyLimitHigh = 180;
int servoyLimitLow = 0;

int Stop = 50;

int TL; int TR;
int DL; int DR;

void setup() {
  Serial.begin(9600); 

  initSensor();
}

void loop() {
  initLDR();   
  
  Serial.print(TL);
  Serial.print(" ");
  Serial.print(TR);
  Serial.print(" ");
  Serial.print(DL);
  Serial.print(" ");
  Serial.print(DR);
  Serial.println("   ");
  delay(10);
}

void initSensor(){
// Deklarasiin Sensor LDR
  pinMode(ldr1, INPUT);
  pinMode(ldr2, INPUT);
  pinMode(ldr3, INPUT);
  pinMode(ldr4, INPUT);
 
// servo x
  horizontal.attach(0);
  horizontal.write(90);  
  
// servo y
  vertical.attach(4);
  vertical.write(45);   
}

void initLDR(){
  TL = analogRead(ldr1); // top left
  TR = analogRead(ldr2); // top right
  DL = analogRead(ldr3); // down left
  DR = analogRead(ldr4); // down rigt
  
  int avgT = (TL + TR) / 2; // average value top
  int avgD = (DL + DR) / 2; // average value down
  int avgL = (TL + DL) / 2; // average value left
  int avgR = (TR + DR) / 2; // average value right

  int dvert = avgT - avgD; // check the diffirence of up and down
  int dhoriz = avgL - avgR;// check the diffirence og left and rigt

// Verikal Angle (Sumbu Y)
  if (-1*Stop > dvert || dvert > Stop) {// check if the diffirence is in the tolerance else change vertical angle
    if (avgT < avgD){
      servoy = servoy + 1;
      if (servoy > servoyLimitHigh) { 
        servoy = servoyLimitHigh;
      }
    }
    else if (avgT > avgD){
      servoy= servoy - 1;
      if (servoy < servoyLimitLow){
        servoy = servoyLimitLow;
      }
    }
    else if (avgT == avgD){
      
    }
    vertical.write(servoy);
    delay(20);
  }


// Horizontal Angle (Sumbu X)
  if (-1*Stop > dhoriz || dhoriz > Stop) {// check if the diffirence is in the tolerance else change vertical angle
    if (avgL < avgR){
      servox = servox + 1;
      if (servox > servoxLimitHigh) { 
        servox = servoxLimitHigh;
      }
    }
    else if (avgL > avgR){
      servox= servox - 1;
      if (servox < servoxLimitLow){
        servox = servoxLimitLow;
      }
    }
    else if (avgL == avgR){
      
    }
    horizontal.write(servox);
    delay(20);
  }
}