#include <vesa.h>
#include <mem.h>


vbe_mode_info_structure_t info;

void init_vesa(uint32_t p) 
{
    memmove(&info, (vbe_mode_info_structure_t*)p, sizeof(vbe_mode_info_structure_t));
}

uint32_t get_framebuffer()
{
    return info.framebuffer;
}

vbe_mode_info_structure_t *get_vesainfo()
{
    return &info;
}

void draw_rectangle(int x, int y, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            SET_PIXEL_COLOR(x+j, y+i, 0x00ff00);
        }
    }
}