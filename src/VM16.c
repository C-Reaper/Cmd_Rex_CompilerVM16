#if defined(__linux__)
#include "/home/codeleaded/System/Static/Library/VM16.h"
#include "/home/codeleaded/System/Static/Library/VM16_Devices.h"
#elif defined(_WINE)
#include "/home/codeleaded/System/Static/Library/VM16.h"
#include "/home/codeleaded/System/Static/Library/VM16_Devices.h"
#elif defined(_WIN32)
#include "F:/home/codeleaded/System/Static/Library/VM16.h"
#include "F:/home/codeleaded/System/Static/Library/VM16_Devices.h"
#elif defined(__APPLE__)
#error "Apple not supported!"
#else
#error "Platform not supported!"
#endif

#define PATH_ASM    "./asm/Main.svm16"
#define PATH_LINK   "./asm/Main.lvm16"
#define PATH_EXE    "./asm/Main.vm16"

int main(int argc,char** argv){
    //if(argc<=1){
    //    printf("Missing Src File ."RexLang_TYPE"!\n");
    //    return 1;
    //}
    //if(argc<=2){
    //    printf("Missing Output File .asm/s!\n");
    //    return 1;
    //}

    VM16 vm = VM16_New();
    VM16_ConnectDevice(&vm,(VM16_Ram[]){    VM16_Ram_New(VM16_MEMORY_LOW,   VM16_MEMORY_HIGH)   });
    VM16_ConnectDevice(&vm,(VM16_Kernel[]){ VM16_Kernel_New(VM16_KERNEL_LOW,VM16_KERNEL_HIGH)   });
    VM16_ConnectDevice(&vm,(VM16_TTY[]){    VM16_TTY_New(VM16_TTY_LOW,      VM16_TTY_HIGH)      });
    //VM16_ConnectDevice(&vm,(VM16_MPU[]){    VM16_MPU_New(VM16_MPU_LOW,      VM16_MPU_HIGH)      });
    VM16_ConnectDevice(&vm,(VM16_DSP[]){    VM16_DSP_New(VM16_DSP_LOW,      VM16_DSP_HIGH)      });
    VM16_ConnectDevice(&vm,(VM16_GPU[]){    VM16_GPU_New(VM16_GPU_LOW,      VM16_GPU_HIGH)      });
    //VM16_ConnectDevice(&vm,(VM16_SSD[]){    VM16_SSD_New(VM16_SSD_LOW,      VM16_SSD_HIGH)      });
    
    VM16_Comp(&vm,PATH_ASM,PATH_LINK);
    VM16_Link(&vm,VM16_MEMORY_LOW,"_start",(CStr[]){ PATH_LINK,NULL },PATH_EXE);
    VM16_Load(&vm,VM16_MEMORY_LOW,"./asm/Main.vm16");
    VM16_Run(&vm,VM16_MEMORY_LOW);

    printf("Exitcode: %d\n",vm.exitcode);

    VM16_Free(&vm);
    
    return 0;
}