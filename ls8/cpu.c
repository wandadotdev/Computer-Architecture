#include "cpu.h"
#include <string.h>
#include <stdlib.h> //exit
#include <stdio.h>

#define DATA_LEN 6

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, int argc, char *argv[])
{
  // TODO: Replace this with something less hard-coded
  // https://www.tutorialspoint.com/c_standard_library/c_function_strtoul.htm

  FILE *fp = fopen(argv[1], "r");
  int address = 0;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: ls8 filename.ls8\n");
    exit(1);
  }

  char line[1028];

  if(fp == NULL)
  {
    fprintf(stderr, "comp: error opening file \"%s\"\n", argv[1]);
    exit(2);      
  }

  while (fgets(line, sizeof line, fp) != NULL)
  {
    char *endptr;

    unsigned char value = strtoul(line, &endptr, 2); //converts strings to nums

    if (endptr == line) {
      continue;
    }

    cpu->ram[address++] = value;

  }

}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      // TODO
      cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
      break;

    // TODO: implement more ALU ops
    case ALU_ADD:
       cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];
      break;
    
    case ALU_CMP:
      
      cpu->FL = 0;
      // printf("regA-CMP: %d\n", cpu->registers[regA]);
      // printf("regB-CMP: %d\n", cpu->registers[regB]);
      // printf("flag before : %d\n", cpu->FL);

      if (cpu->registers[regA] == cpu->registers[regB])
      {
        cpu->FL = 1;
      }
      
      // printf("flag after: %d\n", cpu->FL);
      break;
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

      case MUL:
        alu(cpu, ALU_MUL, operandA, operandB);
        break;
      
      case PUSH:
        cpu->registers[7]--; //decrement the stack pointer
        cpu_ram_write(cpu, cpu->registers[7], cpu->registers[operandA]); 
        //copy the value in the given register to address of the SP
        break;

      case POP:
        //https://stackoverflow.com/questions/1010966/what-is-the-meaning-of-the-gcc-warning-case-label-value-exceeds-maximum-value-f/1010995
        // error I got because I forgot to put `0b` in the POP preprocessor!
        //copy the value from the address pointed to by `SP` to the given register. 
        cpu->registers[operandA] = cpu_ram_read(cpu, cpu->registers[7]);
        cpu->registers[7]++; //increment the stack pointer
        break;
      
      case CALL: //set the pc directly
        cpu->registers[7]--; //address of instruction after call
        cpu_ram_write(cpu, cpu->registers[7], cpu->PC + 1);
        cpu->PC = cpu->registers[operandA] - 2;// set pc to address
        break;

      case RET: //set the pc directly
        cpu->PC = cpu_ram_read(cpu, cpu->registers[7]);
        cpu->registers[7]++;
        break;

      case ADD:
        alu(cpu, ALU_ADD, operandA, operandB);
        break;

      case CMP:
        alu(cpu, ALU_CMP, operandA, operandB);
        break;

      case JMP:
        // printf("opA-JMP: %d\n", cpu->registers[operandA]);
        cpu->PC = cpu->registers[operandA] - 2;
        // printf("opA-JMPafter: %d\n", cpu->registers[operandA]);
        break;

      case JEQ:
        if (cpu->FL == 1)
        {
          cpu->PC = cpu->registers[operandA] - 2;
          // printf("opA-JEQ: %d\n", cpu->registers[operandA]);
        }
        break;
      
      case JNE:
        if (cpu->FL == 0)
        {
          cpu->PC = cpu->registers[operandA] - 2;
          // printf("opA-JNE: %d\n", cpu->registers[operandA]);
        }
        break;

      default:
        // printf("Error!\n");
        printf("Error!: %d\n", IR);
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

/*   char data[DATA_LEN] = {
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
  } */