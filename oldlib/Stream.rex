import "Alx_Lib/System.alx";
import "Alx_Lib/String.alx";

namespace ios {
    define stdin    0;
    define stdout   1;
    define stderr   2;

    struct OStream[
        IO_FILE file,
        string::String data
    ];

    impl OStream {
        pub void OStream::init(OStream* self,mem::Allocator* alloc,IO_FILE file){
            self->file = file;
            self->data = string::String::new(alloc);
        }
        pub OStream OStream::new(mem::Allocator* alloc,IO_FILE file){
            OStream stream;
            stream.init(alloc,file);
            return stream;
        }
        pub OStream OStream::open(mem::Allocator* alloc,i8* path){
            return ios::OStream::new(alloc,sys::open(path,O_WRONLY));
        }
        pub void OStream::delete(OStream* self){
            if self->file>2 {
                sys::close(self->file);
            }
            self->file = null;
            self->data.delete();
        }

        pub string::String* OStream::buff(OStream* self){
            return &(self->data);
        }
        
        pub u64 OStream::size(OStream* self){
            sys::Stat filestate;
            sys::fstat(self->file,&filestate);
            return filestate.st_size;
        }
        pub void OStream::set_pos_start(OStream* self,u64 pos){
            sys::lseek(self->file,pos,SEEK_SET);
        }
        pub void OStream::set_pos_rel(OStream* self,u64 pos){
            sys::lseek(self->file,pos,SEEK_CUR);
        }
        pub void OStream::set_pos_end(OStream* self,u64 pos){
            sys::lseek(self->file,pos,SEEK_END);
        }
        
        pub u64 OStream::ssize(OStream* self){
            return self->data.size();
        }

        pub OStream* OStream::flush(OStream* self){
            u64 size = self->data.size();
            if size>0 {
                sys::write(self->file,self->data.data(),size);
                self->data.clear();
            }
            return self;
        }

        pub OStream* OStream::char(OStream* self,i8 ch){
            self->data.char(ch);
            return self;
        }
        pub OStream* OStream::cstr(OStream* self,i8* ptr){
            self->data.cstr(ptr);
            return self;
        }
        pub OStream* OStream::cstr_s(OStream* self,i8* ptr,u64 len){
            self->data.cstr_s(ptr,len);
            return self;
        }
        pub OStream* OStream::String(OStream* self,string::String* string){
            self->data.String(string);
            return self;
        }
        pub OStream* OStream::uint(OStream* self,u64 n){
            self->data.uint(n);
            return self;
        }
        pub OStream* OStream::int(OStream* self,u64 n){
            self->data.int(n);
            return self;
        }
        pub OStream* OStream::boolean(OStream* self,bool b){
            self->data.boolean(b);
            return self;
        }
        pub OStream* OStream::float(OStream* self,f64 n,u64 postdigits){
            self->data.float(n,postdigits);
            return self;
        }

        pub OStream* OStream::hex(OStream* self,u64 n){
            self->data.hex(n);
            return self;
        }
        pub OStream* OStream::oct(OStream* self,u64 n){
            self->data.oct(n);
            return self;
        }
        pub OStream* OStream::bin(OStream* self,u64 n){
            self->data.bin(n);
            return self;
        }

        pub OStream* OStream::address(OStream* self,void* n){
            self->data.address(n);
            return self;
        }
        pub OStream* OStream::mem(OStream* self,void* n,u64 bytes){
            self->data.mem(n,bytes);
            return self;
        }

        pub void OStream::print(OStream* self){
            io::print("------------------- OStream ------------------\n");
            i8 cstr[50];
            mem::set(cstr,32,49);
            mem::cpy(cstr+48,"\n",2);
            mem::cpy(cstr,"| Header: ",10);
            math::uint::get(cstr+12,(u64)self->file);
            math::uint::get(cstr+30,(u64)self->data.size());
            io::print(cstr);
    
            for u64 i = 0,i<self->data.size(),i++ {
                
                mem::set(cstr,32,49);
                mem::cpy(cstr,"| Element: ",11);
                math::uint::get(cstr+14,(u64)i);
                //math::int_Get(cstr+18,self->data.get(i));
                cstr[18] = self->data.get(i);
                mem::cpy(cstr+48,"\n",2);
                io::print(cstr);
            }
            io::print("----------------------------------------------\n");
        }
    }

    struct IStream[
        IO_FILE file,
        mem::Allocator* alloc
    ];

    impl IStream {
        pub void IStream::init(IStream* self,mem::Allocator* alloc,IO_FILE file){
            self->file = file;
            self->alloc = alloc;
        }
        pub IStream IStream::new(mem::Allocator* alloc,IO_FILE file){
            IStream stream;
            stream.init(alloc,file);
            return stream;
        }
        pub IStream IStream::open(mem::Allocator* alloc,i8* path){
            return ios::IStream::new(alloc,sys::open(path,O_RDONLY));
        }
        pub void IStream::delete(IStream* self){
            if self->file>2 {
                sys::close(self->file);
            }
            self->file = null;
        }

        pub u64 IStream::size(IStream* self){
            sys::Stat filestate;
            sys::fstat(self->file,&filestate);
            return filestate.st_size;
        }
        pub void IStream::set_pos_start(IStream* self,u64 pos){
            sys::lseek(self->file,pos,SEEK_SET);
        }
        pub void IStream::set_pos_rel(IStream* self,u64 pos){
            sys::lseek(self->file,pos,SEEK_CUR);
        }
        pub void IStream::set_pos_end(IStream* self,u64 pos){
            sys::lseek(self->file,pos,SEEK_END);
        }

        pub i8* IStream::line(IStream* self){
            string::String line = string::String::new(self->alloc);
            i8 buffer[16];
        
            bool searching = true;
            while searching {
                mem::set(buffer,0,16);
                i64 bytes_read = sys::read(self->file,buffer,15);
        
                line.cstr(buffer);
        
                if bytes_read<15 || buffer[14]==10 {
                    searching = false;
                }
            }
        
            i8* out = line.cpy();
            line.delete();
            return out;
        }

        pub i8 IStream::char(IStream* self){
            i8* line = self->line();
            i8 ch = line[0];
            self->alloc->free(line);
            return ch;
        }
        pub void IStream::buff(IStream* self,i8* cstr,u64 size){
            i8* line = self->line();
            u64 max = cstr::len(line);
            if max<size {
                mem::cpy(cstr,line,max);
            }else{
                mem::cpy(cstr,line,size);
            }
            self->alloc->free(line);
        }
        pub i8* IStream::cstr(IStream* self,u64 size){
            i8* line = self->line();
            return line;
        }
        pub u64 IStream::uint(IStream* self){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;

            i32 i = cstr::findnot(line,"0123456789");
            u64 num = math::uint::by(line);
            self->alloc->free(line);
            return num;
        }
        pub i64 IStream::int(IStream* self){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            i32 i = cstr::findnot(line,"-0123456789");
            i64 num = math::int::by(line);
            self->alloc->free(line);
            return num;
        }
        pub bool IStream::boolean(IStream* self){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;

            bool b = math::boolean::by(line);
            self->alloc->free(line);
            return b;
        }
        pub f64 IStream::float(IStream* self){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            i32 i = cstr::findnot(line,"-.0123456789");
            f64 num = math::float::by(line);
            self->alloc->free(line);
            return num;
        }
    
        pub u64 IStream::hex(IStream* self){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            i32 i = cstr::findnot(line,"x0123456789ABCDEFabcdef");
            u64 num = math::hex::by(line);
            self->alloc->free(line);
            return num;
        }
        pub u64 IStream::oct(IStream* self){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            i32 i = cstr::findnot(line,"o01234567");
            u64 num = math::oct::by(line);
            self->alloc->free(line);
            return num;
        }
        pub u64 IStream::bin(IStream* self){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            i32 i = cstr::findnot(line,"b01");
            u64 num = math::bin::by(line);
            self->alloc->free(line);
            return num;
        }


        pub bool IStream::uint_s(IStream* self,u64* ptr){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;

            bool ret = math::uint::by_s(line,ptr);
            self->alloc->free(line);
            return ret;
        }
        pub bool IStream::int_s(IStream* self,i64* ptr){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            bool ret = math::int::by_s(line,ptr);
            self->alloc->free(line);
            return ret;
        }
        pub bool IStream::boolean_s(IStream* self,bool* ptr){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;

            bool ret = math::boolean::by_s(line,ptr);
            self->alloc->free(line);
            return ret;
        }
        pub bool IStream::float_s(IStream* self,f64* ptr){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            bool ret = math::float::by_s(line,ptr);
            self->alloc->free(line);
            return ret;
        }
    
        pub bool IStream::hex_s(IStream* self,u64* ptr){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            bool ret = math::hex::by_s(line,ptr);
            self->alloc->free(line);
            return ret;
        }
        pub bool IStream::oct_s(IStream* self,u64* ptr){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            bool ret = math::oct::by_s(line,ptr);
            self->alloc->free(line);
            return ret;
        }
        pub bool IStream::bin_s(IStream* self,u64* ptr){
            i8* line = self->line();
            u64 size = cstr::len(line)-1;
            line[size] = 0;
        
            bool ret = math::bin::by_s(line,ptr);
            self->alloc->free(line);
            return ret;
        }
    }
}

//mem::Allocator alloc = mem::Allocator::new();
//ios::OStream cout = ios::OStream::new(&alloc,1);
