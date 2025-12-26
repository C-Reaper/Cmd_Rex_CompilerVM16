u16[10] a;
    
for u16 i = 0,i<10,i+=1 {
    a[i] = i;
}
u16 sum = 0;
for u16 i = 0,i<10,i+=1 {
    sum += a[i];
}

return sum;


void tty_write(u8* cstr){
    asm "mov		$20480 " cstr;
}
void tty_read(u8* cstr){
    asm "mov		$20481 " cstr;
}

u16 main(){
    u8[32] buffer;
    tty_write("Hello World\n");
    tty_read(buffer);
    tty_write(buffer);
    return 0;
}