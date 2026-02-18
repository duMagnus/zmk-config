#include "mod_battery.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <zmk/battery.h>

void battery_module_init(battery_module_t *m, uint16_t x, uint16_t y) {
    if (!m) {
        return;
    }
    m->x = x;
    m->y = y;
}

void battery_module_update(screen_state_t *state) {
    if (!state) {
        return;
    }

    // ZMK-provided helper: returns 0..100
    state->battery_percent = zmk_battery_state_of_charge();
}

void battery_module_draw(const battery_module_t *m, const render_ctx_t *ctx, const screen_state_t *state) {
    if (!m || !ctx || !state || !ctx->portrait_canvas) {
        return;
    }

    // Keep it short; your 32px width wraps easily.
    // "B:85" is safer than "85%" on narrow portrait width.
    char buf[8];
    if (state->battery_percent > 100) {
        strcpy(buf, "B:?");
    } else {
        snprintf(buf, sizeof(buf), "B:%u", state->battery_percent);
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    lv_canvas_draw_text(ctx->portrait_canvas, m->x, m->y, ctx->portrait_w, &dsc, buf);
}
