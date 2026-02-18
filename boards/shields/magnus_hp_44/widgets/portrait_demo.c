#include <lvgl.h>
#include "portrait_demo.h"

#include <string.h>

// Physical display framebuffer is landscape 128x32
#define DISP_W 128
#define DISP_H 32

// Logical portrait space (what you want to design in)
#define PORTRAIT_W 32
#define PORTRAIT_H 128

// 1bpp buffers (bytes = w*h/8)
static uint8_t portrait_buf[(PORTRAIT_W * PORTRAIT_H) / 8];
static uint8_t landscape_buf[(DISP_W * DISP_H) / 8];

static inline int get_px_1bpp(const uint8_t *buf, int w, int x, int y) {
    // Page-per-8-rows layout: index = (y/8)*w + x, bit = y%8
    int page = y >> 3;
    int bit = y & 7;
    int idx = page * w + x;
    return (buf[idx] >> bit) & 1;
}

static inline void set_px_1bpp(uint8_t *buf, int w, int x, int y, int v) {
    int page = y >> 3;
    int bit = y & 7;
    int idx = page * w + x;
    if (v) buf[idx] |= (1u << bit);
    else   buf[idx] &= ~(1u << bit);
}

// Rotate the portrait (32x128) image into landscape (128x32)
// This mapping is for a 90° clockwise rotation:
// landscape(x,y) = portrait( x', y' ) with x'=y, y'=(PORTRAIT_H-1-x)
static void rotate_portrait_to_landscape_cw(const uint8_t *src_portrait, uint8_t *dst_landscape) {
    memset(dst_landscape, 0, (DISP_W * DISP_H) / 8);

    for (int y = 0; y < DISP_H; y++) {
        for (int x = 0; x < DISP_W; x++) {
            int px = y;                 // 0..31
            int py = (PORTRAIT_H - 1) - x; // 127..0
            int v = get_px_1bpp(src_portrait, PORTRAIT_W, px, py);
            set_px_1bpp(dst_landscape, DISP_W, x, y, v);
        }
    }
}

void magnus_hp_44_portrait_demo_create(lv_obj_t *parent) {
    // Create a canvas that is the PHYSICAL display size (128x32).
    // We'll draw into a portrait buffer, rotate, then show the rotated buffer on this canvas.
    lv_obj_t *canvas = lv_canvas_create(parent);
    lv_obj_set_size(canvas, DISP_W, DISP_H);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);

    lv_canvas_set_buffer(canvas, landscape_buf, DISP_W, DISP_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);

    // ---- Draw into the portrait buffer first ----
    // We use a temporary LVGL image descriptor to let LVGL draw into our portrait_buf.
    // LVGL canvas expects its own canvas object normally, but for simple drawing we can use draw APIs
    // on a temporary canvas object. Easiest is to create a hidden canvas for portrait.
    lv_obj_t *portrait_canvas = lv_canvas_create(parent);
    lv_obj_add_flag(portrait_canvas, LV_OBJ_FLAG_HIDDEN);
    lv_canvas_set_buffer(portrait_canvas, portrait_buf, PORTRAIT_W, PORTRAIT_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(portrait_canvas, lv_color_black(), LV_OPA_TRANSP);

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    // In portrait coordinates, (0,0) is top-left of the screen *as you want to read it*
    // This should now read correctly when the OLED is physically in portrait.
    lv_canvas_draw_text(portrait_canvas, 0, 0, PORTRAIT_W, &dsc, "HELLO");

    // ---- Rotate portrait -> landscape and display it ----
    rotate_portrait_to_landscape_cw(portrait_buf, landscape_buf);

    // Force refresh
    lv_obj_invalidate(canvas);

    // We can delete the temporary portrait canvas object; buffers stay since they're static.
    lv_obj_del(portrait_canvas);
}
