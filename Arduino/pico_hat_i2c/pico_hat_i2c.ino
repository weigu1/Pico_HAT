/* pico_hat_i2c.ino   weigu.lu
 * Raspberry Pi is Master, Pico is slave
 * The Raspi program sends 3 bytes to the Pico, defining how often
 * the onboard LED will blink and defining the delay time in ms between
 * the toggling (command 0x0A).
 * The pico responds with 2x2 bytes (temperature and humidity) when receiving
 * the commands 0x81 and 0x82.
 * More infos on weigu.lu
 */

#include <Wire.h>

//#define DEBUG      // uncomment if more infos needed in serial monitor

const byte PICO_I2C_ADDRESS = 0x20;
const byte LED_PIN = LED_BUILTIN;   // LED_BUILTIN or other pin
bool LED_LOGIC = 1;                 // positive logic: 1, negative logic: 0
const unsigned long DELAY_MS = 3000;
const unsigned long LED_BLINK_DELAY_MS = 100;

struct {
  float volatile t1; // temperature
  float volatile h1; // humidity
} tx_data;

struct {
  byte volatile blink_nr = 3;
  word volatile blink_delay = 100;
} rx_data;

byte volatile command = 0;
byte volatile rx_flag = 0;
byte volatile tx_flag = 0;
const byte tx_table_bytes = 20;
byte volatile tx_table[tx_table_bytes]; // prepare data for sending over I2C

void setup() {
  Serial.begin(115200);             // for debugging
  init_led();
  Wire.begin(PICO_I2C_ADDRESS);     // join i2c bus
  Wire.onReceive(i2c_receive);      // i2c interrupt receive
  Wire.onRequest(i2c_transmit);     // i2c interrupt send
  tx_data.t1 = 21.30;               // simulate a temperature
  tx_data.h1 = 60;                  // simulate humidity value
}

void loop() {
  blink_led_x_times(rx_data.blink_nr,rx_data.blink_delay);
  #ifdef DEBUG
    if (rx_flag) {
      Serial.println("RX with " + String(rx_flag) + "byte; command: " + String(command));
      rx_flag = 0;
    }
    if (tx_flag) {
      print_tx_table();
      tx_flag = 0;
    }
  #else
    delay(3000);
  #endif
}

void i2c_receive(int bytes_count) { // bytes_count gives number of bytes in rx buffer
  if (bytes_count == 0) {           //  master checked only for presence
    return;
  }
  command = Wire.read();
  switch (command) {   // parse commands
    case 0x0A:  // read three bytes (blink nr and delay time)
      rx_data.blink_nr = Wire.read();
      rx_data.blink_delay = Wire.read();
      rx_data.blink_delay = rx_data.blink_delay*256 + Wire.read();
      break;
    default:
      break;
  }
  rx_flag = bytes_count;
}

void i2c_transmit() {
  byte bytes_count = 1;
  int tmp1 = 0;                     // temporary variable
  switch (command) {
    case 0x81: // temperature
      tmp1 = int(round(tx_data.t1 * 100));
      tx_table[0] = (byte)(tmp1 & 0xFF);
      tx_table[1] = (byte)(tmp1 >> 8);
      bytes_count = 2;
      break;
    case 0x82: // humidity
      tmp1 = int(round(tx_data.h1 * 100));
      tx_table[0] = (byte)(tmp1 & 0xFF);
      tx_table[1] = (byte)(tmp1 >> 8);
      bytes_count = 2;
      break;
    default:
      break;
  }
  for (byte i = 0; i < bytes_count; i++) {
    Wire.write(tx_table[i]);
  }
  tx_flag = 1;
}

void print_tx_table() {
  Serial.println("Transmit Table");
  for (byte i = 0; i < tx_table_bytes; i++) {
    Serial.print("  " + String(i) + ": " + String(tx_table[i]));
  }
  Serial.println();
}

/****** LED HELPER functions *************************************************/

// initialise the build in LED and switch it on
void init_led() {
  pinMode(LED_PIN,OUTPUT);
  led_on();
}

// LED on
void led_on() {
  LED_LOGIC ? digitalWrite(LED_PIN,HIGH) : digitalWrite(LED_PIN,LOW);
}

// LED off
void led_off() {
  LED_LOGIC ? digitalWrite(LED_PIN,LOW) : digitalWrite(LED_PIN,HIGH);
}

// blink LED x times (LED was on) with delay_time_ms
void blink_led_x_times(byte x, word delay_time_ms) {
  for(byte i = 0; i < x; i++) { // Blink x times
    led_off();
    delay(delay_time_ms);
    led_on();
    delay(delay_time_ms);
  }
}
