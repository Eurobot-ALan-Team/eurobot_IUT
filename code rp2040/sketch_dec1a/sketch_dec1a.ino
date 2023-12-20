#include <SPI.h>


void setup() {
  // put your setup code here, to run once:
pinMode(0,OUTPUT);
pinMode(5,OUTPUT);
pinMode(26,INPUT);
bool setRX(4);
bool setCS(1);
bool setSCK(2);
bool setTX(3);
SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
}


int a=0;


void loop() {
  // put your main code here, to run repeatedly:
a=analogRead(26);
delay(a);
digitalWrite(0,HIGH);
delay(a);
digitalWrite(0,LOW);


digitalWrite(5, LOW);            // set the SS pin to LOW
  
  for(byte wiper_value = 0; wiper_value <= 128; wiper_value++) {

    SPI.transfer(0x00);             // send a write command to the MCP4131 to write at registry address 0x00
    SPI.transfer(wiper_value);      // send a new wiper value
    Serial.println(analogRead(26)); // read the value from analog pin A0 and send it to serial
    delay(1000); 
  }

  digitalWrite(5, HIGH);           // set the SS pin HIGH


}
