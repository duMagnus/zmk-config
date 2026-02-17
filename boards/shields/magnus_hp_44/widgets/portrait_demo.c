#include <lvgl.h>
#include "portrait_demo.h"

// 128x32 monochrome canvas buffer: 1 bit per pixel
// LVGL v8: use LV_IMG_CF_ALPHA_1BIT for a 1bpp alpha canvas.
// Buffer size is width*height/8.
#define CANVAS_W 128
#define CANVAS_H 32
static uint8_t canvas_buf[(CANVAS_W * CANVAS_H) / 8];

void magnus_hp_44_portrait_demo_create(lv_obj_t *parent) {
    lv_obj_t *canvas = lv_canvas_create(parent);
    lv_obj_set_size(canvas, CANVAS_W, CANVAS_H);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);

    lv_canvas_set_buffer(canvas, canvas_buf, CANVAS_W, CANVAS_H, LV_IMG_CF_ALPHA_1BIT);

    // Clear background (fully transparent)
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);

    // Draw "vertical" text by using newlines (no rotation yet)
    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    // Force a narrow draw area so it looks like a "portrait column"
    lv_area_t a = { .x1 = 0, .y1 = 0, .x2 = 30, .y2 = 31 };

    lv_canvas_draw_text(canvas, 0, 0, 30, &dsc, "H\nE\nL\nL\nO");
}
