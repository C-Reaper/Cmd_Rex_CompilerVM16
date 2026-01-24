import Kernel;
import Display;
import GPU;
import Shell;

void shell_render(u8* buffer,u16* len,u16* stop){
    buffer[(*len)] = '\n';
    (*len) += 1;
    buffer[(*len)] = '$';
    (*len) += 1;
    buffer[(*len)] = ' ';
    (*len) += 1;
    (*stop) = (*len);
}
void shell_start(){
    u16 stop = 0;
    u16 len = 0;
    u8[1000] buffer;

    shell_render(buffer,&len,&stop);

    while dsp_get_running() != 0 {
        dsp_update();

        u16 last = dsp_get_lastchar();
        if dsp_get_stroke(DSP_STROKE_BACKSPACE,DSP_STATE_PRESSED) != 0 {
            if len > stop {
                len -= 1;
                buffer[len] = ' ';
            }
        }elif dsp_get_stroke(DSP_STROKE_ENTER,DSP_STATE_PRESSED) != 0 {
            buffer[len] = 0;
            shell_exe(buffer + stop);
            shell_render(buffer,&len,&stop);
        }elif last != 0 {
            buffer[len] = (u8)last;
            len += 1;
        }

        gpu_clear(GPU_BLACK);
        gpu_rendercstrln(buffer,len,GPU_WHITE);
        
        dsp_render();
    }
}

u16 main(){
    shell_start();
    return 0;
}