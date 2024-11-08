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
#define toneButton A5   // Button to select different alarm tones

// Define los pines que se utilizan para el decodificador
#define pinA 2
#define pinB 3
#define pinC 4
#define pinD 5

// Define los pines utilizados para cada digito del display
#define digit1 6  // Decena de hora
#define digit2 7  // Unidad de hora
#define digit3 8  // Decena de minutos
#define digit4 9  // Unidad de minutos
#define digit5 10 // Decena de segundos
#define digit6 11 // Unidad de segundos

// Define el pin del buzzer y del LED de tono
#define Buzzer 12
#define toneLED 13  // Single LED to indicate the selected tone

//////////////////////////////////////
// Creacion de variables u objetos //
////////////////////////////////////

// Define variable tipo long que almacena el tiempo actual
unsigned long previousMillis = 0;

// Define las variables del tiempo
int hour = 0, minute = 0, second = 0;  

// Variables de alarma
int alarmHour = 0, alarmMinute = 0;
bool alarmSet = false;
int selectedTone = 1;   // Variable to store the selected tone

// Estado del programa
bool settingAlarm = false;

//////////////////////////////////////////////////
// Configuracion e inicializacion del hardware //
////////////////////////////////////////////////

void setup() {
  // Inicializa los pines de los botones
  pinMode(setTimeButton, INPUT);
  pinMode(setAlarmButton, INPUT);
  pinMode(hourButton, INPUT);
  pinMode(minuteButton, INPUT);
  pinMode(secondButton, INPUT);
  pinMode(toneButton, INPUT);

  // Inicializa los pines para el decodificador BCD
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  // Inicializa los pines de cada dígito
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  pinMode(digit5, OUTPUT);
  pinMode(digit6, OUTPUT);

  // Configura cada dígito como inactivo al inicio
  digitalWrite(digit1, HIGH);
  digitalWrite(digit2, HIGH);
  digitalWrite(digit3, HIGH);
  digitalWrite(digit4, HIGH);
  digitalWrite(digit5, HIGH);
  digitalWrite(digit6, HIGH);

  // Configuración inicial del buzzer
  pinMode(Buzzer, OUTPUT);
  pinMode(toneLED, OUTPUT);
  digitalWrite(Buzzer, LOW);
  digitalWrite(toneLED, LOW);  // Turn off the tone indicator LED initially
}

///////////////////////////////////////////////////////
// Configuracion y operacion normal del dispositivo //
/////////////////////////////////////////////////////

void loop() {
  handleButtons();

  // Actualiza el reloj cada segundo
  if (!settingAlarm && (millis() - previousMillis >= 1000)) {
    previousMillis = millis();
    updateClock();
  }

  // Despliega el tiempo actual o la hora de la alarma en modo de configuración
  if (settingAlarm) {
    displayMultiplexed(alarmHour, alarmMinute, 0);  // Show alarm time in Set Alarm mode
  } else {
    displayMultiplexed(hour, minute, second);  // Show current time otherwise
  }
}

////////////////////////////////////////////
// Definicion de las funciones y metodos //
//////////////////////////////////////////

void handleButtons() {
  // Modo de configuración de la hora
  if (digitalRead(setTimeButton) == HIGH) {
    delay(200); // Antirrebote

    // Ajusta las horas
    if (digitalRead(hourButton) == HIGH) {
      delay(200); // Antirrebote
      hour = (hour + 1) % 24;
    }

    // Ajusta los minutos
    if (digitalRead(minuteButton) == HIGH) {
      delay(200); // Antirrebote
      minute = (minute + 1) % 60;
    }

    // Ajusta los segundos
    if (digitalRead(secondButton) == HIGH) {
      delay(200); // Antirrebote
      second = (second + 1) % 60;
    }
  }

  // Modo de configuración de la alarma
  if (digitalRead(setAlarmButton) == HIGH) {
    delay(200); // Antirrebote
    settingAlarm = true;

    // Ajusta la hora de la alarma
    if (digitalRead(hourButton) == HIGH) {
      delay(200); // Antirrebote
      alarmHour = (alarmHour + 1) % 24;
    }

    // Ajusta los minutos de la alarma
    if (digitalRead(minuteButton) == HIGH) {
      delay(200); // Antirrebote
      alarmMinute = (alarmMinute + 1) % 60;
    }

    alarmSet = true;
  } else {
    settingAlarm = false;  // Exit alarm setting mode if button is released
  }

   // Tone selection
  if (digitalRead(toneButton) == HIGH) {
    delay(200); // Debounce
    selectedTone = (selectedTone % 5) + 1;
    indicateTone(selectedTone);
    playTone(selectedTone);
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
        hour = 0;
      }
    }
  }

  // Activa el buzzer si la alarma está configurada y coincide la hora
  if (alarmSet && hour == alarmHour && minute == alarmMinute && second == 0) {
    playTone(selectedTone);
    delay(2000); // Duración de la alarma
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

  digitalWrite(digitPin, LOW);  // Activa el dígito (activo LOW)
  delayMicroseconds(500);       // Ajusta el tiempo según sea necesario
  digitalWrite(digitPin, HIGH); // Desactiva el dígito
}

void playTone(int tone) {
  switch (tone) {
    case 1: tone1(); break;
    case 2: tone2(); break;
    case 3: tone3(); break;
    case 4: tone4(); break;
    case 5: tone5(); break;
  }
}

void indicateTone(int tone) {
  for (int i = 0; i < tone; i++) {
    digitalWrite(toneLED, HIGH);
    delay(200);
    digitalWrite(toneLED, LOW);
    delay(200);
  }
}

// Tone functions with different frequencies
void tone1() { tone(Buzzer, 500); delay(200); noTone(Buzzer); }
void tone2() { tone(Buzzer, 600); delay(200); noTone(Buzzer); }
void tone3() { tone(Buzzer, 700); delay(200); noTone(Buzzer); }
void tone4() { tone(Buzzer, 800); delay(200); noTone(Buzzer); }
void tone5() { tone(Buzzer, 900); delay(200); noTone(Buzzer); }
