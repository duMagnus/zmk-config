#include "mod_layer.h"

#include <lvgl.h>
#include <string.h>

#include <zephyr/sys/util.h> // ARG_UNUSED

#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/layer_state_changed.h>

#include "portrait_demo.h"

static layer_module_t *g_layer;

static int on_layer_changed(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    // We don't need to cache anything; on draw we query highest active + name.
    // Just request a redraw immediately (queued via your work item).
    magnus_hp_44_portrait_demo_redraw();

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(magnus_hp_44_layer_listener, on_layer_changed);
ZMK_SUBSCRIPTION(magnus_hp_44_layer_listener, zmk_layer_state_changed);

void layer_module_init(layer_module_t *m, uint16_t x, uint16_t y, screen_state_t *state) {
    if (!m) {
        return;
    }

    m->x = x;
    m->y = y;
    m->state = state;
    g_layer = m;

    // Initial draw
    magnus_hp_44_portrait_demo_redraw();
}

void layer_module_draw(const layer_module_t *m,
                       const render_ctx_t *ctx,
                       const screen_state_t *state) {
    ARG_UNUSED(state);

    if (!m || !ctx || !ctx->portrait_canvas) {
        return;
    }

    const uint8_t layer = zmk_keymap_highest_layer_active();
    const char *name = zmk_keymap_layer_name(layer);

    // Fallback if name is missing/empty
    char fallback[8];
    if (!name || name[0] == '\0') {
        // Shows e.g. "L3"
        snprintf(fallback, sizeof(fallback), "L%u", (unsigned int)layer);
        name = fallback;
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    // Use the same small font you already know exists in your build.
    dsc.font = &lv_font_unscii_8;

    lv_canvas_draw_text(ctx->portrait_canvas,
                        (int)m->x,
                        (int)m->y,
                        (int)ctx->portrait_w - (int)m->x,
                        &dsc,
                        name);
}
