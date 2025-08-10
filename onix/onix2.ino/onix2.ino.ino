#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>
#include "animacion.h"

// ====================== OLED ======================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ====================== BOTONES ======================
#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_SELECT 6

// ====================== MOTOR PASO A PASO ======================
#define STEPS_PER_REV 2048
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
Stepper myStepper(STEPS_PER_REV, IN1, IN3, IN2, IN4);

// ====================== MENÚ ======================
enum MenuState {
  MENU_TIEMPO,
  MENU_NOMBRE,
  MENU_CANTIDAD,
  MENU_EXTRACCION,
  MENU_TOTAL
};
MenuState menuActual = MENU_TIEMPO;

// ====================== VARIABLES ======================
int submenuTiempoIndex = 0;
const int submenuTiempoTotal = 3;
int vecesAlDia = 1;

// Cantidades y revoluciones
const char* cantidadesTexto[] = {"10g", "20g", "30g", "40g", "50g"};
const int revolucionesPorCantidad[] = {1, 2, 3, 4, 5};
const int totalCantidades = 5;
int cantidadIndex = 0;
bool enSubMenuCantidad = false;

void controlarBotonSelect() {
  if (digitalRead(BUTTON_SELECT) == LOW) {
    if (menuActual == MENU_TIEMPO) {
      submenuTiempoIndex = (submenuTiempoIndex + 1) % submenuTiempoTotal;
      delay(200);
    } else if (menuActual == MENU_CANTIDAD) {
      enSubMenuCantidad = !enSubMenuCantidad; // Entrar/salir del submenú
      delay(200);
    } else if (menuActual == MENU_EXTRACCION) {
      Serial.print("Extrayendo ");
      Serial.println(cantidadesTexto[cantidadIndex]);
      myStepper.setSpeed(10);
      myStepper.step(STEPS_PER_REV * revolucionesPorCantidad[cantidadIndex]);
      delay(200);
    }
  }
}

void mostrarMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);

  switch (menuActual) {

    // ===== TIEMPO =====
    case MENU_TIEMPO:
      if (submenuTiempoIndex == 0) {
        display.setTextSize(2);               
        display.setCursor((128 - 12) / 2, 0);
        display.print((char)24);
        display.setCursor(30, 30);
        display.print("Horario");
        display.setCursor((128 - 12) / 2, 64 - 16);
        display.print((char)25);
      } else if (submenuTiempoIndex == 1) {
        display.setTextSize(1);
        display.setCursor(10, 35);
        display.println("Elija cantidad de");
        display.setCursor(10, 45);
        display.println("veces al dia (01 - 05)");
      } else if (submenuTiempoIndex == 2) {
        display.setCursor(10, 20);
        display.println("veces al dia:");
        display.setTextSize(2);
        display.setCursor(50, 40);
        display.println(vecesAlDia);
      }
      break;

    // ===== CANTIDAD =====
    case MENU_CANTIDAD:
      display.setTextSize(1);
      display.setCursor(10, 10);
      display.println("Cantidad:");

      display.setTextSize(2);
      display.setCursor(40, 35);
      display.println(cantidadesTexto[cantidadIndex]);

      // Cambiar cantidad solo si estamos en submenú
      if (enSubMenuCantidad) {
        if (digitalRead(BUTTON_RIGHT) == LOW) {
          cantidadIndex = (cantidadIndex + 1) % totalCantidades;
          delay(200);
        }
        if (digitalRead(BUTTON_LEFT) == LOW) {
          cantidadIndex = (cantidadIndex - 1 + totalCantidades) % totalCantidades;
          delay(200);
        }
      }
      break;

    // ===== EXTRACCIÓN =====
    case MENU_EXTRACCION:
      display.setTextSize(2);
      display.setCursor(15, 20);
      display.println("Extraccion");
      display.setTextSize(1);
      display.setCursor(10, 50);
      display.println("Presiona SELECT");
      break;
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se detecta la pantalla OLED"));
    while (true);
  }

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  display.clearDisplay();
  display.display();
}

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
    display.setCursor(5, 20);
    display.println("Onix 1.0");
    display.display();
    delay(1000);
  } else {
    mostrarMenu();
    controlarBotonSelect();

    if (!enSubMenuCantidad) { // Solo navegar si no estamos dentro del submenú
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
}
