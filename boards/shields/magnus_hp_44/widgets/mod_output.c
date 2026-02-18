#include "mod_output.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "portrait_demo.h"

// Only the CENTRAL side should show output (USB/BLE).
// On the peripheral/right half, omit it entirely.
#if !defined(CONFIG_ZMK_SPLIT) || defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

#include <zmk/endpoints.h>
#include <zmk/endpoints_types.h>
#include <zmk/event_manager.h>
#include <zmk/events/endpoint_changed.h>

static output_module_t *g_output;

// Tiny 8x8 Bluetooth-ish icon (1bpp), MSB-first per row.
static const uint8_t bt_icon_8x8[8] = {
    0b00011000,
    0b00101000,
    0b01001000,
    0b10011000,
    0b10011000,
    0b01001000,
    0b00101000,
    0b00011000,
};

static void draw_bt_icon(lv_obj_t *canvas, int x, int y) {
    // Canvas is 1bpp; we draw white pixels where bits are 1.
    for (int row = 0; row < 8; row++) {
        uint8_t bits = bt_icon_8x8[row];
        for (int col = 0; col < 8; col++) {
            int on = (bits >> (7 - col)) & 1;
            if (on) {
                lv_canvas_set_px(canvas, x + col, y + row, lv_color_white());
            }
        }
    }
}

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

    // When output changes, refresh cached state and redraw.
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

    // Prime initial state (event will keep it fresh)
    update_output_state();
}

void output_module_draw(const output_module_t *m,
                        const render_ctx_t *ctx,
                        const screen_state_t *state) {
    if (!m || !ctx || !state || !ctx->portrait_canvas) {
        return;
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    if (state->output_is_usb) {
        // Keep this short so it never wraps.
        lv_canvas_draw_text(ctx->portrait_canvas, m->x, m->y, ctx->portrait_w, &dsc, "USB");
        return;
    }

    // BLE: draw icon + 1-based number
    const unsigned int shown = (unsigned int)state->ble_profile_index + 1;

    // icon at (x, y), number to the right
    draw_bt_icon(ctx->portrait_canvas, m->x, m->y);

    char num[4];
    snprintf(num, sizeof(num), "%u", shown);

    // Put the number close to the icon to stay on one line.
    lv_canvas_draw_text(ctx->portrait_canvas, m->x + 10, m->y - 1, ctx->portrait_w - (m->x + 10), &dsc, num);
}

#else
// Peripheral/right half: omit output entirely.
void output_module_init(output_module_t *m, uint16_t x, uint16_t y, screen_state_t *state) {
    ARG_UNUSED(m);
    ARG_UNUSED(x);
    ARG_UNUSED(y);
    ARG_UNUSED(state);
}

void output_module_draw(const output_module_t *m,
                        const render_ctx_t *ctx,
                        const screen_state_t *state) {
    ARG_UNUSED(m);
    ARG_UNUSED(ctx);
    ARG_UNUSED(state);
}
#endif
