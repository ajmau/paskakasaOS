#include <process.h>
#include <interrupts.h>
#include <text_terminal.h>
#include <alloc.h>
#include <portio.h>

extern  void context_switch(process_t* process);

process_t processes[4];

uint16_t sounds[5] = { 523, 440, 380, 680, 523 };

static void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
    //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
}

void processC()
{
    while (1) {
        for (int y = 0; y < 5; y++) {
            int val = get_val();
            play_sound(sounds[y]);

            while ((get_val() - val) < 100) {}

        }
    }
}

void processA()
{
    uint64_t xA = 0;
    while (1) {
        asm("cli");
        print("EKA ");
        print_hex(xA);
        asm("sti");
        xA++;
    }
}

void processB()
{
    uint64_t xB = 0xFFFFFF;
    while (1) {
        asm("cli");
        print("TOINEN ");
        print_hex(xB);
        asm("sti");
        xB++;
    }
}

void switch_to()
{
    context_switch(&processes[0]);
}

uint64_t alloc_stack()
{
    uint64_t allocation = (uint64_t)kalloc(0x4000);
    return allocation+0x4000;
}

void init_scheduler()
{
    processes[3].process_status = READY;
    processes[3].context.rbp = 0;
    processes[3].context.iret_ss = 0x10;
    processes[3].context.iret_rsp = (uint64_t)alloc_stack();
    processes[3].context.iret_flags = 0x202;
    processes[3].context.iret_cs = 0x8;
    processes[3].context.iret_rip = (uint64_t)processC;
    processes[3].context.rdi = 0;

    processes[1].process_status = READY;
    processes[1].context.rbp = 0;
    processes[1].context.iret_ss = 0x10;
    processes[1].context.iret_rsp = (uint64_t)alloc_stack();
    processes[1].context.iret_flags = 0x202;
    processes[1].context.iret_cs = 0x8;
    processes[1].context.iret_rip = (uint64_t)processA;
    processes[1].context.rdi = 0;

    processes[2].process_status = READY;
    processes[2].context.rbp = 0;
    processes[2].context.iret_ss = 0x10;
    processes[2].context.iret_rsp = (uint64_t)alloc_stack();
    processes[2].context.iret_flags = 0x202;
    processes[2].context.iret_cs = 0x8;
    processes[2].context.iret_rip = (uint64_t)processB;
    processes[2].context.rdi = 0;

}

int counter=0;

void save_context(cpu_status_t *current)
{
    processes[counter].process_status = READY;
    processes[counter].context.r15    = current->r15;
    processes[counter].context.r14    = current->r14;
    processes[counter].context.r13    = current->r13;
    processes[counter].context.r12    = current->r12;
    processes[counter].context.r11    = current->r11;
    processes[counter].context.r10    = current->r10;
    processes[counter].context.r9     = current->r9;
    processes[counter].context.r8     = current->r8;
    processes[counter].context.rbp    = current->rbp;
    processes[counter].context.rdi    = current->rdi;
    processes[counter].context.rsi    = current->rsi;
    processes[counter].context.rdx    = current->rdx;
    processes[counter].context.rcx    = current->rcx;
    processes[counter].context.rax    = current->rax;
    processes[counter].context.iret_rip   = current->iret_rip;
    processes[counter].context.iret_cs    = current->iret_cs;
    processes[counter].context.iret_flags = current->iret_flags;
    processes[counter].context.iret_rsp   = current->iret_rsp;
    processes[counter].context.iret_ss    = current->iret_ss;

    if (counter == 3) {
        counter = 0;
    }  else {
        counter++;
    }
}

cpu_status_t* schedule(cpu_status_t *current)
{
    return &processes[counter].context;
}