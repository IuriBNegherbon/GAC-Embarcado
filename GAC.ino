#include <Wire.h>
#include "SSD1306Wire.h" 
#include <string>
#include <iostream>
#include <sstream>
#include <EEPROM.h>
#include <NTPClient.h>//Biblioteca do NTP.
#include <WiFiUdp.h>//Biblioteca do UDP.
#include <WiFi.h>//Biblioteca do WiFi.
#include <IOXhop_FirebaseESP32.h>
//#include "gac.h"
//#include "images.h"

//#define FIREBASE_HOST "gacapp-8b4eb.firebaseio.com" 
#define FIREBASE_HOST "gac-tcc.firebaseio.com"
#define FIREBASE_AUTH "eJsgHfmjEQo3yvHK9vUskFiiMdtcoSuXV1hWET9c"
#define WIFI_SSID "AndroidAP"
#define WIFI_PASSWORD "hqir9667"

WiFiUDP udp;//Cria um objeto "UDP".
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.
SSD1306Wire  display(0x3c, 21, 22);

int botaoSobe = 12;
int botaoDesce = 13;
int botaoOK = 14;
int botaoX = 27;
int botaoDireita = 26;
int botaoEsquerda = 25;
int aHorario[6][5] = {};
int CarregaArray;
int vbotaoDesce=0, vbotaoSobe=0, vbotaoOK = 0, vbotaoX=0, vbotaoDireita=0, vbotaoEsquerda=0;
int tempoPisca = 350;
long previousMillis;
String hora;//Váriavel que armazenara o horario do NTP.

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  pinMode(botaoSobe, INPUT);
  pinMode(botaoDesce, INPUT);
  pinMode(botaoOK, INPUT);
  pinMode(botaoDireita, INPUT);
  pinMode(botaoEsquerda, INPUT);
  pinMode(botaoX, INPUT);

  Serial.println("WIFI");
  /*display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);*/
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);//Conecta ao WiFi.
  delay(500);//Espera a conexão.
  while (WiFi.waitForConnectResult() != WL_CONNECTED) //Verifica se conectou, se não tenta de novo
  {
    Serial.println("Conexao falhou! Reiniciando..."); 
    //display.clear();
    //display.drawString(10, 15, "Conexao falhou! Reiniciando...");
    delay(500);
    ESP.restart();
  }
  ntp.begin();//Inicia o NTP.
  ntp.forceUpdate();//Força o Update.
  Serial.println("Conectou"); 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println("Passou firebase"); 
  
  EEPROM.begin(255);
  for (CarregaArray=0;CarregaArray<6;CarregaArray++) //Carrega as informações salvas na EEPROM para o awwat aHorario
  {
    Serial.println("EEPROM");
    aHorario[CarregaArray][0] = EEPROM.read((CarregaArray*5));          //Hora
    aHorario[CarregaArray][1] = EEPROM.read((CarregaArray*5)+1);        //Minuto
    aHorario[CarregaArray][2] = (EEPROM.read((CarregaArray*5)+2))*10;   //Ração
    aHorario[CarregaArray][3] = EEPROM.read((CarregaArray*5)+3);        //On/Off
    aHorario[CarregaArray][4] = EEPROM.read((CarregaArray*5)+4);        //Id
  }
  EEPROM.end();
  // Initialising the UI will init the display too.
  Serial.println("Antes Firebase");
  //Serial.println(Firebase.getInt("/usuario/gac/user_data_ativo"));
  Serial.println(Firebase.getInt("user_data_ativo"));
  Serial.println("Depois Firebase");
  display.init();
  Bd();
  display.flipScreenVertically();
}

void loop()
{
  //display.clear();
  // draw the current demo method
  //display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  //display.drawXbm(34, 14, 60, 36, WiFi_Logo_bits);
  //display.display();
  //display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  //delay(2000);
  Menu();
  //MovimentaMenu(45);

}

void Bd() {
  //EEPROM.begin(255);
  String var;
  Serial.println("Entrou BD");
  for (CarregaArray=0;CarregaArray<6;CarregaArray++) //Carrega as informações salvas na EEPROM para o awwat aHorario
  {
    var = String("/09319782970/user_data/") + String(CarregaArray);
    //Serial.println(CarregaArray);
    //Serial.println(Firebase.getInt(var + "/user_data_racao"));
    //Serial.println(Firebase.getInt(var + "/user_data_racao") > 0);
    if (Firebase.getInt(var + "/user_data_racao") > 0 ) {   
      aHorario[CarregaArray][0] = Firebase.getInt(var + "/user_data_hora");
      aHorario[CarregaArray][1] = Firebase.getInt(var + "/user_data_minuto");
      aHorario[CarregaArray][2] = Firebase.getInt(var + "/user_data_racao");
      aHorario[CarregaArray][3] = Firebase.getInt(var + "/user_data_ativo");
      aHorario[CarregaArray][4] = CarregaArray;
    }
    else {
      aHorario[CarregaArray][0] = 255;
      aHorario[CarregaArray][1] = 255;
      aHorario[CarregaArray][2] = 255;
      aHorario[CarregaArray][3] = 255;
      aHorario[CarregaArray][4] = 255;
    }
    EEPROM.begin(255);
    EEPROM.write(CarregaArray*5, aHorario[CarregaArray][0]);
    EEPROM.write((CarregaArray*5)+1, aHorario[CarregaArray][1]);
    EEPROM.write((CarregaArray*5)+2, aHorario[CarregaArray][2]/10); //Ração deve ser dividido por 10 por limitações da EEPROM
    EEPROM.write((CarregaArray*5)+3, aHorario[CarregaArray][3]);
    EEPROM.write((CarregaArray*5)+4, aHorario[CarregaArray][4]);
    EEPROM.end();
  }
  //EEPROM.end();
}

void Menu() //Cria tela do Menu
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, "MENU");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  Serial.println("Passou Menu");
  display.drawString(10, 15, "Adicionar horarios");
  display.drawString(10, 25, "Horarios");
  display.display();
  SMovimentaMe();
  /*vbotaoSobe = digitalRead(botaoSobe);
  vbotaoDesce = digitalRead(botaoDesce);
  vbotaoOK = digitalRead(botaoOK);
  vbotaoDireita = digitalRead(botaoDireita);
  vbotaoX = digitalRead(botaoX);
  vbotaoEsquerda = digitalRead(botaoEsquerda);
  Serial.print("OK: ");
  Serial.println(vbotaoOK);
  Serial.print("Desce: ");
  Serial.println(vbotaoDesce);
  Serial.print("Sobe: ");
  Serial.println(vbotaoSobe);
  Serial.print("Direita: ");
  Serial.println(vbotaoDireita);
  Serial.print("Esquerda: ");
  Serial.println(vbotaoEsquerda);
  Serial.print("Cancel: ");
  Serial.println(vbotaoX);*/
}

void SMovimentaMe() //movienta a seta do menu
{
  int coluna = 0, linha = 15;//, vbotaoDesce = 0, vbotaoSobe=0, vbotaoOK = 0;
  bool cancel = false;
  String situacao = "apagado";
  while (true) //Não sai da função até que seja informado
  {
    if (cancel) // Se cancel for true, sai do while principal
    {
      break;
    }
    situacao = VerificaTempo(coluna, linha, "->",situacao); //VerificaTempo é a função para fazer piscar as informações. 3° parametro é a string que piscará e o 4° indica se a string esta apagada ou escrita no display
    //Serial.println(situacao);
    EstadoBotao(); //Pega o estado dos botões
    VerificaHora(); //Verifica Hora atual e, se for igual a alguma das horas cadastradas alimenta cachorro
    while (vbotaoDesce == LOW && vbotaoSobe == LOW && vbotaoOK == LOW)
    {
      situacao = VerificaTempo(coluna, linha, "->",situacao); //VerificaTempo é a função para fazer piscar as informações. 3° parametro é a string que piscará e o 4° indica se a string esta apagada ou escrita no display
      EstadoBotao(); //Pega o estado dos botões
      VerificaHora(); //Verifica Hora atual e, se for igual a alguma das horas cadastradas alimenta cachorro
      if (vbotaoDesce == HIGH)
      {
        if (linha<25) // DESCE
        {
          limpaLed(0,linha,"->"); //limpa do display a variavel que esta piscando
          linha += 10;
          break;
        }
      }
      if (vbotaoSobe == HIGH)
      {
        if (linha>15) //SOBE
        {
          limpaLed(0,linha,"->");
          linha -= 10;
          break;
        }
      }
      if (vbotaoOK == HIGH) // OK
      {
        if (linha == 15)
        {
          Adicionar(true, "00", "00", "000", "Adicionar", 99); //Chama função para adicionar horario
        }
        else if (linha == 25)
        {
          Horarios(); //Chama função que lista os horarios cadastrados
        }
        cancel = true; //Sai do while para dar um "refresh"
        break;
      }
    }
  }
}

void Adicionar(bool chamaMov, String horarioStr, String MinutoStr, String racaoStr, String Tela, int i) //Adicionar e Alterar. i é o Id do horario quando for alterar
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, Tela);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(10, 15, "Horario:");
  display.drawString(60, 15, horarioStr);
  display.drawString(72, 15, ":");
  display.drawString(84, 15, MinutoStr);
  display.drawString(10, 25, "Qtd Ração:");
  display.drawString(75, 25, racaoStr);
  display.display();
  if (chamaMov) //indica se chamará a tela para movimentar a seta ou é apenas para recarregar a pagina
  {
    MovimentAdd(Tela, i);
    //Serial.println("Saiu MovimentaAdd");
  }
}

void MovimentAdd(String Tela, int i)
{
  int coluna = 0, linha = 15, horario=0, minuto=0, racao=0, H = 0,f;
  String horarioStr = "00", minutoStr = "00", racaoStr = "000",caracter="->",situacao = "apagado";
  bool cancel = false;
  
  if (Tela == "Alterar") //Se for para alterar, pega informações do array aHorario na posição de i
  {
    horarioStr = AjustaHora(aHorario[i][0]);
    horario = aHorario[i][0];
    minutoStr = AjustaMinuto(aHorario[i][1]);
    minuto = aHorario[i][1];
    racaoStr = AjustaRacao(aHorario[i][2]);
    racao = aHorario[i][2];
  }
 
  display.setFont(ArialMT_Plain_10);
  while (true)
  {
    if (cancel)
    {
      break;
    }
    situacao = VerificaTempo(coluna, linha, caracter,situacao);
    EstadoBotao();
    while (vbotaoDesce == LOW && vbotaoSobe == LOW && vbotaoOK == LOW && vbotaoX == LOW && vbotaoDireita == LOW && vbotaoEsquerda == LOW)
    {
      situacao = VerificaTempo(coluna, linha, caracter,situacao);
      EstadoBotao();
      if (vbotaoDesce == HIGH)// DESCE
      {
        if (linha<=25)
        {
          if (coluna == 0 && linha < 25)
          {
            limpaLed(0,linha,"->");
            linha += 10;
            caracter = "->";
            break;
          }
          else if (coluna == 60 && linha == 15) // DESCE HORA
          {          
            //Altera as informações da hora
            limpaLed(coluna,linha,horarioStr);
            horario -= 1;
            if (horario < 0) //se hora for negativo, recebe maior horario existente
            {
              horario = 23;
            }
            horarioStr = AjustaHora(horario);
            caracter = horarioStr;
            escreveLed(coluna,linha,caracter);
            break;
          }
           else if (coluna == 84 && linha == 15) //DESCE MINUTO
          {   
            //Altera as informações do minuto
            limpaLed(coluna,linha,minutoStr);
            minuto -= 5;
            if (minuto < 0) //Se minuto for negativo, recebe maior minuto existente
            {       
              minuto = 55;
            }
            minutoStr = AjustaMinuto(minuto);
            caracter = minutoStr;
            escreveLed(coluna,linha,caracter);
            break;
          }
          else if (coluna == 75 && linha == 25) //DESCE RAÇÃO
          {
            //Altera as informações da ração
            limpaLed(coluna,linha,racaoStr);
            racao -= 50;
            if (racao < 0) //Se ração for negativo, recebe o maximo configurado
            {       
              racao = 500;
            }
            racaoStr = AjustaRacao(racao);
            caracter = racaoStr;
            escreveLed(coluna,linha,caracter);
            break;
          }
        }
      }
      
      if (vbotaoSobe == HIGH) //SOBE
      {
          if (coluna == 0 && linha > 15)
          {
            limpaLed(0,linha,"->");
            linha -= 10;
            caracter = "->";
            break;
          }
          else if (coluna == 60 && linha == 15) //SOBE HORA
          {   
            //Altera as informações da hora
            limpaLed(coluna,linha,horarioStr);
            horario += 1;
            if (horario > 23)
            {       
              horario = 0;
            }
            horarioStr = AjustaHora(horario);
            caracter = horarioStr;
            escreveLed(coluna,linha,caracter);
            break;
          }
          else if (coluna == 84 && linha == 15) //SOBE MINUTO
          {   
            //Altera as informações do minuto
            limpaLed(coluna,linha,minutoStr);
            minuto += 5;
            if (minuto > 55)
            {       
              minuto = 0;
            }
            //std::stringstream horarioStr;// = horario;
            minutoStr = AjustaHora(minuto);
            caracter = minutoStr;
            escreveLed(coluna,linha,caracter);
            break;
          }
          if (coluna == 75 && linha == 25)  //SOBE RAÇÃO
          {
            limpaLed(coluna,linha,racaoStr);
            racao += 50;
            if (racao > 500)
            {       
              racao = 0;
            }
            racaoStr = AjustaRacao(racao);
            caracter = racaoStr;
            escreveLed(coluna,linha,caracter);
            break;
          }
      }
      if (vbotaoOK == HIGH) // OK
      {
        if (linha == 15) //Pula linha se for a primeira linha
        {
          escreveLed(coluna,linha,caracter);
          limpaLed(0,linha,"->");
          linha += 10;
          caracter = "->";
        }
        else if (linha == 25) //SALVAR
        {
          limpaLed(0,linha,"->");
          caracter = "->";
          if (aHorario[5][4] != 255 && Tela != "Alterar") //Se a ultima posição possui registro, significa que não é possivel mais inserir. Verifica se é tela de inserir
          {
            TelaErro("Cadastro cheio","Número máximo de","horarios atingidos"); //Informa tela de erro (Titulo,Linha 1,Linha 2)
            Adicionar(false, horarioStr, minutoStr, racaoStr, "Adicionar", 99); //Chama adicionar apenas para recarregar a pagina
            coluna = 0;
            break;
          }
          if (racao == 0) //Se não for informado uma quantidade de ração não permite salvar
          {
            TelaErro("Ração Zerada", "Vai alimentar seu animal", "com nada?"); //Informa tela de erro (Titulo,Linha 1,Linha 2)
            Adicionar(false, horarioStr, minutoStr, racaoStr, "Adicionar", 99); //Chama adicionar apenas para recarregar a pagina
            coluna = 0;
            break;
          }
          if (Tela != "Alterar") //Se for tela de inserir
          {
            for (H=0;H<6;H++)
            {
              if (aHorario[H][4] == 255) //Verifica todas as posições a procura de uma sem registro
              {
                //Salva na EEPROM
                EEPROM.begin(255);
                EEPROM.write(H*5,horario);  
                EEPROM.write((H*5)+1,minuto);
                EEPROM.write((H*5)+2,racao/10); //Ração deve ser dividido por 10 por limitações da EEPROM
                EEPROM.write((H*5)+3,1);
                EEPROM.write((H*5)+4,H);
                EEPROM.end();
                //Termina de salvar na EEPROM
                //Salva no array aHorario
                aHorario[H][0] = horario;
                aHorario[H][1] = minuto;
                aHorario[H][2] = racao;
                aHorario[H][3] = 1;
                aHorario[H][4] = H;
                String var = String("/09319782970/user_data/") + String(H);
                Firebase.setFloat(var + "/user_data_hora", aHorario[H][0]);
                Firebase.setFloat(var + "/user_data_minuto", aHorario[H][1]);
                Firebase.setFloat(var + "/user_data_racao", aHorario[H][2]);
                Firebase.setFloat(var + "/user_data_ativo", aHorario[H][3]);
                //Termina de salvar no array aHorario
                EEPROM.begin(255);
                Serial.println(EEPROM.read(0));
                Serial.println(EEPROM.read(1));
                Serial.println(EEPROM.read(2));
                Serial.println(EEPROM.read(3));
                Serial.println(EEPROM.read(4));
                Serial.println(EEPROM.read(5));
                EEPROM.end();
                cancel = true;
                break;
              }
            }
          }
          else //Se for tela para alterar
          {
            //Não precisa alterar o "Ativo" ou o "Id"
            //Alteração na EEPROM do horario selecionado
            EEPROM.begin(255);
            EEPROM.write(i*5,horario);
            EEPROM.write((i*5)+1,minuto);
            EEPROM.write((i*5)+2,racao/10);
            //EEPROM.write((i*5)+3,1);
            //EEPROM.write((i*5)+4,H);
            EEPROM.end();
            //Fim alteração na EEPROM
            //Alteração no Array
            aHorario[i][0] = horario;
            aHorario[i][1] = minuto;
            aHorario[i][2] = racao;
            Firebase.setInt(String("/09319782970/user_data/") + String(i) + "/user_data_hora", horario);
            Firebase.setInt(String("/09319782970/user_data/") + String(i) + "/user_data_minuto", minuto);
            Firebase.setInt(String("/09319782970/user_data/") + String(i) + "/user_data_racao", racao);
            //aHorario[i][3] = 1;
            //aHorario[i][4] = H;
            //fim alteração Array
            cancel = true;
            break;
          }
        }
        coluna = 0;
        break;
      }
      if (vbotaoX == HIGH) // cancel
      {
        //Sai da tela atual e volta para a anterior
        cancel = true;
        coluna = 0;
        break;
      }   
      if (vbotaoDireita == HIGH) // DIREITA
      {
        if (linha == 15) //Se for linha dos horario
        {
          if (coluna == 0) //Se estiver na seta
          {
            limpaLed(coluna,linha,caracter); //Limpa a informação piscando
            escreveLed(coluna,linha,caracter);  //Escrever informação piscando (é feito isso para evitar que saia da posição e não mostre a informação)
            caracter = horarioStr; //O horario passa a piscar
            coluna = 60; //coluna da variavel do horario
            break;
          }
          else if (coluna == 60) //Se estiver na hora
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            caracter = minutoStr; //Minuto passa a piscar
            coluna = 84; //Coluna da variavel do minuto
            break;
          }
        }
        else if (linha == 25) //Se for linha da ração
        {
          if (coluna == 0)
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            caracter = racaoStr; //Ração passa a piscar
            coluna = 75; //Coluna da variavel da ração
            break;
          }
        }
      }

      if (vbotaoEsquerda == HIGH) // ESQUERDA
      {
        if (linha == 15)
        {
          if (coluna == 60) //Se estiver na hora vai para a seta
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            coluna = 0;
            caracter = "->";
            break;
          }
          else if (coluna == 84) //Se estiver no minuto vai para a hora
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            coluna = 60;
            caracter = horarioStr;
            break;
          }
        }
        else if (linha == 25)
        {
          if (coluna == 75) //Se estiver na ração vai para a seta
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            coluna = 0;
            caracter = "->";
            break;
          }
        } 
      }
      
    }
  }
}

void Horarios()
{
    int i=0,j,linha = 15;
    /*display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 0, "Horarios");
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(10, 14, "Hora");
    display.drawString(30, 14, "Ração");
    display.drawString(50, 14, "Ativo");*/
    ImprimeHo(i);
    MovimentaHo();
}

void MovimentaHo()
{
  int linha=24,i=0,coluna=0,f=0;
  String caracter="->",situacao = "apagado";
  bool cancel = false;
  while (true)
  {
    situacao = VerificaTempo(coluna, linha, caracter,situacao);
    EstadoBotao();
    if (cancel)
    {
      break;
    }
    while (vbotaoDesce == LOW && vbotaoSobe == LOW && vbotaoOK == LOW && vbotaoX == LOW && vbotaoDireita == LOW && vbotaoEsquerda == LOW)
    {
      situacao = VerificaTempo(coluna, linha, caracter,situacao);
      EstadoBotao();
      if (vbotaoDesce == HIGH) // DESCE
      {
        if (linha<54)
        {
          if(i+1 < 6 && aHorario[i+1][4] != 255) //Verificação para não descer a seta mais do que a quantidade de registros no array
          {
            if (coluna == 77) //Se estiver na coluna do ativo (ON/OFF)
            {
              //Serial.println("No Ativo: "+AjustaAtivo(aHorario[i][3]));
              //Serial.println("No Ativo: "+AjustaAtivo(aHorario[i+1][3]));
              limpaLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
              escreveLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
              caracter = AjustaAtivo(aHorario[i+1][3]);
              situacao = "escrito";
            }
            limpaLed(0,linha, "->");
            linha += 10;
            i++;
            break;
          }
        }
        else
        {
          if (i>=3 && aHorario[i+1][4] != 255) // Se estiver no ultimo horario informado no display, deve recarregar a tela com novos horarios
          {
            if(i+1 < 6) // Se o proximo for menor que 6 que é o maximo
            { 
              if (coluna == 77)
              {
                //Serial.println("No Ativo: "+AjustaAtivo(aHorario[i][3]));
                //Serial.println("No Ativo: "+AjustaAtivo(aHorario[i+1][3]));
                limpaLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
                escreveLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
                caracter = AjustaAtivo(aHorario[i+1][3]);
                situacao = "escrito";
              }
              i++;
              ImprimeHo(i-3);
              break;
            }
          }
        }
      }
      if (vbotaoSobe == HIGH) //SOBE
      {
        if (linha>24)
        {
          if (coluna == 77)
          {
            //Serial.println("No Ativo SOBE: "+AjustaAtivo(aHorario[i][3]));
            //Serial.println("No Ativo SOBE: "+AjustaAtivo(aHorario[i-1][3]));
            limpaLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
            escreveLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
            caracter = AjustaAtivo(aHorario[i-1][3]);
            situacao = "escrito";
          }
          limpaLed(0,linha, "->");
          linha -= 10;
          i--;
          break;
        }
        else
        {
          if (i>0)
          {
            if (coluna == 77)
            {
              Serial.println("No Ativo SOBE: "+AjustaAtivo(aHorario[i][3]));
              Serial.println("No Ativo SOBE: "+AjustaAtivo(aHorario[i-1][3]));
              limpaLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
              escreveLed(coluna,linha,AjustaAtivo(aHorario[i][3]));
              caracter = AjustaAtivo(aHorario[i-1][3]);
              situacao = "escrito";
            }
            i--;
            ImprimeHo(i);
            break;
          }
        }
      }
      if (vbotaoOK == HIGH) // OK
      {
        if (coluna == 0 && aHorario[i][4] != 255)
        {
          Adicionar(true, AjustaHora(aHorario[i][0]), AjustaMinuto(aHorario[i][1]), AjustaRacao(aHorario[i][2]), "Alterar", i);
          ImprimeHo(0);
          Serial.println("Saiu Adicionar");
          break;
        }
        if (coluna == 77) //ATIVA/DESATIVA
        {
          if (aHorario[i][3] == 0)
          {
            limpaLed(coluna,linha,caracter);
            situacao = "apagado";
            //escreveLed(coluna,linha,caracter);
            caracter = "ON";
            EEPROM.begin(255);
            EEPROM.write((i*5)+3,1);
            EEPROM.commit();
            aHorario[i][3] = 1;
            Firebase.setInt(String("/09319782970/user_data/") + String(i) + "/user_data_ativo", 1);
          }
          else if (aHorario[i][3] == 1)
          {
            limpaLed(coluna,linha,caracter);
            situacao = "apagado";
            //escreveLed(coluna,linha,caracter);
            caracter = "OFF";
            EEPROM.begin(255);
            EEPROM.write((i*5)+3,0);
            EEPROM.commit();
            aHorario[i][3] = 0;
            Firebase.setInt(String("/09319782970/user_data/") + String(i) + "/user_data_ativo", 0);
          }
          break;
        }
        if (coluna == 105) //EXCLUIR
        {
          Serial.println("Excluir");
          ExcluirHorario(i);
          i=0;
          linha=24;
          coluna=0;
          caracter="->";
          ImprimeHo(0);
          break;
        }
      }
       if (vbotaoX == HIGH) // Cancel
      {
        cancel = true;
        //coluna = 0;
        break;
      }
      if (vbotaoDireita == HIGH) //DIREITA
      {
        if (coluna == 0)
        {
          limpaLed(coluna,linha,caracter);
          escreveLed(coluna,linha,caracter);
          caracter = AjustaAtivo(aHorario[i][3]);
          coluna = 77;
          break;
        }
        else if (coluna == 77)
        {
          limpaLed(coluna,linha,caracter);
          escreveLed(coluna,linha,caracter);
          caracter = "X";
          coluna = 105;
          break;
        }
      }
      if (vbotaoEsquerda == HIGH) //ESQUERDA
      {
        if (coluna == 105)
        {
          limpaLed(coluna,linha,caracter);
          escreveLed(coluna,linha,caracter);
          caracter = AjustaAtivo(aHorario[i][3]);
          coluna = 77;
          break;
        }
        else if (coluna == 77)
        {
          limpaLed(coluna,linha,caracter);
          escreveLed(coluna,linha,caracter);
          caracter = "->";
          coluna = 0;
          break;
        }
      }
    }
  }
}

void ImprimeHo(int i)
{
  int linha = 24;
  int maxarray = i+4;
  int f;
  String HoraStr, MinutoStr, RacaoStr,AtivoStr;
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, "Horarios");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(10, 14, "Hora");
  display.drawString(40, 14, "Ração");
  display.drawString(75, 14, "Ativo");
  for (i;i<maxarray;i++) //Imprime 4 horarios na tela, começando pelo valor de i recebido por parametro
    {
        if (aHorario[i][0] != 255)
        {
          HoraStr = AjustaHora(aHorario[i][0]);
          MinutoStr = AjustaMinuto(aHorario[i][1]);
          RacaoStr = AjustaRacao(aHorario[i][2]);
          AtivoStr = AjustaAtivo(aHorario[i][3]);
          display.drawString(10, linha, HoraStr+":"+MinutoStr);
          display.drawString(45, linha, RacaoStr);
          display.drawString(77, linha, AtivoStr);
          display.drawString(105, linha, "X");
          linha += 10;
        }
    }
    i = i-4;
    //Serial.println("Inicio EEPROM");
    //EEPROM.begin(4);
    //for (f=0;f<256;f++)
    //{
    //  Serial.println(EEPROM.read(f));
    //}
    //EEPROM.end();
    //Serial.println("Fim EEPROM");
    display.display();
}

void EstadoBotao() //Pega o estado dos botões
{
  vbotaoSobe = digitalRead(botaoSobe);
  vbotaoDesce = digitalRead(botaoDesce);
  vbotaoOK = digitalRead(botaoOK);
  vbotaoDireita = digitalRead(botaoDireita);
  vbotaoX = digitalRead(botaoX);
  vbotaoEsquerda = digitalRead(botaoEsquerda);
} 

void limpaLed(int coluna,int linha, String escrita) //Limpa o valor escrito escrevendo em preto por cima
{
   display.setColor(BLACK);
   display.drawString(coluna, linha, escrita);
   display.display();
   display.setColor(WHITE);
}

void escreveLed(int coluna,int linha, String escrita) //Escreve a string passa por parametro
{
  display.setColor(WHITE);
  display.drawString(coluna, linha, escrita);
  display.display();
}

String VerificaTempo(int coluna,int linha, String escrita, String situacao) //Faz a informação piscar no display
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > tempoPisca) //Verifica variação do tempo. tempoPisca é uma constante declarada no inicio do fonte
  { 
    previousMillis = currentMillis;    // Salva o tempo atual
    if (situacao == "escrito") //Se estiver escrito, apaga
    {
      limpaLed(coluna,linha,escrita);
      situacao = "apagado";
      return situacao;
    }
    else if (situacao == "apagado") //Se estiver apagado, escreve
    {
      escreveLed(coluna,linha,escrita);
      situacao = "escrito";
      return situacao;
    }
  }
  return situacao;
}

String AjustaHora(int Hora) //Ajusta a hora passada de inteiro para string
{
  String HoraStr;
  if (Hora < 10)
  {
    HoraStr = "0"+String(Hora);
  }
  else
  {
    HoraStr = String(Hora);
  }
  return HoraStr;
}

String AjustaMinuto(int Minuto) //Ajusta o minuto passado de inteiro para string
{
  String MinutoStr;
  if (Minuto < 10)
  {
    MinutoStr = "0"+String(Minuto);
  }
  else
  {
    MinutoStr = String(Minuto);
  }
  return MinutoStr;
}

String AjustaRacao(int Racao) //Ajusta a ração passada de inteiro para string
{
  String RacaoStr;
  if (Racao < 100)
  {
    RacaoStr = "0"+String(Racao);
    if (Racao < 50)
    {
      RacaoStr = RacaoStr+"0";
    }
  }
  else
  {
    RacaoStr = String(Racao);
  }
  return RacaoStr;
}

String AjustaAtivo(int Ativo) //Ajusta o ativo passado de inteiro para string
{
  String AtivoStr;
  if (Ativo == 1)
  {
    AtivoStr = "ON";
  }
  else
  {
    AtivoStr = "OFF";
  }
  return AtivoStr;
}

void TelaErro(String menu, String linha1, String linha2) //Tela de erro
{
  bool cancel = false;
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, menu);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 24, linha1);
  display.drawString(0, 34, linha2);
  display.display();
  while (true) //Fica preso na tela de erro até que alguma tecla seja pressionada
  {
    if (cancel)
    {
      break;
    }
    EstadoBotao();
    while (vbotaoDesce == LOW && vbotaoSobe == LOW && vbotaoOK == LOW && vbotaoX == LOW && vbotaoDireita == LOW && vbotaoEsquerda == LOW)
    {
      EstadoBotao();
      if (vbotaoDesce == HIGH || vbotaoSobe == HIGH || vbotaoOK == HIGH || vbotaoX == HIGH || vbotaoDireita == HIGH || vbotaoEsquerda == HIGH)
      {
        cancel = true;
        break;
      }
    }
  }
}

//A função de excluir, alem de excluir passa todos os registros cadastrados posteriormente uma posição abaixo, por exemplo
//Supondo que possuem 4 registros: 0 = 12:50, 1 = 10:20, 2 = 19:35, 3 = 22:00.
//Ao exlcuir o registro 1, o registro 2 passa a ser o 1 e o 3 passa a ser o 2, ficando dessa maneira.
//0 = 12:50, 1 = 19:35, 2 = 22:00
void ExcluirHorario(int i)
{
  int vlr = 0;
  bool lExcluido = false;
  //Serial.println("Antes for");
  Firebase.remove("09319782970/user_data/"+ String(i));
  for (vlr=0;vlr<6;vlr++) //Passa por todos os horarios
  {
    //Serial.println("vlr: "+String(vlr) + "  "+String(i));
    if (vlr == i) //Se "vlr" for igual ao Id ho horario, exclui
    {
      if (vlr == 5 || aHorario[vlr+1][4] == 255) //Se "vlr" for a ultima posição ou o proximo registro estiver vazio, limpa o registro atual
      {
        EEPROM.begin(255);
        EEPROM.write(vlr*5,255);
        EEPROM.write((vlr*5)+1,255);
        EEPROM.write((vlr*5)+2,255);
        EEPROM.write((vlr*5)+3,255);
        EEPROM.write((vlr*5)+4,255);
        EEPROM.end();
        aHorario[vlr][0] = 255;
        aHorario[vlr][1] = 255;
        aHorario[vlr][2] = 255;
        aHorario[vlr][3] = 255;
        aHorario[vlr][4] = 255;
      }
      else //Se "vlr" não for a ultima posição e o proximo registro não estiver vazio, salva na posição do vlr as informações do proxima registro
      {
        Serial.println("aHorario: "+String(aHorario[vlr+1][4]));
        EEPROM.begin(255);
        EEPROM.write(vlr*5,aHorario[vlr+1][0]);
        EEPROM.write((vlr*5)+1,aHorario[vlr+1][1]);
        EEPROM.write((vlr*5)+2,aHorario[vlr+1][2]/10);
        EEPROM.write((vlr*5)+3,aHorario[vlr+1][3]);
        EEPROM.write((vlr*5)+4,aHorario[vlr+1][4] - 1);
        EEPROM.end();
        aHorario[vlr][0] = aHorario[vlr+1][0];
        aHorario[vlr][1] = aHorario[vlr+1][1];
        aHorario[vlr][2] = aHorario[vlr+1][2];
        aHorario[vlr][3] = aHorario[vlr+1][3];
        aHorario[vlr][4] = aHorario[vlr+1][4] - 1;
        Firebase.setInt(String("/09319782970/user_data/") + String(aHorario[vlr][4]) + "/user_data_hora", aHorario[vlr+1][0]);
        Firebase.setInt(String("/09319782970/user_data/") + String(aHorario[vlr][4]) + "/user_data_minuto", aHorario[vlr+1][1]);
        Firebase.setInt(String("/09319782970/user_data/") + String(aHorario[vlr][4]) + "/user_data_racao", aHorario[vlr+1][2]);
        Firebase.setInt(String("/09319782970/user_data/") + String(aHorario[vlr][4]) + "/user_data_ativo", aHorario[vlr+1][3]);
        Firebase.remove(String("/09319782970/user_data/") + String(vlr+1));
        i++;
      }
    }
  }
}

void VerificaHora()
{
  int vlrHora;
  hora = "";
  for (vlrHora=0;vlrHora<5;vlrHora++) //Pega os 5 primeiros caracteres da hora atual que é informada como hh:mm:ss
  {
    hora = hora + ntp.getFormattedTime().charAt(vlrHora); //Pega caracter por caracter da hora
  }
  //Serial.println(hora);//Printa a hora já formatada no monitor.
  for (vlrHora=0;vlrHora<6;vlrHora++) //Compara a variavel "hora" com todos os registros salvos no array em busca de igualdade
  {
    //Serial.println(AjustaHora(aHorario[vlrHora][0]) + ":" + AjustaMinuto(aHorario[vlrHora][1]));
    //Se hora atual for igual a alguma hora cadastrada, alimenta o cachorro
    if (hora == AjustaHora(aHorario[vlrHora][0]) + ":" + AjustaMinuto(aHorario[vlrHora][1]) && aHorario[vlrHora][4] != 255)//Se a hora atual for igual à que definimos, irá acender o led.
    {
      Serial.println("Alimentando cachorro");
      break;
    }
  }
  //delay(1000);//Espera 1 segundo.
}
