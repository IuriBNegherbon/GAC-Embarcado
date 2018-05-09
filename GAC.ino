#include <Wire.h>
#include "SSD1306Wire.h" 
#include <string>
#include <iostream>
#include <sstream>
#include <EEPROM.h>

//#include "gac.h"
//#include "images.h"

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
  EEPROM.begin(4);
  for (CarregaArray=0;CarregaArray<6;CarregaArray++)
  {
    aHorario[CarregaArray][0] = EEPROM.read((CarregaArray*5));          //Hora
    aHorario[CarregaArray][1] = EEPROM.read((CarregaArray*5)+1);        //Minuto
    aHorario[CarregaArray][2] = (EEPROM.read((CarregaArray*5)+2))*10;   //Ração
    aHorario[CarregaArray][3] = EEPROM.read((CarregaArray*5)+3);        //On/Off
    aHorario[CarregaArray][4] = EEPROM.read((CarregaArray*5)+4);        //Id
  }
  EEPROM.end();

  // Initialising the UI will init the display too.
  display.init();

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

void Menu()
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

void SMovimentaMe()
{
  int coluna = 0, linha = 15;//, vbotaoDesce = 0, vbotaoSobe=0, vbotaoOK = 0;
  bool cancel = false;
  String situacao = "apagado";
  Serial.println("Entrou menu");
  while (true)
  {
    if (cancel)
    {
      Serial.println("Break ");
      break;
    }
    situacao = VerificaTempo(coluna, linha, "->",situacao);
    Serial.println(situacao);
    EstadoBotao();
    while (vbotaoDesce == LOW && vbotaoSobe == LOW && vbotaoOK == LOW)
    {
      situacao = VerificaTempo(coluna, linha, "->",situacao);
      EstadoBotao();
      if (vbotaoDesce == HIGH)
      {
        if (linha<25) // desce
        {
          limpaLed(0,linha,"->");
          linha += 10;
          break;
        }
      }
      if (vbotaoSobe == HIGH)
      {
        if (linha>15) //sobe
        {
          limpaLed(0,linha,"->");
          linha -= 10;
          break;
        }
      }
      if (vbotaoOK == HIGH) // ok
      {
        if (linha == 15)
        {
          Adicionar();
          Serial.println("Saiu Add");
        }
        else if (linha == 25)
        {
          Horarios();
        }
        cancel = true;
        break;
      }
    }
  }
}

void Adicionar()
{
  int horario = 0, racao = 0;
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, "ADICIONAR");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(10, 15, "Horario:");
  display.drawString(60, 15, "00");
  display.drawString(72, 15, ":");
  display.drawString(84, 15, "00");
  display.drawString(10, 25, "Qtd Ração:");
  display.drawString(75, 25, "000");
  display.display();
  MovimentAdd();
}

void MovimentAdd()
{
  int coluna = 0, linha = 15, horario=0, minuto=0, racao=0, H = 0,f;
  String horarioStr = "00", minutoStr = "00", racaoStr = "000",caracter="->",situacao = "apagado";
  bool cancel = false;
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
      if (vbotaoDesce == HIGH)
      {
        if (linha<=25) // desce
        {
          if (coluna == 0 && linha < 25)
          {
            limpaLed(0,linha,"->");
            linha += 10;
            caracter = "->";
            break;
          }
          else if (coluna == 60 && linha == 15) 
          {          
              limpaLed(coluna,linha,horarioStr);
              horario -= 1;
              if (horario < 0)
              {
                horario = 23;
              }
              horarioStr = AjustaHora(horario);
              caracter = horarioStr;
              //display.setColor(WHITE);
              //display.drawString(coluna, linha, horarioStr);
              //display.display();
              escreveLed(coluna,linha,caracter);
              break;
          }
           else if (coluna == 84 && linha == 15) 
          {   
            limpaLed(coluna,linha,minutoStr);
            minuto -= 5;
            if (minuto < 0)
            {       
              minuto = 55;
            }
            minutoStr = AjustaMinuto(minuto);
            caracter = minutoStr;
            //display.setColor(WHITE);
            //display.drawString(coluna, linha, minutoStr);
            //display.display();
            escreveLed(coluna,linha,caracter);
            break;
          }
          else if (coluna == 75 && linha == 25)
          {
            limpaLed(coluna,linha,racaoStr);
            racao -= 50;
            if (racao < 0)
            {       
              racao = 500;
            }
            //std::stringstream horarioStr;// = horario;
            racaoStr = AjustaRacao(racao);
            caracter = racaoStr;
            escreveLed(coluna,linha,caracter);
            break;
          }
        }
      }
      
      if (vbotaoSobe == HIGH) //sobe
      {
          if (coluna == 0 && linha > 15)
          {
            limpaLed(0,linha,"->");
            linha -= 10;
            caracter = "->";
            break;
          }
          else if (coluna == 60 && linha == 15) 
          {   
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
          else if (coluna == 84 && linha == 15) 
          {   
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
          if (coluna == 75 && linha == 25)
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
      if (vbotaoOK == HIGH) // ok
      {
        if (linha == 15)
        {
          limpaLed(0,linha,"->");
          linha += 10;
          caracter = "->";
        }
        else if (linha == 25)
        {
          limpaLed(0,linha,"->");
          caracter = "->";
          for (H=1;H<=6;H++)
          {
            if (aHorario[H][3] == 0)
            {
              EEPROM.begin(4);
              EEPROM.write(H*5,horario);
              EEPROM.write((H*5)+1,minuto);
              EEPROM.write((H*5)+2,racao/10);
              EEPROM.write((H*5)+3,1);
              EEPROM.write((H*5)+4,H);
              EEPROM.commit();
              aHorario[H][0] = horario;
              aHorario[H][1] = minuto;
              aHorario[H][2] = racao;
              aHorario[H][3] = 1;
              aHorario[H][4] = H;
              EEPROM.begin(4);
              Serial.println(EEPROM.read(0));
              Serial.println(EEPROM.read(1));
              Serial.println(EEPROM.read(2));
              Serial.println(EEPROM.read(3));
              Serial.println(EEPROM.read(4));
              EEPROM.end();
              break;
            }
          }
          cancel = true;
        }
        coluna = 0;
        break;
      }
      if (vbotaoX == HIGH) // cancel
      {
        //linhamax = 45;
        cancel = true;
        coluna = 0;
        break;
      }
      
      if (vbotaoDireita == HIGH) // DIREITA
      {
        if (linha == 15)
        {
          if (coluna == 0)
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            caracter = horarioStr;
            coluna = 60;
            break;
          }
          else if (coluna == 60)
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            caracter = minutoStr;
            coluna = 84;
            break;
          }
        }
        else if (linha == 25)
        {
          if (coluna == 0)
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            caracter = racaoStr;
            coluna = 75;
            break;
          }
        }
      }

      if (vbotaoEsquerda == HIGH) // ESQUERDA
      {
        if (linha == 15)
        {
          if (coluna == 60)
          {
            limpaLed(coluna,linha,caracter);
            escreveLed(coluna,linha,caracter);
            coluna = 0;
            caracter = "->";
            break;
          }
          else if (coluna == 84)
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
          if (coluna == 75)
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
  int linha=24,i=0,coluna=0;
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
      if (vbotaoDesce == HIGH)
      {
        if (linha<45) // desce
        {
          if(aHorario[i+1][0] != 0)
          {
            limpaLed(0,linha, "->");
            linha += 10;
            break;
          }
        }
        else
        {
          if (i<6)
          {
            if(aHorario[i+1][0] != 0)
            { 
              i++;
              ImprimeHo(i);
              break;
            }
          }
        }
      }
      if (vbotaoSobe == HIGH)
      {
        if (linha>15) //sobe
        {
          limpaLed(0,linha, "->");
          linha -= 10;
          break;
        }
        else
        {
          if (i>0)
          {
            i--;
            ImprimeHo(i);
            break;
          }
        }
      }
      if (vbotaoOK == HIGH) // ok
      {
        
      }
       if (vbotaoX == HIGH) // Cancel
      {
        cancel = true;
        //coluna = 0;
        break;
      }
      if (vbotaoDireita == HIGH)
      {
        if (coluna == 0)
        {
          limpaLed(coluna,linha,caracter);
          escreveLed(coluna,linha,caracter);
          caracter = AjustaAtivo(i);
          coluna = 77;
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
  for (i;i<maxarray;i++)
    {
        if (aHorario[i][0] != 0)
        {
          HoraStr = AjustaHora(aHorario[i][0]);
          MinutoStr = AjustaMinuto(aHorario[i][1]);
          RacaoStr = AjustaRacao(aHorario[i][2]);
          Serial.println(aHorario[i][3]);
          AtivoStr = AjustaAtivo(aHorario[i][3]);
          Serial.println(AtivoStr);
          display.drawString(10, linha, HoraStr+":"+MinutoStr);
          display.drawString(45, linha, RacaoStr);
          display.drawString(77, linha, AtivoStr);
          display.drawString(105, linha, "X");
          linha += 10;
        }
    }
    i = i-4;
    Serial.println("Inicio EEPROM");
    EEPROM.begin(4);
    for (f=0;f<256;f++)
    {
      Serial.println(EEPROM.read(f));
    }
    EEPROM.end();
    Serial.println("Fim EEPROM");
    display.display();
}

void EstadoBotao()
{
  vbotaoSobe = digitalRead(botaoSobe);
  vbotaoDesce = digitalRead(botaoDesce);
  vbotaoOK = digitalRead(botaoOK);
  vbotaoDireita = digitalRead(botaoDireita);
  vbotaoX = digitalRead(botaoX);
  vbotaoEsquerda = digitalRead(botaoEsquerda);
} 

void limpaLed(int coluna,int linha, String escrita)
{
   display.setColor(BLACK);
   display.drawString(coluna, linha, escrita);
   display.display();
   display.setColor(WHITE);
}

void escreveLed(int coluna,int linha, String escrita)
{
  display.setColor(WHITE);
  display.drawString(coluna, linha, escrita);
  display.display();
}

String VerificaTempo(int coluna,int linha, String escrita, String situacao)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > tempoPisca) 
  { 
    previousMillis = currentMillis;    // Salva o tempo atual
    if (situacao == "escrito")
    {
      limpaLed(coluna,linha,escrita);
      situacao = "apagado";
      return situacao;
    }
    else if (situacao == "apagado")
    {
      escreveLed(coluna,linha,escrita);
      situacao = "escrito";
      return situacao;
    }
  }
  return situacao;
}

String AjustaHora(int Hora)
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

String AjustaMinuto(int Minuto)
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

String AjustaRacao(int Racao)
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

String AjustaAtivo(int Ativo)
{
  String AtivoStr;
  Serial.println(Ativo);
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
