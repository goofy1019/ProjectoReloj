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
#define toneButton A5

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

// Define el pin del buzzer y el LED indicador
#define Buzzer 12 //dispositivo emisor de sonido
#define toneLED 13 // LED del tono

//////////////////////////////////////
// Creacion de variables u objetos //
////////////////////////////////////

// Define variable tipo long que almacena el tiempo actual
unsigned long previousMillis = 0;  // Verifica la ultima actualizacion del reloj, se inicializa en 00 00 00
                                   // Se utiliza unsigned debido a que el tiempo nunca sera negativo
unsigned long alarmCycleMillis = 0; // Ciclo de on/off de la alarma
unsigned long buzzerMillis = 0; // Intervalos para que el buzzer no suene siempre y me vuelva loco

// Define las variables del tiempo
int hour = 0, minute = 0, second = 0;  

//Variables de alarma
int alarmHour = 0, alarmMinute = 0;
bool alarmSet = false;
int selectedTone = 1; // Tono seleccionado
bool alarmActive = false; // Flag para ver si esta sonando la alarma
bool buzzerOn = false; // Flag para el estado del buzzer
bool userDeactivatedAlarm = false; // Flag para verificar que el ususario apagó la alarma

// Estado del programa
enum Estado { NORMAL, CONFIGURANDO_ALARMA, CONFIGURANDO_TIEMPO }; // crea el estado nuevo en que se esta configurando la alarma
Estado estadoActual = NORMAL; // proporcionado por ChatGPT

//////////////////////////////////////////////////
// Configuracion e inicializacion del hardware //
////////////////////////////////////////////////

void setup() { // Inicializa todo el hardware
  // Esto es equivalente al BIOS de una computadora
  pinMode(setTimeButton, INPUT); // Inicializacion del puerto
  pinMode(setAlarmButton, INPUT); // Inicializacion del puerto
  pinMode(hourButton, INPUT); // Inicializacion del puerto
  pinMode(minuteButton, INPUT); // Inicializacion del puerto
  pinMode(secondButton, INPUT); // Inicializacion del puerto
  pinMode(toneButton, INPUT); // Inicializacion del puerto

  pinMode(pinA, OUTPUT); // Inicializacion del puerto
  pinMode(pinB, OUTPUT); // Inicializacion del puerto
  pinMode(pinC, OUTPUT); // Inicializacion del puerto
  pinMode(pinD, OUTPUT); // Inicializacion del puerto

  pinMode(digit1, OUTPUT); // Inicializacion del puerto
  pinMode(digit2, OUTPUT); // Inicializacion del puerto
  pinMode(digit3, OUTPUT); // Inicializacion del puerto
  pinMode(digit4, OUTPUT); // Inicializacion del puerto
  pinMode(digit5, OUTPUT); // Inicializacion del puerto
  pinMode(digit6, OUTPUT); // Inicializacion del puerto

  pinMode(Buzzer, OUTPUT); // configuracion del buzzer
  pinMode(toneLED, OUTPUT); // configuracion del LED

  digitalWrite(Buzzer, LOW); // Apaga el buzzer inicialmente
  digitalWrite(toneLED, LOW); // Apaga el LED inicialmente
} // Final del setup

///////////////////////////////////////////////////////
// Configuracion y operacion normal del dispositivo //
/////////////////////////////////////////////////////

void loop() { // Crea un lazo infinito
  // Esto es el equivalente al OS de una computadora
  handleButtons(); // Llama a la funcion handleButtons que maneja el estripar de algun boton

  // Actualiza el reloj cada segundo (1000 milisegundos) excepto cuando está en set time
  if (millis() - previousMillis >= 1000 && estadoActual != CONFIGURANDO_TIEMPO) {
    previousMillis = millis();
    updateClock();
  }

  // Despliega el tiempo dependiendo del modo
  if (estadoActual == NORMAL) {
    displayMultiplexed(hour, minute, second);
  } else if (estadoActual == CONFIGURANDO_ALARMA) {
    displayMultiplexed(alarmHour, alarmMinute, 0);
  } else if (estadoActual == CONFIGURANDO_TIEMPO) {
    displayMultiplexed(hour, minute, second);
  } 
  
  // Si se activa la alarma llama a la funcion que la maneja
  if (alarmActive) {
    handleAlarm();
  }

  // Llama a la función que maneja la selección de tonos
  handleToneSelection();

} // Final del loop

////////////////////////////////////////////
// Definicion de las funciones y metodos //
//////////////////////////////////////////

void handleButtons() {
  // Entra al modo de SetAlarm si se estripa el boton
  if (digitalRead(setAlarmButton) == HIGH && estadoActual != CONFIGURANDO_TIEMPO) {
    delay(200); // Antirrebote
    estadoActual = CONFIGURANDO_ALARMA;

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
    
    alarmSet = true; // Establece que se activó una alarma
  } 

  // Entra al modo SetTime si se estripa el boton
  else if (digitalRead(setTimeButton) == HIGH && estadoActual != CONFIGURANDO_ALARMA) {
    delay(200); // Antirrebote
    estadoActual = CONFIGURANDO_TIEMPO;

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

  // Si no está estripado ningún botón, vuelve a operación normal
  else if (digitalRead(setTimeButton) == LOW && digitalRead(setAlarmButton) == LOW) {
    estadoActual = NORMAL;
  }
} // Final de la funcion

// Función que va a manejar la alarma (el ciclo, activar el buzzer, la activación en si, y la desactivación)
void handleAlarm() {
  if (millis() - buzzerMillis >= 1000) { // Crea los intervalos de 1 segundo para que el buzzer no esté constante
    buzzerMillis = millis();  // Reinicia el ciclo
    
    if (buzzerOn) {
      noTone(Buzzer);  // Apaga el buzzer
      buzzerOn = false;
    } else {
      playSelectedTone(); // Activa el buzzer con el tono seleccionado
      buzzerOn = true;
    }
  }

  // Desactiva la alarma si algún boton es presionado (no se toma en cuenta el boton de SetTone)
  if (digitalRead(setTimeButton) == HIGH || digitalRead(setAlarmButton) == HIGH ||
      digitalRead(hourButton) == HIGH || digitalRead(minuteButton) == HIGH ||
      digitalRead(secondButton) == HIGH) {
    alarmActive = false;           // Desactiva la alarma
    userDeactivatedAlarm = true;   // Hace que no se reactive
    noTone(Buzzer);                // Verifica que se apaga el buzzer
    buzzerOn = false;              // Reinicia el estado del buzzer
  }
} // Final de la función

void updateClock() { // Funcion que actualiza el reloj y revisa que no se sobrepase los 60 o las 24
  second++;
  if (second >= 60) {
    second = 0;
    minute++; // Agrega un minuto mas si se alcanzan los 60 segundos
    if (minute >= 60) {
      minute = 0;
      hour++; // Agrega una hora mas si se alcanzan los 60 min
      if (hour >= 24) {
        hour = 0;  // Reinicia el reloj a 00 00 00
      }
    }
  }
  // Comprueba si la alarma está configurada y si la hora de la alarma, y la del reloj coinciden
  if (alarmSet && hour == alarmHour && minute == alarmMinute && !alarmActive && !userDeactivatedAlarm) {
    alarmActive = true;    // Activa la alarma
    buzzerMillis = millis(); // Inicia el ciclo de un segundo
    playSelectedTone(); // Activa el buzzer
    buzzerOn = true;
  }

  // Reinicia el flag si el ususario no estripa nada
  if (hour != alarmHour || minute != alarmMinute) {
    userDeactivatedAlarm = false;
  }
} // Final de la funcion

void displayMultiplexed(int displayHour, int displayMinute, int displaySecond) { // Funcion hecha por ChatGPT para ayudar con el mutiplexing de los sigitos del display
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
} // Final de la funcion

void displayDigit(int number, int digitPin) { // Funcion hecha por ChatGPT para ayudar con el mutiplexing de los sigitos del display
  // Logica para escribir al decodificador y pasar de binario a decimal del display
  digitalWrite(pinA, (number & 0x1) ? HIGH : LOW);
  digitalWrite(pinB, (number & 0x2) ? HIGH : LOW);
  digitalWrite(pinC, (number & 0x4) ? HIGH : LOW);
  digitalWrite(pinD, (number & 0x8) ? HIGH : LOW);

  // Habilita el pin del digito
  digitalWrite(digitPin, LOW);
  delayMicroseconds(1000);
  digitalWrite(digitPin, HIGH);
} // Final de la funcion

// Maneja la selección del tono
void handleToneSelection() {
  if (digitalRead(toneButton) == HIGH && !alarmActive) { // Se activa si se presiona el boton de Set Tone
    delay(200); // Antirrebote
    selectedTone = (selectedTone % 5) + 1; // Hace un ciclo por los 5 tonos
    indicateTone(selectedTone);            // Muestra el tono seleccionado en el LED
    playSelectedTone();                    // Hace que suene el tono momentaneamente
    delay(200);                            // Delay para poder oir el tono, sino no servía
    noTone(Buzzer);                        // Para el tono despues de elegirlo
  }
} // Final de la función

// Hace que suene el tono elegido, hecha por ChatGPT
void playSelectedTone() {
  switch (selectedTone) {
    case 1: tone(Buzzer, 500); break;
    case 2: tone(Buzzer, 600); break;
    case 3: tone(Buzzer, 700); break;
    case 4: tone(Buzzer, 800); break;
    case 5: tone(Buzzer, 900); break;
  }
}

// Muestra el tono elegido en el LED
void indicateTone(int tone) {
  for (int i = 0; i < tone; i++) {
    digitalWrite(toneLED, HIGH);
    delay(100);
    digitalWrite(toneLED, LOW);
    delay(100);
  }
}