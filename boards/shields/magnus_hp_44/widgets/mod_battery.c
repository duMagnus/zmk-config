#include "mod_battery.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#include "portrait_demo.h"

static battery_module_t *g_battery;

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
