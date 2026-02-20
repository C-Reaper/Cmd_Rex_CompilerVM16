define PS4C_X                 304;
define PS4C_O                 305;
define PS4C_TRI               307;
define PS4C_QUD               308;
define PS4C_L1                310;
define PS4C_R1                311;
define PS4C_L2                312;
define PS4C_R2                313;
define PS4C_SELECT            315;
define PS4C_OPTIONS           314;
define PS4C_HOME              316;
define PS4C_STICKL            317;
define PS4C_STICKR            318;

define PS4C_LX                0;
define PS4C_LY                1;
define PS4C_LZ                2;
define PS4C_RX                3;
define PS4C_RY                4;
define PS4C_RZ                5;
define PS4C_HAT0X             16;
define PS4C_HAT0Y             17;

define PS4C_STATE_NONE        0;
define PS4C_STATE_DOWN        1;
define PS4C_STATE_PRESSED     2;
define PS4C_STATE_RELEASED    3;
define PS4C_STATE_REPEAT      4;

u16 ps4c_get_running(){
    u16 running;
    asm "st		" running "  $30720";
    return running;
}
void ps4c_set_running(u16 running){
    asm "ld		$30720  " running;
}
void ps4c_update(){
    asm "ld		$30721  0";
}

u16 ps4c_get_stroke(u16 stroke,u16 state){
    u16 value;
    asm "ld		r0  " stroke;
    asm "ld		r1  " state;
    asm "st		" value "  $30722";
    return value;
}
i16 ps4c_get_abs(u16 stroke){
    u16 value;
    asm "ld		r0  " stroke;
    asm "st		" value "  $30723";
    return value;
}