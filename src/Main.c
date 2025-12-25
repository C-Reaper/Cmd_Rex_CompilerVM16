#if defined(__linux__)
#include "/home/codeleaded/System/Static/Library/RexLang.h"
#include "/home/codeleaded/System/Static/Library/VM16.h"
#include "/home/codeleaded/System/Static/Library/VM16_Devices.h"
#elif defined(_WINE)
#include "/home/codeleaded/System/Static/Library/RexLang.h"
#include "/home/codeleaded/System/Static/Library/VM16.h"
#include "/home/codeleaded/System/Static/Library/VM16_Devices.h"
#elif defined(_WIN32)
#include "F:/home/codeleaded/System/Static/Library/RexLang.h"
#include "F:/home/codeleaded/System/Static/Library/VM16.h"
#include "F:/home/codeleaded/System/Static/Library/VM16_Devices.h"
#elif defined(__APPLE__)
#error "Apple not supported!"
#else
#error "Platform not supported!"
#endif

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
    //RexLang_Print(&ll);
    RexLang_Build(&ll);
    //RexLang_Print(&ll);
    RexLang_Free(&ll);

    // VM16 vm = VM16_New();
    // VM16_ConnectDevice(&vm,(VM16_Ram[]){    VM16_Ram_New(VM16_MEMORY_LOW,   VM16_MEMORY_HIGH)   });
    // VM16_ConnectDevice(&vm,(VM16_Kernel[]){ VM16_Kernel_New(VM16_KERNEL_LOW,VM16_KERNEL_HIGH)   });
    // VM16_ConnectDevice(&vm,(VM16_TTY[]){    VM16_TTY_New(VM16_TTY_LOW,      VM16_TTY_HIGH)      });
    
    // VM16_Comp(&vm,VM16_MEMORY_LOW,"./code/Main.svm16","./code/Main.vm16");
    // VM16_Load(&vm,VM16_MEMORY_LOW,"./code/Main.vm16");
    // VM16_Run(&vm,VM16_MEMORY_LOW);

    // printf("Exitcode: %d\n",vm.exitcode);

    // VM16_Free(&vm);
    
    return 0;
}