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
// Variable que indica si estás dentro del submenú "Veces al día"
bool enSubMenuVeces = false;

// Variable que almacena la cantidad de veces al día (rango de 1 a 5)
int vecesAlDia = 1;

enum MenuState {
  MENU_TIEMPO,
  MENU_NOMBRE,
  MENU_COMIDA,
  MENU_TOTAL
};

MenuState menuActual = MENU_TIEMPO;

void mostrarMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);

switch (menuActual) {

  case MENU_TIEMPO:
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Horario alimentacion");

    if (enSubMenuVeces) {
      display.setCursor(10, 20);
      display.println("Veces al dia:");
      display.setTextSize(2);
      display.setCursor(50, 40);
      display.println(vecesAlDia);
    } else {
      display.setCursor(10, 35);
      display.setTextSize(1);
      display.println("Reloj: --:--");
      display.setCursor(0, 50);
      display.println("Press SELECT para editar");
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
