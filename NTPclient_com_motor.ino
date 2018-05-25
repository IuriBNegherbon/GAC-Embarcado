
#include <NTPClient.h>//Biblioteca do NTP.
#include <WiFiUdp.h>//Biblioteca do UDP.
#include <WiFi.h>//Biblioteca do WiFi.

const char* ssid     = ""; //aqui vai o nome da rede na qual vc esta conectando o ESP32
const char* password = ""; //aqui vai a senha da rede

char horaInformada = "";

int IN1 = 18;
int IN2 = 19;
//int IN3 = 10;
//int IN4 = 11;
 
WiFiUDP udp;//Cria um objeto "UDP".
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.
 
//#define led 2//Define o LED ao pino D4.
 
String hora;//Váriavel que armazenara o horario do NTP.
 
void setup()
{
    //define os pinos como saída
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    //pinMode(IN3, OUTPUT);
    //pinMode(IN4, OUTPUT);
    
   Serial.begin(9600);//Inicia a comunicação serial.
 
   //pinMode(led, OUTPUT);//Define o pino como saida.
   //digitalWrite(led, 1);//Apaga o LED.
 
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);//Conecta ao WiFi.
   while (WiFi.status() != WL_CONNECTED) 
   {
        delay(500);
        Serial.print(".");
   }
   delay(2000);//Espera a conexão.
 
   ntp.begin();//Inicia o NTP.
   ntp.forceUpdate();//Força o Update.
}
 
void loop()
{
   hora = ntp.getFormattedTime();//Armazena na váriavel HORA, o horario atual.
   Serial.println(hora);//Printa a hora já formatada no monitor.

  if(Serial.available() > 0)
  {
    horaInformada = Serial.read();
     if (hora == horaInformada)//Se a hora atual for igual à que foi informada, irá ligar o motor.
     {
  
        //gira o motor A
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        delay(5000);
        //desliga o motor A
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
     }
  }
 
   delay(1000);//Espera 1 segundo.
}

//O codigo a seguir é do sensor com motor, mas ja tem outro sketch com esse codigo funcionando.
/*
int pinosensor = 3;
int pino_led_cheio = 4;
int pino_led_vazio = 5;

int velocidade = 2;
int IN1 = 6;
int IN2 = 7;

void setup()
{
  Serial.begin(9600);
  pinMode(pinosensor, INPUT);
  pinMode(pino_led_cheio, OUTPUT);
  pinMode(pino_led_vazio, OUTPUT);
  
  pinMode(velocidade, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop()
{
  int estado = digitalRead(pinosensor);
  Serial.print("Estado sensor : ");
  Serial.println(estado);
  switch(estado)
  {
  case 0: //pote cheio, registro fechado
    digitalWrite(pino_led_vazio, LOW);
    digitalWrite(pino_led_cheio, HIGH);
    break;
  case 1: // pote acabando a agua
    digitalWrite(pino_led_vazio, HIGH);
    digitalWrite(pino_led_cheio, LOW);
    
    analogWrite(velocidade, 500); 
    digitalWrite(IN1, HIGH);     //liga o motor pra girar o registro
    digitalWrite(IN2, LOW);
    delay(3000);
  digitalWrite(IN1, HIGH);     //para o motor
    digitalWrite(IN2, HIGH);
    
    while (estado == 1)
    {
      estado = digitalRead(pinosensor); //verifica o nível do pote 
    }
    
    digitalWrite(pino_led_vazio, LOW);
    digitalWrite(pino_led_cheio, HIGH);  
    analogWrite(velocidade, 500);
    digitalWrite(IN1, LOW);     //liga o motor no sentido inverso pra fechar o reservatorio
    digitalWrite(IN2, HIGH);
    delay(3000);
      
    digitalWrite(IN1, HIGH);     //para o motor e volta pro estado de cheio
    digitalWrite(IN2, HIGH);
    
    break;
  }
  delay(100);
}
*/
