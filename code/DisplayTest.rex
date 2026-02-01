import Kernel;
import Display;
import GPU;

u16 main(){
    u16 x = 2500;
    u16 y = 2500;

    while dsp_get_running() != 0 {
        dsp_update();

        if dsp_get_stroke(DSP_STROKE_W,DSP_STATE_DOWN) != 0 {
            y -= 5;
        }
        if dsp_get_stroke(DSP_STROKE_S,DSP_STATE_DOWN) != 0 {
            y += 5;
        }
        if dsp_get_stroke(DSP_STROKE_A,DSP_STATE_DOWN) != 0 {
            x -= 5;
        }
        if dsp_get_stroke(DSP_STROKE_D,DSP_STATE_DOWN) != 0 {
            x += 5;
        }

        gpu_clear(GPU_BLACK);
        gpu_circle(x / 100,y / 100,10,GPU_WHITE);
        
        dsp_render();
    }

    return 0;
}