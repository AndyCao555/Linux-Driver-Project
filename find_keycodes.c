#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

int main() {
    const char *device_path = "/dev/input/event1"; // Replace with the correct event device
    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    printf("Listening for key events on %s...\n", device_path);
    printf("Press keys to see their keycodes. Press Ctrl+C to exit.\n");

    struct input_event ev;
    while (1) {
        if (read(fd, &ev, sizeof(ev)) == sizeof(ev)) {
            if (ev.type == EV_KEY && ev.value == 1) { // Key press event
                printf("Key pressed: keycode = %d\n", ev.code);
            }
        }
    }

    close(fd);
    return EXIT_SUCCESS;
}
