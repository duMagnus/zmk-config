#pragma once

#include <stdint.h>
#include "render_ctx.h"

typedef struct {
    uint16_t x;
    uint16_t y;
} battery_module_t;

void battery_module_init(battery_module_t *m, uint16_t x, uint16_t y);
void battery_module_update(screen_state_t *state);
void battery_module_draw(const battery_module_t *m, const render_ctx_t *ctx, const screen_state_t *state);
