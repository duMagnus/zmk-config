#include "mod_battery.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/sys/util.h>

#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#include "portrait_demo.h"

static battery_module_t *g_battery;

/*
 * Tiny 8x8 battery icon (1bpp), MSB-first per row.
 * Terminal is on the right (two pixels wide).
 */
static const uint8_t battery_icon_8x8[8] = {
    0b00000110,
    0b00001100,
    0b00011000,
    0b00111110,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
};

static void draw_battery_icon(lv_obj_t *canvas, int x, int y) {
    // Canvas is 1bpp; draw white pixels where bits are 1.
    for (int row = 0; row < 8; row++) {
        uint8_t bits = battery_icon_8x8[row];
        for (int col = 0; col < 8; col++) {
            int on = (bits >> (7 - col)) & 1;
            if (on) {
                lv_canvas_set_px(canvas, x + col, y + row, lv_color_white());
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

    // Draw battery icon at (x, y)
    draw_battery_icon(ctx->portrait_canvas, m->x, m->y);

    // Prepare text
    char buf[6];
    if (state->battery_percent > 100) {
        strcpy(buf, "??%");
    } else {
        snprintf(buf, sizeof(buf), "%u%%", state->battery_percent);
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    // Draw text to the right of the 8px icon (+2px spacing)
    lv_canvas_draw_text(ctx->portrait_canvas,
                        m->x + 10,
                        m->y - 1,
                        ctx->portrait_w - (m->x + 10),
                        &dsc,
                        buf);
}
