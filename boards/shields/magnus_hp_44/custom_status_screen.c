#include <lvgl.h>
#include <zmk/display/status_screen.h>

#include "widgets/portrait_demo.h"

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    magnus_hp_44_portrait_demo_create(screen);
    return screen;
}
