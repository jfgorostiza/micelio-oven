# Basic example of clearing and drawing pixels on a SSD1305 OLED display.
# This example and library is meant to work with Adafruit CircuitPython API.
# Author: Tony DiCola
# License: Public Domain

from Adafruit_GPIO import SPI
# Import all board pins.
from board import SCL, SDA
import busio

# Import the SSD1305 module.
import adafruit_ssd1305

RST = 17        # Reset pin
DC = 22         # Data/Command pin
SPI_PORT = 0    # SPI port
SPI_DEVICE = 0  # SPI device

# Create the I2C interface.
#i2c = busio.I2C(SCL, SDA)

# Create the SSD1305 OLED class.
# The first two parameters are the pixel width and pixel height.  Change these
# to the right size for your display!
display = adafruit_ssd1305.SSD1305_SPI(128, 32, spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE, max_speed_hz=8000000), dc = DC, reset=RST, cs = )
# Alternatively you can change the I2C address of the device with an addr parameter:
#display = adafruit_ssd1305.SSD1305_I2C(128, 32, i2c, addr=0x31)

# Clear the display.  Always call show after changing pixels to make the display
# update visible!
display.fill(0)

display.show()