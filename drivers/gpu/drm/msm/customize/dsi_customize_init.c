#define pr_fmt(fmt)	"msm-dsi-panel:[%s:%d] " fmt, __func__, __LINE__

#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>

#include <linux/device.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>

#include <video/mipi_display.h>
#include "dsi_panel.h"
#include "dsi_ctrl_hw.h"
#include "msm_drv.h"
#include "sde_connector.h"
#include "msm_mmu.h"
#include "dsi_display.h"
#include "dsi_panel.h"
#include "dsi_ctrl.h"
#include "dsi_ctrl_hw.h"
#include "dsi_drm.h"
#include "dsi_clk.h"
#include "dsi_pwr.h"
#include "sde_dbg.h"

static int previous_bl_level = 0;
static int g_panel_id = 0; //Record Panel ID
static const char *panelname;
#define MIPI_DCS_SET_DISPLAYOFF 0x28
#define MIPI_DCS_SET_DISPLAYON 0x29
#define MIPI_DCS_SET_DISPLAYSLEPPMODE 0x10

#define MIPI_DCS_SET_DISPLAYPIXELOFF 0x22
#define DISPLAY_OFF_DELAY_MS 40
#define BBOX_LCM_DISPLAY_OFF_FAIL do {printk("BBox;%s: Display off fail!\n", __func__); printk("BBox::UEC;0::3\n");} while (0);

#define DISPLAY_STD_LOG_EL(PreviousBl,x)       \
	do { \
		if(PreviousBl ==0){ \
			printk("BBox::STD;140700|Display|Enable|BacklightOn|%d\n",x); \
			printk("BBox::STD;140700|P_LCM|UnCoverOn|All|%d\n",x); \
			printk("BBox::STD;140700|FP_LCM|UnLockOn|All|%d\n",x); \
			printk("BBox::STD;140700|Touch|DoubleTap|All|%d\n",x); \
			printk("BBox::STD;140700|Display|Enable|All|%d\n",x); \
		} \
	} while (0)

#define DISPLAY_STD_LOG_P_LCM(Bl,x)       \
	do { \
		if(Bl ==0){ \
			printk("BBox::STD;140700|P_LCM|UnCoverOff|All|%d\n",x); \
		} \
	} while (0)

#define DISPLAY_STD_LOG_BL(PreviousBl,x)       \
	do { \
		if(PreviousBl ==0){ \
			printk("BBox::STD;140700|Display|Enable|BacklightOn|%d\n",x); \
		} \
	} while (0)

extern char *saved_command_line;

int dsi_panel_parse_customize_panel_props(struct dsi_panel *panel,
				      struct device_node *of_node)
{
	int panel_id=0,rc=0;
	u32 val= 0;

	of_property_read_u32(of_node, "qcom,mdss-dsi-panel-id",
				  &panel_id);
	panel->panel_id= panel_id;
	g_panel_id = panel_id;
	panelname = panel->name;
	pr_err("Panel ID(%d)\n",panel->panel_id);


	rc = of_property_read_u32(of_node, "qcom,mdss-dsi-power-off-reset-timing",
				  &val);
	if(rc)
		panel->reset_offtime=1;
	else
		panel->reset_offtime = val;

	panel->lp11_init = of_property_read_bool(of_node,
			"qcom,mdss-dsi-lp11-init");

	pr_err("MIPI-DSI is %s LP11\n",panel->lp11_init?"USE":"UN-USE");

	panel->panel_always_on = of_property_read_bool(of_node,
			"qcom,mdss-dsi-pwr-always-on");

	panel->pixel_early_off= of_property_read_bool(of_node,
			"qcom,mdss-dsi-pixer-early-off");

	pr_info("DSI Power is always on(%d) \n",panel->panel_always_on);

	return 0;
}


int dsi_panel_parse_customize_bl_props(struct dsi_panel *panel,
				      struct device_node *of_node)
{
	int rc=0;
	u32 val= 0;
	bool reload=0;

	reload = of_property_read_bool(of_node,
			"qcom,mdss-dsi-panel-reload-min-brightness");

	if(reload){
		if (strstr(saved_command_line,"androidboot.hwmodel=PNX ")){
			rc = of_property_read_u32(of_node, "qcom,mdss-dsi-bl-reload-min-level", &val);
			if (rc) {
				pr_info("[%s] bl-min-level unspecified, defaulting\n",
					 panel->name);
			} else {
				panel->bl_config.bl_min_level = val;
			}
			pr_info("%s: Set up brightness mapping min is reload(%d)\n", __func__,panel->bl_config.bl_min_level);

		}else{
			pr_info("%s: Set up brightness mapping min is default(%d)\n", __func__,panel->bl_config.bl_min_level);
		}
	}else{
		pr_info("%s: keep brightness mapping min is (%d)\n", __func__,panel->bl_config.bl_min_level);
	}


	return 0;
}

void dsi_panel_show_brightness(u32 bl_lvl)
{
	if ((bl_lvl == 0) || ((previous_bl_level == 0) && (bl_lvl != 0))){
		pr_err("level=%d\n", bl_lvl);
		printk("BBox::EHCS;51401:i:Backlight status=%d\n", (bl_lvl == 0)?0:1);
		DISPLAY_STD_LOG_EL(previous_bl_level,1);
		DISPLAY_STD_LOG_P_LCM(bl_lvl,1);
	}
	previous_bl_level = bl_lvl;
	return;
}

void dsi_panel_power_off_reset_timing(struct dsi_panel *panel)
{
	if(!panel)
		return;

	usleep_range(panel->reset_offtime*1000,panel->reset_offtime*1000);
	return;
}

int get_Display_ID(void)
{
	printk("BBox::STD;151200|%s\n",panelname);
	return g_panel_id;
}
EXPORT_SYMBOL(get_Display_ID);

bool dsi_always_response_long_package(void)
{
	bool rc=false;
	int panel=get_Display_ID();

	switch(panel)
	{
		case DISPLAY_PANEL_ID_HX83112A_TIANMA_FHD_VIDEO:
		case DISPLAY_PANEL_ID_HX83112A_CTC_FHD_VIDEO:
			rc=true;
		break;
		default:
			rc=false;
		break;
	}
	return rc;
}
EXPORT_SYMBOL(dsi_always_response_long_package);


int dsi_display_pixel_early_off(struct dsi_panel *panel,u32 bl_lvl)
{
	int rc=0;
	u8 payload=0;
	struct mipi_dsi_device *dsi;

	if (!panel || (bl_lvl > 0xffff)) {
		pr_err("invalid params\n");
		return -EINVAL;
	}

	if (panel->type == EXT_BRIDGE)
		return 0;

	if(panel->pixel_early_off==false)
		return 0;

	dsi = &panel->mipi_device;

	//pr_info(">>>bl_lvl(%d),previous_bl_level(%d)<<<panel_initialized=(%d)>>pixel_early_control=%d<<\n",bl_lvl,previous_bl_level,panel->panel_initialized,panel->pixel_early_control);

	DISPLAY_STD_LOG_BL(previous_bl_level,0);

	if(panel->panel_initialized && bl_lvl==0){
		pr_err("++\n");
		rc = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_DISPLAYOFF,
				 &payload, sizeof(payload));
		panel->pixel_early_control= true;
		usleep_range(DISPLAY_OFF_DELAY_MS*1000,DISPLAY_OFF_DELAY_MS*1000);
		pr_err("--\n");
	}else if(panel->panel_initialized && panel->pixel_early_control && bl_lvl!=0 &&previous_bl_level==0){
		//rc = dsi_panel_enable(panel);
		pr_err("Early on pixel ++\n");
		rc = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_DISPLAYON,
				 &payload, sizeof(payload));


		usleep_range(DISPLAY_OFF_DELAY_MS*1000,DISPLAY_OFF_DELAY_MS*1000);
		panel->pixel_early_control=false;
		pr_err("Early on pixel --\n");
	}else{
		pr_debug(">>>bl_lvl(%d),previous_bl_level(%d)<<<\n",bl_lvl,previous_bl_level);
	}
	if(rc){
		pr_err("[%s] failed to send DSI_CMD_SET_OFF cmds, rc=%d\n",
		       panel->name, rc);
		BBOX_LCM_DISPLAY_OFF_FAIL;
		goto error;
	}

error:
	return rc;
}
EXPORT_SYMBOL(dsi_display_pixel_early_off);

int dsi_panel_detection_panel_disable(struct dsi_panel *panel)
{
	int rc=0;

	if (!panel) {
		pr_err("invalid params\n");
		return -EINVAL;
	}

	if (panel->type == EXT_BRIDGE)
		return 0;

	if(panel->pixel_early_off==false)
		return -EINVAL;
	pr_err("++\n");

	if(panel->panel_initialized){
		rc=0;
	}else{
		rc = -EPIPE;
	}

	pr_err("--\n");
	return rc;
}
EXPORT_SYMBOL(dsi_panel_detection_panel_disable);

