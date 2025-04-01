#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "vga_ball.h"

#define DRIVER_NAME "vga_ball"

/* Device registers */
#define BG_RED(x) (x)
#define BG_GREEN(x) ((x)+1)
#define BG_BLUE(x) ((x)+2)
#define BALL_COORDS(x) ((x)+3)

/* Device state */
struct vga_ball_dev {
    struct resource res;
    void __iomem *virtbase;
    vga_ball_color_t background;
    int x, y;
} dev;

/* Write background color */
static void write_background(vga_ball_color_t *background) {
    iowrite8(background->red, BG_RED(dev.virtbase));
    iowrite8(background->green, BG_GREEN(dev.virtbase));
    iowrite8(background->blue, BG_BLUE(dev.virtbase));
    dev.background = *background;
}

/* Write ball coordinates */
// static void write_coords(int x, int y) {
//     iowrite32((y << 16) | (x & 0xFFFF), BALL_COORDS(dev.virtbase));
//     dev.x = x;
//     dev.y = y;
// }
static void write_coords(int x, int y) {
    // NEW: Split into 4 register writes
    iowrite8(x & 0xFF, dev.virtbase + 0);         // X Low
    iowrite8((x >> 8) & 0x03, dev.virtbase + 1);  // X High
    iowrite8(y & 0xFF, dev.virtbase + 2);         // Y Low
    iowrite8((y >> 8) & 0x03, dev.virtbase + 3);  // Y High
    dev.x = x;
    dev.y = y;
}

/* Handle IOCTL calls */
static long vga_ball_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
    if (arg == 0) {
        printk(KERN_ERR "Invalid argument passed to ioctl.\n");
        return -EINVAL;
    }

    vga_ball_arg_t vla;

    switch (cmd) {
    case VGA_BALL_WRITE_BACKGROUND:
        if (copy_from_user(&vla, (vga_ball_arg_t *)arg, sizeof(vga_ball_arg_t))) {
            printk(KERN_ERR "Failed to copy background from user space.\n");
            return -EFAULT;
        }
        write_background(&vla.background);
        break;

    case VGA_BALL_READ_BACKGROUND:
        vla.background = dev.background;
        if (copy_to_user((vga_ball_arg_t *)arg, &vla, sizeof(vga_ball_arg_t))) {
            printk(KERN_ERR "Failed to copy background to user space.\n");
            return -EFAULT;
        }
        break;

    case VGA_BALL_WRITE_COORDS:
        if (copy_from_user(&vla, (vga_ball_arg_t *)arg, sizeof(vga_ball_arg_t))) {
            printk(KERN_ERR "Failed to copy coordinates from user space.\n");
            return -EFAULT;
        }
        write_coords(vla.x, vla.y);
        break;

    case VGA_BALL_READ_COORDS:
        vla.x = dev.x;
        vla.y = dev.y;
        if (copy_to_user((vga_ball_arg_t *)arg, &vla, sizeof(vga_ball_arg_t))) {
            printk(KERN_ERR "Failed to copy coordinates to user space.\n");
            return -EFAULT;
        }
        break;

    default:
        printk(KERN_ERR "Invalid ioctl command.\n");
        return -EINVAL;
    }

    return 0;
}


/* File operations */
static const struct file_operations vga_ball_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = vga_ball_ioctl,
};

/* Register the device */
static struct miscdevice vga_ball_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DRIVER_NAME,
    .fops = &vga_ball_fops,
};

/* Initialization and cleanup */
static int __init vga_ball_init(void) {
    pr_info(DRIVER_NAME ": init\n");
    return misc_register(&vga_ball_misc_device);
}

static void __exit vga_ball_exit(void) {
    misc_deregister(&vga_ball_misc_device);
    pr_info(DRIVER_NAME ": exit\n");
}

module_init(vga_ball_init);
module_exit(vga_ball_exit);
MODULE_LICENSE("GPL");
