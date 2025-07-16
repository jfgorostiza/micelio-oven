import time
import Adafruit_GPIO
from Adafruit_GPIO import SPI
import Adafruit_SSD1306
from PIL import Image, ImageDraw, ImageFont
# Pin configuration
RST = 17        # Reset pin
DC = 22         # Data/Command pin
SPI_PORT = 0    # SPI port
SPI_DEVICE = 0  # SPI device
# Initialize the OLED module
disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST, dc=DC, spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE, max_speed_hz=8000000))
disp.begin()
disp.clear()
disp.display()  # Clear the screen

# Image 1: Draw graphics
width = disp.width
height = disp.height
image1 = Image.new('1', (width, height))
draw1 = ImageDraw.Draw(image1)
padding = 1
shape_width = 50
left = padding
top = padding
right = width - padding
bottom = height - padding
# Draw a rectangle
draw1.rectangle((left, top, right, bottom), outline=1, fill=0)
# Draw an ellipse
draw1.ellipse((left + 10, top + 10, left + 10 + shape_width, top + 10 + shape_width), outline=1, fill=0)
# Draw a triangle
draw1.polygon([(right - 20, top + 10), (width / 2 + 5, bottom - 10), (right - 5, bottom - 10)], outline=1, fill=0)
# Image 2: Load an image
#image2 = Image.open('testImage.jpg').resize((128, 64)).convert('1')
# Image 3: Display text
image3 = Image.new('1', (width, height))
draw3 = ImageDraw.Draw(image3)
font1 = ImageFont.load_default()
#font2 = ImageFont.truetype('04B_30__.TTF', 15)
#font3 = ImageFont.truetype('baby blocks.ttf', 18)
# Display text on image3
draw3.text((0, 0), 'Hello', font=font1, fill=1)
draw3.text((0, 15), 'World!', font=font1, fill=1)
draw3.text((0, 35), 'Enjoy it!', font=font1, fill=1)
try:
    index = 1
    while True:
        if index == 1:
            disp.image(image1)  # Display image1
            index += 1
        elif index == 2:
            disp.image(image1)  # Display image2
            index += 1
        else:
            disp.image(image3)  # Display image3
            index = 1
        disp.display()  # Update the display
        time.sleep(2)  # Pause for 2 seconds
except:
    disp.clear()  # Clear the display in case of an exception
    disp.display()  # Update the display to reflect the clearing