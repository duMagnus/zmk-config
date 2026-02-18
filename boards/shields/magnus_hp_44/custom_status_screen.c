#include <lvgl.h>
#include <zmk/display/status_screen.h>

#include "widgets/portrait_demo.h"

#include <zephyr/sys/util.h>

lv_obj_t *zmk_display_status_screen(void) {
    // ZMK's own built-in status screen uses lv_obj_create(NULL) too,
    // so this is a valid approach when LVGL is configured correctly.
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    magnus_hp_44_portrait_demo_create(screen);

    return screen;
}
