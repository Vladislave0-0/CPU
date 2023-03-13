#include "InputProcessing.h"

//============================================================================================================================

#define STRCOMP(text) !strcmp(TextInfo->ptr_arr[TextInfo->cur_word], text)
#define LEN            strlen(TextInfo->ptr_arr[TextInfo->cur_word])
#define TEXT           TextInfo->ptr_arr[TextInfo->cur_word]

//============================================================================================================================

void TextInfo_ctor(struct TextInfo* TextInfo, struct FlagsInfo* Flags, const char* filename)
{
    open_file(TextInfo, filename);

    num_of_chars(TextInfo, filename);

    chars_buffer(TextInfo);

    count_words(TextInfo);

    fill_ptr_arr(TextInfo);

    lexical_processing(TextInfo, Flags);

    if(TextInfo->error != 0)
    {
        printf("You have lexical error. Incorrect word: \"%s\".\n"
                "Number of uncorrect word in the file input.txt: %lu.\n"
                "You may find it faster with Ctrl + F.\n", TextInfo->ptr_arr[TextInfo->cur_word], TextInfo->cur_word + 1);

        return;
    }

    TextInfo->cur_word = 0;

    flags_processing(TextInfo, Flags);

    if(TextInfo->error != 0)
    {
        printf("You have flag error! Error code: %d.\n", TextInfo->error);

        return;
    }
}

//============================================================================================================================

void open_file(struct TextInfo* TextInfo, const char* filename)
{
    FILE* file_input = fopen(filename, "r");
    assert(file_input != nullptr);

    TextInfo->mainfile = file_input;
}

//============================================================================================================================

void num_of_chars(struct TextInfo* TextInfo, const char* filename)
{
    struct stat buf = {};

    stat(filename, &buf);

    TextInfo->chars_number = buf.st_size + 1;

    // printf("TextInfo->chars_number:\n%lu\n\n", TextInfo->chars_number);
}

//============================================================================================================================

void chars_buffer(struct TextInfo* TextInfo)
{
    TextInfo->chars_buffer_ptr = (char*)calloc(TextInfo->chars_number, sizeof(char) );

    fread(TextInfo->chars_buffer_ptr, sizeof(char), TextInfo->chars_number - 1, TextInfo->mainfile);

    TextInfo->chars_buffer_ptr[TextInfo->chars_number - 1] = '\0';

    fclose(TextInfo->mainfile);

    // printf("TextInfo->chars_buffer_ptr:\n%s\n\n\n", TextInfo->chars_buffer_ptr);
}

//============================================================================================================================

void count_words(struct TextInfo* TextInfo)
{
    char sep[10] = " \n";
    char* istr = {0};

    char dest_str[TextInfo->chars_number];
    strcpy(dest_str, TextInfo->chars_buffer_ptr);

    istr = strtok(dest_str, sep);

    while(istr != nullptr)
    {
        TextInfo->words_num++; 

        istr = strtok(NULL, sep);
    }   

    // printf("words_num: %lu\n", TextInfo->words_num);
}

//============================================================================================================================

void fill_ptr_arr(struct TextInfo* TextInfo)
{
    TextInfo->ptr_arr = (char**)calloc(TextInfo->words_num, sizeof(char*));

    char sep[10] = " \n";
    char* istr = {0};

    istr = strtok(TextInfo->chars_buffer_ptr, sep);

    for(size_t i = 0; istr != nullptr; i++)
    {
        TextInfo->ptr_arr[i] = istr;

        istr = strtok(NULL, sep);
    }

    // for(size_t i = 0; i < TextInfo->words_num; i++)
    // {
    //     printf("%lu : %s\n", i, TextInfo->ptr_arr[i]);
    // }
}

//============================================================================================================================

#define DEF_ASM_TOK(AsmName, TokType, AsmCode)                                              \
        else if(strcasecmp(TextInfo->ptr_arr[TextInfo->cur_word], #AsmName) == 0)           \
        {                                                                                   \
            continue;                                                                       \
        }                                                                                   \

void lexical_processing(struct TextInfo* TextInfo, struct FlagsInfo* Flags)
{
    for(size_t i = 0; i < TextInfo->words_num; i++, TextInfo->cur_word++)
    {
        if(0) {}

        // CMD, REG, JMP processing
        #include "ASM_codes.h"

        // ARG PROCESSING
        
        // if it is VAL
        size_t VAL_chars = 0;

        for(size_t j = 0; j < strlen(TextInfo->ptr_arr[TextInfo->cur_word]); j++)
        {
            if((TextInfo->ptr_arr[TextInfo->cur_word][j] == '-') && (j == 0))
            {
                j++;
            }

            if(isdigit(TextInfo->ptr_arr[TextInfo->cur_word][j]) == 0)
            {
                VAL_chars++;

                break;
            }
        }   

        if(VAL_chars == 0)
        {
            continue;
        }

        // if it is REG
        if(STRCOMP("ax") || STRCOMP("bx") || STRCOMP("cx") || STRCOMP("dx"))
        {
            continue;
        }

        // if it is [VAL]
        if((strlen(TEXT) > strlen("[]")) && (TEXT[0] == '[' && TEXT[LEN-1] == ']') && TEXT[LEN-2] != 'x')
        {
            VAL_chars = 0;

            for(size_t j = 1; j < strlen(TEXT) - 1; j++)
            {
                if(isdigit(TEXT[j]) == 0)
                {
                    VAL_chars++;

                    break;
                }
            } 

            if(VAL_chars == 0)
            {
                continue;
            }
        }

        // if it is [REG]
        if(STRCOMP("[ax]") || STRCOMP("[bx]") || STRCOMP("[cx]") || STRCOMP("[dx]"))
        {
            continue;
        }

        // if it is [VAL+REG]
        if((strlen(TEXT) > strlen("[+ax]")) && 
       (TEXT[LEN-2] == 'x' && TEXT[LEN-4] == '+' && TEXT[0] == '[' && TEXT[LEN-1] == ']') &&
       (TEXT[LEN-3] == 'a' || TEXT[LEN-3] == 'b' || TEXT[LEN-3] == 'c' || TEXT[LEN-3] == 'd'))
        {
            VAL_chars = 0;

            for(size_t j = 1; j < strlen(TEXT) - 4; j++)
            {
                if(isdigit(TEXT[j]) == 0)
                {
                    if((TEXT[j] == '-') && (j == 1) && isdigit(TEXT[j + 1]))
                    {
                        continue;
                    }

                    VAL_chars++;

                    break;
                }
            } 
            

            if(VAL_chars == 0)
            {
                continue;
            }
        }

        // if it is jump argument
        if((strlen(TEXT) > strlen(":")) && (TEXT[LEN-1] == ':' || TEXT[0] == ':'))
        {
            if(TEXT[LEN-1] == ':')
            {
                if(TEXT[0] == ':')
                {
                    TextInfo->error = ERROR_LEXICAL_PROCESSING;

                    return;
                }

                Flags->flags_num++;

                continue;
            }

            else if((TEXT[0] == ':') && (TEXT[LEN-1] != ':'))
            {
                if(TEXT[LEN-1] == ':')
                {
                    TextInfo->error = ERROR_LEXICAL_PROCESSING;

                    return;
                }

                Flags->jumps_num++;

                continue; 
            }
        }


        TextInfo->error = ERROR_LEXICAL_PROCESSING;

        return;
    }
}

#undef DEF_ASM_TOK
#undef TEXT
#undef LEN
#undef STRCOMP

//============================================================================================================================

void flags_processing(struct TextInfo* TextInfo, struct FlagsInfo* Flags)
{
    // проверка на ненулевое количество количество джампов, если меток тоже нуль
    if((Flags->flags_num == 0) && (Flags->jumps_num != 0))
    {
        TextInfo->error = ERROR_FLAGS_NUM;

        return;
    }
    


    // если джампов нет - просто прекращаем работу функции
    if(Flags->jumps_num == 0)
    {
        size_t ip = 0;

        for(size_t i = 0; i < TextInfo->words_num; i++)
        {
            if((TextInfo->ptr_arr[i][0] == '[') && (strlen(TextInfo->ptr_arr[i]) > strlen("[+ax]")))
            {
                ip += 2;

                continue;
            }

            if(TextInfo->ptr_arr[i][strlen(TextInfo->ptr_arr[i]) - 1] != ':')
            {
                ip++;

                continue;
            }

        }

        TextInfo->tok_num = ip;

        return;
    }



    // проверка, что после JUMP и CALL только выражения вида ":..."
    for(size_t i = 0; i < TextInfo->words_num; i++)
    {
        if(!strcasecmp(TextInfo->ptr_arr[TextInfo->words_num - 1], "JMP") || !strcasecmp(TextInfo->ptr_arr[TextInfo->words_num - 1], "JE") ||
           !strcasecmp(TextInfo->ptr_arr[TextInfo->words_num - 1], "JNE") || !strcasecmp(TextInfo->ptr_arr[TextInfo->words_num - 1], "JG") ||
           !strcasecmp(TextInfo->ptr_arr[TextInfo->words_num - 1], "JL")  || !strcasecmp(TextInfo->ptr_arr[TextInfo->words_num - 1], "CALL"))
        {
            TextInfo->error = ERROR_JUMP_WITHOUT_FLAG;

            return;
        }
    }
    // всё та же проверка
    for(size_t i = 0; i < TextInfo->words_num - 1; i++)
    {
        if(((!strcasecmp(TextInfo->ptr_arr[i], "JMP") || !strcasecmp(TextInfo->ptr_arr[i], "JE")    ||
             !strcasecmp(TextInfo->ptr_arr[i], "JNE") || !strcasecmp(TextInfo->ptr_arr[i], "JG")    || 
             !strcasecmp(TextInfo->ptr_arr[i], "JL")) || !strcasecmp(TextInfo->ptr_arr[i], "CALL")) && 
            (TextInfo->ptr_arr[i + 1][0] != ':'))
        {
            TextInfo->error = ERROR_JUMP_WITHOUT_FLAG;

            return;
        }
    }



    Flags->flags_arr = (FlagsToken*)calloc(Flags->flags_num, sizeof(FlagsToken));
    if(Flags->flags_arr == nullptr)
    {
        TextInfo->error = ERROR_FLAGS_CALLOC_NULL;

        return;    
    }



    Flags->jumps_arr = (FlagsToken*)calloc(Flags->jumps_num, sizeof(FlagsToken));
    if(Flags->jumps_arr == nullptr)
    {
        TextInfo->error = ERROR_JUMPS_CALLOC_NULL;

        return;    
    }



    // заполняем текст массива структур
    for(size_t i = 0; i < TextInfo->words_num; i++)
    {
        if(TextInfo->ptr_arr[i][strlen(TextInfo->ptr_arr[i]) - 1] == ':')
        {
            strcpy(Flags->flags_arr[Flags->cur_flag].text, TextInfo->ptr_arr[i]);

            Flags->cur_flag++;
        }

        else if(TextInfo->ptr_arr[i][0] == ':')
        {
            strcpy(Flags->jumps_arr[Flags->cur_jump].text, TextInfo->ptr_arr[i]);

            Flags->cur_jump++;
        }
    }

    // for(size_t i = 0; i < Flags->flags_num; i++)
    // {
    //     printf("Flag[%lu] --> %s\n", i, Flags->flags_arr[i].text);
    // }

    // for(size_t i = 0; i < Flags->jumps_num; i++)
    // {
    //     printf("Jump[%lu] --> %s\n", i, Flags->jumps_arr[i].text);
    // }



    // проверка на повторные метки
    for(size_t i = 0; i < Flags->flags_num; i++)
    {
        for(size_t j = 0; j < Flags->flags_num; j++)
        {
            if((strcmp(Flags->flags_arr[i].text, Flags->flags_arr[j].text) == 0) && (i != j))
            {
                TextInfo->error = ERROR_DOUBLE_FLAGS;

                return;
            }
        }
    }



    // проверка, что у джампа в качестве аргумента стоит метка только из списка меток
    for(size_t i = 0; i < Flags->jumps_num; i++)
    {
        size_t jumps_equals = 0;

        char JMP_text[WORD_LENGTH] = {0};

        strcpy(JMP_text, Flags->jumps_arr[i].text + 1);

        for(size_t j = 0; j < Flags->flags_num; j++)
        {
            char FLAG_text[WORD_LENGTH] = {0};

            strncpy(FLAG_text, Flags->flags_arr[j].text, strlen(Flags->flags_arr[j].text) - 1);

            if(strcmp(JMP_text, FLAG_text) == 0)
            {
                jumps_equals++;
            }
        }

        if(jumps_equals == 0)
        {   
            TextInfo->error = ERROR_UNKNOWN_JUMP_FLAG;

            return;
        }
    }



    // заполняем поле ip в массиве структур flags_arr. То есть ip следующего после метки токена
    size_t cur_ip = 0;

    for(size_t i = 0; i < TextInfo->words_num; i++)
    {
        if((TextInfo->ptr_arr[i][0] == '[') && (strlen(TextInfo->ptr_arr[i]) > strlen("[+ax]")))
        {
            cur_ip += 2;

            continue;
        }

        if(TextInfo->ptr_arr[i][strlen(TextInfo->ptr_arr[i]) - 1] != ':')
        {
            cur_ip++;

            continue;
        }

        else
        {
            for(size_t j = 0; j < Flags->flags_num; j++)
            {
                if(strcmp(TextInfo->ptr_arr[i], Flags->flags_arr[j].text) == 0)
                {
                    Flags->flags_arr[j].ip = cur_ip;
                }
            }
        }
    }

    TextInfo->tok_num = cur_ip;
    // for(size_t i = 0; i < Flags->flags_num; i++)
    // {
    //     printf("Flag[%lu]: %-2s --> ip: %-2lu\n", i, Flags->flags_arr[i].text, Flags->flags_arr[i].ip);
    // }
    


    // заменяем поле text в массиве структур jumps_arr на соответствующий ip из массива структур flags_arr
    for(size_t i = 0; i < Flags->jumps_num; i++)
    {
        for(size_t j = 0; j < Flags->flags_num; j++)
        {
            char FLAG_text[WORD_LENGTH] = {0};

            size_t flag_len = strlen(Flags->flags_arr[j].text);
            
            strncpy(FLAG_text, Flags->flags_arr[j].text, flag_len - 1);

            if(strcmp(Flags->jumps_arr[i].text + 1, FLAG_text) == 0)
            {
                Flags->jumps_arr[i].ip = Flags->flags_arr[j].ip;
            }
        }
    }

    // for(size_t i = 0; i < Flags->jumps_num; i++)
    // {
    //     printf("Jump[%lu]: %s --> ip: %lu\n", i, Flags->jumps_arr[i].text, Flags->jumps_arr[i].ip);
    // }
}

//============================================================================================================================

void TextInfo_dtor(struct TextInfo* TextInfo, struct FlagsInfo* Flags)
{
    free(TextInfo->chars_buffer_ptr);
    free(TextInfo->ptr_arr);
    free(Flags->flags_arr);
    free(Flags->jumps_arr);
}
