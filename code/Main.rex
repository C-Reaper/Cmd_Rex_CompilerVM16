import CStr;

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
    return cstr::len(buffer);
}