// 0      - 255
// 0x0000 - 0x0100

void kernel_exit(u16 code){
    asm "ld		$0  " code;
}
void kernel_sleep(u16 duration_m){
    asm "ld		$1  " duration_m;
}