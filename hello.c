/*
 * Unified userspace program that handles both background colors
 * and bouncing ball through ioctls
 */

 #include <stdio.h>
 #include "vga_ball.h"
 #include <sys/ioctl.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <string.h>
 #include <unistd.h>
 
 int vga_ball_fd;
 
 /* Set both background color and ball coordinates */
 void set_all(const vga_ball_color_t *c, int x, int y) {
   vga_ball_arg_t vla = {
     .background = *c,
     .x = x,
     .y = y
   };
   
   if (ioctl(vga_ball_fd, VGA_BALL_WRITE_COORDS, &vla)) {
       perror("ioctl(VGA_BALL_WRITE_COORDS) failed");
       return;
   }
 }
 
 int main() {
   vga_ball_arg_t vla;
   int dx = 2, dy = 2;  // Movement speed
   const int max_x = 1023, max_y = 1023;
 
   static const vga_ball_color_t colors[] = {
     {0xff, 0x00, 0x00}, {0x00, 0xff, 0x00}, {0x00, 0x00, 0xff}
   };
 
   printf("VGA Ball Controller started\n");
 
   if ((vga_ball_fd = open("/dev/vga_ball", O_RDWR)) == -1) {
     fprintf(stderr, "Could not open device\n");
     return -1;
   }
 
   // Initial position and color
   int x = 320, y = 240;
   int color_index = 0;
 
   while (1) {
     // Update position
     x += dx;
     y += dy;
 
     // Bounce logic
     if (x <= 0 || x >= max_x) {
       dx = -dx;
       color_index = (color_index + 1) % 3;  // Change color on X bounce
     }
     if (y <= 0 || y >= max_y) dy = -dy;
 
     // Update hardware
     set_all(&colors[color_index], x, y);
     usleep(16667);  // 60Hz refresh
   }
 
   close(vga_ball_fd);
   return 0;
 }