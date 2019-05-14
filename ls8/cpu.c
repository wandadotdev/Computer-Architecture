#include "cpu.h"
#include <string.h>
#include <stdio.h>

#define DATA_LEN 6

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu)
{
  char data[DATA_LEN] = {
    // From print8.ls8
    0b10000010, // LDI R0,8
    0b00000000,
    0b00001000,
    0b01000111, // PRN R0
    0b00000000,
    0b00000001  // HLT
  };

  int address = 0;

  for (int i = 0; i < DATA_LEN; i++) {
    cpu->ram[address++] = data[i];
  }

  // TODO: Replace this with something less hard-coded
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      // TODO
      break;

    // TODO: implement more ALU ops
  }
}

/* In `cpu.c`, add functions `cpu_ram_read()` and `cpu_ram_write()` that access theRAM inside the `struct cpu`. */

//read needs address in ram
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char address) 
{
  return cpu->ram[address];
}

//write needs address in ram and the value at to write
unsigned char cpu_ram_write(struct cpu *cpu, unsigned char address, unsigned char value)
{
  return cpu->ram[address] = value;
  //need to write
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction, false is 0

  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char IR = cpu_ram_read(cpu, cpu->PC); //mem address stored in PC
    // 2. Figure out how many operands this next instruction requires
    unsigned char num_operands = IR >> 6;
    // 3. Get the appropriate value(s) of the operands following this instruction
    unsigned char operandA = cpu_ram_read(cpu, cpu->PC + 1);
    unsigned char operandB = cpu_ram_read(cpu, cpu->PC + 2);
    // 4. switch() over it to decide on a course of action.
    switch(IR)
    {
    // 5. Do whatever the instruction should do according to the spec.
      case LDI:
        // Set the value of a register to an integer.
        cpu->registers[operandA] = operandB;
        break;

      case PRN:
        printf("%d\n", cpu->registers[operandA]);
        break;

      case HLT:
        running = 0;
        break;

      default:
        printf("Error!\n");
        break;  
    }
    // 6. Move the PC to the next instruction.
    cpu->PC += (num_operands + 1);
  }
}

/**
 * Initialize a CPU struct
 */
/* The `cpu_init()` function takes a pointer to a `struct cpu` and initializes it as necessary. At first, the PC, registers, and RAM should be cleared to zero.
(`memset()` might help you clear registers and RAM.) */
// https://www.tutorialspoint.com/c_standard_library/c_function_memset.htm
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->PC = 0;
  memset(cpu->registers, 0, 8);
  memset(cpu->ram, 0, 256);

}

