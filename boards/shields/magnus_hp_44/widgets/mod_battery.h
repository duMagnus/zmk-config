#pragma once

#include <stdint.h>
#include "render_ctx.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    screen_state_t *state;
} battery_module_t;

// Battery module owns battery updates via ZMK events.
// It writes into `state->battery_percent` and triggers redraw via portrait_demo_redraw().
void battery_module_init(battery_module_t *m, uint16_t x, uint16_t y, screen_state_t *state);

void battery_module_draw(const battery_module_t *m,
                         const render_ctx_t *ctx,
                         const screen_state_t *state);
