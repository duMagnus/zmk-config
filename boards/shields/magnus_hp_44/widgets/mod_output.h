#pragma once

struct output_module_state {
    char label[12]; // "USB" or "BLE X"
};

void output_module_init(struct output_module_state *state);

const char *output_module_get_label(const struct output_module_state *state);
