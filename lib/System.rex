
namespace sys {
    u16 read(void* file,u8* cstr,u16 count){
        asm "mov r0     " file;
        asm "mov r1     " cstr;
        asm "mov r2     " count;
        asm "mov $0     0";
        
        u16 status;
        asm "mov " status "     r0";
        return status;
    }
    void write(void* file,u8* cstr,u16 count){
        asm "mov r0     " file;
        asm "mov r1     " cstr;
        asm "mov r2     " count;
        asm "mov $0     0";
    }
}
