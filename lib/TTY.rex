void tty_write(u8* cstr){
    asm "ld		$20480 " cstr;
}
void tty_read(u8* cstr){
    asm "ld		$20481 " cstr;
}