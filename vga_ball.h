// #ifndef _VGA_BALL_H
// #define _VGA_BALL_H

// #include <linux/ioctl.h>

// typedef struct {
//   unsigned char red, green, blue;
// } vga_ball_color_t;

// typedef struct {
//   vga_ball_color_t background;
//   int x;
//   int y;
// } vga_ball_arg_t;

// #define VGA_BALL_MAGIC 'q'

// /* IOCTL Commands */
// #define VGA_BALL_WRITE_BACKGROUND _IOW(VGA_BALL_MAGIC, 1, vga_ball_arg_t)
// #define VGA_BALL_READ_BACKGROUND  _IOR(VGA_BALL_MAGIC, 2, vga_ball_arg_t)
// #define VGA_BALL_WRITE_COORDS _IOW(VGA_BALL_MAGIC, 3, vga_ball_arg_t)
// #define VGA_BALL_READ_COORDS  _IOR(VGA_BALL_MAGIC, 4, vga_ball_arg_t)

// #endif


#ifndef _VGA_BALL_H
#define _VGA_BALL_H

#include <linux/ioctl.h>

typedef struct {
  unsigned char red, green, blue;
} __attribute__((__packed__)) vga_ball_color_t;  // Packed to prevent padding

typedef struct {
  vga_ball_color_t background;
  int x;
  int y;
} __attribute__((__packed__)) vga_ball_arg_t;     // Packed for alignment

#define VGA_BALL_MAGIC 'q'

/* IOCTL Commands */
#define VGA_BALL_WRITE_BACKGROUND _IOW(VGA_BALL_MAGIC, 1, vga_ball_arg_t)
#define VGA_BALL_READ_BACKGROUND  _IOR(VGA_BALL_MAGIC, 2, vga_ball_arg_t)
#define VGA_BALL_WRITE_COORDS     _IOW(VGA_BALL_MAGIC, 3, vga_ball_arg_t)
#define VGA_BALL_READ_COORDS      _IOR(VGA_BALL_MAGIC, 4, vga_ball_arg_t)

#endif