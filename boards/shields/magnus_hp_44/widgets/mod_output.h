#pragma once

#include <stdint.h>
#include "render_ctx.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    screen_state_t *state;
} output_module_t;

// Output module owns output updates via ZMK events (central only).
// It writes into:
//   state->output_is_usb (1 if USB, 0 if BLE)
//   state->ble_profile_index (0-based)
// and triggers redraw via portrait_demo_redraw().
void output_module_init(output_module_t *m, uint16_t x, uint16_t y, screen_state_t *state);

void output_module_draw(const output_module_t *m,
                        const render_ctx_t *ctx,
                        const screen_state_t *state);
