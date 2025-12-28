// 20735  - 30351
// 0x50FF - 0x768F
// Inst: 16

define DSP_STROKE_ESC        9;
define DSP_STROKE_W          25;
define DSP_STROKE_S          39;
define DSP_STROKE_A          38;
define DSP_STROKE_D          40;

define DSP_STATE_NONE        0;
define DSP_STATE_DOWN        1;
define DSP_STATE_PRESSED     2;
define DSP_STATE_RELEASED    3;
define DSP_STATE_REPEAT      4;

u16 dsp_get_running(){
    u16 running;
    asm "st		" running "  $20735";
    return running;
}
void dsp_set_running(u16 running){
    asm "ld		$20735  " running;
}
u16 dsp_get_width(){
    u16 width;
    asm "st		" width "  $20736";
    return width;
}
void dsp_set_width(u16 width){
    asm "ld		$20736  " width;
}
u16 dsp_get_height(){
    u16 height;
    asm "st		" height "  $20737";
    return height;
}
void dsp_set_height(u16 height){
    asm "ld		$20737  " height;
}
void dsp_update(){
    asm "ld		$20738  0";
}
void dsp_render(){
    asm "ld		$20739  0";
}

u16 dsp_get_stroke(u16 stroke,u16 state){
    u16 value;
    asm "ld		r0  " stroke;
    asm "ld		r1  " state;
    asm "st		" value "  $20740";
    return value;
}