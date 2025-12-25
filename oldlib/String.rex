import "Alx_Lib/Container.alx";
import "Alx_Lib/Memory.alx";

namespace string {
    struct String[
        pub con::Vector vec,
        mem::Allocator* alloc
    ];

    impl String {
        pub void String::init(String* self,mem::Allocator* alloc){
            self->vec = con::Vector::new(alloc,1);
            self->alloc = alloc;
        }
        pub String String::new(mem::Allocator* alloc){
            String cstr;
            cstr.init(alloc);
            return cstr;
        }
        pub void String::delete(String* self){
            self->vec.delete();
            self->alloc = null;
        }

        pub String String::make(mem::Allocator* alloc,i8* add){
            String string;
            string.init(alloc);
            string.cstr(add);
            return string;
        }

        pub u64 String::size(String* self){
            return self->vec.size();
        }
        pub i8* String::data(String* self){
            return (i8*)self->vec.data();
        }
        pub void String::clear(String* self){
            self->vec.clear();
        }

        pub i8* String::cpy(String* self){
            u64 size = self->vec.size();
            i8* out = self->alloc->alloc(size + 1);
            mem::cpy(out,self->data(),size);
            out[size] = 0;
            return out;
        }

        pub i8* String::get(String* self,i32 i){
            return *(i8*)self->vec.get(i);
        }
        pub void String::set(String* self,i32 i,i8 ch){
            self->vec.set(i,&ch);
        }

        pub String* String::String(String* self,String* ptr){
            self->vec.push_c(ptr->data(),ptr->size());
            return self;
        }
        pub String* String::char(String* self,i8 ch){
            self->vec.push(&ch);
            return self;
        }
        pub String* String::cstr(String* self,i8* ptr){
            u64 len = cstr::len(ptr);
            self->vec.push_c(ptr,len);
            return self;
        }
        pub String* String::cstr_s(String* self,i8* ptr,u64 len){
            self->vec.push_c(ptr,len);
            return self;
        }
        pub String* String::uint(String* self,u64 n){
            i8 block[48];
            mem::set(block,0,48);
            math::uint::get(block,n);
            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }
        pub String* String::int(String* self,i64 n){
            i8 block[48];
            mem::set(block,0,48);
            math::int::get(block,n);
            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }
        pub String* String::boolean(String* self,bool b){
            i8 block[8];
            mem::set(block,0,8);
            math::boolean::get(block,b);
            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }
        pub String* String::float(String* self,f64 n,u64 postdigits){
            i8 block[48];
            mem::set(block,0,48);
            math::float::get(block,n,postdigits);
            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }

        pub String* String::hex(String* self,i64 n){
            i8 block[48];
            mem::set(block,0,48);
            math::hex::get(block,n);
            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }
        pub String* String::oct(String* self,i64 n){
            i8 block[48];
            mem::set(block,0,48);
            math::oct::get(block,n);
            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }
        pub String* String::bin(String* self,i64 n){
            i8 block[48];
            mem::set(block,0,48);
            math::bin::get(block,n);
            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }

        pub String* String::address(String* self,void* n){
            i8 block[48];
            mem::set(block,0,48);
            
            void** ptr = &n;
            for i32 j = 0,j<8,j++ {
                u8 ch = *((u8*)(ptr+j));
                *(block+j*2) = char::to::hex((ch & 0xF0) >> 8);
                *(block+j*2+1) = char::to::hex(ch & 0xF);
            }

            u64 len = cstr::len(block);
            self->vec.push_c(block,len);
            return self;
        }
        pub String* String::mem(String* self,void* n,u64 bytes){
            u64 count = bytes * 2;
            i8* block = self->alloc->alloc(count+1);
            
            mem::set(block,0,count+1);
            
            for i32 j = 0,j<bytes,j++ {
                u8 ch = *((u8*)(n+j));
                *(block+j*2) = char::to::hex((ch & 0xF0) >> 8);
                *(block+j*2+1) = char::to::hex(ch & 0xF);
            }

            self->vec.push_c(block,count);
            self->alloc->free(block);
            return self;
        }
    }
}