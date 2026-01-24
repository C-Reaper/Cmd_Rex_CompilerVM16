// 30351  - 30464
// 0x768F - 0x7700

define GPU_BLACK    0x0000;
define GPU_RED      0xF000;
define GPU_BLUE     0x000F;
define GPU_WHITE    0xFFFF;

void gpu_clear(u16 color){
    asm "ld		$30351  " color;
}
void gpu_rect(u16 x,u16 y,u16 w,u16 h,u16 color){
    u16 reg0 = h;
    asm "ld		r0  " x;
    asm "ld		r1  " y;
    asm "ld		r2  " w;
    asm "ld		$30352  " color;
}
void gpu_line(u16 x1,u16 y1,u16 x2,u16 y2,u16 color){
    u16 reg0 = y2;
    asm "ld		r0  " x1;
    asm "ld		r1  " y1;
    asm "ld		r2  " x2;
    asm "ld		$30353  " color;
}
void gpu_circle(u16 x,u16 y,u16 r,u16 color){
    asm "ld		r0  " x;
    asm "ld		r1  " y;
    asm "ld		r2  " r;
    asm "ld		$30354  " color;
}
void gpu_cstr(u8* buff,u16 len,u16 x,u16 y,u16 color){
    u16 reg0 = y;
    asm "ld		r0  " buff;
    asm "ld		r1  " len;
    asm "ld		r2  " x;
    asm "ld		$30355  " color;
}
void gpu_rendercstrln(u8* buff,u16 len,u16 color){
    asm "ld		r0  " buff;
    asm "ld		r1  " len;
    asm "ld		$30356  " color;
}