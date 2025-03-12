#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define PROC_PLAINTEXT "/proc/usb_keylogger"
#define PROC_ENCRYPTED "/proc/usb_keylogger_encrypted"
#define BUFFER_SIZE 1024  // Define BUFFER_SIZE here

void *read_plaintext(void *arg) {
    int fd = open(PROC_PLAINTEXT, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open plaintext /proc file");
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            printf("Plaintext: %.*s\n", bytes_read, buffer);
        } else {
            perror("Read failed");
            break;
        }
    }

    close(fd);
    return NULL;
}

void *read_encrypted(void *arg) {
    int fd = open(PROC_ENCRYPTED, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open encrypted /proc file");
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            printf("Encrypted: %.*s\n", bytes_read, buffer);
        } else {
            perror("Read failed");
            break;
        }
    }

    close(fd);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Create threads to read from /proc files
    pthread_create(&thread1, NULL, read_plaintext, NULL);
    pthread_create(&thread2, NULL, read_encrypted, NULL);

    // Wait for threads to finish (they won't, since they run in an infinite loop)
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
