ENTRY(_start)

SECTIONS {
    . = 0x100000;           

    kernel_start = .;
    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }
    
    .bss : {
        *(.bss)
    }
    kernel_end = .;
}
