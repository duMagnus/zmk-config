#include "mod_battery.h"
#include <stdio.h>
#include <string.h>

// ZMK battery API (we’ll use the same source ZMK uses internally)
// If this include name doesn’t match your pinned ZMK, we’ll adjust.
// (It’s stable across modern ZMK versions.)
#include <zmk/battery.h>

void battery_module_init(battery_module_t *m, uint16_t x, uint16_t y) {
    m->x = x;
    m->y = y;
}

void battery_module_update(screen_state_t *state) {
    // zmk_battery_get_perc() returns uint8_t (0-100)
    // If battery isn’t available, some boards return 0; we’ll treat 255 as unknown.
    state->battery_percent = zmk_battery_get_perc();
}

void battery_module_draw(const battery_module_t *m, const render_ctx_t *ctx, const screen_state_t *state) {
    char buf[8];

    if (state->battery_percent > 100) {
        strcpy(buf, "B:?");     // unknown
    } else {
        // Keep it short to fit 32px width:
        // "B:85" (no % sign) is usually safer than "85%"
        snprintf(buf, sizeof(buf), "B:%u", state->battery_percent);
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    lv_canvas_draw_text(ctx->portrait_canvas, m->x, m->y, ctx->portrait_w, &dsc, buf);
}
