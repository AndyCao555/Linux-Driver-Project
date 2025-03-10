#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/usb.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name <your.email@example.com>");
MODULE_DESCRIPTION("USB keyboard driver");

// Replace with actual keyboard IDs
#define USBkeyboardID 0x1234  // Example vendor ID
#define USBkeyboardProductID 0x5678  // Example product ID

static int major_number;
static struct class *usb_kbd_class;
static struct device *usb_kbd_device;

// Function runs when device file is opened
static int driver_open(struct inode *device_file, struct file *instance) {
    printk(KERN_INFO "usb_driver: Device file opened\n");
    return 0;
}

// Function runs when device file is closed
static int driver_close(struct inode *device_file, struct file *instance) {
    printk(KERN_INFO "usb_driver: Device file closed\n");
    return 0;
}

// File operations struct
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
};

// USB Device ID Table (Matches Supported Devices)
static struct usb_device_id usb_kbd_id_table[] = {
    { USB_DEVICE(USBkeyboardID, USBkeyboardProductID) },
    {} /* End of table */
};
MODULE_DEVICE_TABLE(usb, usb_kbd_id_table);

// Function runs when USB device is connected
static int usb_kbd_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    printk(KERN_INFO "usb_driver: USB Keyboard connected\n");

    // Register a character device
    major_number = register_chrdev(0, "usb_keylogger", &fops);
    if (major_number < 0) {
        printk(KERN_ERR "usb_driver: Failed to register character device\n");
        return major_number;
    }

    // Create device class
    usb_kbd_class = class_create("usb_keylogger");  // Updated to match new signature
    if (IS_ERR(usb_kbd_class)) {
        printk(KERN_ERR "usb_driver: Failed to create device class\n");
        unregister_chrdev(major_number, "usb_keylogger");
        return PTR_ERR(usb_kbd_class);
    }

    // Create device node
    usb_kbd_device = device_create(usb_kbd_class, NULL, MKDEV(major_number, 0), NULL, "usb_keylogger");
    if (IS_ERR(usb_kbd_device)) {
        printk(KERN_ERR "usb_driver: Failed to create device\n");
        class_destroy(usb_kbd_class);
        unregister_chrdev(major_number, "usb_keylogger");
        return PTR_ERR(usb_kbd_device);
    }

    printk(KERN_INFO "usb_driver: Registered with Major %d\n", major_number);
    return 0;
}

// Function runs when USB device is disconnected
static void usb_kbd_disconnect(struct usb_interface *interface) {
    device_destroy(usb_kbd_class, MKDEV(major_number, 0));
    class_destroy(usb_kbd_class);
    unregister_chrdev(major_number, "usb_keylogger");
    printk(KERN_INFO "usb_driver: USB Keyboard disconnected\n");
}

// USB Driver Structure
static struct usb_driver usb_kbd_driver = {
    .name = "usb_projectnamehere",
    .id_table = usb_kbd_id_table,
    .probe = usb_kbd_probe,
    .disconnect = usb_kbd_disconnect,
};

// Function runs when module is inserted
static int __init init_mdl(void) {
    int retval;
    printk(KERN_INFO "usb_driver: Module loaded\n");

    retval = usb_register(&usb_kbd_driver);
    if (retval) {
        printk(KERN_ERR "usb_driver: Failed to register USB driver\n");
        return retval;
    }

    return 0;
}

// Function runs when module is removed
static void __exit exit_mdl(void) {
    usb_deregister(&usb_kbd_driver);
    printk(KERN_INFO "usb_driver: Module unloaded\n");
}

module_init(init_mdl);
module_exit(exit_mdl);
