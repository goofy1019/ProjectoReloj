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
unsigned long toneMillis = 0; // Timer for non-blocking tone playback

// Define las variables del tiempo
int hour = 0, minute = 0, second = 50;

// Variables de alarma
int alarmHour = 0, alarmMinute = 1;
bool alarmSet = false;
int selectedTone = 1; // Tono seleccionado
bool playingTone = false; // Flag to indicate if a tone is being played

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
    displayMultiplexed(hour, minute, second); // Show time being set in set time mode
  }
  
  // Handle tone selection independently, without interrupting the clock
  handleToneSelection();
}

////////////////////////////////////////////
// Definicion de las funciones y metodos //
//////////////////////////////////////////

void handleButtons() {
  // Set Time Mode
  if (digitalRead(setTimeButton) == HIGH) {
    delay(200); // Debounce
    estadoActual = CONFIGURANDO_TIEMPO;
    
    if (digitalRead(hourButton) == HIGH) {
      delay(200); // Debounce
      hour = (hour + 1) % 24;
    }
    if (digitalRead(minuteButton) == HIGH) {
      delay(200); // Debounce
      minute = (minute + 1) % 60;
    }
    if (digitalRead(secondButton) == HIGH) {
      delay(200); // Debounce
      second = (second + 1) % 60;
    }
  }

  // Set Alarm Mode
  else if (digitalRead(setAlarmButton) == HIGH) {
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

  // Return to normal mode if no configuration button is pressed
  else if (estadoActual != NORMAL) {
    estadoActual = NORMAL;
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

  // Check if alarm should go off
  if (alarmSet && hour == alarmHour && minute == alarmMinute && second == 0) {
    playTone(selectedTone);
    delay(2000); // Alarm duration
    digitalWrite(Buzzer, LOW);
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
  if (digitalRead(toneButton) == HIGH && !playingTone) {
    delay(200); // Debounce
    selectedTone = (selectedTone % 5) + 1; // Cycle through 5 tones
    indicateTone(selectedTone);            // Show the selected tone with LED
    playTone(selectedTone);                // Play the selected tone briefly
  }
}

// Plays the selected tone for a short time
void playTone(int tone) {
  playingTone = true;
  switch (tone) {
    case 1: toneFrequency(500); break;
    case 2: toneFrequency(600); break;
    case 3: toneFrequency(700); break;
    case 4: toneFrequency(800); break;
    case 5: toneFrequency(900); break;
  }
  playingTone = false;
}

// Sets the frequency for each tone
void toneFrequency(int frequency) {
  tone(Buzzer, frequency);
  delay(200);
  noTone(Buzzer);
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
