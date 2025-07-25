#include <render/lv_demo_render.h>

typedef struct {
	const char *name;
	lv_color_format_t format;
} color_format_t;

void test_render(lv_display_t *display, const color_format_t *format)
{
	lv_display_set_color_format(display, format->format);
	LV_LOG_USER("Testing %s", format->name);
	lv_opa_t opa_values[2] = { 0xff, 0x80 };
	uint32_t opa;
	for (opa = 0; opa < 2; opa++) {
		uint32_t i;
		for (i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {
			const char *scene_name = lv_demo_render_get_scene_name(i);
			char buf[128];
			lv_snprintf(buf, sizeof(buf), "ref_images/%s/demo_render_%s_opa_%d.png", format->name, scene_name,
				    opa_values[opa]);
			LV_LOG_USER("Rendering %s", buf);
			lv_demo_render(i, opa_values[opa]);
			lv_test_screenshot_compare(buf);
		}
	}
}

int main(void)
{
	color_format_t formats[] = { { .name = "rgb565", .format = LV_COLOR_FORMAT_RGB565 } };
	lv_init();
	lv_display_t *display = lv_test_display_create(1024, 768);

	for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); ++i) {
		test_render(display, &formats[i]);
	}

	return 0;
}
