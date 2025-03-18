#include <NewPing.h>
#include <TaskScheduler.h>

#define sensor_izquierda 2
#define sensor_derecha 3

#define motor_izquierda_adelante 5
#define motor_izquierda_atras 6
#define motor_derecha_adelante 9
#define motor_derecha_atras 10

#define TRIG_PIN 11
#define ECHO_PIN 12
#define MAX_DISTANCIA 200  /// distancia máxima a medir en cm
#define MIN_DISTANCIA 0    /// distancia mínima confiable

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCIA);

int distancia_ultrasonico = 0;
int valor_sensor_izquierda = 1;
int valor_sensor_derecha = 1;

/// la funciones para tareas
void leer_ultrasonico();
void leer_infrarrojos();
void controlar_robot();

/// programador de tareas
Scheduler runner;

/// tarea para leer el sensor ultrasónico cada 10 ms
Task taskLeerUltrasonico(10, TASK_FOREVER, &leer_ultrasonico); /// forever para que se mantenga
/// Tarea para leer los sensores infrarrojos cada 100 ms
Task taskLeerInfrarrojos(100, TASK_FOREVER, &leer_infrarrojos); /// ""

void setup() {
  Serial.begin(9600);
  
  pinMode(sensor_izquierda, INPUT_PULLUP);
  pinMode(sensor_derecha, INPUT_PULLUP);
  
  /// agregar tareas al programador
  runner.init();
  runner.addTask(taskLeerUltrasonico);
  runner.addTask(taskLeerInfrarrojos);
  
  /// iniciar las tareas
  taskLeerUltrasonico.enable();
  taskLeerInfrarrojos.enable();
  
  delay(1000);  ////retraso del reglamento
}

void loop() {
  runner.execute();  ///ejecutar las tareas

  if (detectar_bordes() == 1) {  ///empuja al oponente sin salir del doyo
      ir_para_atras();}
  else{
      if (distancia_ultrasonico <= 50) {  /// si se detecta el oponente
    empujar_oponente();}
      else {
        buscar_oponente();}
      }
}

/// todas las funciones
void leer_ultrasonico() {
  int cm = sonar.ping_cm();  /// 0btener la distancia en centímetros
  if (cm == 0 || cm < MIN_DISTANCIA) {
    distancia_ultrasonico = MAX_DISTANCIA;  /// como aveces da 0, no devolverlo
  } else {
    distancia_ultrasonico = cm;
  }
  Serial.print("Ultrasonico: ");
  Serial.print(distancia_ultrasonico);
  Serial.println(" cm");
}

void leer_infrarrojos() {
  valor_sensor_izquierda = digitalRead(sensor_izquierda);
  valor_sensor_derecha = digitalRead(sensor_derecha);
}

int detectar_bordes() {
  if (valor_sensor_izquierda == 0 || valor_sensor_derecha == 0) {
    return 1;  /// si algún sensor detectó el borde
  } else {
    return 0;  //// ningún sensor detecta el borde
  }
}

void buscar_oponente() {
    analogWrite(motor_izquierda_adelante, 150);
    analogWrite(motor_derecha_adelante, 0);
    analogWrite(motor_izquierda_atras, 0);
    analogWrite(motor_derecha_atras, 150);
}

void empujar_oponente() {
    analogWrite(motor_izquierda_adelante, 255);
    analogWrite(motor_derecha_adelante, 255);
    analogWrite(motor_izquierda_atras, 0);
    analogWrite(motor_derecha_atras, 0);
}

void ir_para_atras() {
    analogWrite(motor_izquierda_adelante, 0);
    analogWrite(motor_derecha_adelante, 0);
    analogWrite(motor_izquierda_atras, 255);
    analogWrite(motor_derecha_atras, 255);
    delay(500);
    analogWrite(motor_izquierda_adelante, 0);
    analogWrite(motor_derecha_adelante, 0);
    analogWrite(motor_izquierda_atras, 0);
    analogWrite(motor_derecha_atras, 0);
}
