target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE
        custom_status_screen.c
        widgets/portrait_demo.c
        widgets/mod_battery.c
        widgets/mod_output.c
        widgets/mod_layer.c
        widgets/mod_logo32.c
)
