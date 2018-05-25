
#include <NTPClient.h>//Biblioteca do NTP.
#include <WiFiUdp.h>//Biblioteca do UDP.
#include <WiFi.h>//Biblioteca do WiFi.
 
WiFiUDP udp;//Cria um objeto "UDP".
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.
 
#define led 2//Define o LED ao pino D4.
 
String hora;//Váriavel que armazenara o horario do NTP.
 
void setup()
{
   Serial.begin(9600);//Inicia a comunicação serial.
 
   pinMode(led, OUTPUT);//Define o pino como saida.
   digitalWrite(led, 1);//Apaga o LED.
 
   WiFi.mode(WIFI_STA);
   WiFi.begin("Sexta-feira", "0987654321");//Conecta ao WiFi.
   delay(2000);//Espera a conexão.
 
   ntp.begin();//Inicia o NTP.
   ntp.forceUpdate();//Força o Update.
}
 
void loop()
{
   hora = ntp.getFormattedTime();//Armazena na váriavel HORA, o horario atual.
   Serial.println(hora);//Printa a hora já formatada no monitor.
 
   if (hora == "19:23:30")//Se a hora atual for igual à que definimos, irá acender o led.
   {
      digitalWrite(led, 0);//Acende
   }
 
   delay(1000);//Espera 1 segundo.
}

