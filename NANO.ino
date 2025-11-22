
#include <SoftwareSerial.h> //librería para bluetooth
#include <Wire.h> //libreria para acelerómetro
#include <MPU6050.h> //libreria para acelerómetro

SoftwareSerial BT(10, 11); //definimos bluetooth, 10->TX
MPU6050 mpu(0x69); //definimos acelerómetro, dirección x69 con ADO en +

//definimos variables de posición de 16 bits para cada eje con el acelerómetro
int16_t posicionX, posicionY, posicionZ;

//definimos los ángulos a enviar para cada servo
int anguloX = 0;
int anguloY = 0;
int anguloZ = 0;
int ultimoanguloX = 0;
int ultimoanguloY = 0;
int ultimoanguloZ = 0;

//definimos gancho
int segundosguardados = 0;
int angulogancho = 0;
int ultimoangulogancho = 0;
int segundos = 0; //tiempo manteniendo el botón de gancho
bool ganchoabierto = true;

//definimos botones
int botonfijo = 5;
int botongancho = 6;
int ultimoestadofijo = LOW;
int ultimoestadogancho = LOW;

bool bloqueo = false; //modo de bloqueo desactivado


void setup() {
  //inicializamos
  Serial.begin(9600);
  BT.begin(9600);
  Wire.begin();

  mpu.initialize();

  pinMode(botonfijo, INPUT_PULLUP);
  pinMode(botongancho, INPUT_PULLUP);
}


void loop() {
  int estadofijo = digitalRead(botonfijo); //leemos el botón de bloqueo
  int estadogancho = digitalRead(botongancho); //leemos el botón de gancho

  //hacemos el botón de gancho
  if (estadogancho == HIGH) { //empezamos a contar el tiempo una vez se apreta el bot´pn
    segundos = segundos + 1; //mientras mas tiempo se mantiene apretado el botón, más aumenta el tiempo
    segundosguardados = segundos; //asignamos el último valor al ángulo del gancho
  } else {
    segundos = 0; //una vez que se deja de apretar el botón, el contador se reinicia
  }
  angulogancho = segundosguardados;

  if ((segundosguardados < 5) && (ganchoabierto == true)) { //si se apretó rápidamente el botón
    BT.print("G:");
    BT.println(angulogancho); //le mandamos al uno el ángulo de cierre del gancho
    Serial.println("Gancho cerrado");
    ganchoabierto = false;
    Serial.println(angulogancho);
  } else {
    if ((abs(angulogancho - ultimoangulogancho) > 15) && (segundosguardados > 5)) {
      BT.print("G:");
      BT.println(angulogancho);
      Serial.print("Enviado G: ");
      Serial.println(angulogancho);
      ultimoangulogancho = angulogancho;
      ganchoabierto = true;
    }
  }

  //ahora hacemos el botón de bloqueo
  if ((estadofijo == HIGH) && (ultimoestadofijo == LOW)) { //si el botón es apretado y no fue apretado previamente
    bloqueo = !bloqueo; //invertimos el valor del bloqueo, si antes era true, ahora es false, y visceversa
  }
  ultimoestadofijo = estadofijo; //guardamos el último estado del botón para el próximo bucle

  if (bloqueo == true) { //si se activó el modo de bloqueo
    Serial.println("MODO DE BLOQUEO ACTIVADO");
  } else {
    movimientolibre(); //llamamos la función que lee y envía los datos del acelerómetro
  }
}


void movimientolibre() {
  mpu.getAcceleration(&posicionX, &posicionY, &posicionZ); //leemos los datos del acelerómetro

  //definimos ejes
  anguloX = (constrain(map(posicionY, 15000, -15000, 0, 180), 0, 180));
  anguloZ = (constrain(map(posicionZ, 12000, -10000, 0, 180), 0, 180));
  anguloY = constrain(90 + atan2(posicionZ, posicionX) * 180.0 / M_PI, 0, 180);
  anguloY = atan2(posicionZ, posicionX) * 180.0 / M_PI, 0, 180; //relación trigonométrica

  if (anguloY > 0) {
    anguloY = constrain(90 + anguloY, 0, 180);
  } else {
    anguloY = constrain(90 + anguloY, 0, 180);
  }

  //eje X
  if (abs(anguloX - ultimoanguloX) > 15) { //hacemos una diferencia para que el nano solo mande datos si cambia el valor del ángulo, así el uno no recibe valores constantes
    BT.print("X:"); //el uno va a recibir 2 carácteres 'X' + ':'
    BT.println(anguloX); //el uno recibe 'X:' + 'ÁNGULO'
    Serial.print("Enviado X: ");
    Serial.println(anguloX);
    ultimoanguloX = anguloX; //hacemos que el ángulo enviado sea el último para la diferencia en el if
  }

  //eje Y
  if (abs(anguloY - ultimoanguloY) > 15) {
    BT.print("Y:");
    BT.println(anguloY);
    Serial.print("Enviado Y: ");
    Serial.println(anguloY);
    ultimoanguloY = anguloY;
  }

  //eje Z
  if (abs(anguloZ - ultimoanguloZ) > 15) {
    BT.print("Z:");
    BT.println(anguloZ);
    Serial.print("Enviado Z: ");
    Serial.println(anguloZ);
    ultimoanguloZ = anguloZ;
  }
}
