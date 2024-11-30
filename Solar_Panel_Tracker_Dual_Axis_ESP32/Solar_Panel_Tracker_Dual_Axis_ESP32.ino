#include <Wire.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <ESP32_Servo.h>
#include <elapsedMillis.h>
#include <MPU6050_tockn.h>
#include <Adafruit_INA219.h> 
#include <LiquidCrystal_I2C.h>

#define ldr1 35 //LDR top left - BOTTOM LEFT    <--- BDG
#define ldr2 34 //LDR top rigt - BOTTOM RIGHT 
#define ldr3 39 //LDR down left - TOP LEFT
#define ldr4 36 //ldr down rigt - TOP RIGHT

// Deklarasi WiFi yang digunakan
const char *ssid =  "*****";  
const char *pass =  "12345678";
WiFiClient client;

// Channel Thingspeak
unsigned long myChannelNumber = 2072644;
const char * myWriteAPIKey = "LMEI8KH8QV4NHSXN";

// Alamat Sensor INA219 (Tegangan, Arus, Daya)
const int INA_addr = 0x40;  // INA219 address
Adafruit_INA219 ina219(INA_addr);

// Alamat Sensor MPU6050
MPU6050 mpu6050(Wire);
const int MPU_addr=0x68; 

// Alamat LCD 20x4
LiquidCrystal_I2C lcd(0x27,20,4); //LCD address to 0x27

// Millis Sensor 
elapsedMillis readSensorLDRMillis;
elapsedMillis readSensorINAMillis;
elapsedMillis readSensorMPUMillis;

// Millis LCD
elapsedMillis LDRPrintMillis;
elapsedMillis MPUPrintMillis;
elapsedMillis INAPrintMillis;

// Millis Thingspeak
elapsedMillis thingspeakMillis;

// Nilai Interval Pembacaan Sensor
unsigned long readSensorLDRInterval = 10;
unsigned long readSensorINAInterval = 20;
unsigned long readSensorMPUInterval = 30;

// Nilai Interval Pembacaan LCD & Thingspeak
unsigned long LDRPrintInterval = 15000;
unsigned long INAPrintInterval = 20000;
unsigned long MPUPrintInterval = 25000;
unsigned long thingspeakInterval = 30000;

Servo horizontal;
int servox = 90;
int servoxLimitHigh = 180;
int servoxLimitLow = 0;

Servo vertical;   // vertical servo 
int servoy = 45;    //   90;     // stand vertical servo
int servoyLimitHigh = 180;
int servoyLimitLow = 0;

int Stop = 50;

// Nilai Awal Sensor INA219
float tegangan = 00;
float arus = 00; // diukur menggunakan MiliAmpere 
float daya = 00; 

int TL; int TR;
int DL; int DR;

float Sumbu_X;
float Sumbu_Y;

void setup() {
  Serial.begin(9600); 
  
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  lcd.init();
  lcd.backlight();  
  
  initWiFi();
  initSensor();
  
  ThingSpeak.begin(client);
}

void loop() {
  
  if (readSensorLDRMillis >= readSensorLDRInterval){
    initLDR();    
    readSensorLDRMillis = 0;  
  }

  if (readSensorINAMillis >= readSensorINAInterval){
    initINA219();    
    readSensorINAMillis = 0;  
  }

  if (readSensorMPUMillis >= readSensorMPUInterval){
    initMPU();    
    readSensorMPUMillis = 0;  
  }

  if (LDRPrintMillis >= LDRPrintInterval){
    LcdPrintLDR();
    LDRPrintMillis = 0;
  }

  if (INAPrintMillis >= INAPrintInterval){
    LcdPrintINA();
    INAPrintMillis = 0;     
  }

  if (MPUPrintMillis >= MPUPrintInterval){
    LcdPrintMPU();
    MPUPrintMillis = 0;     
  }

  if (thingspeakMillis >= thingspeakInterval){
    thingspeakReading();
    thingspeakMillis = 0;
  }

  Serial.print("Tegangan : "); 
  Serial.print(tegangan); 
  Serial.println(" Volt"); 
  Serial.print("Arus : "); 
  Serial.print(arus); 
  Serial.println(" mAmp"); 
  Serial.print("Daya : "); 
  Serial.print(daya); 
  Serial.println(" Watt"); 
  
  Serial.print(TL);
  Serial.print(" ");
  Serial.print(TR);
  Serial.print(" ");
  Serial.print(DL);
  Serial.print(" ");
  Serial.print(DR);
  Serial.println("   ");
  
  Serial.print("Sumbu_X: ");
  Serial.print(Sumbu_X);
  Serial.print("Sumbu_Y: ");
  Serial.print(Sumbu_Y);
  
  delay(100);
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

// Inisasi Sensor INA219
  ina219.begin();  
}

void initWiFi() {
// Deklarasi Wifi
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("..");
    Serial.println();  
    Serial.println("Connecting To WiFi....");
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("Connecting To WiFi..");
    lcd.setCursor(0,2);
    lcd.print(" ESP32 is Connected ");
    lcd.setCursor(0,3);
    lcd.print("  Preparing Sensor  ");
  }
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

void initINA219(){
  // Sensor INA219
  tegangan = ina219.getBusVoltage_V(); //command untuk pembacaan tegangan 
  arus = ina219.getCurrent_mA(); //command untuk pembacaan arus 
  daya = tegangan * (arus/1000); //rumus untuk mendapatkan nilai watt 
}

void initMPU(){  
  mpu6050.update(); 
  Sumbu_X = mpu6050.getAngleX();
  // Sumbu_Y = mpu6050.getAngleY();
}

void LcdPrintLDR(){
// LCD Print Sensor LDR
  lcd.setCursor(0,0);
  lcd.print("     Sensor LDR   ");
  lcd.setCursor(0,1);
  lcd.print("LDR1:"); lcd.print(TL); lcd.print(" LDR2:"); lcd.print(TR); lcd.print(" ");
  lcd.setCursor(0,2);
  lcd.print("LDR3:"); lcd.print(DL); lcd.print(" LDR4:"); lcd.print(DR); lcd.print(" ");
  lcd.setCursor(0,3);
  lcd.print("                   ");
}

void LcdPrintINA(){
// LCD Print Sensor Ina219
  lcd.setCursor(0,0);
  lcd.print("   Sensor INA219   ");
  lcd.setCursor(0,1);
  lcd.print("  Arus  :"); lcd.print(arus); lcd.print(" mAmp"); lcd.print("  ");
  lcd.setCursor(0,2);
  lcd.print("  Daya  :"); lcd.print(daya); lcd.print(" Watt"); lcd.print("  ");
  lcd.setCursor(0,3);
  lcd.print("Tegangan:"); lcd.print(tegangan); lcd.print(" Volt"); lcd.print("  ");
}

void LcdPrintMPU(){
// LCD Print Sensor MPU6050
  lcd.setCursor(0,0);
  lcd.print("   Sensor MPU6050  ");
  lcd.setCursor(0,1);
  lcd.print("X :"); lcd.print(Sumbu_X); lcd.print(" derajat"); lcd.print("     ");
  lcd.setCursor(0,2);
  lcd.print("                   ");
  lcd.setCursor(0,3);
  lcd.print("                   ");
}

void thingspeakReading(){
  ThingSpeak.setField(1, Sumbu_X);
  ThingSpeak.setField(2, tegangan);
  ThingSpeak.setField(3, arus);
  ThingSpeak.setField(4, daya);
  ThingSpeak.setField(5, TL);
  ThingSpeak.setField(6, TR);
  ThingSpeak.setField(7, DL);
  ThingSpeak.setField(8, DR);

  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}
