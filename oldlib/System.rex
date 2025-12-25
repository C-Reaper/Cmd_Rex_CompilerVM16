namespace sys {
    i64 read(void* file,i8* cstr,i64 count){
        asm "mov rax,0";
        asm "mov rdi," file;
        asm "mov rsi," cstr;
        asm "mov rdx," count;
        asm "syscall";
        
        i64 status;
        asm "mov " status ",rax";
        return status;
    }
    void write(void* file,i8* cstr,i64 count){
        asm "mov rax,1";
        asm "mov rdi," file;
        asm "mov rsi," cstr;
        asm "mov rdx," count;
        asm "syscall";
    }  
    void* open(i8* filepath,i64 flags){
        asm "mov rax,2";
        asm "mov rdi," filepath;
        asm "mov rsi," flags;
        asm "mov rdx,0o644";
        asm "syscall";
        
        void* ptr;
        asm "mov " ptr ",rax";
        return ptr;
    }        
    void close(void* file){
        asm "mov rax,3";
        asm "mov rdi," file;
        asm "syscall";
    }
    void* mmap(i64 size){// mmap syscall
        asm "mov rax,9";
        asm "mov rdi,0";
        asm "mov rsi," size;
        asm "mov rdx,3";
        asm "mov r10,0x22";//34h
        asm "mov r8,0";//-1
        asm "mov r9,0";
        asm "syscall";
        
        void* ptr;
        asm "mov " ptr ",rax";
        return ptr;
    }
    void munmap(void* ptr,i64 size){// munmap syscall
        asm "mov rax,11";
        asm "mov rdi," ptr;
        asm "mov rsi," size;
        asm "syscall";
    }
    
    void exit(i32 exit){
        asm "mov rax,60";
        asm "mov edi," exit;
        asm "syscall";
    }
}
