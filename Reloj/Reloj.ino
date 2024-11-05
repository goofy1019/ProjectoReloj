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

#include <RTClib.h>   // Biblioteca que maneja los RTC (Real Time Clocks)
#include <Wire.h>

RTC_DS3231 rtc;

// Define constants for pins
const int setTimeButton = A0;
const int setAlarmButton = A1;
const int hourButton = A2;
const int minuteButton = A3;
const int secondButton = A4;
const int buzzerPin = 6;

// Define BCD input pins for the 74LS48 (assuming D2-D5 for one digit)
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

int hours = 0, minutes = 0, seconds = 0;
int alarmHour = 0, alarmMinute = 0, alarmSecond = 0;
bool alarmSet = false;
bool alarmActive = false;

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

  pinMode(setTimeButton, INPUT_PULLUP);
  pinMode(setAlarmButton, INPUT_PULLUP);
  pinMode(hourButton, INPUT_PULLUP);
  pinMode(minuteButton, INPUT_PULLUP);
  pinMode(secondButton, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();
  // Check for button presses to set time or alarm
  handleButtons();

  // Update the display
  displayTime(hour, minute, second);

  // Check if the alarm should go off
  if (alarmSet && hours == alarmHour && minutes == alarmMinute && seconds == 0) {
    activateAlarm();
  }

  delay(1000);
}

void handleButtons() {
  if (digitalRead(setTimeButton) == LOW) {
    delay(200); // Debounce
    setTime();
  }
  if (digitalRead(setAlarmButton) == LOW) {
    delay(200); // Debounce
    setAlarm();
  }
}

void setTime() {
  if (digitalRead(hourButton) == LOW) {
    hours = (hours + 1) % 24;
    delay(200);
  }
  if (digitalRead(minuteButton) == LOW) {
    minutes = (minutes + 1) % 60;
    delay(200);
  }
  if (digitalRead(secondButton) == LOW) {
    seconds = (seconds + 1) % 60;
    delay(200);
  }
  displayTime(hours, minutes, seconds);
}

void setAlarm() {
  if (digitalRead(hourButton) == LOW) {
    alarmHour = (alarmHour + 1) % 24;
    delay(200);
  }
  if (digitalRead(minuteButton) == LOW) {
    alarmMinute = (alarmMinute + 1) % 60;
    delay(200);
  }
  displayTime(alarmHour, alarmMinute, 0);  // Display the hour and minute only
}

void activateAlarm() {
  digitalWrite(buzzerPin, HIGH);
  delay(1000);  // Buzzer sound duration
  digitalWrite(buzzerPin, LOW);
}

// Function to display the time on 7-segment display
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

// Function to display a single digit on the 74LS48
void displayDigit(int number, int digitPin) {
  // Convert number to BCD and output to the 74LS48
  digitalWrite(pinA, (number & 0x1) ? HIGH : LOW);
  digitalWrite(pinB, (number & 0x2) ? HIGH : LOW);
  digitalWrite(pinC, (number & 0x4) ? HIGH : LOW);
  digitalWrite(pinD, (number & 0x8) ? HIGH : LOW);

  // Enable the correct digit
  digitalWrite(digitPin, HIGH);
  delay(2);  // Brief delay to stabilize the display
  digitalWrite(digitPin, LOW);  // Turn off digit to avoid ghosting
}