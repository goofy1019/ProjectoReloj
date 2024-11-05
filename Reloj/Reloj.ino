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

///////////////////////////////
// Definicion de constantes //
/////////////////////////////
// Define los botones
const int setTimeButton = A0;
const int setAlarmButton = A0;
const int hourButton = A2;
const int minuteButton = A3;
const int secondButton = A4;

// Define los pines que se utilizan para el decodificador
const int pinA = 2;  // BCD A
const int pinB = 3;  // BCD B
const int pinC = 4;  // BCD C
const int pinD = 5;  // BCD D

// Define los pines utilizados para cada digito del display
const int digit1 = 6;  // Decena de hora
const int digit2 = 7;  // Unidad de hora
const int digit3 = 8;  // Decena de minutos
const int digit4 = 9;  // Unidad de minutos
const int digit5 = 10; // Decena de segundos
const int digit6 = 11; // Unidad de segundos

//////////////////////////////////////
// Creacion de variables u objetos //
////////////////////////////////////
// Define variable tipo long que almacena el tiempo actual
unsigned long previousMillis = 0;  // Verifica la ultima actualizacion del reloj, se inicializa en 00 00 00

// Define las variables del tiempo
int hour = 0, minute = 0, second = 0;  

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);

  // Initialize button pins with assumed external pull-down resistors
  pinMode(setTimeButton, INPUT);
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

  // Set all digits to HIGH (inactive)
  digitalWrite(digit1, HIGH);
  digitalWrite(digit2, HIGH);
  digitalWrite(digit3, HIGH);
  digitalWrite(digit4, HIGH);
  digitalWrite(digit5, HIGH);
  digitalWrite(digit6, HIGH);
}

void loop() {
  handleButtons();

  // Update the clock every second
  if (millis() - previousMillis >= 1000) {
    previousMillis = millis();
    updateClock();
  }

  // Display the current time
  displayMultiplexed(hour, minute, second);
}

void handleButtons() {
  // If setTimeButton is pressed, check individual time adjustment buttons
  if (digitalRead(setTimeButton) == HIGH) {
    delay(200); // Debounce

    // Adjust hours if hourButton is pressed
    if (digitalRead(hourButton) == HIGH) {
      delay(200); // Debounce
      hour = (hour + 1) % 24;
      Serial.print("Set Hour: ");
      Serial.println(hour);
    }

    // Adjust minutes if minuteButton is pressed
    if (digitalRead(minuteButton) == HIGH) {
      delay(200); // Debounce
      minute = (minute + 1) % 60;
      Serial.print("Set Minute: ");
      Serial.println(minute);
    }

    // Adjust seconds if secondButton is pressed
    if (digitalRead(secondButton) == HIGH) {
      delay(200); // Debounce
      second = (second + 1) % 60;
      Serial.print("Set Second: ");
      Serial.println(second);
    }
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

void displayMultiplexed(int displayHour, int displayMinute, int displaySecond) {
  // Separate hours, minutes, and seconds into individual digits
  int digits[] = {
    displayHour / 10, displayHour % 10,   // Tens and units of hours
    displayMinute / 10, displayMinute % 10, // Tens and units of minutes
    displaySecond / 10, displaySecond % 10   // Tens and units of seconds
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
  delayMicroseconds(1000); // Adjust this delay as needed to stabilize display
  digitalWrite(digitPin, HIGH); // Turn off digit to avoid ghosting
}
