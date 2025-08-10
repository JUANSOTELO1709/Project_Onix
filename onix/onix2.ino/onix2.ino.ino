#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>
#include "animacion.h"
// ====================== Configuración OLED ======================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ====================== Botones ======================
#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_SELECT 6

// ====================== Motor Paso a Paso ======================
#define STEPS_PER_REV 2048
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
Stepper myStepper(STEPS_PER_REV, IN1, IN3, IN2, IN4);

// ====================== Animación ======================
extern const unsigned char* animacionBitmaps[];  // Definido en animacion.h
extern const int animacionDelays[];             // Definido en animacion.h
extern const int NUM_BITMAPS;                   // Definido en animacion.h
bool animacionHecha = false;                    // Controla si la animación ya se mostró

// ====================== Estados del Menú ======================
enum MenuState {
  MENU_CANTIDAD,
  MENU_EXTRACCION,
  MENU_HORARIO,
  MENU_TOTAL
};
MenuState menuActual = MENU_CANTIDAD;  // Menú inicial
bool enSubMenu = false;               // Controla si estamos en un submenú

// ====================== Variables de Menú ======================
const char* cantidadesTexto[] = {"10g", "20g", "30g", "40g", "50g"};  // Opciones de cantidad
const int revolucionesPorCantidad[] = {1, 2, 3, 4, 5};                // Vueltas del motor para cada cantidad
const int totalCantidades = 5;                                        // Total de opciones de cantidad
int cantidadIndex = 0;                                                // Índice de cantidad seleccionada

int hora = 8;    // Hora inicial (formato 24h)
int minuto = 0;  // Minuto inicial





int cantidadVeces = 2; // por defecto
int horasAlimentacion[6] = {8, 12, 18, 0, 0, 0};
int minutosAlimentacion[6] = {0, 0, 0, 0, 0, 0};

enum HorarioState {
  HORARIO_MOSTRAR,     // pantalla principal horario
  HORARIO_CANTIDAD,    // seleccionar cantidad de veces
  HORARIO_HORAS        // seleccionar cada hora
};

HorarioState estadoHorario = HORARIO_MOSTRAR;
int indiceHoraActual = 0;


// ====================== Función para mostrar flechas ======================
void mostrarFlechas() {
  display.setTextSize(2);
  // Flecha arriba (↑)
  display.setCursor(SCREEN_WIDTH / 2 - 6, 0);
  display.print((char)24);
  // Flecha abajo (↓)
  display.setCursor(SCREEN_WIDTH / 2 - 6, SCREEN_HEIGHT - 16);
  display.print((char)25);
}

// ====================== Función de Mostrar Menú Principal ======================
void mostrarMenuPrincipal() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  
  // Título del menú actual (centrado)
  switch (menuActual) {
    case MENU_CANTIDAD:
      display.setCursor(SCREEN_WIDTH / 2 - 36, SCREEN_HEIGHT / 2 - 10);
      display.println("CANTIDAD");
      break;
    case MENU_EXTRACCION:
      display.setCursor(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 10);
      display.println("EXTRAER");
      break;
    case MENU_HORARIO:
      display.setCursor(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 10);
      display.println("HORARIO");
      break;
  }
  
  mostrarFlechas();
  display.display();
}

// ====================== Función de Mostrar Submenú Cantidad ======================
void mostrarSubMenuCantidad() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Texto "Cantidad:" (centrado)
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 24, 10);
  display.println("Cantidad:");
  
  // Valor actual (grande y centrado)
  display.setTextSize(2);
  display.setCursor(SCREEN_WIDTH / 2 - 12, 30);
  display.println(cantidadesTexto[cantidadIndex]);
  
  mostrarFlechas();
  display.display();
}

// ====================== Función de Mostrar Submenú Extracción ======================
void mostrarSubMenuExtraccion() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Texto "Extraer:" (centrado)
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 20, 10);
  display.println("Extraer:");
  
  // Valor actual (grande y centrado)
  display.setTextSize(2);
  display.setCursor(SCREEN_WIDTH / 2 - 12, 30);
  display.println(cantidadesTexto[cantidadIndex]);
  
  // Mensaje de confirmación (pequeño)
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 40, 50);
  display.println("[SELECT] para iniciar");
  
  mostrarFlechas();
  display.display();
}

void mostrarSubMenuHorario() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  switch (estadoHorario) {
    case HORARIO_CANTIDAD:
      display.setTextSize(1);
      display.setCursor((SCREEN_WIDTH - 84) / 2, 10);
      display.println("Veces al dia:");
      display.setTextSize(2);
      display.setCursor((SCREEN_WIDTH - 12) / 2, 30);
      display.print(cantidadVeces);
      mostrarFlechas();
      break;

    case HORARIO_HORAS:
      display.setTextSize(1);
      display.setCursor((SCREEN_WIDTH - 80) / 2, 10);
      display.print("Hora comida ");
      display.print(indiceHoraActual + 1);
      display.setTextSize(2);
      display.setCursor((SCREEN_WIDTH - 48) / 2, 30);
      if (horasAlimentacion[indiceHoraActual] < 10) display.print("0");
      display.print(horasAlimentacion[indiceHoraActual]);
      display.print(":");
      if (minutosAlimentacion[indiceHoraActual] < 10) display.print("0");
      display.print(minutosAlimentacion[indiceHoraActual]);
      mostrarFlechas();
      break;

    case HORARIO_MOSTRAR:
      display.setTextSize(1);
      display.setCursor(SCREEN_WIDTH / 2 - 20, 10);
      display.println("Horario:");
      display.setTextSize(2);
      display.setCursor(SCREEN_WIDTH / 2 - 20, 30);
      if (hora < 10) display.print("0");
      display.print(hora);
      display.print(":");
      if (minuto < 10) display.print("0");
      display.print(minuto);
      mostrarFlechas();
      break;
  }

  display.display();
}
// ====================== Control de Botones ======================
void controlarBotones() {
  if (digitalRead(BUTTON_SELECT) == LOW) {
    if (!enSubMenu) {
      enSubMenu = true;  // Entrar al submenú
    } else {
      // Acciones al confirmar en submenú
      switch (menuActual) {
        case MENU_EXTRACCION:
          myStepper.setSpeed(10);
          myStepper.step(STEPS_PER_REV * revolucionesPorCantidad[cantidadIndex]);
          break;
      }
      enSubMenu = false;  // Salir del submenú
    }
    delay(200);
  }

  if (!enSubMenu) {
    // Navegación entre menús principales
    if (digitalRead(BUTTON_RIGHT) == LOW) {
      menuActual = (MenuState)((menuActual + 1) % MENU_TOTAL);
      delay(200);
    }
    if (digitalRead(BUTTON_LEFT) == LOW) {
      menuActual = (MenuState)((menuActual - 1 + MENU_TOTAL) % MENU_TOTAL);
      delay(200);
    }
  } else {
    // Ajustes dentro del submenú
    if (digitalRead(BUTTON_LEFT) == LOW) {
      switch (menuActual) {
        case MENU_CANTIDAD:
          cantidadIndex = (cantidadIndex - 1 + totalCantidades) % totalCantidades;
          break;
        case MENU_HORARIO:
          hora = (hora - 1 + 24) % 24;
          break;
      }
      delay(200);
    }
    if (digitalRead(BUTTON_RIGHT) == LOW) {
      switch (menuActual) {
        case MENU_CANTIDAD:
          cantidadIndex = (cantidadIndex + 1) % totalCantidades;
          break;
        case MENU_HORARIO:
          hora = (hora + 1) % 24;
          break;
      }
      delay(200);
    }
  }
}

// SETUP ----------------

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error en OLED"));
    while (1);
  }
  display.clearDisplay();
  display.display();
}

// LOOP ----------------
void loop() {
  static bool animacionHecha = false;

  // ----- ANIMACIÓN INICIAL (solo una vez) -----
  if (!animacionHecha) {
    // 1. Mostrar todos los frames de la animación
    for (int i = 0; i < NUM_BITMAPS; i++) {
      display.clearDisplay();
      display.drawBitmap(0, 0, animacionBitmaps[i], SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      display.display();
      delay(animacionDelays[i]);
    }

    // 2. Mostrar mensaje "Onix 1.0"
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(
      (SCREEN_WIDTH - 6 * 12) / 2,
      (SCREEN_HEIGHT - 16) / 2
    );
    display.println("Onix 1.0");
    display.display();
    delay(1000);

    animacionHecha = true;
  } 
  
  // ----- LÓGICA PRINCIPAL -----
  else {
    // Mostrar menú actual
    if (!enSubMenu) {
      mostrarMenuPrincipal();
    } else {
      switch (menuActual) {
        case MENU_CANTIDAD:
          mostrarSubMenuCantidad();
          break;
        case MENU_EXTRACCION:
          mostrarSubMenuExtraccion();
          break;
        case MENU_HORARIO:
          mostrarSubMenuHorario(); // ahora maneja todos los estados internos
          break;
      }
    }

    // ----- BOTÓN SELECT -----
    if (digitalRead(BUTTON_SELECT) == LOW) {
      if (!enSubMenu) {
        enSubMenu = true;  // Entrar al submenú
      } else {
        if (menuActual == MENU_EXTRACCION) {
          myStepper.step(STEPS_PER_REV * revolucionesPorCantidad[cantidadIndex]);
          enSubMenu = false;
        }
        else if (menuActual == MENU_HORARIO) {
          if (estadoHorario == HORARIO_MOSTRAR) {
            estadoHorario = HORARIO_CANTIDAD; // paso 1 -> cantidad
          } 
          else if (estadoHorario == HORARIO_CANTIDAD) {
            estadoHorario = HORARIO_HORAS; // paso 2 -> horas
            indiceHoraActual = 0;
          } 
          else if (estadoHorario == HORARIO_HORAS) {
            indiceHoraActual++;
            if (indiceHoraActual >= cantidadVeces) {
              estadoHorario = HORARIO_MOSTRAR; // vuelve a la vista inicial
              enSubMenu = false;
            }
          }
        }
      }
      delay(200);
    }

    // ----- BOTONES NAVEGACIÓN -----
    if (!enSubMenu) {
      // Menú principal
      if (digitalRead(BUTTON_RIGHT) == LOW) {
        menuActual = (MenuState)((menuActual + 1) % MENU_TOTAL);
        delay(200);
      }
      if (digitalRead(BUTTON_LEFT) == LOW) {
        menuActual = (MenuState)((menuActual - 1 + MENU_TOTAL) % MENU_TOTAL);
        delay(200);
      }
    } else {
      // Ajustes dentro del submenú
      if (menuActual == MENU_CANTIDAD) {
        if (digitalRead(BUTTON_LEFT) == LOW) {
          cantidadIndex = (cantidadIndex - 1 + totalCantidades) % totalCantidades;
          delay(200);
        }
        if (digitalRead(BUTTON_RIGHT) == LOW) {
          cantidadIndex = (cantidadIndex + 1) % totalCantidades;
          delay(200);
        }
      }
      else if (menuActual == MENU_HORARIO) {
        if (estadoHorario == HORARIO_MOSTRAR) {
          if (digitalRead(BUTTON_LEFT) == LOW) {
            hora = (hora - 1 + 24) % 24;
            delay(200);
          }
          if (digitalRead(BUTTON_RIGHT) == LOW) {
            hora = (hora + 1) % 24;
            delay(200);
          }
        }
        else if (estadoHorario == HORARIO_CANTIDAD) {
          if (digitalRead(BUTTON_LEFT) == LOW && cantidadVeces > 1) {
            cantidadVeces--;
            delay(200);
          }
          if (digitalRead(BUTTON_RIGHT) == LOW && cantidadVeces < 6) {
            cantidadVeces++;
            delay(200);
          }
        }
        else if (estadoHorario == HORARIO_HORAS) {
          if (digitalRead(BUTTON_LEFT) == LOW) {
            horasAlimentacion[indiceHoraActual] = (horasAlimentacion[indiceHoraActual] - 1 + 24) % 24;
            delay(200);
          }
          if (digitalRead(BUTTON_RIGHT) == LOW) {
            horasAlimentacion[indiceHoraActual] = (horasAlimentacion[indiceHoraActual] + 1) % 24;
            delay(200);
          }
        }
      }
    }
  }
}
