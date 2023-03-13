#ifndef TOKENIZATION_H
#define TOKENIZATION_H
//=============================================================================================================

#include "InputProcessing.h"

//=============================================================================================================

const int MAX_WORD_LENGTH = 32;

//=============================================================================================================

struct Tokens
{
    char text[MAX_WORD_LENGTH] = {0};         //| Token represented as text.

    size_t tok_type = 0;                      //| Token type.
    int asm_code    = 0;                      //| Token represented as ASM code.
    int value       = 0;                      //| Value for tokens with type VAL.

    int error = 0;                            //| Error during tokenization.
};

struct AsmInfo 
{
    char signature[4] = "HUI";                //| The signature of assembler.
    size_t quantity   = 0;                    //| Number of tokens.

    FILE* asm_file = nullptr;                 //| File with assembler listing.
    FILE* bin_file = nullptr;                 //| File with signature, quantity, assembler codes for CPU.

    Tokens* tok_arr  = nullptr;               //| Array of token structures.
    int* code        = nullptr;               //| Array with assembler codes.
    size_t cur_ip    = 0;                     //| Current ip counter.

    size_t error = 0;                         //| Is equal 0, if there were no errors in tokenization.
};

//=============================================================================================================

enum AssemblerCodes
{
    // COMMAND
    HLT  = 0,                   //| The end of assembly code reading.
    PUSH = 1,                   //| Push in stack.
    POP  = 2,                   //| Pop from stack.
    ADD  = 3,                   //| Add two numbers in stack.
    SUB  = 4,                   //| Sub two numbers in stack.
    MUL  = 5,                   //| Mul two numbers in stack.
    DIV  = 6,                   //| Div two numbers in stack.
    IN   = 7,                   //| Waiting an integer from console.
    OUT  = 8,                   //| Prints an integer in console.
    RET  = 9,                   //| Returns the ip that comes after CALL.

    // REGISTER
    AX = 21,                    //| Register ax.
    BX = 22,                    //| Register bx.
    CX = 23,                    //| Register cx.
    DX = 24,                    //| Register dx.

    // JUMP
    JMP  = 31,                  //| Common jump.
    JE   = 32,                  //| Conditional jump --> jump if equal.
    JNE  = 33,                  //| Conditional jump --> jump if not equal.
    JG   = 34,                  //| Conditional jump --> jump if grower.
    JL   = 35,                  //| Conditional jump --> jump if lower.
    CALL = 36,                  //| Jump for functions with stack of returns.
};

enum TypeOfAsmCode
{
    CMD     = 1,                //| Command.
    VAL     = 2,                //| Value.
    REG     = 3,                //| Register.
    JUMP    = 4,                //| Jump.
    FLAG    = 5,                //| Jump's flag.
    UNKNOWN = 0,                //| Unknown type of the assembler code.
};

enum CodeMask
{
    // Example for PUSH:
    // val       == 00000000 00000001 00000000 00000001
    // reg       == 00000000 00000010 00000000 00000001
    // [val]     == 00000000 00000101 00000000 00000001
    // [reg]     == 00000000 00000110 00000000 00000001
    // [val+reg] == 00000000 00000111 00000000 00000001

    VAL_MASK = 1 << 16,
    REG_MASK = 1 << 17,
    RAM_MASK = 1 << 18,
};

enum ErrorTokenTypes
{
    ERROR_TOKENIZATION = -1,    //| If at least one token has an error.
    WITHOUT_ERROR      =  0,    //| There are no errors.
    ERROR_TOKEN        =  1,    //| Unknown token.
    CMD_WITH_ERROR_ARG =  2,    //| Command with invalid argument.
    ARG_WITH_ERROR_CMD =  3,    //| Argument with invalid command.
};

//=============================================================================================================

void AsmInfo_ctor(struct AsmInfo* AsmInfo, struct TextInfo* TextInfo, struct FlagsInfo* Flags);

//=============================================================================================================

FILE* open_file(const char* filename);

//=============================================================================================================

void listing(struct AsmInfo* AsmInfo);

//=============================================================================================================

void tokens_processing(struct AsmInfo* AsmInfo, struct TextInfo* TextInfo, struct FlagsInfo* Flags);

//=============================================================================================================

void args_processing(struct AsmInfo* AsmInfo, struct TextInfo* TextInfo, struct FlagsInfo* Flags, int AsmCode);

//=============================================================================================================

int get_signature(AsmInfo* AsmInfo);

//=============================================================================================================

void bin_file_code(struct AsmInfo* AsmInfo);

//=============================================================================================================

void AsmInfo_dtor(struct AsmInfo* AsmInfo);

//=============================================================================================================

#endif // TOKENIZATION_H
