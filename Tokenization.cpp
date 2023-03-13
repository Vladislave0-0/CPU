#include "Tokenization.h"

//==========================================================================================================================

const int TOKEN_CALLOC_NUMBER  = 2;
const int BINARY_HEADER_NUMBER = 2;

//==========================================================================================================================

#define STRCOMP(text) !strcmp(TextInfo->ptr_arr[TextInfo->cur_word], text)
#define TEXT           TextInfo->ptr_arr[TextInfo->cur_word]
#define LEN            strlen(TextInfo->ptr_arr[TextInfo->cur_word])

//==========================================================================================================================

void AsmInfo_ctor(struct AsmInfo* AsmInfo, struct TextInfo* TextInfo, struct FlagsInfo* Flags)
{
    AsmInfo->quantity  = TextInfo->tok_num;

    AsmInfo->asm_file = open_file("asm_listing.txt");

    AsmInfo->tok_arr = (Tokens*)calloc(AsmInfo->quantity, sizeof(Tokens));
    assert(AsmInfo->tok_arr != nullptr);

    AsmInfo->code = (int*)calloc(AsmInfo->quantity, sizeof(int));
    assert(AsmInfo->code != nullptr);

    tokens_processing(AsmInfo, TextInfo, Flags);

    listing(AsmInfo);

    if(AsmInfo->error != 0)
    {
        AsmInfo->error = ERROR_TOKENIZATION;

        return;
    }

    bin_file_code(AsmInfo);
}

//==========================================================================================================================

FILE* open_file(const char* filename)
{
    FILE* file_input = fopen(filename, "w");
    assert(file_input != nullptr);

    return file_input;
}

//==========================================================================================================================

#define DEF_ASM_TOK(AsmName, TokType, has_arg)                                              \
        else if(strcasecmp(TextInfo->ptr_arr[TextInfo->cur_word], #AsmName) == 0)           \
        {                                                                                   \
            AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = TokType;                           \
                                                                                            \
            AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = AsmName;                           \
                                                                                            \
            if(has_arg)                                                                     \
            {                                                                               \
                TextInfo->cur_word++;                                                       \
                                                                                            \
                args_processing(AsmInfo, TextInfo, Flags, AsmName);                         \
            }                                                                               \
                                                                                            \
            else                                                                            \
            {                                                                               \
                AsmInfo->cur_ip++;                                                          \
            }                                                                               \
        }                                                                                   \

void tokens_processing(struct AsmInfo* AsmInfo, struct TextInfo* TextInfo, struct FlagsInfo* Flags)
{
    for(; (AsmInfo->cur_ip < AsmInfo->quantity) && (TextInfo->cur_word < TextInfo->words_num); TextInfo->cur_word++)
    {
        if(TextInfo->ptr_arr[TextInfo->cur_word][strlen(TextInfo->ptr_arr[TextInfo->cur_word]) - 1] != ':')
        {
            strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, TextInfo->ptr_arr[TextInfo->cur_word]);

            if(0) {}

            #include "ASM_codes.h"

            else
            {
                AsmInfo->tok_arr[AsmInfo->cur_ip].error = ARG_WITH_ERROR_CMD;

                AsmInfo->error++;

                AsmInfo->cur_ip++;
            }
        }
    }
}

#undef DEF_ASM_TOK

//==========================================================================================================================

#define JMP_ARG_TOK                                                                                                 \
        {                                                                                                           \
            AsmInfo->cur_ip++;                                                                                      \
                                                                                                                    \
            AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = FLAG;                                                      \
                                                                                                                    \
            for(size_t i = 0; i < Flags->jumps_num; i++)                                                            \
            {                                                                                                       \
                if(strcmp(TextInfo->ptr_arr[TextInfo->cur_word], Flags->jumps_arr[i].text) == 0)                    \
                {                                                                                                   \
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, TextInfo->ptr_arr[TextInfo->cur_word]);          \
                                                                                                                    \
                    AsmInfo->tok_arr[AsmInfo->cur_ip].value = (int)Flags->jumps_arr[i].ip;                          \
                                                                                                                    \
                    break;                                                                                          \
                }                                                                                                   \
            }                                                                                                       \
                                                                                                                    \
            AsmInfo->cur_ip++;                                                                                      \
                                                                                                                    \
            break;                                                                                                  \
        }                                                                                                           \
        
void args_processing(struct AsmInfo* AsmInfo, struct TextInfo* TextInfo, struct FlagsInfo* Flags, int AsmCode)
{
    switch(AsmCode)
    {
        case(PUSH):
        {
            // if it is VAL
            size_t VAL_chars = 0;

            for(size_t i = 0; i < strlen(TextInfo->ptr_arr[TextInfo->cur_word]); i++)
            {
                if((TextInfo->ptr_arr[TextInfo->cur_word][i] == '-') && (i == 0))
                {
                    i++;
                }

                if(isdigit(TextInfo->ptr_arr[TextInfo->cur_word][i]) == 0)
                {
                    VAL_chars++;

                    break;
                }
            }   

            if(VAL_chars == 0)
            {
                AsmInfo->cur_ip++;

                strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, TextInfo->ptr_arr[TextInfo->cur_word]);

                AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = VAL;

                AsmInfo->tok_arr[AsmInfo->cur_ip].value = atoi(TextInfo->ptr_arr[TextInfo->cur_word]);

                AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += VAL_MASK;

                AsmInfo->cur_ip++;

                return;
            }


            // if it is REG
            if(STRCOMP("ax") || STRCOMP("bx") || STRCOMP("cx") || STRCOMP("dx"))
            {
                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = REG;

                strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, TextInfo->ptr_arr[TextInfo->cur_word]);

                AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += REG_MASK;

                if(STRCOMP("ax"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = AX;
                }

                else if(STRCOMP("bx"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = BX;
                }

                else if(STRCOMP("cx"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = CX;
                }

                else if(STRCOMP("dx"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = DX;
                }

                AsmInfo->cur_ip++;

                return;
            }
            

            // if it is [REG]
            if(STRCOMP("[ax]") || STRCOMP("[bx]") || STRCOMP("[cx]") || STRCOMP("[dx]"))
            {
                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = REG;

                AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += REG_MASK + RAM_MASK;

                if(STRCOMP("[ax]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "ax");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = AX;
                }

                else if(STRCOMP("[bx]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "bx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = BX;
                }

                else if(STRCOMP("[cx]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "cx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = CX;
                }

                else if(STRCOMP("[dx]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "dx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = DX;
                }

                AsmInfo->cur_ip++;

                return;
            }


            // if it is [VAL]
            if((strlen(TEXT) > strlen("[]")) && (TEXT[0] == '[' && TEXT[LEN-1] == ']') && TEXT[LEN-2] != 'x')
            {
                AsmInfo->cur_ip++;

                VAL_chars = 0;
                char VAL_text[MAX_WORD_LENGTH] = {0};

                for(size_t i = 1; i < strlen(TEXT) - 1; i++)
                {
                    if(isdigit(TEXT[i]) == 0)
                    {
                        VAL_chars++;

                        break;
                    }

                    VAL_text[i - 1] = TEXT[i];
                } 

                if(VAL_chars == 0)
                {
                    strcpy(TextInfo->ptr_arr[TextInfo->cur_word], VAL_text);

                    AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = VAL;

                    AsmInfo->tok_arr[AsmInfo->cur_ip].value = atoi(TextInfo->ptr_arr[TextInfo->cur_word] + 1);

                    AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += VAL_MASK + REG_MASK;

                    AsmInfo->cur_ip++;

                    return;
                }
            }


            // if it is [VAL+REG]
            if((TextInfo->ptr_arr[TextInfo->cur_word][0] == '[') && (isdigit(TextInfo->ptr_arr[TextInfo->cur_word][1])  || 
              ((TextInfo->ptr_arr[TextInfo->cur_word][1] == '-') && isdigit(TextInfo->ptr_arr[TextInfo->cur_word][2]))) &&
                TextInfo->ptr_arr[TextInfo->cur_word][strlen(TextInfo->ptr_arr[TextInfo->cur_word]) - 2] == 'x')
            {
                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += VAL_MASK + REG_MASK + RAM_MASK;

                VAL_chars = 0;
                char VAL_text[MAX_WORD_LENGTH] = {0};

                for(size_t i = 1; i < strlen(TEXT) - 4; i++)
                {
                    if(isdigit(TEXT[i]) == 0)
                    {
                        if((TEXT[i] == '-') && (i == 1) && isdigit(TEXT[i + 1]))
                        {
                            VAL_text[i - 1] = TEXT[i];

                            continue;
                        }

                        VAL_chars++;

                        break;
                    }

                    VAL_text[i - 1] = TEXT[i];
                } 

                if(VAL_chars == 0)
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, VAL_text);

                    AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = VAL;

                    AsmInfo->tok_arr[AsmInfo->cur_ip].value = atoi(TextInfo->ptr_arr[TextInfo->cur_word] + 1); 
                }

                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = REG;

                if(TEXT[LEN-3] == 'a')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "ax");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = AX;
                }

                else if(TEXT[LEN-3] == 'b')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "bx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = BX;
                }

                else if(TEXT[LEN-3] == 'c')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "cx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = CX;
                }

                else if(TEXT[LEN-3] == 'd')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "dx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = DX;
                }

                AsmInfo->cur_ip++;
                
                return;           
            }


            // else
            AsmInfo->tok_arr[AsmInfo->cur_ip].error = CMD_WITH_ERROR_ARG;

            AsmInfo->error++;

            AsmInfo->cur_ip++;

            TextInfo->cur_word--;

            return;
        }

        case(POP):
        {
            
            size_t VAL_chars = 0;


            // if it is REG
            if(STRCOMP("ax") || STRCOMP("bx") || STRCOMP("cx") || STRCOMP("dx"))
            {
                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = REG;

                strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, TextInfo->ptr_arr[TextInfo->cur_word]);

                AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += REG_MASK;

                if(STRCOMP("ax"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = AX;
                }

                else if(STRCOMP("bx"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = BX;
                }

                else if(STRCOMP("cx"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = CX;
                }

                else if(STRCOMP("dx"))
                {
                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = DX;
                }

                AsmInfo->cur_ip++;

                return;
            }            




            // if it is [REG]
            if(STRCOMP("[ax]") || STRCOMP("[bx]") || STRCOMP("[cx]") || STRCOMP("[dx]"))
            {
                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = REG;

                AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += REG_MASK + RAM_MASK;

                if(STRCOMP("[ax]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "ax");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = AX;
                }

                else if(STRCOMP("[bx]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "bx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = BX;
                }

                else if(STRCOMP("[cx]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "cx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = CX;
                }

                else if(STRCOMP("[dx]"))
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "dx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = DX;
                }

                AsmInfo->cur_ip++;

                return;
            }


            // if it is [VAL]
            if((strlen(TEXT) > strlen("[]")) && (TEXT[0] == '[' && TEXT[LEN-1] == ']') && TEXT[LEN-2] != 'x')
            {
                AsmInfo->cur_ip++;

                VAL_chars = 0;
                char VAL_text[MAX_WORD_LENGTH] = {0};

                for(size_t i = 1; i < strlen(TEXT) - 1; i++)
                {
                    if(isdigit(TEXT[i]) == 0)
                    {
                        VAL_chars++;

                        break;
                    }

                    VAL_text[i - 1] = TEXT[i];
                } 

                if(VAL_chars == 0)
                {
                    strcpy(TextInfo->ptr_arr[TextInfo->cur_word], VAL_text);

                    AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = VAL;

                    AsmInfo->tok_arr[AsmInfo->cur_ip].value = atoi(TextInfo->ptr_arr[TextInfo->cur_word] + 1);

                    AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += VAL_MASK + REG_MASK;

                    AsmInfo->cur_ip++;

                    return;
                }
            }


            // if it is [VAL+REG]
            if((TextInfo->ptr_arr[TextInfo->cur_word][0] == '[') && (isdigit(TextInfo->ptr_arr[TextInfo->cur_word][1])  || 
              ((TextInfo->ptr_arr[TextInfo->cur_word][1] == '-') && isdigit(TextInfo->ptr_arr[TextInfo->cur_word][2]))) &&
                TextInfo->ptr_arr[TextInfo->cur_word][strlen(TextInfo->ptr_arr[TextInfo->cur_word]) - 2] == 'x')
            {
                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip - 1].asm_code += VAL_MASK + REG_MASK + RAM_MASK;

                VAL_chars = 0;
                char VAL_text[MAX_WORD_LENGTH] = {0};

                for(size_t i = 1; i < strlen(TEXT) - 4; i++)
                {
                    if(isdigit(TEXT[i]) == 0)
                    {
                        if((TEXT[i] == '-') && (i == 1) && isdigit(TEXT[i + 1]))
                        {
                            VAL_text[i - 1] = TEXT[i];

                            continue;
                        }

                        VAL_chars++;

                        break;
                    }

                    VAL_text[i - 1] = TEXT[i];
                } 
                

                if(VAL_chars == 0)
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, VAL_text);

                    AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = VAL;

                    AsmInfo->tok_arr[AsmInfo->cur_ip].value = atoi(TextInfo->ptr_arr[TextInfo->cur_word] + 1); 
                }

                AsmInfo->cur_ip++;

                AsmInfo->tok_arr[AsmInfo->cur_ip].tok_type = REG;

                if(TEXT[LEN-3] == 'a')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "ax");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = AX;
                }

                else if(TEXT[LEN-3] == 'b')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "bx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = BX;
                }

                else if(TEXT[LEN-3] == 'c')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "cx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = CX;
                }

                else if(TEXT[LEN-3] == 'd')
                {
                    strcpy(AsmInfo->tok_arr[AsmInfo->cur_ip].text, "dx");

                    AsmInfo->tok_arr[AsmInfo->cur_ip].asm_code = DX;
                }

                AsmInfo->cur_ip++;
                
                return; 
            }

            AsmInfo->tok_arr[AsmInfo->cur_ip].error = CMD_WITH_ERROR_ARG;

            AsmInfo->error++;

            AsmInfo->cur_ip++;

            TextInfo->cur_word--;

            return;
        }

        case(JMP):  JMP_ARG_TOK

        case(JE):   JMP_ARG_TOK

        case(JNE):  JMP_ARG_TOK

        case(JG):   JMP_ARG_TOK

        case(JL):   JMP_ARG_TOK

        case(CALL): JMP_ARG_TOK

        default:
        {
            AsmInfo->tok_arr[AsmInfo->cur_ip].error = ERROR_TOKEN;

            AsmInfo->error++;

            break;
        }
    }
}

#undef JMP_ARG_TOK
#undef STRCOMP
#undef TEXT
#undef LEN

//==========================================================================================================================

void listing(struct AsmInfo* AsmInfo)
{
    fprintf(AsmInfo->asm_file, "signature: %s\n", AsmInfo->signature);
    fprintf(AsmInfo->asm_file, "quantity: %lu\n\n", AsmInfo->quantity);
    fprintf(AsmInfo->asm_file, "| ip |\t\tCOMMAND\t\t|\tTYPE\t\tASM_CODE\t|\tERROR\n");

    for(size_t i = 0; i < AsmInfo->quantity; i++)
    {
        fprintf(AsmInfo->asm_file, "|%04lu|\t\t", i);
        fprintf(AsmInfo->asm_file, "%-12s|\t", AsmInfo->tok_arr[i].text);

        switch(AsmInfo->tok_arr[i].tok_type)
        {
            case(CMD):
            {
                fprintf(AsmInfo->asm_file, "%-7s\t\t", "cmd");

                break;
            }

            case(VAL):
            {
                fprintf(AsmInfo->asm_file, "%-7s\t\t", "val");

                break;    
            }

            case(REG):
            {
                fprintf(AsmInfo->asm_file, "%-7s\t\t", "reg");

                break;    
            }

            case(JUMP):
            {
                fprintf(AsmInfo->asm_file, "%-7s\t\t", "jmp");

                break;    
            }

            case(FLAG):
            {
                fprintf(AsmInfo->asm_file, "%-7s\t\t", "flg");

                break;    
            }

            default:
            {
                fprintf(AsmInfo->asm_file, "%-7s\t\t", "unknown");

                break;
            }
        }


        switch(AsmInfo->tok_arr[i].tok_type)
        {
            case(VAL):
            {
                fprintf(AsmInfo->asm_file, "%-8d\t|\t", AsmInfo->tok_arr[i].value);

                break;
            }

            case(FLAG):
            {
                fprintf(AsmInfo->asm_file, "%-8d\t|\t", AsmInfo->tok_arr[i].value);

                break;
            }

            default:
            {
                fprintf(AsmInfo->asm_file, "%-8d\t|\t", AsmInfo->tok_arr[i].asm_code);

                break;
            }
        }

        fprintf(AsmInfo->asm_file, "%2d\n", AsmInfo->tok_arr[i].error);
    }

    fclose(AsmInfo->asm_file);
}

//==========================================================================================================================

int get_signature(AsmInfo* AsmInfo)
{
    int result = 0;
    int coef   = 1;
    int step   = 256;

    for(size_t i = 0; i < sizeof(int); i++)
    {
        result += AsmInfo->signature[i] * coef;
        coef *= step;
    }

    return result;
}

//==========================================================================================================================

void bin_file_code(struct AsmInfo* AsmInfo)
{
    for(size_t i = 0; i < AsmInfo->quantity; i++)
    {
        if((AsmInfo->tok_arr[i].tok_type == VAL) || (AsmInfo->tok_arr[i].tok_type == FLAG))
        {
            AsmInfo->code[i] = AsmInfo->tok_arr[i].value;

            continue;
        }

        AsmInfo->code[i] = AsmInfo->tok_arr[i].asm_code;
    }


    AsmInfo->bin_file = open_file("bin_file.code");

    int* binary_file_calloc = (int*)calloc(AsmInfo->quantity + 2, sizeof(int));
    binary_file_calloc[0] = get_signature(AsmInfo);
    binary_file_calloc[1] = (int)AsmInfo->quantity;
    
    for(size_t i = BINARY_HEADER_NUMBER; i < AsmInfo->quantity + BINARY_HEADER_NUMBER; i++)
    {
        binary_file_calloc[i] = AsmInfo->code[i - BINARY_HEADER_NUMBER];
    }

    fwrite(binary_file_calloc, sizeof(int), AsmInfo->quantity + BINARY_HEADER_NUMBER, AsmInfo->bin_file);

    free(binary_file_calloc);
}

//==========================================================================================================================

void AsmInfo_dtor(struct AsmInfo* AsmInfo)
{
    free(AsmInfo->tok_arr);
    free(AsmInfo->code);
}
