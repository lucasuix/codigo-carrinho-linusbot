#include <PETEletrica.h>

//DECLARACAO DO SENSOR
SensorInfravermelhoRC qtr((unsigned char[]) {14,15,16,17,18}, 5, 2000, QTR_NO_EMITTER_PIN);
unsigned int sensores[5];

float mediaSensores;
float s0,s1,s2,s3,s4;
int triger_pin =11; int echo_pin=10;
float d=0;

SensorUltrassonico ultrassonico(triger_pin, echo_pin);




//VARIAVEIS DE CONTROLE
float u = 0, kp = 0.0575;
float vel_ref = 120;
float erro;
float sensors_ref = 2000;

//VETOR DAS CURVAS 0 é DIREITA 1 é ESQUERDA
int Curva[5] = {1, 1, 1, 1, 1}; //AQUI CADA ELEMENTO REPRESENTA UMA CURVA DA PISTA
int j = 0; //Iterador das curvas, a cada curva adicionamos 1, provavelmente vamos dar mais de uma volta, então resetar se j = 5;


//TENSOR DO DESVIO 0 é DIREITA 1 é ESQUERDA
//{{0, 240, 800}, {100, 200, 250}, {240, 0, 700}, {100, 200, 800}, {240, 0, 600}, {100, 200, 100}, {0, 0, 150000000000}, {180, 180, 0}}
int Turn[2][8][3] ={{{0, 240, 800}, {100, 200, 250}, {240, 0, 700}, {100, 200, 700}, {240, 0, 600}, {100, 200, 300}, {0, 240, 700}, {100, 200, 0}},
                    {{240, 0, 500}, {100, 200, 500}, {0, 240, 600}, {100, 200, 600}, {100, 200, 0}, {100, 200, 0}, {240, 0, 0}, {100, 200, 0}}};

//MOTORES
int IN1=5 , IN2= 3, IN3= 9, IN4= 6; //MOTORES, IN1 a IN4
Motores motor(IN1, IN2, IN3, IN4);

void setup() {
  //Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  for(int i = 0; i < 200;i++){//QUANTO MAIOR O VALOR DE i MAIS CALIBRADO ESTÁ
    qtr.calibrate();
    delay(10);
  }
}

void loop() {
  unsigned int position = qtr.readLine(sensores);
  s0= analogRead(A0);
  s1= analogRead(A1);
  s2= analogRead(A2);
  s3= analogRead(A3);
  s4= analogRead(A4);

  mediaSensores =(s0+s1+s2+s3+s4)/5;


  //ERRO PROPORCIONAL - PERMITE QUE O CARRO SIGA SUA REFERENCIA CORRIGINDO
  //O SEU CURSO
  erro = sensors_ref - position;
  u = kp*erro;
  /*if(vel_ref - u > 250)
  {
    u = -90;
    motor.set_motors(vel_ref - u, vel_ref + u);
  }
  else{
  if(vel_ref + u > 250)
  {
    u = 90;
    motor.set_motors(vel_ref - u*1.5, vel_ref + u);
  }
  else{
    motor.set_motors(vel_ref - u, vel_ref + u);
  }
  }*/
  motor.set_motors(vel_ref - u, vel_ref + u);
  delay(10);





    if (mediaSensores > 650)
  {
    float a = 1;
    long microsec = ultrassonico.timing();
    d = ultrassonico.convert(microsec, SensorUltrassonico::CM);

    while (d > 28)
    {
    d = 0;
    for(int k = 0; k < 5; k++)
    {
      long microsec = ultrassonico.timing();
      d = d + ultrassonico.convert(microsec, SensorUltrassonico::CM);
   
    unsigned int position = qtr.readLine(sensores);
    erro = sensors_ref - position;
    u = kp*erro;
    motor.set_motors(vel_ref - u, vel_ref + u);
   /* if(vel_ref - u > 250)
  {
    u = -50;
    motor.set_motors(vel_ref - u, vel_ref + u*3);
  }
  else{
  if(vel_ref + u > 250)
  {
    u = 50;
    motor.set_motors(vel_ref - u*4, vel_ref + u);
  }
  else{
    motor.set_motors(vel_ref - u, vel_ref + u);
  }
  }*/
    delay(10);
    }
    d = d/5;
    }


   
    for (int i = 0; i < 8; i++) {
      motor.set_motors(Turn[Curva[j]][i][0], Turn[Curva[j]][i][1]);
      delay(Turn[Curva[j]][i][2]);
    }
    j++;
    if (j == 4) j = 0;

    mediaSensores = 0;
    while(mediaSensores < 200)
    {
      s0= analogRead(A0);
      s1= analogRead(A1);
      s2= analogRead(A2);
      s3= analogRead(A3);
      s4= analogRead(A4);

      mediaSensores =(s0+s1+s2+s3+s4)/5;
      delay(10);
    }
  }
}
