#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// ihave changjiiajnfo?

Servo servo1;

LiquidCrystal_I2C lcd(0x27, 16, 2);
int force = A0;
int buzzer = 10;
String Phone_Number = "01792037908";
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 5000;

// HCSR04 hc1(9, 8); //initialisation class HCSR04 (trig pin , echo pin)
// HCSR04 hc2(7, 6); //initialisation class HCSR04 (trig pin , echo pin)
#define SIM800_TX_PIN 4
#define SIM800_RX_PIN 5
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

int sonar1[] = { 9, 8 };
int sonar2[] = { 7, 6 };

int sonar(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.0343 / 2;
  return distance;
}

void updateSerial() {
  while (Serial.available()) {
    serialSIM800.write(Serial.read());  // Forward what Serial received to Software Serial Port
  }
  while (serialSIM800.available()) {
    Serial.write(serialSIM800.read());  // Read the received character
  }
}

void sms(String number) {
  String recepiant = "AT+CMGS=\"+88" + number + "\"";
  serialSIM800.println(recepiant);  // 1)
  updateSerial();
  delay(1000);
  serialSIM800.print("TRAIN DETECTED. PLEASE BE AWARE. CHECK THE GATE");  // 2) text content
  updateSerial();
  delay(1000);
  serialSIM800.write((char)26);  // 3)
  delay(1000);
  //01792037908
  //Send new SMS command and message number
  // serialSIM800.write("AT+CMGS=\"01644644810\"\r\n");
  // delay(1000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  serialSIM800.begin(9600);
  lcd.init();
  lcd.backlight();
  servo1.attach(3);
  pinMode(force, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(sonar1[0], OUTPUT);
  pinMode(sonar1[1], INPUT);
  pinMode(sonar2[0], OUTPUT);
  pinMode(sonar2[1], INPUT);
  servo1.write(0);
  serialSIM800.println("AT");  //Once the handshake test is successful, it will back to OK
  updateSerial();
  delay(1000);
  serialSIM800.println("AT+CMGF=1");  // Configuring TEXT mode
  updateSerial();
  delay(1000);
  serialSIM800.println("AT+CNMI=1,2,0,0,0");  // Decides how newly arrived SMS messages should be handled
  updateSerial();
  delay(1000);
  lcd.setCursor(3, 0);
  lcd.print("SMART RAIL");
  lcd.setCursor(0, 1);
  lcd.print("MONITOR SYSTEM");
  delay(3000);
  lcd.clear();
}

void loop() {

  int force_out = analogRead(force);

  if (force_out >= 850) {
    servo1.write(0);
    sms(Phone_Number);

    while (true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("TRAIN DETECTED");
      lcd.setCursor(4, 1);
      lcd.print("WARNING");
      tone(buzzer, 1000);
      delay(300);
      noTone(buzzer);
      int distance1 = sonar(sonar1[0], sonar1[1]);
      Serial.println(distance1);
      if (distance1 >= 1 && distance1 < 5) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TRAIN VERY NEAR");
        lcd.setCursor(2, 1);
        lcd.print("FROM GATE");
        servo1.write(90);
        while (true) {
          tone(buzzer, 1500);
          delay(200);
          int distance2 = sonar(sonar2[0], sonar2[1]);
          if (distance2 >= 1 && distance2 < 5) {
            noTone(buzzer);
            lcd.clear();
            servo1.write(0);
            lcd.setCursor(1, 0);
            lcd.print("NOW IT IS SAFE");
            delay(2000);
            lcd.clear();
            break;
          }
        }
        break;
      }
    }
  } else {
    lcd.setCursor(3, 0);
    lcd.print("SMART RAIL");
    lcd.setCursor(0, 1);
    lcd.print("MONITOR SYSTEM");
  }
}