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
#define Buzzer 12 //dispositivo emisor de sonido

//////////////////////////////////////
// Creacion de variables u objetos //
////////////////////////////////////

// Define variable tipo long que almacena el tiempo actual
unsigned long previousMillis = 0;  // Verifica la ultima actualizacion del reloj, se inicializa en 00 00 00
                                   // Se utiliza unsigned debido a que el tiempo nunca sera negativo

// Define las variables del tiempo
int hour = 0, minute = 0, second = 0;  

//Variables de alarma
int alarmHour = 0, alarmMinute = 0;
bool alarmSet = false;

// Estado del programa
enum Estado { NORMAL, CONFIGURANDO_ALARMA }; //crea el estado nuevo en que se esta configurando la alarma
Estado estadoActual = NORMAL;                //proporcionado por ChatGPT

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

  digitalWrite(digit1, HIGH); // Modifica el digito a inactivo
  digitalWrite(digit2, HIGH); // Modifica el digito a inactivo
  digitalWrite(digit3, HIGH); // Modifica el digito a inactivo
  digitalWrite(digit4, HIGH); // Modifica el digito a inactivo
  digitalWrite(digit5, HIGH); // Modifica el digito a inactivo
  digitalWrite(digit6, HIGH); // Modifica el digito a inactivo

  pinMode (Buzzer, OUTPUT); //configuracion del buzzer
  digitalWrite(Buzzer, LOW); //Apaga el Buzzer inicialmente
} // Final del setup

///////////////////////////////////////////////////////
// Configuracion y operacion normal del dispositivo //
/////////////////////////////////////////////////////
void loop() { // Crea un lazo infinito
  // Esto es el equivalente al OS de una computadora
  
  handleButtons(); // Llama a la funcion handleButtons que maneja el estripar de algun boton

  // Actualiza el reloj cada segundo (1000 milisegundos)
  if (millis() - previousMillis >= 1000) {
    previousMillis = millis();
    updateClock();
  }

  // Despliega el tiempo
  displayMultiplexed(hour, minute, second); // Llama a la funcion que despliega el tiempo
} // Final del loop

////////////////////////////////////////////
// Definicion de las funciones y metodos //
//////////////////////////////////////////

void handleButtons() { // Funcion para manejo de botones
  // Si el boton de Set Time esta estripado, actualiza la hora dependiendo del boton que se estripe
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
  // Si el botón de Set Alarm está presionado, entra al modo de ajuste de alarma
  if (digitalRead(setAlarmButton) == HIGH) {
    delay(200); // Antirrebote
    estadoActual = CONFIGURANDO_ALARMA;
    // Ajusta la hora de la alarma
    if (digitalRead(hourButton) == HIGH) {
      delay(200); // Antirrebote
      alarmHour = (alarmHour + 1) % 60;
    }
    // Ajusta los minutos de la alarma
    if (digitalRead(minuteButton) == HIGH) {
      delay(200); // Antirrebote
      alarmMinute = (alarmMinute + 1) % 60;
    }
    alarmSet = true; // Indica que la alarma está configurada
    }else if (estadoActual == CONFIGURANDO_ALARMA) {
    estadoActual = NORMAL;
  }
} // Final de la funcion

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
  if (alarmSet && hour == alarmHour && minute == alarmMinute && second == 0) {
    digitalWrite(Buzzer, HIGH);
    delay(2000); // Duración de la alarma
    digitalWrite(Buzzer, LOW);
  }
} // Final de la funcion

void displayMultiplexed(int displayHour, int displayMinute, int displaySecond) { // Funcion hecha por ChatGPT para ayudar con el mutiplexing de los sigitos del display
  if (estadoActual == CONFIGURANDO_ALARMA) {
    displayHour = alarmHour;
    displayMinute = alarmMinute;
    displaySecond = 0;
  } else {
    displayHour = hour;
    displayMinute = minute;
    displaySecond = second;
  }
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