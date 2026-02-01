import Integer;

namespace stream {
    void p_cstr(u8* buffer,u8* cstr){
        u16 size = 0;
        while buffer[size] != 0 {
            size += 1;
        }

        u16 cstrsize = 0;
        while cstr[cstrsize] != 0 {
            buffer[size + cstrsize] = cstr[cstrsize];
            cstrsize += 1;
        }
    }
    void p_u16(u8* buffer,u16 n){
        u16 size = 0;
        while buffer[size] != 0 {
            size += 1;
        }

        u16 d = int::digits(n);
        u16 i = d;
        u16 dig = 0;
        while i > 0 {
            dig = n % 10;
            n /= 10;
            i -= 1;
            buffer[size + i] = 48 + dig;
        }
    }
}