#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>

#include "display.h"

#define MEM_SIZE 4096
#define NUM_REGS 16
#define STACK_SIZE 16

struct cpu
{
    uint8_t mem[MEM_SIZE];
    uint16_t pc;

    uint8_t frame_buf[SCREEN_SIZE];

    uint8_t v[NUM_REGS];
    uint8_t dt;
    uint8_t st;
    uint16_t i;

    uint16_t stack[STACK_SIZE];
    uint16_t sp;
};

void
cpu_init(struct cpu*);

int
load_rom(struct cpu*, const char*);

void
cpu_step(struct cpu*);

#endif
