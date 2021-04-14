// pico_raspi_i2c_test.ino    weigu.lu 

#include <Wire.h>

const byte PICO_I2C_ADDRESS = 0x20;
byte volatile rx_flag = 0;
byte volatile tx_flag = 0;
byte volatile rx_buffer[] = {0,0,0,0,0,0,0,0,0,0};
byte volatile tx_buffer[] = {0,0,0,0,0,0,0,0,0,0};

void setup() {
  Serial.begin(115200);  // for debugging
  Wire.begin(PICO_I2C_ADDRESS);    // join i2c bus
  Wire.onReceive(i2c_receive);     // i2c interrupt receive
  Wire.onRequest(i2c_transmit);    // i2c interrupt send 
}

void loop() {
  if (rx_flag) {
    for (byte i=0; i<rx_flag; i++) {
      Serial.print("RX: ");
      Serial.print(rx_buffer[i]);
      Serial.print('\t');
    }
    Serial.print('\n');
    rx_flag = 0;
  }

  if (tx_flag) {
    Serial.println("TX");
    Serial.println(rx_buffer[0]);
    tx_flag = 0;
  }  
  delay(1); 
}

void i2c_receive(int bytes_count) {  // bytes_count gives number of bytes in rx buffer    
  for (byte i=0; i<bytes_count;i++) {
    rx_buffer[i] = Wire.read(); 
  }  
  rx_flag = bytes_count;
}


void i2c_transmit() { 
  tx_flag = 1;
  Wire.write("0");
}
