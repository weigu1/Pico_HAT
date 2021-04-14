#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# pico_
# Raspberry Pi is Master, Pico is slave

import smbus
import time

PORT = 1 # (0 for rev.1, 1 for rev 2!)
I2c = smbus.SMBus(PORT)
PICO_I2C_ADDRESS = 0x20
command = 0x64

#I2c.write_byte(PICO_I2C_ADDRESS,command)
time.sleep(0.001)
print(hex(I2c.read_byte_data(PICO_I2C_ADDRESS,command)))
time.sleep(0.001)
#I2c.write_i2c_block_data(PICO_I2C_ADDRESS, 0x0A, (0x20))    
