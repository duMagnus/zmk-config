#include <lvgl.h>
#include "portrait_demo.h"

#include <string.h>
#include <zephyr/kernel.h>

#include "render_ctx.h"
#include "mod_battery.h"
#include "mod_output.h"

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

// Persistent canvases
static lv_obj_t *landscape_canvas;
static lv_obj_t *portrait_canvas;

// Shared context/state
static render_ctx_t ctx;
static screen_state_t state;

// Modules
static battery_module_t battery_mod;
static output_module_t output_mod;

// Redraw work item (so we never redraw from event callbacks)
static struct k_work redraw_work;
static bool initialized;

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

// Rotate portrait(32x128) -> landscape(128x32) CW
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

static void draw_scene(void) {
    // Clear portrait canvas
    lv_canvas_fill_bg(ctx.portrait_canvas, lv_color_black(), LV_OPA_TRANSP);

    // Draw modules using current state
    battery_module_draw(&battery_mod, &ctx, &state);
    output_module_draw(&output_mod, &ctx, &state);

    // Rotate portrait -> landscape for the physical OLED
    rotate_portrait_to_landscape_cw(portrait_buf, landscape_buf);

    // Ask LVGL to refresh visible canvas
    lv_obj_invalidate(landscape_canvas);
}

static void redraw_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (!initialized || !landscape_canvas || !portrait_canvas) {
        return;
    }

    draw_scene();
}

void magnus_hp_44_portrait_demo_redraw(void) {
    // Safe to call from anywhere (module callbacks, timers, etc.)
    if (!initialized) {
        return;
    }
    k_work_submit(&redraw_work);
}

void magnus_hp_44_portrait_demo_create(lv_obj_t *parent) {
    // Visible canvas (what LVGL flushes to the display)
    landscape_canvas = lv_canvas_create(parent);
    lv_obj_set_size(landscape_canvas, DISP_W, DISP_H);
    lv_obj_align(landscape_canvas, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_buffer(landscape_canvas, landscape_buf, DISP_W, DISP_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(landscape_canvas, lv_color_black(), LV_OPA_TRANSP);

    // Hidden portrait canvas (draw upright here)
    portrait_canvas = lv_canvas_create(parent);
    lv_obj_add_flag(portrait_canvas, LV_OBJ_FLAG_HIDDEN);
    lv_canvas_set_buffer(portrait_canvas, portrait_buf, PORTRAIT_W, PORTRAIT_H, LV_IMG_CF_ALPHA_1BIT);
    lv_canvas_fill_bg(portrait_canvas, lv_color_black(), LV_OPA_TRANSP);

    // Fill ctx
    ctx.portrait_canvas = portrait_canvas;
    ctx.landscape_canvas = landscape_canvas;
    ctx.portrait_w = PORTRAIT_W;
    ctx.portrait_h = PORTRAIT_H;

    // Init state (unknowns until modules/events populate)
    state.battery_percent = 255;
    state.output_is_usb = 0;
    state.ble_profile_index = 0;

    // Work item for redraw
    k_work_init(&redraw_work, redraw_work_handler);

    // Init modules (they own updates and trigger redraw on changes)
    battery_module_init(&battery_mod, 0, 0, &state);
    output_module_init(&output_mod, 0, 14, &state);

    initialized = true;

    // Initial draw
    magnus_hp_44_portrait_demo_redraw();
}
