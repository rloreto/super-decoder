#include <joystick.h>
#include <nes.h>
#include "neslib.h"

// CRITICO: oam_off debe estar en ZEROPAGE
#pragma bss-name(push, "ZEROPAGE")
unsigned char oam_off;
#pragma bss-name(pop)

const unsigned char palette[16] = {
    0x0F, 0x30, 0x00, 0x10,  // BG: negro, blanco, negro, gris
    0x0F, 0x30, 0x00, 0x10,
    0x0F, 0x30, 0x00, 0x10,
    0x0F, 0x30, 0x00, 0x10
};

void main(void) {
    // Apagar PPU
    ppu_off();

    // Cargar paleta
    pal_bg(palette);
    pal_spr(palette);

    // Llenar toda la pantalla con tile 1 (blanco)
    vram_adr(NAMETABLE_A);
    vram_fill(1, 960);

    // Encender PPU
    ppu_on_all();

    // Loop principal
    while(1) {
        // Limpiar sprites
        oam_off = 0;

        // Dibujar UN sprite en el centro
        oam_off = oam_spr(128, 120, 1, 0, oam_off);
        oam_hide_rest(oam_off);

        ppu_wait_nmi();
    }
}
