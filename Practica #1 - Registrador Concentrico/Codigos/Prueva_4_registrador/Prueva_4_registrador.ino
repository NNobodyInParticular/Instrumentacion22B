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

  MPU6050
  A4  - SDA
  A5  - SCL
  
-------------------------------------------------------------------------------*/

// Seccion 1 - Definicion de variables y objetos --------------------------------

// Librerias
#include <Servo.h>

// Librerias del Giroscopio
#include "Simple_MPU6050.h"         // incluye libreria Simple_MPU6050
#define MPU6050_ADDRESS_AD0_LOW     0x68      // direccion I2C con AD0 en LOW o sin conexion
#define MPU6050_ADDRESS_AD0_HIGH    0x69      // direccion I2C con AD0 en HIGH
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_LOW // por defecto AD0 en LOW

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

// Objeto Giroscopio y Configuraciones
Simple_MPU6050 mpu;
//#define OFFSETS  -1306,    2996,    1044,      85,     -32,       0  // Colocar valores personalizados
#define spamtimer(t) for (static uint32_t SpamTimer; (uint32_t)(millis() - SpamTimer) >= (t); SpamTimer = millis())
// spamtimer funcion para generar demora al escribir en monitor serie sin usar delay()

// paso completo simple
int desfase = 0;          // desface de la entrada de datos analogica

int paso [4][4] =   // matriz (array bidimensional) con la secuencia de pasos
{
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1}
};

int medRoll = 0;

// Funcion Giroscopio
void mostrar_valores (int16_t *gyro, int16_t *accel, int32_t *quat, uint32_t *timestamp) {  
  uint8_t SpamDelay = 100;      // demora para escribir en monitor serie de 100 mseg
  Quaternion q;         // variable necesaria para calculos posteriores
  VectorFloat gravity;        // variable necesaria para calculos posteriores
  float ypr[3] = { 0, 0, 0 };     // array para almacenar valores de yaw, pitch, roll
  float xyz[3] = { 0, 0, 0 };       // array para almacenar valores convertidos a grados de yaw, pitch, roll
  spamtimer(SpamDelay) {      // si han transcurrido al menos 100 mseg entonces proceder
    mpu.GetQuaternion(&q, quat);    // funcion para obtener valor para calculo posterior
    mpu.GetGravity(&gravity, &q);   // funcion para obtener valor para calculo posterior
    mpu.GetYawPitchRoll(ypr, &q, &gravity); // funcion obtiene valores de yaw, ptich, roll
    mpu.ConvertToDegrees(ypr, xyz);   // funcion convierte a grados sexagesimales

    medRoll = int(xyz[2]);
  }
}

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

  // Inicio de lo relacionado a la Comunicacion I2C
  uint8_t val;
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE  // activacion de bus I2C a 400 Khz
    Wire.begin();
    Wire.setClock(400000);
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  // Configuraciones del Giroscopio
  #ifdef OFFSETS                // si existen OFFSETS
  Serial.println(F("Usando Offsets predefinidos"));     // texto estatico
  mpu.SetAddress(MPU6050_ADDRESS_AD0_LOW).load_DMP_Image(OFFSETS);  // inicializacion de sensor
  
  #else                   // sin no existen OFFSETS
    Serial.println(F(" No se establecieron Offsets, haremos unos nuevos.\n" // muestra texto estatico
                     " Colocar el sensor en un superficie plana y esperar unos segundos\n"
                     " Colocar los nuevos Offsets en #define OFFSETS\n"
                     " para saltar la calibracion inicial \n"
                     " \t\tPresionar cualquier tecla y ENTER"));
    //while (Serial.available() && Serial.read());    // lectura de monitor serie
    //while (!Serial.available());        // si no hay espera              
    //while (Serial.available() && Serial.read());    // lecyura de monitor serie
    mpu.SetAddress(MPU6050_ADDRESS_AD0_LOW).CalibrateMPU().load_DMP_Image();  // inicializacion de sensor
  #endif

  mpu.on_FIFO(mostrar_valores);   // llamado a funcion mostrar_valores si memoria FIFO tiene valores
}

//-------------------------------------------------------------------------------

// Seccion 3 - Loob Infinito ----------------------------------------------------

void loop() {
  servoPluma.write(18);
  servoEle.write(0);
  delay(1000);

  // Primera Revolucion
  revo1();

  // Segunda Revolucion
  revo2();

  // Tercera Revolucion
  revo3();

  // Cuarta Revolucion
  revo4();

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

  // Paso_Simple
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

  // Revolucion 1
  void revo1(){
    for (int i = 0, grados = 0; i < 512; i++)
    {
      paso_Simple();
      mpu.dmp_read_fifo();
      desfase = map(medRoll, -100, 100, -18, 18);
      grados = map(i, 0, 511, 18, 54) + desfase;
      servoPluma.write(grados);
    }
  }

  // Revolucion 2
  void revo2(){
    for (int i = 0, grados = 0; i < 512; i++) // 512*4 = 2048 pasos
    {
      paso_Simple();
      mpu.dmp_read_fifo();
      desfase = map(medRoll, -90, 90, -18, 18);
      grados = map(i, 0, 511, 54, 90) + desfase;
      servoPluma.write(grados);
    }
  }

  // Revolucion 3
  void revo3(){
    for (int i = 0, grados = 0; i < 512; i++) // 512*4 = 2048 pasos
    {
      paso_Simple();
      mpu.dmp_read_fifo();
      desfase = map(medRoll, -90, 90, -18, 18);
      grados = map(i, 0, 511, 90, 126) + desfase;
      servoPluma.write(grados);
    }
  }

  // Revolucion 4
  void revo4(){
    for (int i = 0, grados = 0; i < 512; i++) // 512*4 = 2048 pasos
    {
      paso_Simple();
      mpu.dmp_read_fifo();
      desfase = map(medRoll, -90, 90, -18, 18);
      grados = map(i, 0, 511, 126, 162) + desfase;
      servoPluma.write(grados);
    }
  }
  
//-------------------------------------------------------------------------------
