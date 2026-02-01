void tty_print(u8* cstr){
    asm "ld		$20480 " cstr;
}
void tty_print_i16(i16 n){
    asm "ld		$20481 " n;
}
void tty_print_hex(u16 n){
    asm "ld		$20482 " n;
}
void tty_get(u8* cstr){
    asm "ld		$20483 " cstr;
}