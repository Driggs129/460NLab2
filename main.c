/*
    Remove all unnecessary lines (including this one)
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Douglas Riggs
    Name 2: Ran Trakhtengerts
    UTEID 1: Dwr726
    UTEID 2: rt24869
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
  int k;

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;
  NEXT_LATCHES = CURRENT_LATCHES;

  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

//bits 11:9
int getR0(int instruction){
    int output = instruction>>9;
    output &= 0x00000007;
    return CURRENT_LATCHES.REGS[output];
}


int getDR(int instruction){
    int instructionValue = instruction>>9;
    return instructionValue&0x00000007;
}


//bits 8:6
int getR1(int instruction){
    int output = instruction>>6;
    output &= 0x00000007;
    return CURRENT_LATCHES.REGS[output];
}


//bits 2:0
int getR2(int instruction){
    int output = instruction & 0x00000007;
    return CURRENT_LATCHES.REGS[output];
}

int getImmediate(int instruction){
    if(instruction&0x00000010){
        return (instruction & 0x0000001F)|0x0000FFE0;
    }
    return instruction & 0x0000001F;
}




int calculateShiftedOffset(int instruction, int bits){
    const unsigned int mask = 0x0000FFFF;
    int signedBit = 0x00001000;
    signedBit = instruction & (signedBit>>(16-bits));
    instruction &= (mask>>(16-bits));
    if(signedBit){
        instruction|=mask<<bits;
    }
    instruction = Low16bits(instruction<<1);
    return instruction;
}


int calculateUnshiftedOffset(int instruction, int bits){
    const unsigned int mask = 0x0000FFFF;
    int signedBit = 0x00001000;
    signedBit = instruction & (signedBit>>(16-bits));
    instruction &= (mask>>(16-bits));
    if(signedBit){
        instruction|=mask<<bits;
    }
    return instruction;
}


void setNZP(int value){
    if(value>0){
        NEXT_LATCHES.N=0;
        NEXT_LATCHES.Z=0;
        NEXT_LATCHES.P=1;
        return;
    }
    if(value==0){
        if(value>0){
            NEXT_LATCHES.N=0;
            NEXT_LATCHES.Z=1;
            NEXT_LATCHES.P=0;
        }
    }
    NEXT_LATCHES.N=1;
    NEXT_LATCHES.Z=0;
    NEXT_LATCHES.P=0;
}

int checkBranch(int instruction){
    int nzpBits = getDR(instruction);
    int n, z, p;
    p = nzpBits & 0x00000001;
    z = (nzpBits & 0x00000002)>>1;
    n = (nzpBits&0x00000004)>>2;
    if((CURRENT_LATCHES.N==n)||
            (CURRENT_LATCHES.Z==z)||
            (CURRENT_LATCHES.P==p)){
        return 1;
    }
    return 0;
}

int isImmediate(int instruction){
    if(instruction & 0x00000020){
        return 1;
    }
    return 0;
}



void process_instruction(){
    int instruction = MEMORY[CURRENT_LATCHES.PC/2][0] + (MEMORY[CURRENT_LATCHES.PC/2][1]<<8);
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC+2;
    //CURRENT_LATCHES.PC = CURRENT_LATCHES.PC+2;
    int opcode = instruction>>12;
    int bitValue = 0;
    int value = 0;
    switch(opcode){
        case 0:
            //BR
            if(checkBranch(instruction)){
                NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC+2+calculateShiftedOffset(instruction,9));
            }
            break;
        case 1:
            //ADD
            if(isImmediate(instruction)){
                NEXT_LATCHES.REGS[getDR(instruction)]=Low16bits(getR1(instruction) + getImmediate(instruction));
                setNZP(getR1(instruction) + getR2(instruction));
            }
            else{
                NEXT_LATCHES.REGS[getDR(instruction)]=Low16bits(getR1(instruction) + getR2(instruction));
                setNZP(getR1(instruction) + getR2(instruction));
            }
            break;
        case 2:
            value = MEMORY[calculateUnshiftedOffset(instruction,6)/2]
            [calculateUnshiftedOffset(instruction,6)%2];
            if((value&0x00000020)>0){
                value = value|0x0000FF00;
            }
            NEXT_LATCHES.REGS[getDR(instruction)]=value;
            setNZP(value);
            //LDB
            break;
        case 3:
            MEMORY[calculateUnshiftedOffset(instruction,6)/2][calculateUnshiftedOffset(instruction,6)%2]=getR1(instruction);
            //STB
            break;
        case 4:
            //JSR
            bitValue = (instruction>>11)&0x00000001;
            if(bitValue==0){
                //JSRR
                NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2+calculateShiftedOffset(instruction,11);
                NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC+2;
            }
            else{
                //JSR
                NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC+2;
                NEXT_LATCHES.PC = getR1(instruction);
            }
            break;
        case 5:
            //AND
            if(isImmediate(instruction)){
                NEXT_LATCHES.REGS[getDR(instruction)]=Low16bits(getR1(instruction) & getImmediate(instruction));
                setNZP(getR1(instruction) & getR2(instruction));
            }
            else{
                NEXT_LATCHES.REGS[getDR(instruction)]=Low16bits(getR1(instruction) & getR2(instruction));
                setNZP(getR1(instruction) & getR2(instruction));
            }

            break;
        case 6:
            //LDW
            NEXT_LATCHES.REGS[getDR(instruction)] = MEMORY[calculateShiftedOffset(instruction,6)/2][0]+
                                                (MEMORY[calculateShiftedOffset(instruction,6)/2][1]<<8);
            setNZP(MEMORY[calculateShiftedOffset(instruction,6)/2][0]+
                   (MEMORY[calculateShiftedOffset(instruction,6)/2][1]<<8));
            break;
        case 7:
            //STW
            MEMORY[getR1(instruction)/2][0] = getR0(instruction)& 0x000000FF;
            MEMORY[getR1(instruction)/2][1] = getR0(instruction)>>8;
            break;
        case 8:
            //RTI
            //unnecessary to implement
            break;
        case 9:
            //XOR
            if(isImmediate(instruction)){
                NEXT_LATCHES.REGS[getDR(instruction)]=Low16bits(getR1(instruction) ^ getImmediate(instruction));
                setNZP(getR1(instruction) ^ getR2(instruction));
            }
            else{
                NEXT_LATCHES.REGS[getDR(instruction)]=Low16bits(getR1(instruction) ^ getR2(instruction));
                setNZP(getR1(instruction) ^ getR2(instruction));
            }

            break;
        /*case 10:
            //NOT USED
            break;
        case 11:
            //NOT USED
            break;*/
        case 12:
            NEXT_LATCHES.PC = getR1(instruction);
            //JMP
            break;
        case 13:
            //SHF
            if ((instruction&0x00000010)) {
               //RSHF
                if((instruction&0x00000020)){
                    //RSHFA
                    NEXT_LATCHES.REGS[getDR(instruction)] = Low16bits(getR1(instruction)>>(instruction&0x0000000F));
                    if(instruction&0x00000008){
                        int mask = Low16bits(0xFFFFFFFF<<24-(instruction&0x0000000F));
                        NEXT_LATCHES.REGS[getDR(instruction)]|=mask;
                    }
                }
                else{
                    //RSHFL
                    NEXT_LATCHES.REGS[getDR(instruction)] = Low16bits(getR1(instruction)>>(instruction&0x0000000F));
                }
            }
            else{
                //LSHF
                NEXT_LATCHES.REGS[getDR(instruction)] = Low16bits(getR1(instruction)<<getImmediate(instruction));                        //5th/6th bits are 0, so i just used the getImmediate subfunction
            }
            break;
        case 14:
            bitValue = (instruction>>8)&0x00000001;
            value = calculateShiftedOffset(instruction,9);
            if(bitValue){
                value = 0x0000FE00|instruction;
            }
            NEXT_LATCHES.REGS[getDR(instruction)] = Low16bits(CURRENT_LATCHES.PC+2+value);
            //LEA
            break;
        case 15:
            NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC+2;
            NEXT_LATCHES.PC = MEMORY[(instruction & 0x000000FF)/2][0]+(MEMORY[(instruction & 0x000000FF)/2][1]<<8);
            //TRAP
            break;
        default: exit(10);
    }
  /*  function: process_instruction
   *
   *    Process one instruction at a time
   *       -Fetch one instruction
   *       -Decode
   *       -Execute
   *       -Update NEXT_LATCHES
   */

}