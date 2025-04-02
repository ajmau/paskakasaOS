#include <vesa.h>
#include <mem.h>

vbe_mode_info_structure_t vesa_info;

void init_vesa(uint32_t p) 
{
    memmove(&vesa_info, (vbe_mode_info_structure_t*)p, sizeof(vbe_mode_info_structure_t));
}

void put_pixel(uint32_t x, uint32_t y)
{
    uint32_t* pixel_offset = (uint32_t*)(y * vesa_info.pitch + (x * (vesa_info.bpp/8)) + vesa_info.framebuffer);
    //uint32_t* pixel_offset = (uint32_t*)(vesa_info.framebuffer + (y * vesa_info.pitch) + (x * (vesa_info.bpp / 8)));
    *pixel_offset = 0xFF000000;

}

void render_glyph(uint8_t *glyph, int x, int y) {
    // Assuming 8x16 pixel display
    for (int row = 0; row < 16; row++) {
        uint8_t row_data = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (row_data & (1 << (7 - col))) {
                put_pixel(x + col, y + row);  // Draw pixel if the bit is set
            }
        }
    }
}