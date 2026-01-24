import Kernel;
import Display;
import GPU;

u16 main(){
    i16 p1_x = 0;
    i16 p1_y = 0;
    i16 p1_w = 500;
    i16 p1_h = 1500;

    i16 p2_x = 11500;
    i16 p2_y = 0;
    i16 p2_w = 500;
    i16 p2_h = 1500;

    i16 ball_x = 6000;
    i16 ball_y = 4000;
    i16 ball_r = 400;
    i16 ball_vx = 6;
    i16 ball_vy = 6;

    i16 ball_tx;
    i16 ball_ty;

    i16 score1 = 0;
    i16 score2 = 0;

    while dsp_get_running() != 0 {
        dsp_update();

        if dsp_get_stroke(DSP_STROKE_W,DSP_STATE_DOWN) != 0 {
            p1_y -= 20;
        }
        if dsp_get_stroke(DSP_STROKE_S,DSP_STATE_DOWN) != 0 {
            p1_y += 20;
        }
        if dsp_get_stroke(DSP_STROKE_UP,DSP_STATE_DOWN) != 0 {
            p2_y -= 20;
        }
        if dsp_get_stroke(DSP_STROKE_DOWN,DSP_STATE_DOWN) != 0 {
            p2_y += 20;
        }
        if p1_y < 0 {
            p1_y = 0;
        }
        if p2_y < 0 {
            p2_y = 0;
        }
        if p1_y > 8000 {
            p1_y = 8000;
        }
        if p2_y > 8000 {
            p2_y = 8000;
        }

        ball_x += ball_vx;
        ball_y += ball_vy;

        if ball_x < 0 {
            ball_x = 6000;
            ball_y = 4000;
            ball_vx = 6;
            ball_vy = 6;
            score2 += 1;
        }
        if ball_y < 0 {
            ball_y = 0;
            ball_vy = -ball_vy;
        }
        if ball_x > 12000 {
            ball_x = 6000;
            ball_y = 4000;
            ball_vx = 6;
            ball_vy = 6;
            score1 += 1;
        }
        if ball_y > 8000 {
            ball_y = 8000;
            ball_vy = -ball_vy;
        }

        if ball_y + ball_r >= p1_y && ball_y - ball_r < p1_y + p1_h && ball_x + ball_r >= p1_x && ball_x - ball_r < p1_x + p1_w {
            ball_vx = -ball_vx;
            ball_vx = ball_vx + ball_vx / 2;
        }
        if ball_y + ball_r >= p2_y && ball_y - ball_r < p2_y + p1_h && ball_x + ball_r >= p2_x && ball_x - ball_r < p2_x + p2_w {
            ball_vx = -ball_vx;
            ball_vx = ball_vx + ball_vx / 2;
        }

        gpu_clear(GPU_BLACK);

        gpu_circle(ball_x / 100,ball_y / 100,ball_r / 100,GPU_WHITE);
        gpu_rect(p1_x / 100,p1_y / 100,p1_w / 100,p1_h / 100,GPU_RED);
        gpu_rect(p2_x / 100,p2_y / 100,p2_w / 100,p2_h / 100,GPU_BLUE);
        
        dsp_render();
    }

    return 0;
}