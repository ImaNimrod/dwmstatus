#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>

int main(void) {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "cannot open X display\n");
        exit(EXIT_FAILURE);
    }

    Window window = DefaultRootWindow(display);

    while (!usleep(1e6 / 60)) {
        char status_text[100] = {0};

        time_t timestamp = time(NULL);
        struct tm* time_info = localtime(&timestamp);
        strftime(status_text, sizeof(status_text), "[%a %X %x]", time_info);

        XStoreName(display, window, status_text);
        XSync(display, 0);
    }

    XCloseDisplay(display);
    return EXIT_SUCCESS;
}
