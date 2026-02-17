/*
 * Simple vertical (stacked) status screen for 128x32 OLED
 *
 * Shows:
 *  - Output (USB/BLE)
 *  - Highest active layer
 *  - Battery percent/icon
 */

#include <lvgl.h>

#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/output_status.h>

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_widget;
#endif

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    // Make the screen a simple vertical layout container.
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // Tight padding so we can fit 3 rows into 32px height.
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(screen, 0, LV_PART_MAIN);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_widget, screen);
    lv_obj_t *out = zmk_widget_output_status_obj(&output_widget);
    lv_obj_set_style_text_font(out, lv_theme_get_font_small(screen), LV_PART_MAIN);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_widget, screen);
    lv_obj_t *lay = zmk_widget_layer_status_obj(&layer_widget);
    lv_obj_set_style_text_font(lay, lv_theme_get_font_small(screen), LV_PART_MAIN);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_widget, screen);
    lv_obj_t *bat = zmk_widget_battery_status_obj(&battery_widget);
    lv_obj_set_style_text_font(bat, lv_theme_get_font_small(screen), LV_PART_MAIN);
#endif

    return screen;
}
