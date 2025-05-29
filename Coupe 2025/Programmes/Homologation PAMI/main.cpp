#include "SRF05.h"
#include "mbed.h"
//#include "nRF24L01P.h" // Inclure la bibliothèque appropriée pour nRF24L01+

#define debug 0 // 1pour debug
#define periodpwm 100
#define VITESSE 30 //25
#define P 20 //20
#define D 0.15

#define couleur 1 // 0=jaune 1=bleu

#define TRANSFER_SIZE 4

DigitalIn jack(PA_9);
DigitalIn inter(PA_11);
PwmOut motG(PA_12);
PwmOut motD(PB_0);
AnalogIn capt1(PA_1);
AnalogIn capt2(PA_0);
AnalogIn capt3(PA_3);
AnalogIn capt4(PB_1);
DigitalOut sensG(PA_8);
DigitalOut sensD(PF_0);
DigitalOut mot_danse(PB_3);

SRF05 tele(D0, D12);

// nRF24L01P my_nrf24l01p(PA_7, PA_6, PA_5, PA_4, PF_1, PB_0);

Timer t1, t2, tsuivi; // temps

void vitesse_moteurs(int vG, int vD);
void init(void);
void marque_D(void);
void marque_G(void);
void suivie(void);
void tourne(int);
void stop(void);
void danse(void);

int mrqD = 0, mrqG = 0; // marque sur tapis
// variablke pour suivie
float Vg = 0, Vd = 0;
float g = 0, d = 0;

double c1, c2, c3, c4;

float erreur = 0, erreur_AV;

float temp_derivee = 0;

int main() {

  init();
  int etat = 0;
  int temps = 0;
  int distance_tel = 0;
  int cmd_servo = 1300, tservo;


  while (1) {

    c1 = capt1.read(); // '1' c'est le blanc
    c2 = capt2.read();
    c3 = capt3.read();
    c4 = capt4.read();
    int j = jack.read();
    int choix = inter.read();
    temps = (int)t1.read();
    tservo = t2.read_us();
    distance_tel = (int)tele.read();
    if (debug == 1) {
      printf("etat=%d tps=%d / tele=%d c1=%d c2=%d "
             "c3=%d c4=%d -JACK=%d - choix=%d cmdservo=%d tservo=%d\n\r",
             etat, temps, distance_tel, (int)(100 * c1), (int)(100 * c2),
             (int)(100 * c3), (int)(100 * c4), j, choix, cmd_servo, tservo);
    }
    if (tservo > 20000) {
      t2.reset();
    } else {
      if (tservo > cmd_servo) {
        mot_danse.write(0);
      } else {
        mot_danse.write(1);
      }
    }
    switch (etat) {
    case 0:
      if (j == 1) {
        etat = 1;
        t1.start();
        t1.reset();
      }
      break;
    case 1:
      if (temps >= 86) { // temps avant depart
        etat = 2;
        tsuivi.start();
        tsuivi.reset();
      }
      break;
    case 2:
      if (distance_tel <= 15) {
        etat = 10;
      }
      if (temps > 92) {
        etat = 3;
      }
      break;
    case 3:
      etat = 4;
      break;
    case 4:
      break;
    case 10:
      if (distance_tel >= 17) {
        etat = 2;
        tsuivi.start();
        tsuivi.reset();
      }
       if (temps > 98) {
        etat = 4;
      }
      break;
    }

    switch (etat) {
    case 0:
      stop();
      break;
    case 1:
      stop();
      break;
    case 2:
      suivie();
      break;
    case 3:
      tourne(choix);
      break;
    case 4:
      stop();
      if ((temps % 2) == 0) {
        cmd_servo = 1300;
      } else {
        cmd_servo = 1700;
      }
      break;
    case 10:
      stop();
      break;
    }
  }
}
void vitesse_moteurs(int vG, int vD) {
  if (vG >= periodpwm) {
    vG = periodpwm;
  }
  if (vD >= periodpwm) {
    vD = periodpwm;
  }
  if (vG < 0) {
    vG = 0;
  }
  if (vD < 0) {
    vD = 0;
  }

  motG.pulsewidth_us(vG);
  motD.pulsewidth_us(vD);
}

void suivie(void) {

  erreur_AV = erreur;
  erreur = c2 - c3;
  temp_derivee = tsuivi.read();
  tsuivi.reset();
  Vg = VITESSE - P * erreur - D * ((erreur - erreur_AV) / temp_derivee);
  Vd = VITESSE + P * erreur + D * ((erreur - erreur_AV) / temp_derivee);
  Vg = (int)(Vg);
  Vd = (int)(Vd);
  // printf("Vg = %.2f || Vd = %.2f || %f   c1= %.2f  c2=%.2f  c3=%.2f
  // c4=%.2f\n\r",Vg,Vd,temp_derivee,c1,c2,c3,c4);
  vitesse_moteurs(Vg, Vd);
  wait_us(200);
}

void tourne(int choix) {
  if(debug){printf("\n\r>>>>>>>JE TOURNE --- choix =%d\n",choix);}
  if (choix == 1) {
    vitesse_moteurs(0, 50);
    wait_us(300000);
  } else {
    vitesse_moteurs(50, 0);
    wait_us(300000);
  }
  stop();
}

void stop(void) { vitesse_moteurs(0, 0); }

void init(void) {
  // mot_danse.period_ms(20);
  jack.mode(PullUp);
  inter.mode(PullUp);
  motG.period_us(periodpwm);
  motD.period_us(periodpwm);
  vitesse_moteurs(0, 0);
  t2.start();
  t2.reset();
}