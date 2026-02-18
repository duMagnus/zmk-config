#pragma once

#include <stdint.h>
#include "render_ctx.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    screen_state_t *state;
} layer_module_t;

void layer_module_init(layer_module_t *m, uint16_t x, uint16_t y, screen_state_t *state);

void layer_module_draw(const layer_module_t *m,
                       const render_ctx_t *ctx,
                       const screen_state_t *state);
