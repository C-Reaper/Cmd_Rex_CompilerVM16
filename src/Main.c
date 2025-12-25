#include "RexLang.h"
#define PATH_SRC "./code/Main.rex"
#define PATH_ASM "./asm/Main.svm16"

int main(int argc,char** argv){
    //if(argc<=1){
    //    printf("Missing Src File ."RexLang_TYPE"!\n");
    //    return 1;
    //}
    //if(argc<=2){
    //    printf("Missing Output File .asm/s!\n");
    //    return 1;
    //}

    RexLang ll = RexLang_New("./bin",PATH_SRC,PATH_ASM,16);
    RexLang_Print(&ll);
    RexLang_Build(&ll);
    //RexLang_Print(&ll);
    RexLang_Free(&ll);
    
    return 0;
}