#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "lvgl.h"
#include "lv_demos.h"

static void dummy_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
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
	if (result != 0) {
		LV_LOG_ERROR("clock_gettime failed");
	}

	uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

	return tick;
}

static void on_benchmark_end(const lv_demo_benchmark_summary_t *summary)
{
	lv_demo_benchmark_summary_display(summary);
	exit(0);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Usage %s <hres> <vres>\n", argv[0]);
		return EXIT_FAILURE;
	}
	const uint32_t hres = atoi(argv[1]);
	const uint32_t vres = atoi(argv[1]);
	const uint32_t stride = 0;
	const lv_color_format_t color_format = LV_COLOR_FORMAT_RGB565;

	lv_init();
	lv_tick_set_cb(millis);

	lv_draw_buf_t *draw_buf = lv_draw_buf_create(hres, vres, color_format, stride);
	lv_display_t *disp = lv_display_create(hres, vres);
	lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
	lv_display_set_color_format(disp, color_format);
	lv_display_set_draw_buffers(disp, draw_buf, NULL);
	lv_display_set_flush_cb(disp, dummy_cb);

	lv_demo_benchmark();
	lv_demo_benchmark_set_end_cb(on_benchmark_end);

	while (1) {
		/* Returns the time to the next timer execution */
		const uint32_t idle_time = lv_timer_handler();
		usleep(idle_time * 1000);
	}

	return EXIT_SUCCESS;
}
