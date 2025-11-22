#include <SoftwareSerial.h> //librería para bluetooth
#include <Servo.h> //librería para servos

SoftwareSerial BTSerial(2, 3); //definimos bluetooth, 2->TX

//definimos servos
Servo servoX; //servo de la base
Servo servoY; //servo de adelante/atrás
Servo servoZ; //servo de arriba/abajo
Servo servoG; //servo del gancho

//definimos angulos predeterminados
int anguloX = 90; //Tiene los 180 grados de libertad, empieza en el medio
int anguloY = 90; //calibración: 40 grados hacia delante, 40 grados hacia atrás
int anguloZ = 90; //calibración: 50 grados hacia arriba, 80 grados hacia abajo
int anguloG = 53; //calibración: 60 grados cerrado, se puede abrir hasta 90 grados

String mensaje = ""; //el mensaje completo recibido por bluetooth


void setup() {
  //inicializamos
  Serial.begin(9600);
  BTSerial.begin(9600);

  //inicializamos servos
  servoX.attach(5);
  servoY.attach(6);
  servoZ.attach(9);
  servoG.attach(10);

  servoX.write(anguloX);
  servoY.write(anguloY);
  servoZ.write(anguloZ);
  servoG.write(anguloG);
}


void loop() {
  //leemos los datos del bluetooth
  while (BTSerial.available()) {
    char letra = BTSerial.read(); //lee carácter por carácter lo recibido
    if (letra == '\n') { //si hubo un salto de linea -> se terminó de recibir el mensaje por completo
      moverservo(mensaje); //llamamos una función que use el mensaje completo
      mensaje = ""; //dejamos en blanco el mensaje para el próximo que se reciba
      while (BTSerial.available()) {
        BTSerial.read(); //se vacía el buffer
      }
    } else {
      mensaje = mensaje + letra; //vamos completando el mensaje carácter por carácter hasta llegar al salto de línea (\n)
    }
  }
}


void moverservo(String mensajedef) { //formato de mensaje esperado ej: "X:100"
  mensajedef.trim();

  bool caractervalido = true;
  for (int i = 0; i < mensajedef.length(); i++) {
    if (!isPrintable(mensajedef.charAt(i))) {
      caractervalido = false;
      break;
    }
  }

  bool numerovalido = true;
  for (int i = 2; i < mensajedef.length(); i++) {
    if (!isDigit(mensajedef.charAt(i))) {
      numerovalido = false;
      break;
    }
  }


  if ((!caractervalido) || (!numerovalido)) {
    return;
  }


  if ((mensajedef.length() >= 3 && mensajedef.length() < 6) && (mensajedef.charAt(1) == ':') && (mensajedef.charAt(0) != ':') && (caractervalido) && (numerovalido)) { //confirmamos que el mensaje enviado tenga más de 3 carácteres y que el segundo sea :
    char eje = mensajedef.charAt(0); //definimos qué eje se recibió analizando el primer carácter enviado
    String valorstring = mensajedef.substring(2); //valor del ángulo del mensaje, el ángulo en el que se moverá el servo del eje seleccionado
    int valor = constrain(valorstring.toInt(), 0, 180); //convertimos el valor de ángulo obtenido a enteros, valor número

    Serial.println(mensajedef);
    //según el eje obtenido, movemos el servo correspondiente
    switch (eje) {

      case 'X': //servo de la base
        anguloX = valor;
        servoX.write(anguloX);
        break;


      case 'Y': //servo del brazo de adelante/atrás
        anguloY = valor;
        anguloY = constrain(map(anguloY, 0, 180, 40, 110), 40, 160); //calibramos el servo según los ángulos que pueda tomar
        servoY.write(anguloY);
        break;


      case 'Z': //servo del brazo de altura
        anguloZ = valor;
        anguloZ = constrain(map(anguloZ, 0, 180, 40, 170), 40, 170);
        servoZ.write(anguloZ);
        break;


      case 'G': //servo del gancho
        anguloG = valor;
        anguloG = constrain(map(anguloG, 0, 180, 53, 150), 53, 150);
        servoG.write(anguloG);
        break;

      default:
        return;
      break;
    }
  } else {
    return;
  }
}
