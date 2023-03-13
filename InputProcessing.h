#ifndef INPUT_PROCESSING_H
#define INPUT_PROCESSING_H

//================================================================================================

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

//================================================================================================

const int WORD_LENGTH = 32;

//================================================================================================

struct FlagsToken
{
    char text[WORD_LENGTH] = {0};           //| 
    size_t ip = 0;                          //| 
};

struct FlagsInfo
{
    FlagsToken* flags_arr = nullptr;        //| 
    size_t cur_flag       = 0;              //| 
    size_t flags_num      = 0;              //| 

    FlagsToken* jumps_arr = nullptr;        //| 
    size_t cur_jump       = 0;              //| 
    size_t jumps_num      = 0;              //| 
};

struct TextInfo
{
    FILE* mainfile         = nullptr;       //| 
    char* chars_buffer_ptr = nullptr;       //| 
    char** ptr_arr         = nullptr;       //| 

    size_t chars_number = 0;                //| 
    size_t words_num    = 0;                //| 
    size_t cur_word     = 0;                //| 
    size_t tok_num      = 0;                //| 

    int error = 0;                          //| 
};

//================================================================================================

enum FLagsError
{
    ERROR_LEXICAL_PROCESSING = 1,           //| 
    ERROR_FLAGS_NUM          = 2,           //| 
    ERROR_DOUBLE_FLAGS       = 3,           //| 
    ERROR_JUMP_WITHOUT_FLAG  = 4,           //| 
    ERROR_UNKNOWN_JUMP_FLAG  = 5,           //| 
    ERROR_FLAGS_CALLOC_NULL  = 6,           //| 
    ERROR_JUMPS_CALLOC_NULL  = 7,           //| 
};

//================================================================================================

void TextInfo_ctor(struct TextInfo* TextInfo, struct FlagsInfo* Flags, const char* filename);

//================================================================================================

void open_file(struct TextInfo* TextInfo, const char* filename);

//================================================================================================

void num_of_chars(struct TextInfo* TextInfo, const char* filename);

//================================================================================================

void chars_buffer(struct TextInfo* TextInfo);

//================================================================================================

void count_words(struct TextInfo* TextInfo);

//================================================================================================

void fill_ptr_arr(struct TextInfo* TextInfo);

//================================================================================================

void lexical_processing(struct TextInfo* TextInfo, struct FlagsInfo* Flags);

//================================================================================================

void flags_processing(struct TextInfo* TextInfo, struct FlagsInfo* Flags);

//================================================================================================

void TextInfo_dtor(struct TextInfo* TextInfo, struct FlagsInfo* Flags);

//================================================================================================

#endif // INPUT_PROCESSING_H
