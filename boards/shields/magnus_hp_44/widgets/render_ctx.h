#pragma once
#include <lvgl.h>
#include <stdint.h>

typedef struct {
    lv_obj_t *portrait_canvas;   // 32x128 hidden canvas
    lv_obj_t *landscape_canvas;  // 128x32 visible canvas
    uint16_t portrait_w;
    uint16_t portrait_h;
} render_ctx_t;

typedef struct {
    uint8_t battery_percent; // 0..100, 255 = unknown
} screen_state_t;
