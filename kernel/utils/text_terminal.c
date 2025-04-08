#include <stdint.h>
#include <vesa.h>
#include <mem.h>

typedef struct text_terminal {
    uint32_t x;
    uint32_t y;
    uint32_t  color;
    uint32_t height;
    uint32_t width;
} text_terminal_t;

typedef struct terminal_row {
    char characters[128];
    uint32_t length;
} terminal_row_t;


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

uint8_t glyphs[256][16] = {0};

PSFv2_t header;
text_terminal_t terminal;
terminal_row_t lines[96];

vbe_mode_info_structure_t *vesa_info;

void render_glyph(uint8_t *glyph, int x, int y) {
    // Assuming 8x16 pixel display
   for (int row = 0; row < 16; row++) {
        uint8_t row_data = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (row_data & (1 << (7 - col))) {
                SET_PIXELP(x + col, y + row);  // Draw pixel if the bit is set
            }
        }
    }
}

void clear_glyph(int x, int y) {
    // Assuming 8x16 pixel display
    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 8; col++) {
                CLEAR_PIXELP(x + col, y + row);
        }
    }
}


void print_char(char c, int x, int y)
{
    switch (c) {
        case -61:
            render_glyph(glyphs[228], x*header.width, y*header.height);
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
    for (y=0; y < terminal.height; y++) {

        // clear one row
        for (x=0; x < lines[y].length; x++) {
            clear_glyph(x*header.width, y*header.height); 
        }

        int a = 1;
        int b = 0;

        // update terminals rows in buffers 
        memset(lines[y].characters, 0, lines[y].length);
        memmove(lines[y].characters, lines[y+1].characters, lines[y+1].length);
        lines[y].length = lines[y+1].length;

        // write characters to screen
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

void print(char *string) {
    int i = 0;

    int len = strlen(string);

    for (i=0; i < len; i++) {

        // check if character is '\n'
        if (string[i] == 0xa) {
            // move cursor and move to next row buffer
            if (terminal.y+1 == terminal.height) {
                scroll();
            } else {
                terminal.y++;
            }
            terminal.x=0;
            continue;
        }

        // check if max width reached
        if (terminal.x == terminal.width)  {
            // move to next row buffer
            if (terminal.y+1 == terminal.height) {
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

uint16_t *install_font(uint64_t* font)
{
    memmove(&header, font, sizeof(PSFv2_t));

    uint8_t* ptr = (uint8_t*)font + sizeof(PSFv2_t);
    int i; 
    for (i = 0; i < 256; i++) {
        memmove(glyphs[i],  ptr+i*16, 16);
    }

    return &glyphs;
}

void draw_border()
{
    int x=0;
    int y=0;
    
    for (x  = 0; x < terminal.width*8; x++) {
        SET_PIXELP(x, y);
    }

    for (y = 0; y < terminal.height*16; y++) {
        SET_PIXELP(x, y);
        SET_PIXELP((x+terminal.width*8)-1, y);
    }

    for (x  = 0; x < terminal.width*8; x++) {
        SET_PIXELP(x, (terminal.height*16-1));
    }
}

void init_text_terminal(uint64_t* font)
{
    install_font(font);
    vesa_info =  get_vesainfo();
    terminal.x = 0;
    terminal.y = 0;
    terminal.height = (vesa_info->height/header.height);
    terminal.width = (vesa_info->width/header.width);
    terminal.color = 0x000f00;

}