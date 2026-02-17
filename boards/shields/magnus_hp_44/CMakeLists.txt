if(CONFIG_ZMK_DISPLAY)
    # Add our custom status screen implementation when custom status screens are enabled
    if(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
        target_sources(app PRIVATE
                custom_status_screen.c
                widgets/portrait_demo.c
        )
    endif()

    # (Optional) if you add headers later:
    target_include_directories(app PRIVATE include)
endif()
