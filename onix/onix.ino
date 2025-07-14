

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "bitmaps.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//ojos abiertos
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}



void loop() {

  display.clearDisplay();
  display.drawBitmap(0, 0, cat_openojosabiertos, 128, 64, SSD1306_WHITE);
  display.display();
  delay(150);

  display.clearDisplay();
  display.drawBitmap(0, 0, cat_openojoscerrados, 128, 64, SSD1306_WHITE);
  display.display();
  delay(150);

  display.clearDisplay();
  display.drawBitmap(0, 0, cat_openojosabiertos, 128, 64, SSD1306_WHITE);
  display.display();
  delay(150);

  display.clearDisplay();
  display.drawBitmap(0, 0, cat_openojoscerrados, 128, 64, SSD1306_WHITE);
  display.display();
  delay(150);

  display.clearDisplay();
  display.drawBitmap(0, 0, cat_openojosabiertos, 128, 64, SSD1306_WHITE);
  display.display();
  delay(150);
  
  display.clearDisplay();
  display.drawBitmap(0, 0, epd_bitmap_bocaAbierta, 128, 64, SSD1306_WHITE);
  display.display();
  delay(150);


  // Animación de lengua optimizada
  const unsigned char* animacionLengua[] = {
    epd_bitmap_lengua1, epd_bitmap_lengua2, epd_bitmap_lengua3, epd_bitmap_lengua4,
    epd_bitmap_lengua5, epd_bitmap_lengua6, epd_bitmap_lengua7, epd_bitmap_lengua8,
    epd_bitmap_lengua9, epd_bitmap_lengua10, epd_bitmap_lengua11, epd_bitmap_lengua12
  };
  for (int i = 0; i < 12; i++) {
    display.clearDisplay();
    display.drawBitmap(0, 0, animacionLengua[i], 128, 64, SSD1306_WHITE);
    display.display();
    delay(1);
  }





  
}
