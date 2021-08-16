#include <stdlib.h>
#include <string.h>

#include "cpu.h"

#define PC_START 0x200

#define ADDR(i)     (i & 0x0FFF)
#define NIBBLE(i)   (i & 0xF)
#define X(i)        ((i >> 8) & 0x0F)
#define Y(i)        ((i >> 4) & 0x0F)
#define BYTE(i)     (i & 0xFF)
#define HIGH_NIB(i) (i >> 12)

static const uint8_t font_sprites[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
    0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
    0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
    0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
    0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
    0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
    0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
    0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
    0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
    0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
    0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
    0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
    0xE0, 0x80, 0x80, 0x80, 0xE0, /* D */
    0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
    0xF0, 0x80, 0xF0, 0x80, 0x80, /* F */
};

static void
nibble_zero(struct cpu* cpu, uint16_t op)
{
    switch (op) {
        case 0x00E0:
            memset(cpu->frame_buf, 0, SCREEN_SIZE);
            break;
        case 0x00EE:
            cpu->pc = cpu->stack[--cpu->sp];
            break;
    }
}

static void
nibble_one(struct cpu* cpu, uint16_t op)
{
    cpu->pc = ADDR(op);
}

static void
nibble_two(struct cpu* cpu, uint16_t op)
{
    cpu->stack[cpu->sp++] = cpu->pc;
    cpu->pc = ADDR(op);
}

static void
nibble_three(struct cpu* cpu, uint16_t op)
{
    if (cpu->v[X(op)] == BYTE(op)) {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
    }
}

static void
nibble_four(struct cpu* cpu, uint16_t op)
{
    if (cpu->v[X(op)] != BYTE(op)) {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
    }
}

static void
nibble_five(struct cpu* cpu, uint16_t op)
{
    if (cpu->v[X(op)] == cpu->v[Y(op)]) {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
    }
}

static void
nibble_six(struct cpu* cpu, uint16_t op)
{
    cpu->v[X(op)] = BYTE(op);
}

static void
nibble_seven(struct cpu* cpu, uint16_t op)
{
    cpu->v[X(op)] += BYTE(op);
}

static void
nibble_eight(struct cpu* cpu, uint16_t op)
{
    uint8_t x, y;
    x = X(op);
    y = Y(op);
    switch (NIBBLE(op)) {
        case 0x0:
            cpu->v[x] = cpu->v[y];
            break;
        case 0x1:
            cpu->v[x] |= cpu->v[y];
            break;
        case 0x2:
            cpu->v[x] &= cpu->v[y];
            break;
        case 0x3:
            cpu->v[x] ^= cpu->v[y];
            break;
        case 0x4:
            cpu->v[0xF] = cpu->v[x] > ((cpu->v[x] + cpu->v[y]) & 0xFF);
            cpu->v[x] += cpu->v[y];
            break;
        case 0x5:
            cpu->v[0xF] = cpu->v[x] > cpu->v[y];
            cpu->v[x] -= cpu->v[y];
            break;
        case 0x6:
            cpu->v[0xF] = cpu->v[x] & 1;
            cpu->v[x] >>= 1;
            break;
        case 0x7:
            cpu->v[0xF] = cpu->v[y] > cpu->v[x];
            cpu->v[x] = cpu->v[y] - cpu->v[x];
            break;
        case 0xE:
            cpu->v[0xF] = (cpu->v[x] >> 7) & 1;
            cpu->v[x] <<= 1;
            break;
    }
}

static void
nibble_nine(struct cpu* cpu, uint16_t op)
{
    if (cpu->v[X(op)] != cpu->v[Y(op)]) {
        cpu->pc = (cpu->pc + 2) & 0xFFF;
    }
}

static void
nibble_a(struct cpu* cpu, uint16_t op)
{
    cpu->i = ADDR(op);
}

static void
nibble_b(struct cpu* cpu, uint16_t op)
{
    cpu->pc = (ADDR(op) + cpu->v[0]) & 0xFFF;
}

static void
nibble_c(struct cpu* cpu, uint16_t op)
{
    cpu->v[X(op)] = (rand() % 256) & BYTE(op);
}

static void
nibble_d(struct cpu* cpu, uint16_t op)
{
    uint8_t x, y, n, byte;
    x = X(op);
    y = Y(op);
    n = NIBBLE(op);
    cpu->v[0xF] = 0;
    for (int byte_offset = 0; byte_offset < n; byte_offset++) {
        byte = cpu->mem[cpu->i + byte_offset];
        for (int bit_offset = 0; bit_offset < 8; bit_offset++) {
            int bit = (byte & (0x80 >> bit_offset)) != 0;
            int px = (cpu->v[x] + bit_offset) % SCREEN_WIDTH;
            int py = (cpu->v[y] + byte_offset) % SCREEN_HEIGHT;
            int pos = px + py * SCREEN_WIDTH;
            cpu->v[0xF] |= (cpu->frame_buf[pos] & bit);
            cpu->frame_buf[pos] ^= bit;
        }
    }
}

static void
nibble_e(struct cpu* cpu, uint16_t op)
{
    switch (BYTE(op)) {
        case 0x9E:
            /* TODO */
            break;
        case 0xA1:
            /* TODO */
            break;
    }
}

static void
nibble_f(struct cpu* cpu, uint16_t op)
{
    uint8_t x = X(op);
    switch (BYTE(op)) {
        case 0x07:
            cpu->v[x] = cpu->dt;
            break;
        case 0x0A:
            /* TODO */
            break;
        case 0x15:
            cpu->dt = cpu->v[x];
            break;
        case 0x18:
            cpu->st = cpu->v[x];
            break;
        case 0x1E:
            cpu->i += cpu->v[x];
            break;
        case 0x29:
            cpu->i = 5 * (cpu->v[x] & 0xF);
            break;
        case 0x33:
            cpu->mem[cpu->i] = cpu->v[x] / 100;
            cpu->mem[cpu->i + 1] = (cpu->v[x] / 10) % 10;
            cpu->mem[cpu->i + 2] = cpu->v[x] % 10;
            break;
        case 0x55:
            for (int r = 0; r <= x; r++) {
                cpu->mem[cpu->i + r] = cpu->v[r];
            }
            break;
        case 0x65:
            for (int r = 0; r <= x; r++) {
                cpu->v[r] = cpu->mem[cpu->i + r];
            }
            break;
    }
}

typedef void (*operation)(struct cpu*, uint16_t);

static const operation operations[16] = {
    &nibble_zero,  &nibble_one,  &nibble_two, &nibble_three,
    &nibble_four,  &nibble_five, &nibble_six, &nibble_seven,
    &nibble_eight, &nibble_nine, &nibble_a,   &nibble_b,
    &nibble_c,     &nibble_d,    &nibble_e,   &nibble_f,
};

void
cpu_init(struct cpu* cpu)
{
    memset(cpu, 0, sizeof(struct cpu));
    memcpy(cpu->mem, font_sprites, 80);
    cpu->pc = PC_START;
}

int
load_rom(struct cpu* cpu, const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file) {
        return 1;
    }

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    fseek(file, 0, SEEK_SET);

    fread(cpu->mem + PC_START, len, 1, file);
    fclose(file);

    return 0;
}

void
cpu_step(struct cpu* cpu)
{
    uint16_t op;
    /* Fetch next instruction */
    op = cpu->mem[cpu->pc] << 8 | cpu->mem[cpu->pc + 1];
    cpu->pc = (cpu->pc + 2) & 0xFFF;
    /* decode/execute */
    operations[HIGH_NIB(op)](cpu, op);
}
