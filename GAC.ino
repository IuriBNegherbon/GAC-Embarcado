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
  EEPROM.begin(255);
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
  while (true)
  {
    if (cancel)
    {
      break;
    }
    situacao = VerificaTempo(coluna, linha, "->",situacao);
    //Serial.println(situacao);
    EstadoBotao();
    while (vbotaoDesce == LOW && vbotaoSobe == LOW && vbotaoOK == LOW)
    {
      situacao = VerificaTempo(coluna, linha, "->",situacao);
      EstadoBotao();
      if (vbotaoDesce == HIGH)
      {
        if (linha<25) // DESCE
        {
          limpaLed(0,linha,"->");
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
          Adicionar(true, "00", "00", "000", "Adicionar", 99);
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

void Adicionar(bool chamaMov, String horarioStr, String MinutoStr, String racaoStr, String Tela, int i)
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
  if (chamaMov)
  {
    MovimentAdd(Tela, i);
    Serial.println("Saiu MovimentaAdd");
  }
}

void MovimentAdd(String Tela, int i)
{
  int coluna = 0, linha = 15, horario=0, minuto=0, racao=0, H = 0,f;
  String horarioStr = "00", minutoStr = "00", racaoStr = "000",caracter="->",situacao = "apagado";
  bool cancel = false;
  
  if (Tela == "Alterar")
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
              limpaLed(coluna,linha,horarioStr);
              horario -= 1;
              if (horario < 0)
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
            limpaLed(coluna,linha,minutoStr);
            minuto -= 5;
            if (minuto < 0)
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
            limpaLed(coluna,linha,racaoStr);
            racao -= 50;
            if (racao < 0)
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
        if (linha == 15)
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
          if (aHorario[5][4] != 255 && Tela != "Alterar")
          {
            TelaErro("Cadastro cheio","Número máximo de","horarios atingidos");
            Adicionar(false, horarioStr, minutoStr, racaoStr, "Adicionar", 99);
            coluna = 0;
            break;
          }
          if (racao == 0)
          {
            TelaErro("Ração Zerada", "Vai alimentar seu animal", "com nada?");
            Adicionar(false, horarioStr, minutoStr, racaoStr, "Adicionar", 99);
            coluna = 0;
            break;
          }
          if (Tela != "Alterar")
          {
            for (H=0;H<6;H++)
            {
              if (aHorario[H][4] == 255)
              {
                  EEPROM.begin(255);
                  EEPROM.write(H*5,horario);
                  EEPROM.write((H*5)+1,minuto);
                  EEPROM.write((H*5)+2,racao/10);
                  EEPROM.write((H*5)+3,1);
                  EEPROM.write((H*5)+4,H);
                  EEPROM.end();
                  aHorario[H][0] = horario;
                  aHorario[H][1] = minuto;
                  aHorario[H][2] = racao;
                  aHorario[H][3] = 1;
                  aHorario[H][4] = H;
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
          else
          {
              EEPROM.begin(255);
              EEPROM.write(i*5,horario);
              EEPROM.write((i*5)+1,minuto);
              EEPROM.write((i*5)+2,racao/10);
              //EEPROM.write((i*5)+3,1);
              //EEPROM.write((i*5)+4,H);
              EEPROM.end();
              aHorario[i][0] = horario;
              aHorario[i][1] = minuto;
              aHorario[i][2] = racao;
              //aHorario[i][3] = 1;
              //aHorario[i][4] = H;
              cancel = true;
              break;
          }
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
          if(i+1 < 6 && aHorario[i+1][4] != 255)
          {
            if (coluna == 77)
            {
              Serial.println("No Ativo: "+AjustaAtivo(aHorario[i][3]));
              Serial.println("No Ativo: "+AjustaAtivo(aHorario[i+1][3]));
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
          if (i>=3 && aHorario[i+1][4] != 255) // Se valor da linha for 4
          {
            if(i+1 < 6) // Se o proximo for menor que 6 que é o maximo
            { 
              if (coluna == 77)
              {
                Serial.println("No Ativo: "+AjustaAtivo(aHorario[i][3]));
                Serial.println("No Ativo: "+AjustaAtivo(aHorario[i+1][3]));
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
            Serial.println("No Ativo SOBE: "+AjustaAtivo(aHorario[i][3]));
            Serial.println("No Ativo SOBE: "+AjustaAtivo(aHorario[i-1][3]));
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
        if (coluna == 0 && aHorario[i+1][4] != 255)
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
  for (i;i<maxarray;i++)
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

void TelaErro(String menu, String linha1, String linha2)
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
  while (true)
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

void ExcluirHorario(int i)
{
  int vlr = 0;
  bool lExcluido = false;
  Serial.println("Antes for");
  for (vlr=0;vlr<6;vlr++)
  {
    Serial.println("vlr: "+String(vlr) + "  "+String(i));
    if (vlr == i)
    {
      if (vlr == 5 || aHorario[vlr+1][4] == 255)
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
      else
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
        i++;
      }
    }
  }
}
