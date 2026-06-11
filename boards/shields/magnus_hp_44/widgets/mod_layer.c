#include "mod_layer.h"

#include <lvgl.h>
#include <string.h>

#include <zephyr/sys/util.h> // ARG_UNUSED

#include "portrait_demo.h"

// Only the CENTRAL side has keymap/layer APIs available.
// Peripheral/right half often doesn't link keymap, so we must not reference those symbols there.
#if !defined(CONFIG_ZMK_SPLIT) || defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

static layer_module_t *g_layer;

static void update_layer_state(void) {
    if (!g_layer || !g_layer->state) {
        return;
    }

    // Highest active layer is returned as an ORDERED layer index.
    // Convert to layer ID, then get the configured name.
    const zmk_keymap_layer_index_t idx = zmk_keymap_highest_layer_active();
    const zmk_keymap_layer_id_t id = zmk_keymap_layer_index_to_id(idx);

    const char *name = zmk_keymap_layer_name(id);
    if (!name) {
        name = "?";
    }

    // Copy into shared screen state (safe, stable for draw code).
    strncpy(g_layer->state->layer_name, name, sizeof(g_layer->state->layer_name) - 1);
    g_layer->state->layer_name[sizeof(g_layer->state->layer_name) - 1] = '\0';
}

static int on_layer_state_changed(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    update_layer_state();
    magnus_hp_44_portrait_demo_redraw();

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(magnus_hp_44_layer_listener, on_layer_state_changed);
ZMK_SUBSCRIPTION(magnus_hp_44_layer_listener, zmk_layer_state_changed);

void layer_module_init(layer_module_t *m, uint16_t x, uint16_t y, screen_state_t *state) {
    if (!m) {
        return;
    }

    m->x = x;
    m->y = y;
    m->state = state;

    g_layer = m;

    // Prime the initial value
    update_layer_state();
}

void layer_module_draw(const layer_module_t *m,
                       const render_ctx_t *ctx,
                       const screen_state_t *state) {
    if (!m || !ctx || !state || !ctx->portrait_canvas) {
        return;
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_white();

    // Use the same small font you already know exists in your build.
    dsc.font = &lv_font_montserrat_10;

    const char *name = state->layer_name[0] ? state->layer_name : "?";

    // Draw the layer name. If it's too long, LVGL will wrap; keep names short in keymap.
    lv_canvas_draw_text(ctx->portrait_canvas, m->x, (int)m->y - 1, ctx->portrait_w - m->x, &dsc, name);
}

#else
// Peripheral/right half: omit layer handling entirely (no keymap symbols available).
void layer_module_init(layer_module_t *m, uint16_t x, uint16_t y, screen_state_t *state) {
    ARG_UNUSED(m);
    ARG_UNUSED(x);
    ARG_UNUSED(y);
    ARG_UNUSED(state);
}

void layer_module_draw(const layer_module_t *m,
                       const render_ctx_t *ctx,
                       const screen_state_t *state) {
    ARG_UNUSED(m);
    ARG_UNUSED(ctx);
    ARG_UNUSED(state);
}
#endif
