#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "animacion.h" 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_SELECT 4
bool enSubMenuVeces = false;

// Variable que almacena la cantidad de veces al día (rango de 1 a 5)
int vecesAlDia = 1;
int submenuTiempoIndex = 0;  // 0: título, 1: explicación, 2: selección veces
const int submenuTiempoTotal = 3; // Total de pantallas dentro del menú

enum MenuState {
  MENU_TIEMPO,
  MENU_NOMBRE,
  MENU_COMIDA,
  MENU_TOTAL
};

MenuState menuActual = MENU_TIEMPO;

void controlarBotonSelect() {
  if (digitalRead(BUTTON_SELECT) == LOW) {
    if (menuActual == MENU_TIEMPO) {
      submenuTiempoIndex = (submenuTiempoIndex + 1) % submenuTiempoTotal;
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

 case MENU_TIEMPO:

    display.setTextSize(1);
    display.setCursor(0, 0);

    if (submenuTiempoIndex == 0) {
      display.setTextSize(2);
      display.println("Horario");
      display.setCursor(0, 20);
      display.println("alimentacion");

    } else if (submenuTiempoIndex == 1) {
      display.setTextSize(1);
      display.setCursor(10, 35);
      display.println("Elija cantidad de");
      display.setCursor(10, 45);
      display.println("veces al dia (01 - 05)");

    } else if (submenuTiempoIndex == 2) {
      display.setTextSize(1);
      display.setCursor(10, 20);
      display.println("Veces al dia:");
      display.setTextSize(2);
      display.setCursor(50, 40);
      display.println(vecesAlDia);
    }

    break;

  case MENU_NOMBRE:
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Mascota");
    display.setCursor(10, 35);
    display.println("Nombre: Onix");
    break;


    case MENU_COMIDA:
      display.println("Comida");
      display.setTextSize(1);
      display.setCursor(10, 35);
      display.println("Cantidad: 50g");
      break;
  }


  display.display();

}

void setup() {
  Serial.begin(115200);
  delay(100);

  // ¡Ya no usamos Wire.setSDA ni setSCL!
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
    display.setTextSize(2.5);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 20);
    display.println("Onix 1.0");
    display.display();
    delay(1000);
  } else {

    



    mostrarMenu();

    // Navegación con botones
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
