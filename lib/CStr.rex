
namespace cstr {
    u16 len(u8* cstr){
        u16 size = 0;
        while cstr[size] != 0 {
            size = size + 1;
        }
        return size;
    }
    void set(u8* ptr,u8 value,u16 size){
        u16 i = 0;
        while i<size {
            ptr[i] = value;
            i = i + 1;
        }
    }
    void cpy(u8* dst,u8* src,u16 size){
        u16 i = 0;
        while i<size {
            dst[i] = src[i];
            i = i + 1;
        }
    }

    bool cmp(u8* cstr1,u8* cstr2){
        u16 size1 = cstr::len(cstr1);
        u16 size2 = cstr::len(cstr2);
        
        if size1 != size2 {
            return false;
        }
        for u16 i = 0,i<size1,i+=1 {
            if cstr1[i]!=cstr2[i] {
                return false;
            }
        }
        return true;
    }
}