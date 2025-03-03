# Linux-Driver-Project



things we might be using to include.and why


 #include <linux/kernel.h>  -provides basic kernel functions like printk


#includev <Linux/module.h> -allows us to create loadable kernel modules(lkms)
defines macros like MODULE_LICENSE() and AUTHOR
need this to load or unload our modules


#include <Linux/input.h>
provides definitions for keyboard events? defines key codes like KEY_A
needed to recognize which key is being pressed


#include <Linux/keyboard.h>
provides tools for keybopard event tracking
-used to capture key presses(when and how long a key is being pressed) eg tracking sticky keys 

 #include <Linux/file.h> and #include <Linux/fs.h> are both used to open read and write files
required to save key logs
