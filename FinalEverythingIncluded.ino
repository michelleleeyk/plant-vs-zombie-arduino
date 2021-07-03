// PVZ

// Set up DC motor and start button
#define ENABLE 5
#define DIRA 3
#define DIRB 4
const int startButtonPin = 2;
int startButtonState = 0;

// Set up kill button
const int buttonPin = 12;
const int ledPin =  13;
int buttonState = 0;
int clickCounter = 0;

// Set up LCD Screen
#include <LiquidCrystal.h>
const int rs = 42, en = 40, d4 = 30, d5 = 28, d6 = 26, d7 = 24;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Set up neopixels
#include <Adafruit_NeoPixel.h>
#define PIN 9
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Set up ultrasonic sensor
#include "SR04.h"
#define TRIG_PIN 7
#define ECHO_PIN 6
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long distanceFromStart;
const int resetButtonPin = 11;
int resetButtonState = 0;

// Set up plant servo motor
#include <Servo.h>
Servo plantServo;
int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(9600);
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  pinMode(startButtonPin, INPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("PLANT VS ZOMBIE");
  lcd.setCursor(0, 1);
  lcd.print("<Start Game>");

  pixels.begin();
  pixels.clear();

  pinMode(resetButtonPin, INPUT);

  plantServo.attach(10);  // attach servo object to pin 10
  plantServo.write(0);
}

void loop() {
  startButtonState = digitalRead(startButtonPin);
  resetButtonState = digitalRead(resetButtonPin);

  if (startButtonState == HIGH){
    delay(800);
    Serial.println("Zombie starts invading house");
    analogWrite(ENABLE, 255); //enable on
    digitalWrite(DIRA, LOW);  //one way
    digitalWrite(DIRB, HIGH);

    // rotate at slow speed
    analogWrite(ENABLE, 170);
    delay(200);

    lcd.setCursor(0,1);
    lcd.print("Zombie attacks!");
    delay(200);
  }

  if (resetButtonState == HIGH) {
    Serial.println("Resetting conveyor belt");
    analogWrite(ENABLE, 255); //enable on
    digitalWrite(DIRA, HIGH); // opposite direction than starting motor
    digitalWrite(DIRB, LOW);

    // rotate at slow speed
    analogWrite(ENABLE, 135);
    delay(200);
    lcd.clear();
    lcd.print("Resetting game");

    Serial.println(analogRead(ENABLE));

    while (analogRead(ENABLE) != 0) {
      distanceFromStart = sr04.Distance();     //Return distance in cm
      Serial.print(distanceFromStart);
      Serial.println("cm   ");
      delay(50);

      if (distanceFromStart < 3) {
        digitalWrite(DIRA, LOW);
        Serial.println("Conveyor belt back to original starting point");
        delay(50);
      }
    }
  }
  
  while (digitalRead(DIRB)==HIGH){    // while DC motor is running
    for (pos = 10; pos <= 70; pos += 5) {
      buttonState = digitalRead(buttonPin);
      plantServo.write(pos);
      delay(10);
      gamePlay();
      if (digitalRead(ENABLE) == LOW){
        break;
      }
    }
    if (digitalRead(ENABLE) == LOW){
      break;
    }
    for (pos = 70; pos >= 0; pos -= 5) {
      buttonState = digitalRead(buttonPin);
      plantServo.write(pos);
      delay(10);
      gamePlay();
      if (digitalRead(ENABLE) == LOW){
        break;
      }
    }
    if (digitalRead(ENABLE) == LOW){
      break;
    }
  }
}

void gamePlay() {
//  Serial.println("running game play");
  if (buttonState == HIGH and pos > 25 and pos <55){
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    clickCounter += 1;
    Serial.println(clickCounter);

    pixels.setPixelColor(clickCounter, pixels.Color(0, 255, 0));
    pixels.show();

    // set the cursor to column 0, line 1
    lcd.clear();
    lcd.print("Keep shooting!");
    delay(200); //otherwise the counter will increment too quickly (detect too fast)
    pixels.clear();
    delay(100);

//    if (clickCounter == 5) {
//      Serial.println("Zombie moves faster!");
//      lcd.clear();
//      lcd.print("Zombie moves");
//      lcd.setCursor(0, 1);
//      lcd.print("even faster!");
////      analogWrite(ENABLE, 220);
////      Serial.println("speed 220");
//    }

    if (clickCounter == 2) {
      Serial.println("Successfully killed zombie!");
      lcd.clear();
      lcd.print("Success!");
      lcd.setCursor(0, 1);
      lcd.print("Killed zombie");
      digitalWrite(ENABLE, LOW);
      plantServo.detach();
      pixels.clear();
      pixels.show();
    }
  }

}
