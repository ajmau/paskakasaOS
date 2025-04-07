ENTRY(_start)

SECTIONS {
    . = 0xFFFFFFFF80000000;           

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
