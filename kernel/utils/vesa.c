#include <vesa.h>
#include <mem.h>

#define CLEAR_PIXEL(x, y) (*(uint32_t*)((y) * vesa_info.pitch + ((x) * (vesa_info.bpp / 8)) + vesa_info.framebuffer) = 0x0)
#define SET_PIXEL(x, y) (*(uint32_t*)((y) * vesa_info.pitch + ((x) * (vesa_info.bpp / 8)) + vesa_info.framebuffer) = terminal.color)


typedef struct text_terminal {
    uint32_t x;
    uint32_t y;
    uint32_t  color;
} text_terminal_t;

typedef struct terminal_row {
    char characters[128];
    uint32_t length;
} terminal_row_t;

vbe_mode_info_structure_t vesa_info;
PSFv2_t header;
text_terminal_t terminal;

uint8_t glyphs[256][16] = {0};

uint8_t off = {0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

terminal_row_t lines[96];

void print_string(char *string, int len);

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

void init_vesa(uint32_t p, uint64_t* font) 
{
    memmove(&vesa_info, (vbe_mode_info_structure_t*)p, sizeof(vbe_mode_info_structure_t));
    install_font(font);
    terminal.x = 0;
    terminal.y = 0;
    terminal.color = 0xFF;

}

uint8_t get_byte(uint32_t x, uint32_t y)
{
    uint32_t* pixel_offset = (uint32_t*)(y * vesa_info.pitch + (x * (vesa_info.bpp/8)) + vesa_info.framebuffer);
    return *pixel_offset;
}

void put_pixel(uint32_t x, uint32_t y)
{
    uint32_t* pixel_offset = (uint32_t*)(y * vesa_info.pitch + (x * (vesa_info.bpp/8)) + vesa_info.framebuffer);
    *pixel_offset = terminal.color;
}

void clear_pixel(uint32_t x, uint32_t y)
{
    uint32_t* pixel_offset = (uint32_t*)(y * vesa_info.pitch + (x * (vesa_info.bpp/8)) + vesa_info.framebuffer);
    *pixel_offset = 0x0;
}

void render_glyph(uint8_t *glyph, int x, int y) {
    // Assuming 8x16 pixel display
    for (int row = 0; row < 16; row++) {
        uint8_t row_data = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (row_data & (1 << (7 - col))) {
                SET_PIXEL(x + col, y + row);  // Draw pixel if the bit is set
            }
        }
    }
}

void clear_glyph(int x, int y) {
    // Assuming 8x16 pixel display
    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 8; col++) {
                CLEAR_PIXEL(x + col, y + row);  // Draw pixel if the bit is set
        }
    }
}

void print_char(char c, int x, int y)
{
    render_glyph(glyphs[c], terminal.x*header.width, terminal.y*header.height);
}

void scroll_glyph(int x, int y) {
    for (int row = x; row < 16; row++) {
        uint8_t row_data = get_byte(x, y);
        for (int col = y; col < 8; col++) {
            if (row_data & (1 << (7 - col))) {
                SET_PIXEL(x + col, y + row);  // Draw pixel if the bit is set
            }
        }
    }
}

void scroll_row(int row)
{
    int x;
    for (x=0; x < (vesa_info.width/8); x++) {
        scroll_glyph(x, row);
    }
}

void scroll()
{
    terminal.x = 0;
    terminal.y = 0;
    int x=0;
    int y=0;
    // clear screen
    for (y=0; y < (vesa_info.height); y++) {
        for (x=0; x < (vesa_info.width); x++) {
            CLEAR_PIXEL(x, y);
        }
    }
    int a = 1;
    int b = 0;

    // scroll lines buffer
    for (a=1; a < 96; a++) {
        memmove(lines[a-1].characters, lines[a].characters, 128);
        lines[a-1].length = lines[a].length;
    }

    for (a=1; a < (vesa_info.height/16); a++) {
        for (b = 0; b<lines[a].length; b++) {
            if (lines[a].characters[b] != 0xa) {
             print_char(lines[a].characters[b], terminal.x, terminal.y);
            }
            terminal.x++;
        }
        terminal.y++;
        terminal.x=0;
    }

}

void print_string(char *string, int len) {
    int i = 0;
    for (i; i < len; i++) {
        if (terminal.x+1 == (vesa_info.width/8)) {
            terminal.x = 0;
            terminal.y++;
        }

        if (terminal.y == (vesa_info.height/16)) {
            scroll();
        }

        if (string[i] == 0xa) { // 0xa = /n
            terminal.x=0;
            terminal.y++;
            continue;
        }
        print_char(string[i], terminal.x++, terminal.y);
    }
    memmove(lines[terminal.y].characters, string, len);
    lines[terminal.y].length = len;
}