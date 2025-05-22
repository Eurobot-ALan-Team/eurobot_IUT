#define ID_carte_BAT_Is_On_5V_Puissance  0x609
#define ID_carte_BAT_Tension_Baterie_1  0x603
#include "mbed.h"
CAN can1(PA_11, PA_12);
CANMessage msg;
DigitalOut CommandeRELAIS(PF_0);
AnalogIn bat(PA_4);
int main() {
    char datacan[1];
    datacan[0]=1;
  CommandeRELAIS.write(0);
  wait(1);
  CommandeRELAIS.write(1);
  wait(1);
  can1.write(CANMessage(ID_carte_BAT_Is_On_5V_Puissance, datacan, 1));
  while (1) {
    int val=(char)(bat.read() * 3.3 * 130 / 3.0);
    printf("vbat=%d V\n\r",val);
    datacan[0]=(char)val;
    can1.write(CANMessage(ID_carte_BAT_Tension_Baterie_1, datacan, 1));
    wait(1);
  }
}
