int pinosensor = 17;
//int pino_led_cheio = 5;
//int pino_led_vazio = 4;

//int velocidade = 2;
int IN1 = 18;
int IN2 = 19;

void setup()
{
  Serial.begin(9600);
  pinMode(pinosensor, INPUT);
  //pinMode(pino_led_cheio, OUTPUT);
  //pinMode(pino_led_vazio, OUTPUT);
  
  //pinMode(velocidade, OUTPUT);
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
    //digitalWrite(pino_led_vazio, LOW);
    //digitalWrite(pino_led_cheio, HIGH);
    break;
  case 1: // pote acabando a agua
    //digitalWrite(pino_led_vazio, HIGH);
    //digitalWrite(pino_led_cheio, LOW);
    
    //analogWrite(velocidade, 500); 
    digitalWrite(IN1, HIGH);     //liga o motor pra girar o registro
    digitalWrite(IN2, LOW);
    delay(3000);
    digitalWrite(IN1, HIGH);     //para o motor
    digitalWrite(IN2, HIGH);
    
    while (estado == 1)
    {
      estado = digitalRead(pinosensor); //verifica o nível do pote 
    }
    
    //digitalWrite(pino_led_vazio, LOW);
    //digitalWrite(pino_led_cheio, HIGH);  
    //analogWrite(velocidade, 500);
    digitalWrite(IN1, LOW);     //liga o motor no sentido inverso pra fechar o reservatorio
    digitalWrite(IN2, HIGH);
    delay(3000);
      
    digitalWrite(IN1, HIGH);     //para o motor e volta pro estado de cheio
    digitalWrite(IN2, HIGH);
    
    break;
  }
  delay(100);
}
