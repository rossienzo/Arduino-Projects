#include <NewPing.h>

#define mic 8
#define rele 2

/* Tempo Microfone */
#define tempoMaximoDeUmaPalma  150 //milisegundos
#define tempoMaximoEntrePalmas 500 //milisegundos

/* Ultrassônico */
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
unsigned int pingSpeed = 100;
unsigned long pingTimer;

int contaPalmas = 0;
long tempoEspera = 0;
long tempoEsperaEntrePalmas = 0;
boolean releLigado = false;

boolean executaAcaoPalmas(boolean releLigado);
boolean ultra(boolean releLigado);
boolean releAtivado(boolean releLigado);

void setup() {
  pinMode(mic, INPUT);
  pinMode(rele, OUTPUT); // pino que será acionado pelo arduino

  /* Sensor Ultra */
  pingTimer = millis();
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(rele, HIGH); // deixa o rele desativado quando o arduino ligar
  
  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.s
}

void loop() {

  if (millis() >= pingTimer) {
    pingTimer += pingSpeed;
    sonar.ping_timer(echoCheck);
  }
  
  /* Liga ou desliga a luz com o sensor ultrassônico*/
  releLigado = ultra(releLigado);
 
  int sensorSom = digitalRead(mic); //leitura no pino do microfone  
 
  //se o sensor detectou palmas
  if (sensorSom == LOW) {
     //espera um tempo para nao detectar a mesma palma mais de uma vez 
     if (tempoEspera == 0) {
        tempoEspera = tempoEsperaEntrePalmas = millis(); 
        contaPalmas++;
     } else if ((millis() - tempoEspera) >= tempoMaximoDeUmaPalma) {
        tempoEspera = 0;
     }
  }

  //caso exceda o tempo maximo entre palmas, zera o contador de palmas
  if ( (contaPalmas != 0) && ((millis() - tempoEsperaEntrePalmas) > 500) ) {
     
     if(contaPalmas == 2) {
       releLigado = releAtivado(releLigado);
     }
     
     contaPalmas = 0;
     tempoEsperaEntrePalmas = millis();
     delay(50);
  }
}

void echoCheck() {
  if (sonar.check_timer()) {
    Serial.print("Ping: ");
    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Send ping, get distance in cm and print result (0 = outside set distance range)
    Serial.println("cm");
  }
}
boolean ultra(boolean releLigado) {
  if(sonar.check_timer() && sonar.ping_result / US_ROUNDTRIP_CM <= 20) {
    releLigado = releAtivado(releLigado);
    delay(1000);
  }
  return releLigado;
}

boolean releAtivado(boolean releLigado) {
  if (releLigado) {
    digitalWrite(rele, HIGH);
    releLigado = false;
  } else {
    digitalWrite(rele, LOW);
    releLigado = true;
  }

  return releLigado;
}
