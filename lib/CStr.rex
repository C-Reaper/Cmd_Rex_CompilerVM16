
namespace cstr {
    i16 len(i8* str){
        i16 size = 0;
        i8 ch = str[0];
        while ch != 0 {
            size = size + 1;
            ch = str[size];
        }
        return size;
    }
    void set(i8* ptr,i8 value,i16 size){
        i16 i = 0;
        while i<size {
            ptr[i] = value;
            i = i + 1;
        }
    }
    void cpy(i8* dst,i8* src,i16 size){
        i16 i = 0;
        while i<size {
            dst[i] = src[i];
            i = i + 1;
        }
    }

    bool cmp(i8* cstr1,i8* cstr2){
        i16 size1 = cstr::len(cstr1);
        i16 size2 = cstr::len(cstr2);
        
        if size1 != size2 {
            return false;
        }
        for i16 i = 0,i<size1,i+=1 {
            if cstr1[i]!=cstr2[i] {
                return false;
            }
        }
        return true;
    }
}