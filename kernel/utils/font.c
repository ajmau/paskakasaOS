#include <mem.h>
#include <font.h>
#include <vesa.h>


uint16_t *install_font(uint64_t* fonts)
{
    memmove(&header, fonts, sizeof(PSFv2_t));


    uint8_t* ptr = (uint8_t*)fonts + sizeof(PSFv2_t);
    int i; 
    for (i = 0; i < 256; i++) {
        memmove(glyphs[i],  ptr+i*16, 16);
    }

    return &glyphs;
}