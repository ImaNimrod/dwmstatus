#include <X11/Xlib.h>

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#define MUTED (-2)

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

    int ret = -1;

    int bat_perc;
    if (fscanf(f, "%d\n", &bat_perc) == EOF) {
        ret = -1;
        goto end;
    }

    ret = bat_perc;

end:
    fclose(f);
    return ret;
}
#endif

#if ENABLE_VOLUME
static int get_volume_percentage(void) {
    FILE* f = popen("/usr/bin/wpctl get-volume @DEFAULT_SINK@", "r");
    if (!f) {
        return -1;
    }

    char buf[128];
    int ret = -1;

    if (fgets(buf, sizeof(buf), f)) {
        float vol;

        if (strstr(buf, "MUTED")) {
            ret = MUTED;
        } else if (sscanf(buf, "Volume: %f", &vol) == 1) {
            ret = (int) (vol * 100.f);
        }
    }

    pclose(f);
    return ret;
}
#endif

int main(void) {
    Display* display = XOpenDisplay(getenv("DISPLAY"));
    if (!display) {
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
        } else if (vol_perc == MUTED) {
            append(buffer, "[VOL: MUTE]");
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
