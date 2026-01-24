import TTY;

u16 main(){
    i16 a = 10;

    while(a > 0){
        if a > 10 {
            tty_print("a > 10\n");
        }
        elif a > 6 {
            tty_print("a > 6\n");
        }
        elif a > 3 {
            tty_print("a > 3\n");
        }
        elif a > 0 {
            tty_print("a > 0\n");
        }
        else{
            tty_print("else\n");
        }

        a -= 1;
    }


    /*
    i16 ball_x = 6000;
    i16 ball_y = 4000;
    i16 ball_vx = 4;
    i16 ball_vy = 4;

    while(true){
        ball_x += ball_vx;
        ball_y += ball_vy;

        tty_print_i16(ball_x);
        tty_print_i16(ball_y);
        tty_print_i16(ball_vx);
        tty_print_i16(ball_vy);

        if ball_x < 0 {
            ball_x = 0;
            ball_vx = -ball_vx;
        }
        if ball_y < 0 {
            ball_y = 0;
            ball_vy = -ball_vy;
        }
        if ball_x > 12000 {
            ball_x = 12000;
            ball_vx = -ball_vx;
        }
        if ball_y > 6000 {
            ball_y = 6000;
            ball_vy = -ball_vy;
        }

        tty_print_i16(ball_x);
        tty_print_i16(ball_y);
        tty_print_i16(ball_vx);
        tty_print_i16(ball_vy);
    }
    */
    return 0;
}