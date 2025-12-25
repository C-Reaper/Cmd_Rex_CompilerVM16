
namespace sys {
    i16 read(void* file,i8* cstr,i16 count){
        asm "mov r0     " file;
        asm "mov r1     " cstr;
        asm "mov r2     " count;
        asm "mov $0     0";
        
        i16 status;
        asm "mov " status "     r0";
        return status;
    }
    void write(void* file,i8* cstr,i16 count){
        asm "mov r0     " file;
        asm "mov r1     " cstr;
        asm "mov r2     " count;
        asm "mov $0     0";
    }
}
