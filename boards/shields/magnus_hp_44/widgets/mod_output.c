#include "mod_output.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/events/endpoint_selection_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/ble.h>

#include "portrait_demo.h"

static output_module_t *g_output;

static void update_output_state(void) {
    if (!g_output || !g_output->state) {
        return;
    }

    enum zmk_endpoint ep = zmk_endpoints_selected();
    g_output->state->output_is_usb = (ep == ZMK_ENDPOINT_USB) ? 1 : 0;

#if defined(CONFIG_ZMK_BLE)
    // ZMK uses a 0-based index internally. Your UI wants 1-based.
    g_output->state->ble_profile_index = (uint8_t)(zmk_ble_active_profile_index() + 1);
#else
    g_output->state->ble_profile_index = 0;
#endif
}

static int on_output_event(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    update_output_state();
    magnus_hp_44_portrait_demo_redraw();

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(magnus_hp_44_output_listener, on_output_event);
ZMK_SUBSCRIPTION(magnus_hp_44_output_listener, zmk_endpoint_selection_changed);

#if defined(CONFIG_USB)
ZMK_SUBSCRIPTION(magnus_hp_44_output_listener, zmk_usb_conn_state_changed);
#endif

#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(magnus_hp_44_output_listener, zmk_ble_active_profile_changed);
#endif

void output_module_init(output_module_t *m, uint16_t x, uint16_t y, screen_state_t *state) {
    if (!m) {
        return;
    }

    m->x = x;
    m->y = y;
    m->state = state;

    g_output = m;

    update_output_state();
}

void output_module_draw(const output_module_t *m,
                        const render_ctx_t *ctx,
                        const screen_state_t *state) {
    if (!m || !ctx || !state || !ctx->portrait_canvas) {
        return;
    }

    char buf[8];
    if (state->output_is_usb) {
        strcpy(buf, "USB");
    } else {
        if (state->ble_profile_index == 0) {
            strcpy(buf, "BLE ?");
        } else {
            snprintf(buf, sizeof(buf), "BLE %u", state->ble_profile_index);
        }
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    lv_canvas_draw_text(ctx->portrait_canvas, m->x, m->y, ctx->portrait_w, &dsc, buf);
}
