#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <process.h>

cpu_status_t*  schedule(cpu_status_t*);
void  save_context(cpu_status_t*);
cpu_status_t*  init_scheduler();

#endif