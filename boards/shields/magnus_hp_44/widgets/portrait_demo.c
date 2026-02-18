#include "portrait_demo.h"

#include <lvgl.h>
#include <string.h>

// Physical display buffer (what LVGL flushes to SSD1306)
enum { DISP_W = 128, DISP_H = 32 };

// Logical portrait space (what you design your UI in)
enum { PORTRAIT_W = 32, PORTRAIT_H = 128 };

#define STRIDE_BYTES(w) (((w) + 7) / 8)

// 1bpp canvas buffers (LV_IMG_CF_ALPHA_1BIT, row-major bit packing)
static uint8_t portrait_buf[PORTRAIT_H * STRIDE_BYTES(PORTRAIT_W)];
static uint8_t landscape_buf[DISP_H * STRIDE_BYTES(DISP_W)];

static lv_obj_t *landscape_canvas;
static lv_obj_t *portrait_canvas;

static inline int img1bpp_get(const uint8_t *buf, int w, int x, int y) {
    const int stride = STRIDE_BYTES(w);
    const int byte_index = y * stride + (x >> 3);
    const int bit_index = 7 - (x & 7); // LVGL uses MSB-first for 1bpp images
    return (buf[byte_index] >> bit_index) & 1;
}

static inline void img1bpp_set(uint8_t *buf, int w, int x, int y, int v) {
    const int stride = STRIDE_BYTES(w);
    const int byte_index = y * stride + (x >> 3);
    const int bit_index = 7 - (x & 7);
    const uint8_t mask = (uint8_t)(1u << bit_index);

    if (v) buf[byte_index] |= mask;
    else   buf[byte_index] &= (uint8_t)~mask;
}

// Rotate portrait(32x128) -> landscape(128x32), 90° clockwise.
// dst(x,y) = src(y, H-1-x)
static void rotate_portrait_to_landscape_cw(void) {
    memset(landscape_buf, 0, sizeof(landscape_buf));

    for (int y = 0; y < DISP_H; y++) {
        for (int x = 0; x < DISP_W; x++) {
            const int px = y;
            const int py = (PORTRAIT_H - 1) - x;
            const int v = img1bpp_get(portrait_buf, PORTRAIT_W, px, py);
            img1bpp_set(landscape_buf, DISP_W, x, y, v);
        }
    }
}

static void draw_portrait_demo(void) {
    // Clear portrait canvas
    lv_canvas_fill_bg(portrait_canvas, lv_color_black(), LV_OPA_TRANSP);

    // Draw text in portrait coordinates (upright as you hold the keyboard)
    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    lv_canvas_draw_text(portrait_canvas, 0, 0, PORTRAIT_W, &dsc, "HELLO");
}

void magnus_hp_44_portrait_demo_redraw(void) {
    if (!portrait_canvas || !landscape_canvas) {
        return;
    }

    draw_portrait_demo();
    rotate_portrait_to_landscape_cw();

    // Tell LVGL to repaint the visible canvas
    lv_obj_invalidate(landscape_canvas);
}

void magnus_hp_44_portrait_demo_create(lv_obj_t *parent) {
    // Visible canvas (128x32)
    landscape_canvas = lv_canvas_create(parent);
    lv_obj_set_size(landscape_canvas, DISP_W, DISP_H);
    lv_obj_align(landscape_canvas, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_buffer(landscape_canvas, landscape_buf, DISP_W, DISP_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(landscape_canvas, lv_color_black(), LV_OPA_TRANSP);

    // Hidden portrait canvas (32x128)
    portrait_canvas = lv_canvas_create(parent);
    lv_obj_add_flag(portrait_canvas, LV_OBJ_FLAG_HIDDEN);
    lv_canvas_set_buffer(portrait_canvas, portrait_buf, PORTRAIT_W, PORTRAIT_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(portrait_canvas, lv_color_black(), LV_OPA_TRANSP);

    // Initial draw
    magnus_hp_44_portrait_demo_redraw();
}
