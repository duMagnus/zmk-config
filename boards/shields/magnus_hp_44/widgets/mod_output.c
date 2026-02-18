#include "mod_output.h"

#include <stdio.h>
#include <string.h>

#include <zmk/endpoints.h>
#include <zmk/endpoints_types.h>
#include <zmk/event_manager.h>
#include <zmk/events/endpoint_changed.h>

static struct output_module_state *g_state;

static void set_label(const char *text) {
    if (!g_state) {
        return;
    }

    strncpy(g_state->label, text, sizeof(g_state->label) - 1);
    g_state->label[sizeof(g_state->label) - 1] = '\0';
}

static void refresh_label(void) {
    if (!g_state) {
        return;
    }

    const struct zmk_endpoint_instance ep = zmk_endpoints_selected();

    if (ep.transport == ZMK_TRANSPORT_USB) {
        set_label("USB");
        return;
    }

    if (ep.transport == ZMK_TRANSPORT_BLE) {
        unsigned int idx = (unsigned int)ep.ble.profile_index + 1; // show 1-based
        snprintf(g_state->label, sizeof(g_state->label), "BLE %u", idx);
        return;
    }

    set_label("UNK");
}

void output_module_init(struct output_module_state *state) {
    g_state = state;
    set_label("");
    refresh_label();
}

const char *output_module_get_label(const struct output_module_state *state) {
    return state ? state->label : "";
}

/* Event listener: update whenever output endpoint changes (USB <-> BLE, profile switch) */
static int output_listener(const zmk_event_t *eh) {
    if (as_zmk_endpoint_changed(eh) != NULL) {
        refresh_label();
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(magnus_hp_44_output, output_listener);
ZMK_SUBSCRIPTION(magnus_hp_44_output, zmk_endpoint_changed);
