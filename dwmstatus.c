#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>

#include "config.h"

static void append(char* buf, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf + strlen(buf), fmt, ap);
    va_end(ap);
}

#if ENABLE_BATTERY
static int get_battery_percentage(void) {
    FILE* f = fopen("/sys/class/power_supply/" BATTERY_NAME "/capacity", "r");
    if (!f) {
        return -1;
    }

    int bat_perc;
    if (fscanf(f, "%d\n", &bat_perc) == EOF) {
        return -1;
    }

    fclose(f);
    return bat_perc;
}
#endif

#if ENABLE_VOLUME
static int get_volume_percentage(void) {
    FILE* f = popen("/usr/bin/wpctl get-volume @DEFAULT_SINK@", "r");
    if (!f) {
        return -1;
    }

    float raw_vol_perc;
    if (fscanf(f, "Volume: %f\n", &raw_vol_perc) == EOF) {
        return -1;
    }

    int vol_perc = (int) (raw_vol_perc * 100.f);

    pclose(f);
    return vol_perc;
}
#endif

int main(void) {
    Display* display = XOpenDisplay(getenv("DISPLAY"));
    if (display == NULL) {
        fprintf(stderr, "cannot open X display\n");
        exit(EXIT_FAILURE);
    }

    Window window = DefaultRootWindow(display);

    char buffer[256];

#if ENABLE_HOSTNAME
    char* hostname = malloc(20);
    gethostname(hostname, 20);
#endif

    while (!usleep(REFRESH_DELAY * 1e6)) {
        buffer[0] = '\0';

#if ENABLE_HOSTNAME
        append(buffer, "[%s]", hostname);
#endif

#if ENABLE_VOLUME
        const int vol_perc = get_volume_percentage();
        if (vol_perc == -1) {
            append(buffer, "[VOL: N/A]");
        } else {
            append(buffer, "[VOL: %d%%]", vol_perc);
        }
#endif

#if ENABLE_BATTERY
        const int bat_perc = get_battery_percentage();
        if (bat_perc == -1) {
            append(buffer, "[BAT: N/A]");
        } else {
            append(buffer, "[BAT: %d%%]", bat_perc);
        }
#endif

        const time_t timestamp = time(NULL);
        strftime(buffer + strlen(buffer), sizeof(buffer), "[" TIME_FORMAT "]", localtime(&timestamp));

        XStoreName(display, window, buffer);
        XSync(display, 0);
    }

    free(hostname);

    XCloseDisplay(display);
    return EXIT_SUCCESS;
}
