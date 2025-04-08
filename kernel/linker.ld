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
         . = ALIGN(16);

    }

    kernel_end = .;

    . += 0x10000; /*padding */
    .stack ALIGN(16K) : {
        PROVIDE(stack_bottom = .);
        . = . + 0x20000; 
        PROVIDE(stack_top = .);
    }
}

