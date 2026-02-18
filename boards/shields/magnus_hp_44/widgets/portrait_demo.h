#pragma once
#include <lvgl.h>

// Creates the screen content (called once from zmk_display_status_screen()).
void magnus_hp_44_portrait_demo_create(lv_obj_t *parent);

// Safe to call from modules (battery change, layer change, output change, etc.)
// This schedules a redraw on a work item.
void magnus_hp_44_portrait_demo_redraw(void);
