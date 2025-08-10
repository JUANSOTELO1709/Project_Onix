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

// ====================== Mostrar Menú Principal ======================
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

// ====================== Mostrar Submenú Cantidad ======================
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

// ====================== Mostrar Submenú Extracción ======================
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

// ====================== Mostrar Submenú Horario ======================
void mostrarSubMenuHorario() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Texto "Horario:" (centrado)
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 20, 10);
  display.println("Horario:");
  
  // Hora actual (grande y centrado)
  display.setTextSize(2);
  display.setCursor(SCREEN_WIDTH / 2 - 20, 30);
  if (hora < 10) display.print("0");  // Formato 08:00
  display.print(hora);
  display.print(":");
  if (minuto < 10) display.print("0");
  display.print(minuto);
  
  mostrarFlechas();
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

// ====================== Setup y Loop ======================
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

void loop() {
  static bool animacionHecha = false;  // Controla si la animación ya se mostró

  // ----- Animación Inicial -----
  if (!animacionHecha) {
    // Mostrar cada frame de la animación
    for (int i = 0; i < NUM_BITMAPS; i++) {
      display.clearDisplay();
      display.drawBitmap(0, 0, animacionBitmaps[i], 128, 64, SSD1306_WHITE);
      display.display();
      delay(animacionDelays[i]);
    }
    
    // Mensaje "Onix 1.0" después de la animación
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(5, 20);
    display.println("Onix 1.0");
    display.display();
    delay(1000);
    
    animacionHecha = true;  // Marcar animación como completada
  } 
  
  // ----- Lógica Principal de Menús -----
  else {
    // Mostrar menú principal o submenú según el estado
    if (!enSubMenu) {
      mostrarMenuPrincipal();  // Muestra CANTIDAD/EXTRAER/HORARIO en texto grande
    } else {
      // Mostrar submenú específico
      switch (menuActual) {
        case MENU_CANTIDAD:
          mostrarSubMenuCantidad();  // Ajuste de cantidad (10g, 20g...)
          break;
        case MENU_EXTRACCION:
          mostrarSubMenuExtraccion();  // Confirmación para extraer
          break;
        case MENU_HORARIO:
          mostrarSubMenuHorario();  // Ajuste de hora
          break;
      }
    }
    
    // Gestionar botones (navegación y acciones)
    controlarBotones();
  }
}