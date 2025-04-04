#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "vga_ball.h"

int main() {
    // Initialize ALL struct fields
    vga_ball_arg_t ball = { 
        .background = { .red = 0, .green = 0, .blue = 0 }, 
        .x = 0, 
        .y = 0 
    };
    
    // Set max coordinates to match hardware limits (10-bit)
    int max_x = 1023, max_y = 1023;
    int fd = open("/dev/vga_ball", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device. Make sure the device is loaded using insmod.");
        return 1;
    }

    vga_ball_arg_t ball = { .x = 0, .y = 0 };
    int dx = 1, dy = 1;
    int max_x = 640, max_y = 480;

    // while (1) {
    //     ball.x += dx;
    //     ball.y += dy;

    //     if (ball.x <= 0 || ball.x >= max_x) dx = -dx;
    //     if (ball.y <= 0 || ball.y >= max_y) dy = -dy;

    //     // Check for ioctl error
    //     if (ioctl(fd, VGA_BALL_WRITE_COORDS, &ball) == -1) {
    //         perror("Failed to write coordinates using ioctl");
    //         break;
    //     }
    //     usleep(10000); // 10 ms delay for smooth movement
    // }

    while (1) {
        ball.x += dx;
        ball.y += dy;

        if (ball.x <= 0 || ball.x >= max_x) dx = -dx;
        if (ball.y <= 0 || ball.y >= max_y) dy = -dy;

        if (ioctl(fd, VGA_BALL_WRITE_COORDS, &ball) == -1) {
            perror("Failed to write coordinates using ioctl");
            break;
        }
        // NEW: Sync with 60Hz refresh rate
        usleep(16667);  // 16.67ms delay
    }

    close(fd);
    return 0;
}
