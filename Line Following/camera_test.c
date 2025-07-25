#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "cam.h"

// === Motor Pin Setup ===
#define A_PHASE 16
#define A_ENABLE 17
#define B_PHASE 18
#define B_ENABLE 19

#define WRAP 255
#define CLK_DIV 1.0f

// === PID Constants ===
float Kp = 1.4;
float Ki = 0.0;
float Kd = 1.5;

float prev_error = 0;
float integral = 0;

void init_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice, WRAP);
    pwm_set_clkdiv(slice, CLK_DIV);
    pwm_set_enabled(slice, true);
}

void set_motor(uint phase_pin, uint pwm_pin, int speed) {
    if (speed >= 0) {
        gpio_put(phase_pin, 0); // Forward
    } else {
        gpio_put(phase_pin, 1); // Reverse
        speed = -speed;
    }
    if (speed > WRAP) speed = WRAP;
    pwm_set_gpio_level(pwm_pin, speed);
}

void set_motor_speeds(int com) {
    int target = 40;
    int error = com - target;

    integral += error;
    int derivative = error - prev_error;
    prev_error = error;

    float control = Kp * error + Ki * integral + Kd * derivative;

    int base_speed = WRAP * 0.55;
    int left_speed = base_speed + (int)control;
    int right_speed = base_speed - (int)control;

    // Clamp motor speeds
    if (left_speed > WRAP) left_speed = WRAP;
    if (left_speed < 0) left_speed = 0;
    if (right_speed > WRAP) right_speed = WRAP;
    if (right_speed < 0) right_speed = 0;

    set_motor(A_PHASE, A_ENABLE, left_speed);
    set_motor(B_PHASE, B_ENABLE, right_speed);

    printf("COM: %d | Error: %d | Control: %.2f | L:%d R:%d\n", com, error, control, left_speed, right_speed);
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, camera!\n");

    init_camera_pins();

    gpio_init(A_PHASE);
    gpio_set_dir(A_PHASE, GPIO_OUT);
    gpio_init(B_PHASE);
    gpio_set_dir(B_PHASE, GPIO_OUT);

    init_pwm(A_ENABLE);
    init_pwm(B_ENABLE);

    while (true) {
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            char ch = (char)c;
            if (ch == 'q' || ch == 'Q') {
                printf("Quitting.\n");
                break;
            }
        }

        setSaveImage(1);
        while (getSaveImage() == 1) {}

        convertImage();
        int com = findLine(IMAGESIZEY / 2);
        setPixel(IMAGESIZEY / 2, com, 0, 255, 0);
        printImage();
        printf("%d\r\n", com);
        set_motor_speeds(com);

        sleep_ms(100);
    }

    // Stop motors
    set_motor(A_PHASE, A_ENABLE, 0);
    set_motor(B_PHASE, B_ENABLE, 0);

    return 0;
}
