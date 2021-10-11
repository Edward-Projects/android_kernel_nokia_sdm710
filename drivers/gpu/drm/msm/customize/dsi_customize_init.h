

extern int dsi_panel_parse_customize_panel_props(struct dsi_panel *panel,
				      struct device_node *of_node);

extern void dsi_panel_show_brightness(u32 bl_lvl);

extern void dsi_panel_power_off_reset_timing(struct dsi_panel *panel);

extern bool dsi_always_response_long_package(void);

extern void dsi_display_pixel_early_off(struct dsi_panel *panel,u32 bl_lvl);

extern int dsi_panel_detection_panel_disable(struct dsi_panel *panel);

extern int dsi_panel_parse_customize_bl_props(struct dsi_panel *panel,
				      struct device_node *of_node);
