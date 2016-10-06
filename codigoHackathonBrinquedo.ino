#include <Servo.h>

#define SERVO1 12
#define SERVO2 7

//Estados da aplicacao
#define APRESENTACAO 1
#define EXIBICAO_SEQUENCIA 2
#define AGUARDANDO_INPUT 3
#define PROCESSANDO_INPUT 4
#define PROCESSANDO_ERRO 5
#define PROCESSANDO_ACERTO 6
#define PROCESSANDO_VITORIA 7
#define AGUARDANDO_INICIO 8

#define BUZZER 6
#define PAUSA 20

#define NIVEL_MAXIMO 4

Servo servo;
Servo servo2;

int pinosLeds[4] = { 8, 9, 10, 11 };
int pinosBotoes[4] = { 2, 3, 4, 5 };
int sequencia[100] = {};
int notas[4] = {262, 294, 330, 349};
int botaoStart = 13;

int estadoDaAplicacao = AGUARDANDO_INICIO;
int nivel = -1;
int indexInputUsuario = 0;
int inputUsuario = -1;

void setup() {
  servo.attach(SERVO1);
  servo2.attach(SERVO2);
  Serial.begin(9600);
  servo.write(30);
  servo2.write(150);
  
  for (int i = 0; i < 4; i++) {
    pinMode(pinosLeds[i], OUTPUT);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(pinosBotoes[i], INPUT_PULLUP);
  }

  pinMode(botaoStart, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
  switch(estadoDaAplicacao){
    case(AGUARDANDO_INICIO): doInicio(); break;
    case(APRESENTACAO): doApresentacao(); break;
    case(EXIBICAO_SEQUENCIA): doExibicaoSequencia(); break;
    case(AGUARDANDO_INPUT): doAguardandoInput(); break;
    case(PROCESSANDO_INPUT): doProcessandoInput(); break;
    case(PROCESSANDO_ERRO): doProcessandoErro(); break;
    case(PROCESSANDO_ACERTO): doProcessandoAcerto(); break;
    case(PROCESSANDO_VITORIA): doProcessandoVitoria(); break;
  }
}

void doInicio() {
  if (digitalRead(botaoStart) == LOW) {
        estadoDaAplicacao = APRESENTACAO;
        delay(100);
  }
}

void doApresentacao(){
  for (int i = 0; i < 16; i++) {
    digitalWrite(pinosLeds[i%4], HIGH);
    notada(notas[i%4], 100);
    digitalWrite(pinosLeds[i%4], LOW);    
  }
  delay(400);

  nivel = -1;
  estadoDaAplicacao = EXIBICAO_SEQUENCIA;
}

void doExibicaoSequencia(){
  nivel = nivel+1;
  sequencia[nivel] = random(0,4);
  for (int i = 0; i <= nivel; i++) {
    tocar(sequencia[i]);
  }
  indexInputUsuario = -1;
  estadoDaAplicacao = AGUARDANDO_INPUT;
}

void doProcessandoVitoria() {
  int pos = 0;
  for(pos = 30; pos < 180; pos+=1) {
    servo.write(pos);
    servo2.write(180-pos);
    delay(15);
  }
  tocar_musica();
  for(pos = 180; pos >= 30; pos-=2) {
    servo.write(pos);
    servo2.write(180-pos);
    delay(15);
  }
}

void doAguardandoInput(){
  int novoInputUsuario = -1;
  for (int i = 0; i <= 3; i++) {
      if (digitalRead(pinosBotoes[i]) == LOW) {
        novoInputUsuario = i;
      }
  }
  if (digitalRead(botaoStart) == LOW) {
    estadoDaAplicacao = APRESENTACAO;
  }
  if(inputUsuario != -1 && novoInputUsuario == -1){
    novoInputUsuario = inputUsuario;
    estadoDaAplicacao = PROCESSANDO_INPUT;
    delay(100);
  }
  inputUsuario = novoInputUsuario;
}

void doProcessandoInput(){
  indexInputUsuario = indexInputUsuario + 1;
  tocar(inputUsuario);
  if(sequencia[indexInputUsuario] != inputUsuario){
    estadoDaAplicacao = PROCESSANDO_ERRO;
  } else if (indexInputUsuario == nivel){
    estadoDaAplicacao = PROCESSANDO_ACERTO;
  }
  else{
    estadoDaAplicacao = AGUARDANDO_INPUT;
  }
  inputUsuario = -1;
}

void doProcessandoErro(){
  tocaPerdeu();
  estadoDaAplicacao = APRESENTACAO;
}

void doProcessandoAcerto(){
  if(nivel >= NIVEL_MAXIMO){
    estadoDaAplicacao = PROCESSANDO_VITORIA;
    return;
  }
  doProcessandoAcertoServo();
  delay(1000);
  estadoDaAplicacao = EXIBICAO_SEQUENCIA;
}

void doProcessandoAcertoServo() {
  int pos = 0;
  int posicaoFinal = 0;
  if (nivel == 0) {
    posicaoFinal = 60;
  } else if (nivel == 1) {
    posicaoFinal = 90;
  } else if (nivel == 2){
    posicaoFinal = 135;
  } else if (nivel == 3) {
    posicaoFinal = 160;
  } else {
    posicaoFinal = 180;
  }
  for (pos = 30; pos < posicaoFinal; pos+=1) {
    servo.write(pos);
    servo2.write(180-pos);
    delay(15);
  }
  delay(500);
  if (posicaoFinal != 180) for (pos = posicaoFinal; pos >= 30; pos-=1) {
    servo.write(pos);
    servo2.write(180-pos);
    delay(5);
  }  
}

void tocar(int cod){
  tone(BUZZER, notas[cod]);
  digitalWrite(pinosLeds[cod], HIGH);
  delay(600);
  digitalWrite(pinosLeds[cod], LOW);
  noTone(BUZZER);
  delay(200);
}

void notada(int frequencia, int tempo){
  tone(BUZZER, frequencia);
  delay(tempo-PAUSA);
  noTone(BUZZER);
  delay(PAUSA);
}

void marcha(int f1, int f2){
  notada(f1, 300);
  notada(f2, 150);
  notada(f2, 150);
}

void desmarcha(int f1){
  notada(f1, 150);
  notada(f1, 150);
}

void tocar_musica(){
  if(deveParar()) return;
  marcha(262, 262);
  if(deveParar()) return;
  marcha(262, 262);
  if(deveParar()) return;
  marcha(330, 262);
  if(deveParar()) return;
  notada(262,600);

  if(deveParar()) return;
  marcha(330, 330);
  if(deveParar()) return;
  marcha(330, 330);
  if(deveParar()) return;
  marcha(394, 330);
  if(deveParar()) return;
  notada(330,600);

  if(deveParar()) return;
  marcha(330, 330);
  if(deveParar()) return;
  marcha(330, 330);
  if(deveParar()) return;  
  desmarcha(330);
  if(deveParar()) return;
  notada(330,900);

  if(deveParar()) return;
  marcha(330, 330);
  if(deveParar()) return;
  marcha(330, 330);
  if(deveParar()) return;
  desmarcha(330);
  if(deveParar()) return;
  notada(330,300);

  for(int i = 0; i < 2; i++){
  if(deveParar()) return;
  notada(330,150);
  if(deveParar()) return;
  notada(394,450);
  if(deveParar()) return;
  notada(440,1800);

  if(deveParar()) return;
  notada(440,150);
  if(deveParar()) return;
  notada(494,450);
  if(deveParar()) return;
  notada(330,1800);

  if(deveParar()) return;
  notada(330,150);
  if(deveParar()) return;
  notada(394,450);
  if(deveParar()) return;
  notada(440,1800);

  if(deveParar()) return;
  notada(440,150);
  if(deveParar()) return;
  notada(494,450);
  if(deveParar()) return;
  notada(330,1800);

  if(deveParar()) return;
  noTone(BUZZER);
  delay(300);
  if(deveParar()) return;
  notada(294,150);
  if(deveParar()) return;
  notada(262,150);
  if(deveParar()) return;
  notada(294,450);
  if(deveParar()) return;
  notada(262,150);
  if(deveParar()) return;
  notada(294,150);
  if(deveParar()) return;
  notada(330,1050);
  if(deveParar()) return;

  noTone(BUZZER);
  delay(300);
  if(deveParar()) return;
  notada(523, 150);
  if(deveParar()) return;
  notada(523, 150);
  if(deveParar()) return;
  notada(494, 300);
  if(deveParar()) return;
  notada(494, 150);
  if(deveParar()) return;
  notada(440, 300);
  if(deveParar()) return;
  notada(440, 150);
  if(deveParar()) return;
  notada(392, 600);
  if(deveParar()) return;
  notada(349, 300);
  
  if(deveParar()) return;
  notada(330, 1400);
  if(deveParar()) return;
  notada(330, 150);
  if(deveParar()) return;
  notada(349, 450);

  if(deveParar()) return;
  notada(330, 2400);
  }
  delay(800);
  finaliza();
  
}

void finaliza() {
  estadoDaAplicacao = AGUARDANDO_INICIO;
  for (int i = 0; i < 4; i++) {
    digitalWrite(pinosLeds[i], LOW);
  }
}

void tocaPerdeu(){
    
    notaFunebre(262, 600, 1);
    
    notaFunebre(262, 400, 1);
    notaFunebre(262, 200, 1);

    notaFunebre(262, 600, 1);
    
    notaFunebre(311, 400, 3);
    notaFunebre(294, 200, 2);

    notaFunebre(294, 400, 2);
    notaFunebre(262, 200, 1);
    
    notaFunebre(262, 400, 1);
    notaFunebre(247, 200, 0);
    
    notaFunebre(262, 600, 1);

    noTone(BUZZER);
    delay(600);
}

void notaFunebre(int f, int tempo, int led){
    digitalWrite(pinosLeds[led], HIGH);
    notada(f, tempo);
    digitalWrite(pinosLeds[led], LOW);     
}
int deveParar(){
    digitalWrite(pinosLeds[random(0,4)], HIGH);
    digitalWrite(pinosLeds[random(0,4)], LOW);
    if(estadoDaAplicacao == PROCESSANDO_VITORIA)  {
        int novoInputUsuario = -1;
        for (int i = 0; i <= 3; i++) {
            if (digitalRead(pinosBotoes[i]) == LOW) {
              novoInputUsuario = i;
            }
        }
        if(novoInputUsuario != -1){
            estadoDaAplicacao = APRESENTACAO;
            return true;
        }
    }
    return false;
  }

