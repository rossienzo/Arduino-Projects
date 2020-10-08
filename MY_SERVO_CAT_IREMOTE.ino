#include <Wire.h> //INCLUSÃO DA BIBLIOTECA
#include <Servo.h>
#include <IRremote.h> //Biblioteca do controle infravermelho
#include <RTClib.h> //INCLUSÃO DA BIBLIOTECA

//Definição de pinos
#define pinSensor 11 // infravermelho
#define servoPin 9
#define buttonPin 12

// INSTANCIANDO OBJETOS
Servo myservo;
IRrecv sensorIR (pinSensor);
decode_results comando;

//OBJETO DO TIPO RTC_DS3231
RTC_DS3231 rtc; 

//DECLARAÇÃO DOS DIAS DA SEMANA
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

const int HORA_PADRAO = 6; // padrão em hora para a refeição
int hourFood = -10; //hora que a comida irá cair
int minFood = 0; // minuto que a comida irá cair

// Dados da última refeição

int monthLastFood = 0;
int dayLastFood = 0;
int hourLastFood = 0;
int minLastFood = 0;
int secLastFood = 0;

void servo(int voltas = 3)
{
  // Por padrão irá dar 3 voltas
  /* Desloca o servo */
  DateTime now = rtc.now(); //CHAMADA DE FUNÇÃO
  
  for(int i = 0 ; i < voltas; i++)
  {
    myservo.attach(servoPin);
    myservo.write(30);
    delay(455); //aqui
    myservo.write(120);
    delay(455); //aqui
    myservo.detach();

    if(voltas == 3) // se der 3 voltas irá calcular a próxima refeição
    {
      hourFood = now.hour() + HORA_PADRAO;
      minFood = now.minute();  
    }
  }
}

void btnPress()
{
  DateTime now = rtc.now(); //CHAMADA DE FUNÇÃO
  int buttonVal = digitalRead(buttonPin);
  
    if(buttonVal == LOW) {
      
      // salva os dados da última refeição
      monthLastFood = now.month();
      dayLastFood = now.day();
      // -----
      hourLastFood = now.hour();
      minLastFood = now.minute();
      secLastFood = now.second();
      
      // ejeta a comida
      servo();
      
      // define o horário da próxima refeição
      // hourFood = now.hour() + HORA_PADRAO;
      // minFood = now.minute();      
    }
    delay(13);
}
  
void setup(){
  myservo.attach(servoPin);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  myservo.write(180);
  delay(1000);
  myservo.detach();

  Serial.begin(9600); //INICIALIZA A SERIAL
  
  // HABILITANDO SENSOR IR
  sensorIR.enableIRIn();
  Serial.println("Sensor IR Habilitado");
  
  if(! rtc.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ
    Serial.println("DS3231 não encontrado"); //IMPRIME O TEXTO NO MONITOR SERIAL
    while(1); //SEMPRE ENTRE NO LOOP
  }
  if(rtc.lostPower()){ //SE RTC FOI LIGADO PELA PRIMEIRA VEZ / FICOU SEM ENERGIA / ESGOTOU A BATERIA, FAZ
    Serial.println("DS3231 OK!"); //IMPRIME O TEXTO NO MONITOR SERIAL
    //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
    //rtc.adjust(DateTime(2020, 10, 07, 16, 31, 02)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
  }
  delay(100); //INTERVALO DE 100 MILISSEGUNDOS
}
 
void loop () {
  DateTime now = rtc.now(); //CHAMADA DE FUNÇÃO
  Serial.print("Data: "); //IMPRIME O TEXTO NO MONITOR SERIAL
  Serial.print(now.day(), DEC); //IMPRIME NO MONITOR SERIAL O DIA
  Serial.print('/'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.print(now.month(), DEC); //IMPRIME NO MONITOR SERIAL O MÊS
  Serial.print('/'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.print(now.year(), DEC); //IMPRIME NO MONITOR SERIAL O ANO
  Serial.print(" / Dia: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]); //IMPRIME NO MONITOR SERIAL O DIA
  Serial.print(" / Horas: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(now.hour(), DEC); //IMPRIME NO MONITOR SERIAL A HORA
  Serial.print(':'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.print(now.minute(), DEC); //IMPRIME NO MONITOR SERIAL OS MINUTOS
  Serial.print(':'); //IMPRIME O CARACTERE NO MONITOR SERIAL
  Serial.print(now.second(), DEC); //IMPRIME NO MONITOR SERIAL OS SEGUNDOS
  Serial.println(); //QUEBRA DE LINHA NA SERIAL
  Serial.print(hourFood);
  Serial.print('@');
  Serial.print(minFood);
  Serial.println(); //QUEBRA DE LINHA NA SERIAL
  delay(1000); //INTERVALO DE 1 SEGUNDO

 /*
 * configura o horário da primeira refeição
 * BEGIN IF
 */
  if (hourFood == - 10)
  {
    if (now.hour() == 12)
    {
      hourFood = 12;
      minFood = 0;
    }
    
    if (now.hour() > 12) 
    {  
      if (now.hour() == 18) 
      {
        hourFood = 18;
        minFood = now.minute();
      }
      else if (now.hour() > 18)
      {
        hourFood = 18 + HORA_PADRAO;
        minFood = 0;
      }
      else 
      {
        hourFood = 12 + HORA_PADRAO;
        minFood = 0;
      }
    }

    if (now.hour() < 12) {
      hourFood = HORA_PADRAO;
      minFood = 0;
    } 
  } // END IF

 /*
 * Calcula o horário das próximas refeições
 */
  if (hourFood == 24)
  {
    hourFood = 0; 
  }
  else if (hourFood > 24) 
  {
    hourFood = hourFood - 24;   
  }
  else
  {

    if (now.hour() == hourFood && now.minute() == minFood)
    {
      // salva os dados da última refeição
      monthLastFood = now.month();
      dayLastFood = now.day();
      // -----
      hourLastFood = now.hour();
      minLastFood = now.minute();
      secLastFood = now.second();

      Serial.print("Entrou auqiffffffffff");
      // ejeta a comida
      servo();

      // define o horário da próxima refeição
      //hourFood = hourFood + HORA_PADRAO;
      //minFood = now.minute(); 
    }
  }

  // Se o botão for pressionado
  btnPress(); 

  // Se algum comando do controle for recebido
  if (sensorIR.decode(&comando)) 
  {
    Serial.println(comando.value, HEX);

    switch (comando.value) 
    {  
      case (0xFFA25D): // Botão 1
      {
          servo(1); // apenas 1 volta 
          break;
      }
      case (0xFF629D): // Botão 2
      {
          servo(2); // 2 voltas 
          break;
      }
      case (0xFFE21D): // Botão 3
      {
          servo();
          // Deu 3 voltas então irá definir o horário da próxima refeição  
          break;
      }
      case (0xFF38C7): // Botão 'OK'
      {
          servo();
          // Deu 3 voltas então irá definir o horário da próxima refeição  
          break;
      }
    }    
    sensorIR.resume();// Encerra e aguarda novo comando
  }
}
