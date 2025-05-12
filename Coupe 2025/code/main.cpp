#include "mbed.h"
#include "nRF24L01P.h"

Serial pc(USBTX, USBRX); // tx, rx
Timer Time1;

nRF24L01P my_nrf24l01p(A6, A5, A4, A2, A3, A0); // mosi, miso, sck, csn, ce, irq

DigitalOut myled1(LED1);
DigitalOut myled2(LED2);

#define TRANSFER_SIZE 4

void float_to_char_array(float value, char output[4]) {
  memcpy(output, &value, 4); // Copie les 4 octets du float dans le tableau
}

float char_array_to_float(const char input[4]) {
  float result;
  memcpy(&result, input, 4); // Copie les 4 octets dans un float
  return result;
}

void NRFInit(void) {

  my_nrf24l01p.powerUp();

  // Display the (default) setup of the nRF24L01+ chip
  pc.printf("nRF24L01+ Frequency    : %d MHz\r\n",
            my_nrf24l01p.getRfFrequency());
  pc.printf("nRF24L01+ Output power : %d dBm\r\n",
            my_nrf24l01p.getRfOutputPower());
  pc.printf("nRF24L01+ Data Rate    : %d kbps\r\n",
            my_nrf24l01p.getAirDataRate());
  pc.printf("nRF24L01+ TX Address   : 0x%010llX\r\n",
            my_nrf24l01p.getTxAddress());
  pc.printf("nRF24L01+ RX Address   : 0x%010llX\r\n",
            my_nrf24l01p.getRxAddress());

  pc.printf("Type keys to test transfers:\r\n  (transfers are grouped into %d "
            "characters)\r\n",
            TRANSFER_SIZE);

  my_nrf24l01p.setTransferSize(TRANSFER_SIZE);

  my_nrf24l01p.setReceiveMode();
  my_nrf24l01p.enable();
}
int StartSyncTX(Timer time);
float StartSyncRX(void);

char txData[TRANSFER_SIZE], rxData[TRANSFER_SIZE];
int txDataCnt = 0;
int rxDataCnt = 0;
int stat = 0;

int main() {
  float timebeforstart = 0;
  NRFInit();


// les fonction son bloquante 

// pour l'emeterur
  stat = StartSyncTX(Time1);
// pour recepteur
  timebeforstart = StartSyncRX();


  while (1) {

    // If we've received anything over the host serial link...
    if (pc.readable()) {

      // ...add it to the transmit buffer
      txData[txDataCnt++] = pc.getc();

      // If the transmit buffer is full
      if (txDataCnt >= sizeof(txData)) {

        // Send the transmitbuffer via the nRF24L01+
        my_nrf24l01p.write(NRF24L01P_PIPE_P0, txData, txDataCnt);

        txDataCnt = 0;
      }

      // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
      myled1 = !myled1;
    }

    // If we've received anything in the nRF24L01+...
    if (my_nrf24l01p.readable()) {

      // ...read the data into the receive buffer
      rxDataCnt = my_nrf24l01p.read(NRF24L01P_PIPE_P0, rxData, sizeof(rxData));

      // Display the receive buffer contents via the host serial link
      for (int i = 0; rxDataCnt > 0; rxDataCnt--, i++) {

        pc.putc(rxData[i]);
      }

      // Toggle LED2 (to help debug nRF24L01+ -> Host communication)
      myled2 = !myled2;
    }
  }
}

// return 1 if sync
int StartSyncTX(Timer timeUser) {
  float timeStart = timeUser.read();
  float timeStartRx = 0;
  int stat = 0;
  float_to_char_array(timeStart, txData);
  my_nrf24l01p.write(NRF24L01P_PIPE_P0, txData, txDataCnt);
  while ((timeUser.read() - 2.0) < timeStart || stat == 1) {

    if (my_nrf24l01p.readable()) {

      // ...read the data into the receive buffer
      rxDataCnt = my_nrf24l01p.read(NRF24L01P_PIPE_P0, rxData, sizeof(rxData));
      timeStartRx = char_array_to_float(rxData);
      if (timeStartRx <= timeStart + 0.5 && timeStartRx >= timeStart - 0.5) {
        stat = 1;
      }
    }
  }
  return stat;
}

// return Time if sync
float StartSyncRX(void) {
  float timeStart = 0;

  while (timeStart == 0) {
    if (my_nrf24l01p.readable()) {

      // ...read the data into the receive buffer
      rxDataCnt = my_nrf24l01p.read(NRF24L01P_PIPE_P0, rxData, sizeof(rxData));
      timeStart = char_array_to_float(rxData);

      float_to_char_array(timeStart, txData);
      my_nrf24l01p.write(NRF24L01P_PIPE_P0, txData, txDataCnt);
    }
  }
  return timeStart;
}
