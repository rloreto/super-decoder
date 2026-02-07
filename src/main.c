#include "neslib.h"
#include <joystick.h>
#include <nes.h>

/* CRITICO: oam_off debe estar en ZEROPAGE */
/* clang-format off */
#pragma bss-name(push, "ZEROPAGE")
unsigned char oam_off;
#pragma bss-name(pop)
/* clang-format on */

unsigned char cursor_x, cursor_y;
unsigned char selected_color; // 0=ninguno, 1=rojo, 2=amarillo, 3=azul
unsigned char grid[4][4];     // Estado del grid (0=vacío, 1-3=colores)

const unsigned char palette[32] = {
    // Paletas de background
    0x0F, 0x30, 0x10, 0x20, // BG: negro, blanco, gris claro, gris oscuro
    0x0F, 0x30, 0x10, 0x20, 0x0F, 0x30, 0x10, 0x20, 0x0F, 0x30, 0x10, 0x20,

    // Paletas de sprites (colores muy contrastantes)
    0x0F, 0x00, 0x10, 0x30, // Paleta 0: cursor - negro, gris, blanco
    0x0F, 0x00, 0x00, 0x16, // Paleta 1: ROJO BRILLANTE (color 3)
    0x0F, 0x00, 0x00, 0x27, // Paleta 2: NARANJA BRILLANTE (color 3)
    0x0F, 0x00, 0x00, 0x11  // Paleta 3: AZUL BRILLANTE (color 3)
};

void main(void) {
  unsigned char x, y, row, i, j;
  unsigned char ctrl, prev_ctrl = 0;

  cursor_x = 0;
  cursor_y = 0;
  selected_color = 1; // Empezar con rojo

  // Limpiar grid
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      grid[i][j] = 0;
    }
  }

  joy_install(joy_static_stddrv);

  // Configurar pantalla (VOLVER A LA VERSION QUE FUNCIONABA)
  ppu_off();

  // Cargar paletas: pal_bg recibe los primeros 16 bytes, pal_spr los siguientes
  // 16
  pal_bg(palette);
  pal_spr(palette +
          16); // Apuntar a la segunda mitad del array (paletas de sprites)

  // Llenar pantalla con tile 1 (blanco)
  vram_adr(NAMETABLE_A);
  vram_fill(1, 960);

  // Dibujar grid 4x4 con tile 0 (negro)
  for (y = 0; y < 4; y++) {
    for (x = 0; x < 4; x++) {
      unsigned char tx = 8 + x * 5;
      unsigned char ty = 6 + y * 5;
      for (row = 0; row < 4; row++) {
        vram_adr(NTADR_A(tx, ty + row));
        vram_put(0);
        vram_put(0);
        vram_put(0);
        vram_put(0);
      }
    }
  }

  ppu_on_all();

  // Loop principal
  while (1) {
    // Limpiar y preparar sprites
    oam_off = 0;

    // Dibujar colores en el grid (16 sprites por celda = 4x4)
    for (y = 0; y < 4; y++) {
      for (x = 0; x < 4; x++) {
        if (grid[x][y] > 0) {
          unsigned char px = 64 + x * 40;
          unsigned char py = 47 + y * 40;
          unsigned char color_pal = grid[x][y];

          // Llenar la celda con sprites (4x4 = 16 sprites)
          for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
              oam_off = oam_spr(px + j * 8, py + i * 8, 1, color_pal, oam_off);
            }
          }
        }
      }
    }

    // Indicador de color seleccionado (esquina superior izquierda, 2x2 sprites)
    for (i = 0; i < 2; i++) {
      for (j = 0; j < 2; j++) {
        oam_off = oam_spr(16 + j * 8, 14 + i * 8, 1, selected_color, oam_off);
      }
    }

    // Dibujar cursor (4 sprites en las esquinas)
    {
      unsigned char px = 64 + cursor_x * 40;
      unsigned char py = 47 + cursor_y * 40;

      oam_off = oam_spr(px, py, 1, 0, oam_off);
      oam_off = oam_spr(px + 24, py, 1, 0, oam_off);
      oam_off = oam_spr(px, py + 24, 1, 0, oam_off);
      oam_off = oam_spr(px + 24, py + 24, 1, 0, oam_off);
      oam_hide_rest(oam_off);
    }

    // Leer controles
    ctrl = joy_read(0);

    // Movimiento del cursor
    if ((ctrl & PAD_UP) && !(prev_ctrl & PAD_UP) && cursor_y > 0)
      cursor_y--;
    if ((ctrl & PAD_DOWN) && !(prev_ctrl & PAD_DOWN) && cursor_y < 3)
      cursor_y++;
    if ((ctrl & PAD_LEFT) && !(prev_ctrl & PAD_LEFT) && cursor_x > 0)
      cursor_x--;
    if ((ctrl & PAD_RIGHT) && !(prev_ctrl & PAD_RIGHT) && cursor_x < 3)
      cursor_x++;

    // Cambiar color seleccionado (Select = izquierda, Start = derecha)
    if ((ctrl & PAD_SELECT) && !(prev_ctrl & PAD_SELECT)) {
      selected_color--;
      if (selected_color == 0)
        selected_color = 3;
    }
    if ((ctrl & PAD_START) && !(prev_ctrl & PAD_START)) {
      selected_color++;
      if (selected_color > 3)
        selected_color = 1;
    }

    // Colocar color (botón B)
    if ((ctrl & PAD_B) && !(prev_ctrl & PAD_B)) {
      grid[cursor_x][cursor_y] = selected_color;
    }

    // Borrar (botón A)
    if ((ctrl & PAD_A) && !(prev_ctrl & PAD_A)) {
      grid[cursor_x][cursor_y] = 0;
    }

    prev_ctrl = ctrl;

    ppu_wait_nmi();
  }
}
