// #include <WiFi.h>
// #include <ThingSpeak.h>
#include "Wire.h"
#include <ESP32_Servo.h>
#include <Adafruit_INA219.h> 

#define enA 26
#define enB 25
#define in1 34
#define in2 35
#define ldr1 27 //LDR top left - BOTTOM LEFT    <--- BDG
#define ldr2 14 //LDR top rigt - BOTTOM RIGHT 
#define ldr3 13 //LDR down left - TOP LEFT
#define ldr4 12 //ldr down rigt - TOP RIGHT

// const char* ssid = "****";   // your network SSID (name) 
// const char* password = "REPLACE_WITH_YOUR_PASSWORD";   // your network password
// WiFiClient  client;

// unsigned long myChannelNumber = X;
// const char * myWriteAPIKey = "XXXXXXXXXXXXXXXX";

// Alamat Sensor INA219 (Tegangan, Arus, Daya)
const int INA_addr = 0x40;  // INA219 address
Adafruit_INA219 ina219(INA_addr);

Servo vertical;   // vertical servo 
int servoy = 90;    //   90;     // stand vertical servo
int servoyLimitHigh = 120;
int servoyLimitLow = 15;

int Stop = 20;
int MotorB1 = 0;
int MotorB2 = 0;

// Nilai Awal Sensor INA219
float tegangan = 00;
float arus = 00; // diukur menggunakan MiliAmpere 
float daya = 00; 

void setup() {
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
  MotorB1 = 70; //0-255
  MotorB2 = 70; //0-255
  analogWrite(enA, MotorB1);
  analogWrite(enB, MotorB2); 
  
// servo y
  vertical.attach(10);
  vertical.write(45);   
  
  delay(3000);
  

// Inisasi Sensor INA219
  ina219.begin();
}

void loop() {
  int TL = analogRead(ldr4); // top left
  int TR = analogRead(ldr3); // top right
  int DL = analogRead(ldr2); // down left
  int DR = analogRead(ldr1); // down rigt
  
  int avgT = (TL + TR) / 2; // average value top
  int avgD = (DL + DR) / 2; // average value down
  int avgL = (TL + DL) / 2; // average value left
  int avgR = (TR + DR) / 2; // average value right

  int dvert = abs(avgT - avgD); // check the diffirence of up and down
  int dhoriz = abs(avgL - avgR);// check the diffirence og left and rigt
  
// Horizontal Angle (Sumbu X)
  if (dhoriz < Stop){
    if (avgL > avgR){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      delay(3000);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }     
    else if (avgL < avgR){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      delay(3000);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    } 
  }   

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

// Sensor INA219
  tegangan = ina219.getBusVoltage_V(); //command untuk pembacaan tegangan 
  arus = ina219.getCurrent_mA(); //command untuk pembacaan arus 
  daya = tegangan * (arus/1000); //rumus untuk mendapatkan nilai watt 
  
  Serial.print("Tegangan : "); 
  Serial.print(tegangan); 
  Serial.println(" Volt"); 
  Serial.print("Arus : "); 
  Serial.print(arus); 
  Serial.println(" mAmp"); 
  Serial.print("Daya : "); 
  Serial.print(daya); 
  Serial.println(" Watt"); 
  
  Serial.println(""); 

  Serial.print(TL);
  Serial.print(" ");
  Serial.print(TR);
  Serial.print(" ");
  Serial.print(DL);
  Serial.print(" ");
  Serial.print(DR);
  Serial.println("   ");
}
