#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


int convertStrToByteCode( const char *str, unsigned char inst[], int size );

//These declear all of the function of the disassembler

char* mainDisassembler(int byte0,int byte1,int byte2,int byte3,int byte4,int byte5);

char* getInstructions(int byte0, int byteinstrId[]);

char* getInstrType(int byte0, int byteinstrId[]);

const char* getRegister(int nibble);

int getIntger(char byte1, char byte2, char byte3, char byte4);

unsigned int getDest(unsigned char byte1,unsigned char byte2,unsigned char byte3,unsigned char byte4);


int main ( int argc, char** argv )
{
  FILE *pFile = NULL;
  
  char buffer[15];

  if (argc < 2 )
  {
    pFile = fopen("./test1.txt", "r");
  }
  else
  {
    pFile = fopen(argv[1], "r");
  }
  
  if ( pFile == NULL )
  {
    printf("Error open test file, please make sure they exist.\n");
    
    return 0;
  }

  while( fgets(buffer, 15, pFile ) )
  {
      //This unsigned char array stores an instruction read from the file
      //As the largest y86 instruction is 6 bytes, there are 6 unsigned char in the array where
      //each represents a byte.
      unsigned char instruction[6] = {0,0,0,0,0,0};
      convertStrToByteCode(buffer, instruction, 6);
      

      //This call the function mainDisassembler which transltes the 6 bytes passed into it into assembly code
      char* temp = mainDisassembler(instruction[0],instruction[1],instruction[2],instruction[3],instruction[4],instruction[5]);
      //prints the result of the disassembled byte code
      printf("%s\n",temp);
      
  }

  fclose(pFile);

  return 0;
}


char* mainDisassembler(int byte0,int byte1,int byte2,int byte3,int byte4,int byte5){
  //These are array that will be used during the disassembling process
  int byteinstrId[] = {0x00,0x10,0x90,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x60,0x61,0x62,0x63,0xA0,0xB0,0x30,0x40,0x50,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x80};
  char* types[] = {"None","2reg","1reg","unused 1reg int","2reg int 1","2reg int 2","32 bit unused"};

  //These are all the variables that need to disassemble the byte code into a string
  char* mnemonic = "";
  char* type = "";
  int intger = 0;
  unsigned int dest = 0;
  char strIntger[12];
  char complete_instruction[128] = "";

  //These calls gets the mnemonic of the instrcutions and the type
  mnemonic = getInstructions(byte0,byteinstrId);
  type = getInstrType(byte0,byteinstrId);
  //This combines the mnemonic string to return value
  strcat(complete_instruction,mnemonic);

  //This if stament checks what type of instruction is being disassembled and will proceed to get relevent registers and intger value for it
  if (type == types[0]){
  }
  else if (type == types[1]){
    //This if stament is for 2 register instructions
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
    //This if stament is for 1 register and 32 bit intger
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
  //This ends the string and then converts into a data type that can be returned
  strcat(complete_instruction,"\0");
  char* return_instruction = complete_instruction;
  return return_instruction;
}


char* getInstructions(int byte0, int byteinstrId[]){
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

char* getInstrType(int byte0, int byteinstrId[]){
  
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
This function converts a line of machine code read from the text file
into machine byte code.
The machine code is stored in an unsigned char array.
******************************************************************************/
int convertStrToByteCode( const char *str, unsigned char inst[], int size )
{
  int numHexDigits = 0;
  char *endstr;
  //Each instruction should consist of at most 12 hex digits
  numHexDigits = strlen(str) - 1;
  //Convert the string to integer, N.B. this integer is in decimal
  long long value = strtol(str, &endstr, 16);

  int numBytes = numHexDigits >> 1;
  int byteCount = numHexDigits >> 1;

  while ( byteCount > 0 )
  {
    unsigned long long mask = 0xFF;
    unsigned long shift = (numBytes - byteCount) << 3; 

    inst[byteCount - 1] = (value & (mask<<shift))>>shift;
    byteCount --;
  }

  //Return the size of the instruction in bytes
  return numHexDigits >> 1;
}