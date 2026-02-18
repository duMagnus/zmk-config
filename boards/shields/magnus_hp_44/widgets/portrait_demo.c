#include <lvgl.h>
#include "portrait_demo.h"

#include <string.h>

// Physical display (SSD1306) is landscape 128x32 in LVGL coordinates
#define DISP_W 128
#define DISP_H 32

// Logical portrait space you want to design in
#define PORTRAIT_W 32
#define PORTRAIT_H 128

// LVGL 1bpp canvas packing is row-major:
// Each row uses STRIDE bytes, bits go left->right in a byte.
#define STRIDE(w) (((w) + 7) / 8)

// Buffers for LVGL canvases (1bpp alpha)
static uint8_t portrait_buf[PORTRAIT_H * STRIDE(PORTRAIT_W)];
static uint8_t landscape_buf[DISP_H * STRIDE(DISP_W)];

static inline int get_px_lvgl_1bpp(const uint8_t *buf, int w, int x, int y) {
    const int stride = STRIDE(w);
    const int byte_index = y * stride + (x >> 3);
    const int bit_index = 7 - (x & 7); // MSB-first is how LVGL packs 1bpp images
    return (buf[byte_index] >> bit_index) & 1;
}

static inline void set_px_lvgl_1bpp(uint8_t *buf, int w, int x, int y, int v) {
    const int stride = STRIDE(w);
    const int byte_index = y * stride + (x >> 3);
    const int bit_index = 7 - (x & 7);
    const uint8_t mask = (uint8_t)(1u << bit_index);
    if (v) buf[byte_index] |= mask;
    else   buf[byte_index] &= (uint8_t)~mask;
}

// Rotate portrait(32x128) -> landscape(128x32)
// Try CW first: dst(x,y) gets src(px,py) where px=y, py=H-1-x
static void rotate_portrait_to_landscape_cw(const uint8_t *src, uint8_t *dst) {
    memset(dst, 0, sizeof(landscape_buf));

    for (int y = 0; y < DISP_H; y++) {
        for (int x = 0; x < DISP_W; x++) {
            int px = y;                      // 0..31
            int py = (PORTRAIT_H - 1) - x;   // 127..0
            int v = get_px_lvgl_1bpp(src, PORTRAIT_W, px, py);
            set_px_lvgl_1bpp(dst, DISP_W, x, y, v);
        }
    }
}

void magnus_hp_44_portrait_demo_create(lv_obj_t *parent) {
    // Canvas that LVGL will actually flush to the display
    lv_obj_t *canvas = lv_canvas_create(parent);
    lv_obj_set_size(canvas, DISP_W, DISP_H);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);

    lv_canvas_set_buffer(canvas, landscape_buf, DISP_W, DISP_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);

    // Hidden portrait canvas where we draw in the "upright" coordinate system
    lv_obj_t *p = lv_canvas_create(parent);
    lv_obj_add_flag(p, LV_OBJ_FLAG_HIDDEN);
    lv_canvas_set_buffer(p, portrait_buf, PORTRAIT_W, PORTRAIT_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(p, lv_color_black(), LV_OPA_TRANSP);

    // Draw something simple in portrait space
    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    // Draw a word at the top of the portrait screen
    lv_canvas_draw_text(p, 0, 0, PORTRAIT_W, &dsc, "HELLO");

    // Rotate portrait -> landscape
    rotate_portrait_to_landscape_cw(portrait_buf, landscape_buf);

    // Force redraw
    lv_obj_invalidate(canvas);

    // Delete the hidden portrait canvas object (buffers are static)
    lv_obj_del(p);
}
