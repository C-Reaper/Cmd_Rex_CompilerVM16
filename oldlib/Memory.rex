import "Alx_Lib/System.alx";
import "Alx_Lib/IO.alx";

namespace mem {
    void set(void* ptr,i8 value,u64 size){
        i8* cptr = (i8*)ptr;
        i64 todo = size / 8 * 8;
        i64 rest = size - todo;
        
        i64* dst64 = (i64*)cptr;
        i64 value64 = value;
        i64 comp = value64 | value64<<8 | value64<<16 | value64<<24 | value64<<32 | value64<<40 | value64<<48 | value64<<56;
        i64 i = 0;
        for ,i<todo,i+=8 {
            *(dst64+i) = comp;
        }
        for ,i<size,i++ {
            cptr[i] = value;
        }
    }
    void cpy(void* dst,void* src,i64 size){
        i8* cdst = (i8*)dst;
        i8* csrc = (i8*)src;
        
        i64 todo = size / 8 * 8;
        i64 rest = size - todo;
        
        i64* dst64 = (i64*)dst;
        i64* src64 = (i64*)src;
        i64 i = 0;
        for ,i<todo,i+=8 {
            *(dst64+i) = (i64)*(src64+i);
        }
        for ,i<size,i++ {
            cdst[i] = (i8)csrc[i];
        }
    }
    void mov(void* dst,void* src,i64 size){
        i8* cdst = (i8*)dst;
        i8* csrc = (i8*)src;
        
        i64 todo = size / 8 * 8;
        i64 rest = size - todo;
        
        i64* dst64 = (i64*)dst;
        i64* src64 = (i64*)src;
        i64 i = 0;
        for ,i<todo,i+=8 {
            *(dst64+i) = (i64)*(src64+i);
            *(src64+i) = 0;
        }
        for ,i<size,i++ {
            cdst[i] = (i8)csrc[i];
            csrc[i] = 0;
        }
    }
    void shl(void* dst,i64 count,i64 size){
        i8* cdst = (i8*)dst;
        
        i64 todo = size / 8 * 8;
        i64 rest = size - todo;
        
        i8* src = (i8*)(dst - count);
        i64* dst64 = (i64*)src;
        i64* src64 = (i64*)dst;
        i64 i = 0;
        for ,i<todo,i+=8 {
            *(dst64+i) = (i64)*(src64+i);
        }
        for ,i<size,i++ {
            cdst[i] = (i8)src[i];
        }
    }
    void shr(void* dst,i64 count,i64 size){
        i8* cdst = (i8*)dst;
        
        i64 todo = size / 8 * 8;
        i64 rest = size - todo;
        
        i8* src = (i8*)(dst + count);
        i64* dst64 = (i64*)src;
        i64* src64 = (i64*)dst;
        i64 i = size-1;
        for ,i>todo,i-- {
            cdst[i] = (i8)src[i];
        }
        for ,i>=0,i-=8 {
            *(dst64+i) = (i64)*(src64+i);
        }
    }

    void msleep(u64 msec){
        //u64 msecwo = msec - msec / 1000 * 1000;
        sys::Timespec sp = sys::Timespec[ msec / 1000,(msec % 1000) * 1000000 ];
        sys::nanosleep(&sp,0);
    }
    void usleep(u64 usec){
        //u64 msecwo = msec - msec / 1000 * 1000;
        sys::Timespec sp = sys::Timespec[ usec / 1000000,(usec % 1000000) * 1000 ];
        sys::nanosleep(&sp,0);
    }
    void nsleep(u64 nsec){
        //u64 msecwo = msec - msec / 1000 * 1000;
        sys::Timespec sp = sys::Timespec[ nsec / 1000000000,nsec % 1000000000 ];
        sys::nanosleep(&sp,0);
    }

    struct Header[
        pub u32 length,
        pub u8 flags
    ];

    struct Allocator[
        void* memory,
        u64 length
    ];

    impl Allocator{
        pub void Allocator::init(Allocator* self,u64 size){
            self->memory = sys::mmap(size);
            self->length = size;
            mem::set(self->memory,0,size);

            Header* first = self->memory;
            first->length = size - 5;
        }
        pub void Allocator::delete(Allocator* self){
            if self->memory != 0 {
                sys::munmap(self->memory,self->length);
            }
            self->memory = 0;
            self->length = 0;
        }
        
        /*pub destroy void Allocator::delete(Allocator* self){
            if self->memory != 0 {
                sys::munmap(self->memory,self->length);
                io::print("Freed!\n");
            }
            self->memory = 0;
            self->length = 0;

            io::print("Destroyed!\n");
        }
        pub move void Allocator::mover(Allocator* self,Allocator* other){
            self->memory = other->memory;
            self->length = other->length;
            other->memory = 0;
            other->length = 0;
        }*/
        
        pub Allocator Allocator::new(){
            Allocator alloc;
            alloc.init(10000);
            return alloc;
        }
        pub Allocator Allocator::make(u64 size){
            Allocator alloc;
            alloc.init(size);
            return alloc;
        }

        pub void* Allocator::getMemory(Allocator* self){
            return self->memory;
        }
        pub u64 Allocator::getLength(Allocator* self){
            return self->length;
        }

        pub void* Allocator::alloc(Allocator* self,u64 size){
            void* ptr = null;
            Header* first = self->memory;
            
            while true {
                if first+5+size>self->memory+self->length {
                    return null;
                }elif first->flags==0 && first->length>=size {
                    u64 rem = first->length - size;
                    if rem<=5 {
                        size = first->length;
                    }
                    first->flags = 1;
                    first->length = size;
                    ptr = first + 5;

                    Header* next = ptr + size;
                    if rem>5 && next+rem<=self->memory+self->length {
                        next->length = rem - 5;
                        next->flags = 0;
                    }
                    return ptr;
                }else{
                    first = first + 5 + first->length;
                }
            }
            return ptr;
        }
        pub void Allocator::free(Allocator* self,void* ptr){
            if ptr<self->memory || ptr>self->memory+self->length {
                return;
            }
            
            Header* prev = null;
            Header* first = self->memory;
            
            while true {
                if first+5+first->length > self->memory+self->length {
                    return;
                }elif ptr >= first && ptr < first+5+first->length {
                    first->flags = 0;
                    
                    Header* next = first+5+first->length;
                    if next<=self->memory+self->length {
                        if next->flags==0 {
                            first->length += 5 + next->length;
                        }
                    }

                    if prev>=self->memory {
                        if prev->flags==0 {
                            prev->length += 5 + first->length;
                        }
                    }
                    return;
                }else{
                    prev = first;
                    first = first + 5 + first->length;
                }
            }
        }
        
        pub void* Allocator::realloc(Allocator* self,void* ptr,u64 size){
            if ptr<self->memory || ptr>self->memory+self->length {
                return null;
            }
            
            Header* first = self->memory;
            
            while true {
                if first+5+first->length > self->memory+self->length {
                    return null;
                }elif ptr >= first && ptr < first+5+first->length {
                    Header* next = first+5+first->length;
                    
                    if first->length>size {
                        i32 diff = first->length - size - 5;
                        if diff>0 {
                            first->length = size;
                            Header* renext = first+5+first->length;
                            renext->flags = 0;
                            renext->length = diff;
                        }
                        return ptr;
                    }elif next->flags==0 {
                        if first->length+5+next->length>size {
                            i32 diff = first->length + next->length - size;// + 5 & - 5 => 0
                            if diff>0 {
                                first->length = size;
                                Header* renext = first+5+first->length;
                                renext->flags = 0;
                                renext->length = diff;
                            }else{
                                first->length += 5 + next->length;
                            }
                            return ptr;
                        }
                    }

                    void* newptr = self->alloc(size);
                    mem::cpy(newptr,ptr,first->length);
                    self->free(ptr);
                    return newptr;
                }else{
                    first = first + 5 + first->length;
                }
            }
            return ptr;
        }
        pub void* Allocator::calloc(Allocator* self,u64 count,u64 elementsize){
            void* ptr = self->alloc(count * elementsize);
            mem::set(ptr,0,count * elementsize);
            return ptr;
        }

        pub void Allocator::print(Allocator* self){
            io::print("------------------ Allocator -----------------\n");
            i8 cstr[48];
            mem::set(cstr,32,45);
            mem::cpy(cstr,"| Memory:",9);
            math::uint::get(cstr+10,self->getMemory());
            mem::cpy(cstr+45,"\n",2);
            io::print(cstr);

            mem::set(cstr,32,45);
            mem::cpy(cstr,"| Length:",9);
            math::uint::get(cstr+10,self->getLength());
            mem::cpy(cstr+45,"\n",2);
            io::print(cstr);

            Header* first = self->memory;
            while first>=self->memory && first<self->memory+self->length {
                mem::set(cstr,32,45);
                mem::cpy(cstr,"| Block:",8);
                math::uint::get(cstr+9,first);
                math::uint::get(cstr+28,first->length);
                math::uint::get(cstr+35,first->flags);
                mem::cpy(cstr+45,"|",1);
                mem::cpy(cstr+46,"\n",2);
                io::print(cstr);
            
                first = first + 5 + first->length;
            }

            io::print("----------------------------------------------\n");
        }
    }
}
