/*********************************************
* Universidad Fidelitas                      *
* Curso: Programacion y Diseño de Algoritmos *
* Autor: Adolfo Amador Afonso                *
*        Edgar Picado                        *
*        Kevin Esquivel Cartin               *
* Fecha: 08/11/2024                          *
* Nombre: Reloj.ino                          *
**********************************************/
/////////////////////////////
// Bibliotecas del sistema //
////////////////////////////

#include <RTClib.h>   // Real Time Clock library
#include <Wire.h>

RTC_DS3231 rtc;

// Define BCD input pins for the 74LS48
const int pinA = 2;  // BCD A
const int pinB = 3;  // BCD B
const int pinC = 4;  // BCD C
const int pinD = 5;  // BCD D

// Define digit enable pins for each display position
const int digit1 = 6;  // Enable pin for the first digit (tens of hours)
const int digit2 = 7;  // Enable pin for the second digit (units of hours)
const int digit3 = 8;  // Enable pin for the third digit (tens of minutes)
const int digit4 = 9;  // Enable pin for the fourth digit (units of minutes)
const int digit5 = 10; // Enable pin for the fifth digit (tens of seconds)
const int digit6 = 11; // Enable pin for the sixth digit (units of seconds)

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);

  // Initialize BCD pins
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  // Initialize digit enable pins
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  pinMode(digit5, OUTPUT);
  pinMode(digit6, OUTPUT);
}

void loop() {
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();

  displayTime(hour, minute, second);
}

void displayTime(int hour, int minute, int second) {
  // Separate hours, minutes, and seconds into individual digits
  int digits[] = {
    hour / 10, hour % 10,   // Tens and units of hours
    minute / 10, minute % 10, // Tens and units of minutes
    second / 10, second % 10   // Tens and units of seconds
  };

  // Display each digit by enabling one at a time and sending BCD values
  displayDigit(digits[0], digit1);  // Tens of hours
  displayDigit(digits[1], digit2);  // Units of hours
  displayDigit(digits[2], digit3);  // Tens of minutes
  displayDigit(digits[3], digit4);  // Units of minutes
  displayDigit(digits[4], digit5);  // Tens of seconds
  displayDigit(digits[5], digit6);  // Units of seconds
}

void displayDigit(int number, int digitPin) {
  // Convert number to BCD and output to the 74LS48
  digitalWrite(pinA, (number & 0x1) ? HIGH : LOW);
  digitalWrite(pinB, (number & 0x2) ? HIGH : LOW);
  digitalWrite(pinC, (number & 0x4) ? HIGH : LOW);
  digitalWrite(pinD, (number & 0x8) ? HIGH : LOW);

  // Enable the correct digit
  digitalWrite(digitPin, LOW);   // Adjust to HIGH or LOW based on your display's enable logic
  delayMicroseconds(1000);       // Adjust delay as necessary for stable multiplexing
  digitalWrite(digitPin, HIGH);  // Turn off digit to avoid ghosting
}