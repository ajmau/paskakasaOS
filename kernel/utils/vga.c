#include <stdint.h>

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

#define VGA_WIDTH   160
#define VGA_HEIGHT  25

struct position  {
    uint32_t x;
    uint32_t y;
    uint8_t color;
    char* buffer;
};

struct position position;

void init_vga()
{
    position.x = 0;
    position.y = 0;
    position.color = 0xa;
    position.buffer = (char*)0xb8000;
}

void newline()
{
    // scroll
    if (position.y == 25) {

        position.y =  0;
        position.x =  0;
    }
    position.x = 0;
    position.y++;
}

void write_char(char c)
{
    if (c == 'x') {
        position.x = 0;
        if (position.y != 25) {
            position.y++;
        }
        return; 
    }
  
    if (position.x == 80) {
        position.x = 0;
        if (position.y != 25) {
           position.y++;
        }
    } else {
        position.x+=2;
    }

    //uitn16_t pos = position.x+position.y*VGA_WIDTH

    *(position.buffer+position.x+position.y*VGA_WIDTH) = c;
    *(position.buffer+position.x+position.y*VGA_WIDTH+1) = position.color;
}

void write_message(char *message, uint32_t len, uint32_t line) 
{
    //char *buffer = position.buffer+(line*160);
    if (position.y == 25)  {
        int y;
        for (y=1; y < 25; y++) {
            int x;
            for (x = 0; x < 160; x++) {
                uint8_t character = *(position.buffer+x+y*VGA_WIDTH);
                *(position.buffer+x+((y-1)*VGA_WIDTH)) = character;
         }
        }
    }
    uint32_t i;
    for (i=0; i < len; i++) {
        write_char(message[i]);
    }
}