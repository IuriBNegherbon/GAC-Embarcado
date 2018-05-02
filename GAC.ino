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
int aHorario[6][4] = {};
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
    aHorario[CarregaArray][0] = EEPROM.read((CarregaArray*4));
    aHorario[CarregaArray][1] = EEPROM.read((CarregaArray*4)+1);
    aHorario[CarregaArray][2] = (EEPROM.read((CarregaArray*4)+2))*10;
    aHorario[CarregaArray][3] = EEPROM.read((CarregaArray*4)+3);
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
  display.drawString(60, 15, "0");
  display.drawString(72, 15, ":");
  display.drawString(84, 15, "0");
  display.drawString(10, 25, "Qtd Ração:");
  display.drawString(75, 25, "0");
  display.display();
  MovimentAdd();
}

void MovimentAdd()
{
  int coluna = 0, linha = 15, horario=0, minuto=0, racao=0, H = 0,f;
  String horarioStr = "0", minutoStr = "0", racaoStr = "0",caracter="->",situacao = "apagado";;
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
              horarioStr = String(horario);
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
            minutoStr = String(minuto);
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
            racaoStr = String(racao);
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
            horarioStr = String(horario);
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
            minutoStr = String(minuto);
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
            racaoStr = String(racao);
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
              EEPROM.write(H*4,horario);
              EEPROM.write((H*4)+1,minuto);
              EEPROM.write((H*4)+2,racao/10);
              EEPROM.write((H*4)+3,H);
              EEPROM.commit();
              aHorario[H][0] = horario;
              aHorario[H][1] = minuto;
              aHorario[H][2] = racao;
              aHorario[H][3] = H;
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
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 0, "Horarios");
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    ImprimeHo(i);
    MovimentaHo();
}

void MovimentaHo()
{
  int linha = 15,i=0;
  bool cancel = false;
  while (true)
  {
    display.setColor(WHITE);
    display.drawString(0, linha, "->");
    display.display();
    EstadoBotao();
    if (cancel)
    {
      break;
    }
    while (vbotaoDesce == LOW && vbotaoSobe == LOW && vbotaoOK == LOW && vbotaoX == LOW)
    {
      EstadoBotao();
      if (vbotaoDesce == HIGH)
      {
        if (linha<45) // desce
        {
          if(aHorario[i+1][0] != 0)
          {
            display.setColor(BLACK);
            display.drawString(0, linha, "->");
            display.display();
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
          display.setColor(BLACK);
          display.drawString(0, linha, "->");
          display.display();
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
    }
  }
}

void ImprimeHo(int i)
{
  int linha = 15;
  int maxarray = i+4;
  int f;
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, "Horarios");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  for (i;i<maxarray;i++)
    {
        if (aHorario[i][0] != 0)
        {
          display.drawString(10, linha, "Hora: "+String(aHorario[i][0])+":"+String(aHorario[i][1])+" Ração: "+String(aHorario[i][2]));
          linha += 10;
        }
    }
    i = i-4;
    for (f=0;f<256;f++)
  {
    Serial.println(EEPROM.read(f));
  }
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

