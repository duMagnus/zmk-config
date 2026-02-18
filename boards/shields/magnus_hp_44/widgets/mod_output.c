#include "mod_output.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <zmk/endpoints.h>
#include <zmk/endpoints_types.h>
#include <zmk/event_manager.h>
#include <zmk/events/endpoint_changed.h>

#include "portrait_demo.h"

static output_module_t *g_output;

static void update_output_state(void) {
    if (!g_output || !g_output->state) {
        return;
    }

    const struct zmk_endpoint_instance ep = zmk_endpoints_selected();

    if (ep.transport == ZMK_TRANSPORT_USB) {
        g_output->state->output_is_usb = 1;
        g_output->state->ble_profile_index = 0;
        return;
    }

    if (ep.transport == ZMK_TRANSPORT_BLE) {
        g_output->state->output_is_usb = 0;
        g_output->state->ble_profile_index = (uint8_t)ep.ble.profile_index; // 0-based
        return;
    }

    // Unknown transport
    g_output->state->output_is_usb = 0;
    g_output->state->ble_profile_index = 0;
}

static int on_endpoint_changed(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    update_output_state();
    magnus_hp_44_portrait_demo_redraw();

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(magnus_hp_44_output_listener, on_endpoint_changed);
ZMK_SUBSCRIPTION(magnus_hp_44_output_listener, zmk_endpoint_changed);

void output_module_init(output_module_t *m, uint16_t x, uint16_t y, screen_state_t *state) {
    if (!m) {
        return;
    }

    m->x = x;
    m->y = y;
    m->state = state;

    g_output = m;

    // Prime at init (event will keep it fresh later)
    update_output_state();
}

void output_module_draw(const output_module_t *m,
                        const render_ctx_t *ctx,
                        const screen_state_t *state) {
    if (!m || !ctx || !state || !ctx->portrait_canvas) {
        return;
    }

    char buf[10];

    if (state->output_is_usb) {
        strcpy(buf, "USB");
    } else {
        // Show 1-based index to humans
        unsigned int shown = (unsigned int)state->ble_profile_index + 1;
        snprintf(buf, sizeof(buf), "BLE %u", shown);
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    lv_canvas_draw_text(ctx->portrait_canvas, m->x, m->y, ctx->portrait_w, &dsc, buf);
}
