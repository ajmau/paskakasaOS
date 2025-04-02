#include <mem.h>
#include <font.h>

typedef struct PSFv2 {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyphs;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
} __attribute__((packed)) PSFv2_t;

PSFv2_t header;

uint8_t glyphs[256][16] = {0};

uint8_t *install_font(uint64_t* fonts)
{
    memmove(&header, fonts, sizeof(PSFv2_t));


    uint8_t* ptr = (uint8_t*)fonts + sizeof(PSFv2_t);
    int i; 
    for (i = 0; i < 256; i++) {
        memmove(glyphs[i],  ptr+i*16, 16);
    }


    return &glyphs;
}