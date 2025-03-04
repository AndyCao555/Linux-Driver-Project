#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/usb.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("");
MODULE_DESCRIPTION("USB keyboard driver ");

//  (Replace with actual keyboard IDs)
#define USBkeyboardID //
#define USBkeyboardProductID //

static int major_number; 

// Function runs when device file is opened
static int driver_open(struct inode* device_file, struct file* instance) {
    printk(KERN_INFO "usb_driver: Device file opened\n");
    return 0;
}

// Function runs when device file is closed
static int driver_close(struct inode* device_file, struct file* instance) {
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

    printk(KERN_INFO "usb_driver: Registered with Major %d\n", major_number);
    return 0;
}

// Function runs when USB device is disconnected
static void usb_kbd_disconnect(struct usb_interface *interface) {
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

