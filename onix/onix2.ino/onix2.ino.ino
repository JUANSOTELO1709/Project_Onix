#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>

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

// ====================== MENÚ PRINCIPAL ======================
enum MenuState {
  MENU_CANTIDAD,    // Opción 1: Ajustar cantidad
  MENU_EXTRACCION,  // Opción 2: Extraer
  MENU_HORARIO,     // Opción 3: Ajustar horario
  MENU_TOTAL
};
MenuState menuActual = MENU_CANTIDAD;

// ====================== VARIABLES ======================
const char* cantidadesTexto[] = {"10g", "20g", "30g", "40g", "50g"};
const int revolucionesPorCantidad[] = {1, 2, 3, 4, 5};
const int totalCantidades = 5;
int cantidadIndex = 0;

int hora = 8;  // Ejemplo: Hora inicial

// ====================== FUNCIONES ======================
void mostrarMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  switch (menuActual) {
    // ===== CANTIDAD =====
    case MENU_CANTIDAD:
      display.setCursor(10, 10);
      display.println("Cantidad:");
      display.setTextSize(2);
      display.setCursor(40, 30);
      display.println(cantidadesTexto[cantidadIndex]);
      break;

    // ===== EXTRACCIÓN =====
    case MENU_EXTRACCION:
      display.setCursor(10, 10);
      display.println("Extraer:");
      display.setTextSize(2);
      display.setCursor(30, 30);
      display.println(cantidadesTexto[cantidadIndex]);
      break;

    // ===== HORARIO =====
    case MENU_HORARIO:
      display.setCursor(10, 10);
      display.println("Horario:");
      display.setTextSize(2);
      display.setCursor(40, 30);
      display.print(hora);
      display.println(":00");
      break;
  }

  // Indicador de selección (flecha)
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print(">");
  display.display();
}

void controlarBotones() {
  // Navegación izquierda/derecha entre opciones principales
  if (digitalRead(BUTTON_RIGHT) == LOW) {
    menuActual = (MenuState)((menuActual + 1) % MENU_TOTAL);
    delay(200);
  }
  if (digitalRead(BUTTON_LEFT) == LOW) {
    menuActual = (MenuState)((menuActual - 1 + MENU_TOTAL) % MENU_TOTAL);
    delay(200);
  }

  // Acción con SELECT (depende de la opción)
  if (digitalRead(BUTTON_SELECT) == LOW) {
    switch (menuActual) {
      case MENU_CANTIDAD:
        // Cambiar cantidad (con LEFT/RIGHT)
        if (digitalRead(BUTTON_LEFT) == LOW) {
          cantidadIndex = (cantidadIndex - 1 + totalCantidades) % totalCantidades;
        }
        if (digitalRead(BUTTON_RIGHT) == LOW) {
          cantidadIndex = (cantidadIndex + 1) % totalCantidades;
        }
        break;

      case MENU_EXTRACCION:
        // Activar motor para extraer
        myStepper.setSpeed(10);
        myStepper.step(STEPS_PER_REV * revolucionesPorCantidad[cantidadIndex]);
        break;

      case MENU_HORARIO:
        // Ajustar hora (con LEFT/RIGHT)
        if (digitalRead(BUTTON_LEFT) == LOW) {
          hora = (hora - 1) % 24;
        }
        if (digitalRead(BUTTON_RIGHT) == LOW) {
          hora = (hora + 1) % 24;
        }
        break;
    }
    delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error en OLED"));
    while (true);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  mostrarMenu();
  controlarBotones();
}