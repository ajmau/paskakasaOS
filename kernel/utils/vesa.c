#include <vesa.h>
#include <mem.h>

#define CLEAR_PIXEL(x, y) (*(uint32_t*)((y) * vesa_info.pitch + ((x) * (vesa_info.bpp / 8)) + vesa_info.framebuffer) = 0x0)
#define SET_PIXEL(x, y) (*(uint32_t*)((y) * vesa_info.pitch + ((x) * (vesa_info.bpp / 8)) + vesa_info.framebuffer) = terminal.color)
#define SET_PIXEL_COLOR(x, y,c) (*(uint32_t*)((y) * vesa_info.pitch + ((x) * (vesa_info.bpp / 8)) + vesa_info.framebuffer) = c)

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
    terminal.color = 0x000f00;

}

uint32_t get_framebuffer()
{
    return vesa_info.framebuffer;
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
    switch (c) {
        case -61:
            render_glyph(glyphs[246], x*header.width, y*header.height);
            break;
        default:
            render_glyph(glyphs[c], x*header.width, y*header.height);
    }
}

void scroll()
{
    int tx = 0;
    int ty = 0;
    int x=0;
    int y=0;
    // clear screen
    for (y=0; y < 49; y++) {
        for (x=0; x < (vesa_info.width/8); x++) {
            clear_glyph(x*header.width, y*header.height); // clear one row
        }

    int a = 1;
    int b = 0;

    memset(lines[y].characters, 0, 128);
    memmove(lines[y].characters, lines[y+1].characters, 128);
    lines[y].length = lines[y+1].length;

    for (b = 0; b<lines[y].length; b++) {
        if (lines[y].characters[b] != 0xa) {
            print_char(lines[y].characters[b], tx, ty);
        }
        tx++;
    }
    ty++;
    tx=0;
}

}

void clear_row(int line) {
    int i;
    for (i=0; i < (vesa_info.width/8); i++) {
        clear_glyph(i*header.width, line*header.height);
    }
}

void print_string(char *string, int len) {
    int i = 0;
    int height = (vesa_info.height/16);

    for (i=0; i < len; i++) {

        // check if character is '\n'
        if (string[i] == 0xa) {

            // move cursor and move to next row buffer
            if (terminal.y+1 == height) {
                scroll();
            } else {
                terminal.y++;
            }
            terminal.x=0;
        }

        // check if max width reached
        if (terminal.x == (vesa_info.width/8))  {
            // move to next row buffer
            if (terminal.y+1 == height) {
                scroll();
            } else {
                terminal.y++;
            }
            terminal.x=0;
        }

        print_char(string[i], terminal.x, terminal.y);
        lines[terminal.y].characters[terminal.x] = string[i];
        lines[terminal.y].length++; 

        // move cursor
        terminal.x++;

    }
}

void put_pixel(uint32_t x, uint32_t y) {
    SET_PIXEL(x, y);
}

void draw_rectangle(int x, int y, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            //uint32_t *pixel_offset = (uint32_t*)((y + i) * vesa_info.width + (x + j))
            SET_PIXEL_COLOR(x+j, y+i, 0x00ff00);
        }
    }
}