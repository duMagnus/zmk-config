# Compile our custom status screen only when custom screens are enabled
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE status.c)
