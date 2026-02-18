#pragma once

#include <stdint.h>

#include "portrait_demo.h"

typedef struct {
    uint16_t x;
    uint16_t y;
} logo32_module_t;

void logo32_module_init(logo32_module_t *m, uint16_t x, uint16_t y);

void logo32_module_draw(const logo32_module_t *m,
                        const render_ctx_t *ctx,
                        const screen_state_t *state);
