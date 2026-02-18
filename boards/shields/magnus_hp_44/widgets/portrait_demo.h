#pragma once
#include <lvgl.h>

// Creates the screen content (can be called once from zmk_display_status_screen()).
void magnus_hp_44_portrait_demo_create(lv_obj_t *parent);

// Call this later when you want to redraw (battery change, layer change, timer tick, etc.)
void magnus_hp_44_portrait_demo_redraw(void);
