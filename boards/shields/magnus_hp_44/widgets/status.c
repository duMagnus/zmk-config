#include <lvgl.h>

#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/output_status.h>

static struct zmk_widget_output_status output_widget;
static struct zmk_widget_layer_status layer_widget;
static struct zmk_widget_battery_status battery_widget;

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    // Fill the OLED area
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));

    // Vertical "portrait-style" layout: stack items top-to-bottom
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen,
                          LV_FLEX_ALIGN_START,  // main axis: top
                          LV_FLEX_ALIGN_START,  // cross axis: left
                          LV_FLEX_ALIGN_START);

    // Tight padding so 3 rows fit on 128x32
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(screen, 0, LV_PART_MAIN);

    zmk_widget_output_status_init(&output_widget, screen);
    zmk_widget_layer_status_init(&layer_widget, screen);
    zmk_widget_battery_status_init(&battery_widget, screen);

    return screen;
}
