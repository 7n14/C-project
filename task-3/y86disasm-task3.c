#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//This is the help function that reads y86 binary code from a file
unsigned int loadBinFromFile(const char *filename, unsigned char memory[], unsigned int memsize);

//This is an array of register mnemonics in y86
const char* register_names[] =
{
 "%eax",
 "%ecx",
 "%edx",
 "%ebx",
 "%esp",
 "%ebp",
 "%esi",
 "%edi",
 "UNKNOWN_REGSITER"
};

int instrByteSize[] = {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,4,4,4,4,4,4,4,4};
int byteinstrId[] = {0x00,0x10,0x90,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x60,0x61,0x62,0x63,0xA0,0xB0,0x30,0x40,0x50,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x80};

//These declear all of the function of the disassembler

char* mainDisassembler(int byte0,int byte1,int byte2,int byte3,int byte4,int byte5);

char* getInstructions(int byte0);

char* getInstrType(int byte0);

const char* getRegister(int nibble);

int getIntger(char byte1, char byte2, char byte3, char byte4);

unsigned int getDest(unsigned char byte1,unsigned char byte2,unsigned char byte3,unsigned char byte4);


int main(int argc, char **argv)
{
  unsigned int MAX_MEMSIZE = 4096;
  unsigned char memory[MAX_MEMSIZE]; //This array represents the 4KB memory space
  unsigned int PC = 0;               //This is the initial program counter address

  if (argc < 2)
  {
    printf("Usage: please specify a y86 binary program file in the argument.\n");
    return -1;
  }

  unsigned int program_size = loadBinFromFile(argv[1], memory, MAX_MEMSIZE);

  if (program_size == 0)
    return 0;

  //TODO: Implement your disassembler from here
  //At the moment, it simply display each byte from the memory.
  
  while (PC < program_size)
  {
    //This clears all the possible bytes I need to disassemble code
    
    int byte0 = memory[PC];
    int byte1 = 0;
    int byte2 = 0;
    int byte3 = 0;
    int byte4 = 0;
    int byte5 = 0;

    //This checks how many bits the opcode needs and stores it in byteType
    int byteType = -1;
    char counter = -1;
    do{
      counter ++;
      if (byte0 == byteinstrId[counter]){
        byteType = instrByteSize[counter];
      }
    } while (byte0 != byteinstrId[counter] && counter < (sizeof(instrByteSize)/4));

    //These if statments get the relevant bytes for the opcode to be disassembled
    if (byteType == 0){//This statment is for only 1 byte opcode like halt
      PC++;
      char* temp1 = mainDisassembler(byte0, byte1, byte2, byte3, byte4, byte5);
      }
    else if (byteType == 1){ //This statment is for 2 byte opcode 
      byte1 =memory[PC+1];
      PC += 2;
      char* temp1 = mainDisassembler(byte0, byte1, byte2, byte3, byte4, byte5);
    }
    else if (byteType == 5){ //This statment is for 6 byte opcode
      byte1 = memory[PC+1];
      byte2 = memory[PC+2];
      byte3 = memory[PC+3];
      byte4 = memory[PC+4];
      byte5 = memory[PC+5];
      PC += 6;
      char* temp1 = mainDisassembler(byte0, byte1, byte2, byte3, byte4, byte5);
    }
    else if (byteType == 4){ //This statment is for 6 byte opcode
      byte1 = memory[PC+1];
      byte2 = memory[PC+2];
      byte3 = memory[PC+3];
      byte4 = memory[PC+4];
      
      PC += 5;
      char* temp1 = mainDisassembler(byte0, byte1, byte2, byte3, byte4, byte5);
    }
    else if (byteType == -1){//This statment is for any opcode that is not within the byteinstrId list
      //printf("%02X ", memory[PC]);
      PC ++;
      //printf("Passed\n");
    }
  
  }
  
  return 0;
}


char* mainDisassembler(int byte0,int byte1,int byte2,int byte3,int byte4,int byte5){
  //This array is in parallel with byteinstrId array 
  char* types[] = {"None","2reg","1reg","unused 1reg int","2reg int 1","2reg int 2","32 bit unused"};
  
  //These are all the variables that need to disassemble the byte code into a string
  char* mnemonic = "";
  char* type = "";
  int intger = 0;
  unsigned int dest = 0;
  char strIntger[12];
  char complete_instruction[128] = "";
  
  //These calls gets the mnemonic of the instrcutions and the type
  mnemonic = getInstructions(byte0);
  type = getInstrType(byte0);
  //This combines the mnemonic string to return value
  strcat(complete_instruction,mnemonic);
  
  //This if stament checks what type of instruction is being disassembled and will proceed to get relevent registers and intger value for it
  if (type == types[0]){ }
  //This if stament is for 2 register instructions
  else if (type == types[1]){
    const char* regA = getRegister(byte1>>4);
    const char* regB = getRegister(byte1&0xF);
    strcat(complete_instruction," ");
    strcat(complete_instruction,regA);
    strcat(complete_instruction,", ");
    strcat(complete_instruction,regB);
    
  }
  else if (type == types[2]){
    //This if stament is for 1 register instrcutions
    const char* regA = getRegister(byte1>>4);
    strcat(complete_instruction," ");
    strcat(complete_instruction,regA);
  }
  else if (type == types[3]){
    //This if stament is for 1 register and 32 bit 
    const char* regB = getRegister(byte1&0xF);
    intger = getIntger(byte2,byte3,byte4,byte5);
    sprintf(strIntger, "%d",intger); 

    strcat(complete_instruction," $");
    strcat(complete_instruction,strIntger);
    strcat(complete_instruction,", ");
    strcat(complete_instruction,regB);

    
  }
  else if (type == types[4]){
    //This if stament is for 2 register and 32 bit intger for the rmmovl format
    const char* regA = getRegister(byte1>>4);
    const char* regB = getRegister(byte1&0xF);
    intger = getIntger(byte2,byte3,byte4,byte5);
    sprintf(strIntger, "%d",intger); 

    strcat(complete_instruction," ");
    strcat(complete_instruction,regA);
    strcat(complete_instruction,", ");
    strcat(complete_instruction,strIntger);
    strcat(complete_instruction,"(");
    strcat(complete_instruction,regB);
    strcat(complete_instruction,")");
    

    
  }
  else if (type == types[5]){
    //This if stament is for 2 register and 32 bit intger for mrmovl format
    const char* regA = getRegister(byte1>>4);
    const char* regB = getRegister(byte1&0xF);
    intger = getIntger(byte2,byte3,byte4,byte5);
    sprintf(strIntger, "%d",intger); 

    strcat(complete_instruction," ");
    strcat(complete_instruction,strIntger);
    strcat(complete_instruction,"(");
    strcat(complete_instruction,regB);
    strcat(complete_instruction,")");
    strcat(complete_instruction,", ");
    strcat(complete_instruction,regA);
    
  }
  else if (type == types[6]){
    //This if stament is for just 32 bit unsigned intger
    dest = getDest(byte1,byte2,byte3,byte4);
    sprintf(strIntger, "%u",dest); 
    
    strcat(complete_instruction," ");
    strcat(complete_instruction,strIntger);
  }
  strcat(complete_instruction,"\0");

  char* return_instruction = complete_instruction;
  //Due to a bug with the console onyl displaying a certain length of a printf statment I had to print it within the disassembler function
  printf("%s\n",return_instruction);

  return return_instruction;
}


char* getInstructions(int byte0){
  //This array works in parallel will mutiple arrays
  char* instructions[] = {"halt","nop","ret","rrmovl","cmovle","cmovl","cmove","cmovne","cmovge","cmovg","addl","subl","andl","xorl","pushl","popl","irmovl","rmmovl","mrmovl","jmp","jle","jl","je","jne","jge","jg","call"};

  int counter = -1;
  char* returnInstr = ""; 
  //This searches the byteinstrId list and finds the instrcution that is parralel with it and returns it
  do{
    counter ++;
    if (byte0 == byteinstrId[counter]){
      returnInstr = instructions[counter];
    }
  } while (byte0 != byteinstrId[counter]);

  return returnInstr;
}

char* getInstrType(int byte0){
  
  //This array contains what type of instrcutions and values it will have
  char* instrType[] = {"None","None","None","2reg","2reg","2reg","2reg","2reg","2reg","2reg","2reg","2reg","2reg","2reg","1reg","1reg","unused 1reg int","2reg int 1","2reg int 2","32 bit unused","32 bit unused","32 bit unused","32 bit unused","32 bit unused","32 bit unused","32 bit unused","32 bit unused"};

  int counter = -1;
  char* returnType = ""; 
  do{
    counter ++;
    if (byte0 == byteinstrId[counter]){
      returnType = instrType[counter];
    }
  } while (byte0 != byteinstrId[counter]);

  return returnType;
}

const char* getRegister(int nibble){
  //This returns the register name from the array of register_names
  return register_names[nibble];
}

int getIntger(char byte1, char byte2, char byte3, char byte4){
  //This converts the 4 bytes into a signed intger value using little endian
  int intger = ((byte4&0xff)<<24)+((byte3&0xff)<<16)+((byte2&0xff)<<8)+(byte1&0xff);
    
  return intger;
}

unsigned int getDest(unsigned char byte1,unsigned char byte2,unsigned char byte3,unsigned char byte4){
  //This converts the 4 bytes into a unsigned intger value using little endian
  unsigned int intger = ((byte4&0xff)<<24)+((byte3&0xff)<<16)+((byte2&0xff)<<8)+(byte1&0xff);
    
  return intger;
}

/****************************************************************************
N.B. You do not need to modify or work in this function.
Description:
This function reads in a y86 machine bytecode from file and
store them in an unsigned char array.
******************************************************************************/
unsigned int loadBinFromFile(const char *filename, unsigned char memory[], unsigned int memsize)
{
  unsigned int bytes_read = 0;

  unsigned int file_size = 0;

  FILE *pfile = fopen(filename, "rb");

  if (!pfile)
  {
    printf("Unable to load file %s, please check if the path and name are correct.\n", filename);
    return 0;
  }

  fseek(pfile, 0, SEEK_END);
  file_size = ftell(pfile);
  rewind(pfile);

  if (file_size > memsize)
  {
    printf("Program size exceeds memory size of %d.\n", memsize);
    return 0;
  }

  bytes_read = fread(memory, 1, file_size, pfile);

  if (bytes_read != file_size)
  {
    printf("Bytes read does not match the file size.\n");
    return 0;
  }

  fclose(pfile);

  return bytes_read;
}
