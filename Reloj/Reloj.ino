/*********************************************
* Universidad Fidelitas                      *
* Curso: Programacion y Diseño de Algoritmos *
* Autor: Adolfo Amador Afonso                *
*        Edgar Picado                        *
*        Kevin Esquivel Cartin               *
* Fecha: 08/11/2024                          *
* Nombre: Reloj.ino                          *
**********************************************/

///////////////////////////////
// Definicion de constantes //
/////////////////////////////

// Define los botones
#define setTimeButton A0
#define setAlarmButton A1
#define hourButton A2
#define minuteButton A3
#define secondButton A4
#define toneButton A5 // Button to select tone

// Define los pines que se utilizan para el decodificador
#define pinA 2
#define pinB 3
#define pinC 4
#define pinD 5

// Define los pines utilizados para cada digito del display
#define digit1 6
#define digit2 7
#define digit3 8
#define digit4 9
#define digit5 10
#define digit6 11

// Define el pin del buzzer y el LED indicador
#define Buzzer 12
#define toneLED 13 // LED to indicate selected tone

//////////////////////////////////////
// Creacion de variables u objetos //
////////////////////////////////////

// Define variable para almacenar el tiempo
unsigned long previousMillis = 0;
unsigned long alarmCycleMillis = 0; // Timer for 1-minute ON/OFF cycles
unsigned long buzzerMillis = 0; // Timer for 1-second buzzer ON/OFF intervals

// Define las variables del tiempo
int hour = 0, minute = 0, second = 0;

// Variables de alarma
int alarmHour = 0, alarmMinute = 0;
bool alarmSet = false;
int selectedTone = 1; // Tono seleccionado
bool alarmActive = false; // Flag to indicate if the alarm is being played
bool buzzerOn = false; // Flag to track the 1-second buzzer state
bool userDeactivatedAlarm = false; // Flag to prevent reactivation until next time

// Estado del programa
enum Estado { NORMAL, CONFIGURANDO_ALARMA, CONFIGURANDO_TIEMPO };
Estado estadoActual = NORMAL;

//////////////////////////////////////////////////
// Configuracion e inicializacion del hardware //
////////////////////////////////////////////////

void setup() {
  // Inicializa los botones y pines
  pinMode(setTimeButton, INPUT);
  pinMode(setAlarmButton, INPUT);
  pinMode(hourButton, INPUT);
  pinMode(minuteButton, INPUT);
  pinMode(secondButton, INPUT);
  pinMode(toneButton, INPUT);

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  pinMode(digit5, OUTPUT);
  pinMode(digit6, OUTPUT);

  pinMode(Buzzer, OUTPUT);
  pinMode(toneLED, OUTPUT);

  digitalWrite(Buzzer, LOW);
  digitalWrite(toneLED, LOW); // Apaga el LED inicialmente
}

///////////////////////////////////////////////////////
// Configuracion y operacion normal del dispositivo //
/////////////////////////////////////////////////////

void loop() {
  handleButtons();
  
  // Continuously update the clock every second, except in set time mode
  if (millis() - previousMillis >= 1000 && estadoActual != CONFIGURANDO_TIEMPO) {
    previousMillis = millis();
    updateClock();
  }

  // Display time or alarm based on the current mode
  if (estadoActual == NORMAL) {
    displayMultiplexed(hour, minute, second);
  } else if (estadoActual == CONFIGURANDO_ALARMA) {
    displayMultiplexed(alarmHour, alarmMinute, 0);
  } else if (estadoActual == CONFIGURANDO_TIEMPO) {
    displayMultiplexed(hour, minute, second);
  }
  
  // Handle the alarm cycle if it is active
  if (alarmActive) {
    handleAlarm();
  }

  handleToneSelection();
}

////////////////////////////////////////////
// Definicion de las funciones y metodos //
//////////////////////////////////////////

void handleButtons() {
  // Set Alarm Mode
  if (digitalRead(setAlarmButton) == HIGH && estadoActual != CONFIGURANDO_TIEMPO) {
    delay(200); // Debounce
    estadoActual = CONFIGURANDO_ALARMA;

    // Adjust alarm hour
    if (digitalRead(hourButton) == HIGH) {
      delay(200);
      alarmHour = (alarmHour + 1) % 24;
    }
    
    // Adjust alarm minute
    if (digitalRead(minuteButton) == HIGH) {
      delay(200);
      alarmMinute = (alarmMinute + 1) % 60;
    }
    
    alarmSet = true;
  } 

  // Set Time Mode (only if not already in CONFIGURANDO_ALARMA)
  else if (digitalRead(setTimeButton) == HIGH && estadoActual != CONFIGURANDO_ALARMA) {
    delay(200); // Debounce
    estadoActual = CONFIGURANDO_TIEMPO;

    // Adjust time hour
    if (digitalRead(hourButton) == HIGH) {
      delay(200); // Debounce
      hour = (hour + 1) % 24;
    }
    // Adjust time minute
    if (digitalRead(minuteButton) == HIGH) {
      delay(200); // Debounce
      minute = (minute + 1) % 60;
    }
    // Adjust time second
    if (digitalRead(secondButton) == HIGH) {
      delay(200); // Debounce
      second = (second + 1) % 60;
    }
  }

  // Return to normal mode if no configuration button is pressed
  else if (digitalRead(setTimeButton) == LOW && digitalRead(setAlarmButton) == LOW) {
    estadoActual = NORMAL;
  }
}

// Function to manage the 1-second buzzer ON/OFF cycle while alarm is active
void handleAlarm() {
  if (millis() - buzzerMillis >= 1000) { // 1-second interval
    buzzerMillis = millis();  // Reset timer for next cycle
    
    if (buzzerOn) {
      noTone(Buzzer);  // Turn OFF the buzzer
      buzzerOn = false;
    } else {
      playSelectedTone(); // Turn ON the buzzer with the selected tone
      buzzerOn = true;
    }
  }

  // Check if any button is pressed to deactivate the alarm
  if (digitalRead(setTimeButton) == HIGH || digitalRead(setAlarmButton) == HIGH ||
      digitalRead(hourButton) == HIGH || digitalRead(minuteButton) == HIGH ||
      digitalRead(secondButton) == HIGH) {
    alarmActive = false;           // Deactivate the alarm
    userDeactivatedAlarm = true;   // Set flag to prevent reactivation
    noTone(Buzzer);                // Ensure the buzzer is off
    buzzerOn = false;              // Reset the buzzer state
  }
}

void updateClock() {
  // Always increment time if not in set time mode
  second++;
  if (second >= 60) {
    second = 0;
    minute++;
    if (minute >= 60) {
      minute = 0;
      hour++;
      if (hour >= 24) {
        hour = 0;
      }
    }
  }

  // Check if alarm should go off and activate immediately
  if (alarmSet && hour == alarmHour && minute == alarmMinute && !alarmActive && !userDeactivatedAlarm) {
    alarmActive = true;    // Activate alarm
    buzzerMillis = millis(); // Start 1-second cycle timer
    playSelectedTone(); // Immediately activate the buzzer with the selected tone
    buzzerOn = true;
  }

  // Reset userDeactivatedAlarm flag if the time no longer matches the alarm time
  if (hour != alarmHour || minute != alarmMinute) {
    userDeactivatedAlarm = false;
  }
}

void displayMultiplexed(int displayHour, int displayMinute, int displaySecond) {
  int digits[] = {
    displayHour / 10, displayHour % 10,
    displayMinute / 10, displayMinute % 10,
    displaySecond / 10, displaySecond % 10
  };

  displayDigit(digits[0], digit1);
  displayDigit(digits[1], digit2);
  displayDigit(digits[2], digit3);
  displayDigit(digits[3], digit4);
  displayDigit(digits[4], digit5);
  displayDigit(digits[5], digit6);
}

void displayDigit(int number, int digitPin) {
  digitalWrite(pinA, (number & 0x1) ? HIGH : LOW);
  digitalWrite(pinB, (number & 0x2) ? HIGH : LOW);
  digitalWrite(pinC, (number & 0x4) ? HIGH : LOW);
  digitalWrite(pinD, (number & 0x8) ? HIGH : LOW);

  digitalWrite(digitPin, LOW);
  delayMicroseconds(1000);
  digitalWrite(digitPin, HIGH);
}

// Handles tone selection without altering estadoActual or pausing the clock
void handleToneSelection() {
  if (digitalRead(toneButton) == HIGH && !alarmActive) {
    delay(200); // Debounce
    selectedTone = (selectedTone % 5) + 1; // Cycle through 5 tones
    indicateTone(selectedTone);            // Show the selected tone with LED
    playSelectedTone();                    // Play the selected tone briefly
    delay(200);                            // Brief delay to hear the tone
    noTone(Buzzer);                        // Stop the tone after selection
  }
}

// Plays the selected tone based on selectedTone
void playSelectedTone() {
  switch (selectedTone) {
    case 1: tone(Buzzer, 500); break;
    case 2: tone(Buzzer, 600); break;
    case 3: tone(Buzzer, 700); break;
    case 4: tone(Buzzer, 800); break;
    case 5: tone(Buzzer, 900); break;
  }
}

// Shows the selected tone on the LED
void indicateTone(int tone) {
  for (int i = 0; i < tone; i++) {
    digitalWrite(toneLED, HIGH);
    delay(100);
    digitalWrite(toneLED, LOW);
    delay(100);
  }
}
