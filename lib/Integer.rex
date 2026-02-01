
namespace int {
    u16 digits(u16 n){
        u16 i = 0;
        u16 divid = 10000;
        u16 digit = 0;
        while digit == 0 && i < 5 {
            digit = n / divid;
            divid /= 10;
            i += 1;
        }
        return 6 - i;
    }
}