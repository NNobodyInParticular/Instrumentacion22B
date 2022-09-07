/*-------------------------------------------------------------------------------
  Practica numero 01 - Registrador
  Desc: (Codigo de Prueva 01)

  Atzin Inyan Reyes Gutiérrez.

  Motor a Pasos:
  Este Motor a Pasos es incapas de hacer movimientos menores a 10 ms
  B1  - D9
  B2  - D10
  B3  - D11
  B4  - D12

  Servo Elevador  - D8
  Servo Pluma     - D7
-------------------------------------------------------------------------------*/

// Seccion 1 - Definicion de variables y objetos --------------------------------

// Librerias
#include <Servo.h>


// Entradas del Motor a Pasos
#define IN1         9
#define IN2         10
#define IN3         11
#define IN4         12
#define demora      10

// Definicion de Servos
#define INServoPluma  7   // Entrada del Servo Pluma
#define INServoEle    8   // Entrada del Servo Elevador

// Obejeto Servos
Servo servoPluma;         // Objeto Servo Pluma
Servo servoEle;           // Objeto Servo Elevador

// paso completo simple
int paso [4][4] =   // matriz (array bidimensional) con la secuencia de pasos
{
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1}
};

//-------------------------------------------------------------------------------

// Seccion 2 - Configuracion del arduino ----------------------------------------

void setup() {
  // Definicion de Salidas del Motor a Pasos
  pinMode(IN1, OUTPUT);   // todos los pines como salida
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Definicion Salida Servos
  servoPluma.attach(INServoPluma);
  servoEle.attach(INServoEle);
}

//-------------------------------------------------------------------------------

// Seccion 3 - Loob Infinito ----------------------------------------------------

void loop() {
  servoPluma.write(18);
  servoEle.write(0);
  delay(1000);

  // Primera Revolucion
  for (int i = 0, grados = 0; i < 512; i++) // 512*4 = 2048 pasos
  {
    paso_Simple();
    grados = map(i, 0, 511, 18, 54);
    servoPluma.write(grados);
  }

  // Segunda Revolucion
  for (int i = 0, grados = 0; i < 512; i++) // 512*4 = 2048 pasos
  {
    paso_Simple();
    grados = map(i, 0, 511, 54, 90);
    servoPluma.write(grados);
  }

  // Tercera Revolucion
  for (int i = 0, grados = 0; i < 512; i++) // 512*4 = 2048 pasos
  {
    paso_Simple();
    grados = map(i, 0, 511, 90, 126);
    servoPluma.write(grados);
  }

  // Cuarta Revolucion
  for (int i = 0, grados = 0; i < 512; i++) // 512*4 = 2048 pasos
  {
    paso_Simple();
    grados = map(i, 0, 511, 126, 162);
    servoPluma.write(grados);
  }

  digitalWrite(IN1, LOW); // detiene por 5 seg.
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(5000);

  for(int i = 0; i < 181; i++){
    servoEle.write(i);
    delay(20);
  }
  for(int i = 180; i >= 0; i--){
    servoEle.write(i);
    delay(20);
  }
}

//-------------------------------------------------------------------------------

// Seccion 4 - Funciones --------------------------------------------------------
  
  void paso_Simple(){
    int j;
    for (j = 0; j < 4; j++)             // bucle recorre la matriz de a una fila por vez
    {                                   // para obtener los valores logicos a aplicar
      digitalWrite(IN1, paso[j][0]);    // a IN1, IN2, IN3 e IN4
      digitalWrite(IN2, paso[j][1]);
      digitalWrite(IN3, paso[j][2]);
      digitalWrite(IN4, paso[j][3]);
      delay(demora);
    }
  }
  
//-------------------------------------------------------------------------------
