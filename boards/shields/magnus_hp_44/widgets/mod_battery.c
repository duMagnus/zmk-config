#include "mod_battery.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/sys/util.h>   // ARG_UNUSED

#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#include "portrait_demo.h"

static battery_module_t *g_battery;

static const uint8_t batt_icon_8x8[8] = {
    0b00000110,
    0b00001100,
    0b00011000,
    0b00111110,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
};

static void draw_1bpp_icon_8x8(lv_obj_t *canvas, int x, int y, const uint8_t icon[8]) {
    for (int row = 0; row < 8; row++) {
        uint8_t bits = icon[row];
        for (int col = 0; col < 8; col++) {
            int on = (bits >> (7 - col)) & 1;
            if (on) {
                lv_canvas_set_px(canvas, x + col, y + row, lv_color_white());
                lv_canvas_set_px_opa(canvas, x + col, y + row, LV_OPA_COVER);
            }
        }
    }
}

static void update_battery_state(void) {
    if (!g_battery || !g_battery->state) {
        return;
    }
    g_battery->state->battery_percent = zmk_battery_state_of_charge();
}

static int on_battery_changed(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    // Update cached state, then request redraw (safe: redraw is scheduled on work item)
    update_battery_state();
    magnus_hp_44_portrait_demo_redraw();

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(magnus_hp_44_battery_listener, on_battery_changed);
ZMK_SUBSCRIPTION(magnus_hp_44_battery_listener, zmk_battery_state_changed);

void battery_module_init(battery_module_t *m, uint16_t x, uint16_t y, screen_state_t *state) {
    if (!m) {
        return;
    }

    m->x = x;
    m->y = y;
    m->state = state;

    g_battery = m;

    // Prime at init (may be 0/unknown until first measurement; event will update later)
    update_battery_state();
}

void battery_module_draw(const battery_module_t *m,
                         const render_ctx_t *ctx,
                         const screen_state_t *state) {
    if (!m || !ctx || !state || !ctx->portrait_canvas) {
        return;
    }

    // Draw battery icon on the left
    draw_1bpp_icon_8x8(ctx->portrait_canvas, m->x, m->y, batt_icon_8x8);

    char buf[6];
    if (state->battery_percent > 100) {
        strcpy(buf, "??%");
    } else {
        snprintf(buf, sizeof(buf), "%u%%", state->battery_percent);
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    // Force a small font so this fits in 32px width (including 100%).
    // (These are compiled in your build log, so they're available.)
    dsc.font = &lv_font_montserrat_8;

    // Text starts immediately after the 8px icon. No gap, to maximize space.
    const int text_x = (int)m->x + 8;
    const int text_w = (int)ctx->portrait_w - text_x;
    lv_canvas_draw_text(ctx->portrait_canvas, text_x, m->y - 1, text_w, &dsc, buf);
}
