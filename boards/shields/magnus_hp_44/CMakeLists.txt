target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE
        custom_status_screen.c
        widgets/portrait_demo.c
        widgets/mod_battery.c
)

# Only the central (left) half knows the selected endpoint and receives endpoint change events.
# So only build the output module for:
#  - non-split builds, or
#  - split central builds
if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    target_sources(app PRIVATE
            widgets/mod_output.c
    )
endif()
