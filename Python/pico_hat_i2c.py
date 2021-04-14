#!/usr/bin/env python3
# -*- coding: utf-8 -*-
''' pico_hat_i2c.py  weigu.lu
    Raspberry Pi is Master, Pico is slave
    This test program sends the 3 bytes to the Pico, defining how often
    the onboard LED will blink and defining the delay time in ms between
    the toggling (command 0x0A).
    It reads 2x2 bytes from the Arduino (temperature (0x81) and humidity (0x82)
    More infos on weigu.lu '''

import smbus

PORT = 1 # (0 for rev.1, 1 for rev 2!)
I2C = smbus.SMBus(PORT)
PICO_I2C_ADDRESS = 0x20

COMMAND_TEMP1 = 0x81  # MSB = 1 for read
COMMAND_HUM1 = 0x82
COMMAND_BLINK = 0x0A  # MSB = 0 for write
BLINK_RATE = 10       # how many times to blink (1 byte)
BLINK_DELAY = 50      # delay between toggling im ms (1 word)

def get_sensor_data(command):
    ''' write command and read data (1 word)'''
    I2C.write_byte(PICO_I2C_ADDRESS, command)
    return I2C.read_word_data(PICO_I2C_ADDRESS, command)/100.0

def set_blink(blink_command, blink_rate, blink_delay):
    ''' write to Pico: blink rate (1 byte) and delay (2 byte)'''
    blink = [] # list for 3 bytes
    blink.append(blink_rate)
    blink.append(blink_delay//256)
    blink.append(blink_delay%256)
    I2C.write_i2c_block_data(PICO_I2C_ADDRESS, blink_command, blink)

# main: try the commands
print("Temperature 1: ", get_sensor_data(COMMAND_TEMP1), "°C")
print("Humidity 1:    ", get_sensor_data(COMMAND_HUM1), "%")
set_blink(COMMAND_BLINK, BLINK_RATE, BLINK_DELAY)
