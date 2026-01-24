import TTY;
import Display;
import GPU;

u16 main(){
    i16 ball_x = 6000;
    i16 ball_y = 4000;
    i16 ball_r = 400;
    i16 ball_vx = 4;
    i16 ball_vy = 4;

    i16 ball_tx;
    i16 ball_ty;

    while(dsp_get_running() != 0){
        dsp_update();

        if dsp_get_stroke(DSP_STROKE_W,DSP_STATE_DOWN) != 0 {
            ball_y -= 5;
        }
        if dsp_get_stroke(DSP_STROKE_S,DSP_STATE_DOWN) != 0 {
            ball_y += 5;
        }
        if dsp_get_stroke(DSP_STROKE_A,DSP_STATE_DOWN) != 0 {
            ball_x -= 5;
        }
        if dsp_get_stroke(DSP_STROKE_D,DSP_STATE_DOWN) != 0 {
            ball_x += 5;
        }

        ball_x += ball_vx;
        ball_y += ball_vy;

        if ball_x < 100 {
            ball_x = 100;
            ball_vx = -ball_vx;
        }
        if ball_y < 100 {
            ball_y = 100;
            ball_vy = -ball_vy;
        }
        if ball_x > 12000 {
            ball_x = 12000;
            ball_vx = -ball_vx;
        }
        if ball_y > 8000 {
            ball_y = 8000;
            ball_vy = -ball_vy;
        }

        gpu_clear(GPU_BLACK);
        gpu_circle(ball_x / 100,ball_y / 100,ball_r / 100,GPU_WHITE);

        dsp_render();
    }
    return 0;
}