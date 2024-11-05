#include <Wire.h>
// Define button pins
const int setTimeButton = A0;
const int setAlarmButton = A1;
const int hourButton = A2;
const int minuteButton = A3;
const int secondButton = A4;

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

unsigned long previousMillis = 0;  // Tracks the last time we updated the clock
int hour = 0, minute = 0, second = 0;  // Time variables
int alarmHour = 0, alarmMinute = 0, alarmSecond = 0; // Alarm time variables

enum Mode { NORMAL, SET_TIME, SET_ALARM };
Mode currentMode = NORMAL;  // Initial mode

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);

  // Initialize button pins without pull-up (assuming external pull-down resistors are used)
  pinMode(setTimeButton, INPUT);
  pinMode(setAlarmButton, INPUT);
  pinMode(hourButton, INPUT);
  pinMode(minuteButton, INPUT);
  pinMode(secondButton, INPUT);

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

  // Initialize all digits to HIGH (inactive)
  digitalWrite(digit1, HIGH);
  digitalWrite(digit2, HIGH);
  digitalWrite(digit3, HIGH);
  digitalWrite(digit4, HIGH);
  digitalWrite(digit5, HIGH);
  digitalWrite(digit6, HIGH);
}

void loop() {
  handleButtons();

  // Update the clock every second if in NORMAL mode
  if (currentMode == NORMAL && millis() - previousMillis >= 1000) {
    previousMillis = millis();
    updateClock();
  }

  // Continuously refresh display for smooth multiplexing
  int displayHour = (currentMode == SET_ALARM) ? alarmHour : hour;
  int displayMinute = (currentMode == SET_ALARM) ? alarmMinute : minute;
  int displaySecond = (currentMode == SET_ALARM) ? alarmSecond : second;

  // Refresh each digit
  displayMultiplexed(displayHour, displayMinute, displaySecond);
}

void handleButtons() {
  // Switch to Set Time Mode
  if (digitalRead(setTimeButton) == HIGH) {  // Changed to HIGH
    delay(200); // Debounce
    currentMode = (currentMode == SET_TIME) ? NORMAL : SET_TIME;
    Serial.println("Switched to Set Time Mode");
  }

  // Switch to Set Alarm Mode
  if (digitalRead(setAlarmButton) == HIGH) {  // Changed to HIGH
    delay(200); // Debounce
    currentMode = (currentMode == SET_ALARM) ? NORMAL : SET_ALARM;
    Serial.println("Switched to Set Alarm Mode");
  }

  // Adjust hours in Set Mode
  if (digitalRead(hourButton) == HIGH) {  // Changed to HIGH
    delay(200); // Debounce
    if (currentMode == SET_TIME) {
      hour = (hour + 1) % 24;
    } else if (currentMode == SET_ALARM) {
      alarmHour = (alarmHour + 1) % 24;
    }
    Serial.println("Hour incremented");
  }

  // Adjust minutes in Set Mode
  if (digitalRead(minuteButton) == HIGH) {  // Changed to HIGH
    delay(200); // Debounce
    if (currentMode == SET_TIME) {
      minute = (minute + 1) % 60;
    } else if (currentMode == SET_ALARM) {
      alarmMinute = (alarmMinute + 1) % 60;
    }
    Serial.println("Minute incremented");
  }

  // Adjust seconds in Set Mode
  if (digitalRead(secondButton) == HIGH) {  // Changed to HIGH
    delay(200); // Debounce
    if (currentMode == SET_TIME) {
      second = (second + 1) % 60;
    } else if (currentMode == SET_ALARM) {
      alarmSecond = (alarmSecond + 1) % 60;
    }
    Serial.println("Second incremented");
  }
}

void updateClock() {
  second++;
  if (second >= 60) {
    second = 0;
    minute++;
    if (minute >= 60) {
      minute = 0;
      hour++;
      if (hour >= 24) {
        hour = 0;  // Reset to 00:00:00 at midnight
      }
    }
  }

  // Debugging: Print the current time to Serial Monitor
  Serial.print("Time: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
}

void displayMultiplexed(int hour, int minute, int second) {
  // Separate hours, minutes, and seconds into individual digits
  int digits[] = {
    hour / 10, hour % 10,   // Tens and units of hours
    minute / 10, minute % 10, // Tens and units of minutes
    second / 10, second % 10   // Tens and units of seconds
  };

  // Display each digit briefly for multiplexing
  displayDigit(digits[0], digit1);
  displayDigit(digits[1], digit2);
  displayDigit(digits[2], digit3);
  displayDigit(digits[3], digit4);
  displayDigit(digits[4], digit5);
  displayDigit(digits[5], digit6);
}

void displayDigit(int number, int digitPin) {
  // Convert number to BCD and output to the 74LS48
  digitalWrite(pinA, (number & 0x1) ? HIGH : LOW);
  digitalWrite(pinB, (number & 0x2) ? HIGH : LOW);
  digitalWrite(pinC, (number & 0x4) ? HIGH : LOW);
  digitalWrite(pinD, (number & 0x8) ? HIGH : LOW);

  // Enable the correct digit (active LOW)
  digitalWrite(digitPin, LOW);
  delayMicroseconds(500); // Brief delay to stabilize the display
  digitalWrite(digitPin, HIGH); // Turn off digit to avoid ghosting
}