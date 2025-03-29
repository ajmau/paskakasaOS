ENTRY(_start)

SECTIONS {
    . = 0xD000;           /* Set the base address to 0x7C00 */

    .text : {
        *(.text)          /* Place all text sections (code) here */
    }

    .data : {
        *(.data)          /* Place all data sections (including constants) here */
    }
    
    .bss : {
        *(.bss)           /* BSS section for uninitialized data */
    }
    _end = .;
}
