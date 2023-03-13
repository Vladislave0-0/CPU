#include "InputProcessing.h"
#include "Tokenization.h"

//================================================================================================

int main()
{
    struct TextInfo TextInfo = {};
    struct FlagsInfo Flags   = {};
    struct AsmInfo AsmInfo   = {};

    TextInfo_ctor(&TextInfo, &Flags, "input.txt");
    if(TextInfo.error != 0)
    {
        TextInfo_dtor(&TextInfo, &Flags);

        return TextInfo.error;
    }


    AsmInfo_ctor(&AsmInfo, &TextInfo, &Flags);
    if(AsmInfo.error != 0)
    {
        TextInfo_dtor(&TextInfo, &Flags);

        AsmInfo_dtor(&AsmInfo);

        return -1;
    }

    // printf("code:\n");
    // for(size_t i = 0; i < AsmInfo.quantity; i++)
    // {
    //     printf("%d ", AsmInfo.code[i]);
    // }
    // printf("\n");

    TextInfo_dtor(&TextInfo, &Flags);
    AsmInfo_dtor(&AsmInfo);
}
