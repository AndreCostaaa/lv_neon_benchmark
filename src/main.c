#include "misc/lv_color.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "lvgl.h"
#include "lv_demos.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <src/misc/lv_profiler_builtin_private.h>

static uint64_t my_get_tick_cb(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

static int my_get_tid_cb(void)
{
    return (int)syscall(SYS_gettid);
}

static int my_get_cpu_cb(void)
{
    int cpu_id = 0;
    syscall(SYS_getcpu, &cpu_id, NULL);
    return cpu_id;
}

static void dummy_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    lv_display_flush_ready(disp);
    return;
}

static uint32_t millis(void)
{
    struct timespec ts;

    int result = clock_gettime(CLOCK_MONOTONIC, &ts);
    if(result != 0) {
        LV_LOG_ERROR("clock_gettime failed");
    }

    uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    return tick;
}

static void on_benchmark_end(const lv_demo_benchmark_summary_t * summary)
{
    lv_demo_benchmark_summary_display(summary);
    exit(0);
}
static const char * color_format_to_str(lv_color_format_t format)
{
    switch(format) {
        case LV_COLOR_FORMAT_UNKNOWN:
            return "LV_COLOR_FORMAT_UNKNOWN";
        case LV_COLOR_FORMAT_RAW:
            return "LV_COLOR_FORMAT_RAW";
        case LV_COLOR_FORMAT_RAW_ALPHA:
            return "LV_COLOR_FORMAT_RAW_ALPHA";
        case LV_COLOR_FORMAT_L8:
            return "LV_COLOR_FORMAT_L8";
        case LV_COLOR_FORMAT_I1:
            return "LV_COLOR_FORMAT_I1";
        case LV_COLOR_FORMAT_I2:
            return "LV_COLOR_FORMAT_I2";
        case LV_COLOR_FORMAT_I4:
            return "LV_COLOR_FORMAT_I4";
        case LV_COLOR_FORMAT_I8:
            return "LV_COLOR_FORMAT_I8";
        case LV_COLOR_FORMAT_A8:
            return "LV_COLOR_FORMAT_A8";
        case LV_COLOR_FORMAT_RGB565:
            return "LV_COLOR_FORMAT_RGB565";
        case LV_COLOR_FORMAT_ARGB8565:
            return "LV_COLOR_FORMAT_ARGB8565";
        case LV_COLOR_FORMAT_RGB565A8:
            return "LV_COLOR_FORMAT_RGB565A8";
        case LV_COLOR_FORMAT_AL88:
            return "LV_COLOR_FORMAT_AL88";
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            return "LV_COLOR_FORMAT_RGB565_SWAPPED";
        case LV_COLOR_FORMAT_RGB888:
            return "LV_COLOR_FORMAT_RGB888";
        case LV_COLOR_FORMAT_ARGB8888:
            return "LV_COLOR_FORMAT_ARGB8888";
        case LV_COLOR_FORMAT_XRGB8888:
            return "LV_COLOR_FORMAT_XRGB8888";
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            return "LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED";
        case LV_COLOR_FORMAT_A1:
            return "LV_COLOR_FORMAT_A1";
        case LV_COLOR_FORMAT_A2:
            return "LV_COLOR_FORMAT_A2";
        case LV_COLOR_FORMAT_A4:
            return "LV_COLOR_FORMAT_A4";
        case LV_COLOR_FORMAT_ARGB1555:
            return "LV_COLOR_FORMAT_ARGB1555";
        case LV_COLOR_FORMAT_ARGB4444:
            return "LV_COLOR_FORMAT_ARGB4444";
        case LV_COLOR_FORMAT_ARGB2222:
            return "LV_COLOR_FORMAT_ARGB2222";
        case LV_COLOR_FORMAT_YUV_START:
            return "LV_COLOR_FORMAT_YUV_START";
        case LV_COLOR_FORMAT_I422:
            return "LV_COLOR_FORMAT_I422";
        case LV_COLOR_FORMAT_I444:
            return "LV_COLOR_FORMAT_I444";
        case LV_COLOR_FORMAT_I400:
            return "LV_COLOR_FORMAT_I400";
        case LV_COLOR_FORMAT_NV21:
            return "LV_COLOR_FORMAT_NV21";
        case LV_COLOR_FORMAT_NV12:
            return "LV_COLOR_FORMAT_NV12";
        case LV_COLOR_FORMAT_YUY2:
            return "LV_COLOR_FORMAT_YUY2";
        case LV_COLOR_FORMAT_UYVY:
            return "LV_COLOR_FORMAT_UYVY";
        case LV_COLOR_FORMAT_PROPRIETARY_START:
            return "LV_COLOR_FORMAT_PROPRIETARY_START";
        case LV_COLOR_FORMAT_NEMA_TSC6:
            return "LV_COLOR_FORMAT_NEMA_TSC6";
        case LV_COLOR_FORMAT_NEMA_TSC6A:
            return "LV_COLOR_FORMAT_NEMA_TSC6A";
        case LV_COLOR_FORMAT_NEMA_TSC6AP:
            return "LV_COLOR_FORMAT_NEMA_TSC6AP";
        case LV_COLOR_FORMAT_NEMA_TSC12:
            return "LV_COLOR_FORMAT_NEMA_TSC12";
        case LV_COLOR_FORMAT_NEMA_TSC12A:
            return "LV_COLOR_FORMAT_NEMA_TSC12A";
            break;
    }
}

int main(int argc, char ** argv)
{
    if(argc < 4) {
        printf("Usage %s <hres> <vres> <color_format>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const uint32_t hres = atoi(argv[1]);
    const uint32_t vres = atoi(argv[2]);
    const uint32_t cf = atoi(argv[3]);
    const uint32_t stride = 0;
    const lv_color_format_t color_format = cf;
    printf("Resolution: %ux%u Color Format: %s\n", hres, vres, color_format_to_str(cf));

    lv_init();
    lv_tick_set_cb(millis);
#if LV_USE_PROFILER
    lv_profiler_builtin_config_t config;
    lv_profiler_builtin_config_init(&config);
    config.tick_per_sec = 1000000000; /* One second is equal to 1000000000 nanoseconds */
    config.tick_get_cb = my_get_tick_cb;
    config.tid_get_cb = my_get_tid_cb;
    config.cpu_get_cb = my_get_cpu_cb;
    lv_profiler_builtin_init(&config);
#endif /* LV_USE_PROFILER*/

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(hres, vres, color_format, stride);
    lv_display_t * disp = lv_display_create(hres, vres);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_color_format(disp, color_format);
    lv_display_set_draw_buffers(disp, draw_buf, NULL);
    lv_display_set_flush_cb(disp, dummy_cb);

    lv_demo_benchmark();
    lv_demo_benchmark_set_end_cb(on_benchmark_end);

    while(1) {
        /* Returns the time to the next timer execution */
        const uint32_t idle_time = lv_timer_handler();
        usleep(idle_time * 1000);
    }

    return EXIT_SUCCESS;
}
