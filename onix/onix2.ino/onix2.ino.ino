#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>
#include "animacion.h" 

// ====================== OLED ======================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ====================== BOTONES ======================
#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_SELECT 6
bool enSubMenuVeces = false;

// ====================== VARIABLES DE MENÚ ======================
int vecesAlDia = 1;
int submenuTiempoIndex = 0;  
const int submenuTiempoTotal = 3; 

enum MenuState {
  MENU_TIEMPO,
  MENU_NOMBRE,
  MENU_COMIDA,
  MENU_TOTAL
};

MenuState menuActual = MENU_TIEMPO;

// ====================== MOTOR PASO A PASO ======================
#define STEPS_PER_REV 2048
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
Stepper myStepper(STEPS_PER_REV, IN1, IN3, IN2, IN4);

// ====================== FUNCIONES ======================
void controlarBotonSelect() {
  if (digitalRead(BUTTON_SELECT) == LOW) {
    if (menuActual == MENU_TIEMPO) {
      submenuTiempoIndex = (submenuTiempoIndex + 1) % submenuTiempoTotal;
      delay(200);
    }
    if (menuActual == MENU_COMIDA) {
      // Acciona el motor al presionar SELECT en menú comida
      Serial.println("Motor activado para dispensar comida...");
      myStepper.setSpeed(10);  
      myStepper.step(STEPS_PER_REV / 4);  // 1/4 de vuelta (ajustable)
    }
  }
}

void mostrarMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);

  switch (menuActual) {

    case MENU_TIEMPO:
      display.setTextSize(1);
      display.setCursor(0, 0);

      if (submenuTiempoIndex == 0) {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor((128 - 12) / 2, 0);
        display.print((char)24); // ↑

        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds("Horario", 0, 0, &x1, &y1, &w, &h);
        display.setCursor((128 - w) / 2, (64 - h) / 2);
        display.print("Horario");

        display.setTextSize(2);
        display.setCursor((128 - 12) / 2, 64 - 16); 
        display.print((char)25); // ↓
      }
      else if (submenuTiempoIndex == 1) {
        display.setTextSize(1);
        display.setCursor(10, 35);
        display.println("Elija cantidad de");
        display.setCursor(10, 45);
        display.println("veces al dia (01 - 05)");
      } 
      else if (submenuTiempoIndex == 2) {
        display.setTextSize(1);
        display.setCursor(10, 20);
        display.println("veces al dia:");
        display.setTextSize(2);
        display.setCursor(50, 40);
        display.println(vecesAlDia);
      }
      break;

    case MENU_COMIDA:
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor((128 - 12) / 2, 0);
      display.print((char)24); // ↑

      int16_t x2, y2;
      uint16_t w2, h2;
      display.getTextBounds("Cantidad", 0, 0, &x2, &y2, &w2, &h2);
      display.setCursor((128 - w2) / 2, (64 - h2) / 2);
      display.print("Cantidad");

      display.setTextSize(2);
      display.setCursor((128 - 12) / 2, 64 - 16); 
      display.print((char)25); // ↓
      break;
  }

  display.display();
}

// ====================== SETUP ======================
void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se detecta la pantalla OLED"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
}

// ====================== LOOP ======================
void loop() {
  static bool animacionHecha = false;

  if (!animacionHecha) {
    for (int i = 0; i < NUM_BITMAPS; i++) {
      display.clearDisplay();
      display.drawBitmap(0, 0, animacionBitmaps[i], 128, 64, SSD1306_WHITE);
      display.display();
      delay(animacionDelays[i]);
    }
    animacionHecha = true;

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 20);
    display.println("Onix 1.0");
    display.display();
    delay(1000);
  } else {
    mostrarMenu();
    controlarBotonSelect();

    if (digitalRead(BUTTON_RIGHT) == LOW) {
      menuActual = (MenuState)((menuActual + 1) % MENU_TOTAL);
      delay(200); 
    }
    if (digitalRead(BUTTON_LEFT) == LOW) {
      menuActual = (MenuState)((menuActual - 1 + MENU_TOTAL) % MENU_TOTAL);
      delay(200); 
    }
  }
}
