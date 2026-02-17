#include <lvgl.h>
#include <zmk/display/status_screen.h>

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    // 128x32: remove padding/scroll to keep it simple and stable
    lv_obj_set_size(screen, 128, 32);
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    // Draw ONE label so we confirm LVGL is alive and stable
    lv_obj_t *lbl = lv_label_create(screen);
    lv_label_set_text(lbl, "HELLO");
    lv_obj_set_pos(lbl, 0, 0);

    return screen;
}
