void shell_exe(u8* exe){
    asm "ld		$30480 " exe;
}