#ifndef PROCESS_H
#define PROCESS_H
#include <interrupts.h>

typedef enum {
    READY,
    RUNNING,
    DEAD
} status_t;

typedef struct process_t {
    status_t process_status;
    cpu_status_t context;
    struct process_t* next;
} process_t;

#endif