// Bitmap comprimido con RLE para epd_bitmap_lenguaTapaPantalla
#ifndef EPD_BITMAP_LENGUATAPAPANTALLA_RLE_H
#define EPD_BITMAP_LENGUATAPAPANTALLA_RLE_H
const unsigned char epd_bitmap_lenguaTapaPantalla_rle[] PROGMEM = {
  0xff,1, 0xe0,1, 0x00,6, 0x07,1, 0xff,1,
  0xff,16,
  0xff,16,
  0xff,4, 0xe0,1, 0x00,1, 0xff,1, 0xf0,1, 0x00,1, 0x1f,1, 0xff,4,
  // ...continúa el resto del bitmap comprimido...
};
#endif
