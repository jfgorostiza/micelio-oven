import time
import board
import busio
import adafruit_ccs811

i2c_bus = busio.I2C(board.SCL, board.SDA)
ccs811 = adafruit_ccs811.CCS811(i2c_bus)
print(ccs811.eco2)
print(ccs811.tvoc)
print(ccs811.temperature)
