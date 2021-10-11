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

static int aod_en=0;
//extern int mxt_touch_state(int);
#ifdef CONFIG_FIH_BATTERY
extern int panel_notifier_call(int power_mode); /* defined in qpnp-smb2.c */
#endif

int fih_set_glance(int enable)
{
	aod_en = enable;
	pr_err("*** AOD enabled(%d)***\n",aod_en);
	return 0;
}
EXPORT_SYMBOL(fih_set_glance);

int fih_get_glance(void)
{
	pr_debug("*** Is AOD enabled(%d)***\n",aod_en);
	return aod_en;
}
EXPORT_SYMBOL(fih_get_glance);

void dsi_panel_power_mode(struct dsi_panel *panel,int pass,int mode)
{
	if (!panel) {
		pr_err("Invalid params\n");
		return;
	}
	if(pass){
		pr_err("The panel can't change mode\n");
		return;
	}
	panel->panel_power_mode=mode;
	return;
}
EXPORT_SYMBOL(dsi_panel_power_mode);

int dsi_display_set_power_by_panel(struct dsi_display *display,struct mipi_dsi_device *dsi,int power_mode,int panel_id)
{
	int rc = 0;
	if (!display || !display->panel) {
		pr_err("invalid display/panel\n");
		return -EINVAL;
	}

#ifdef CONFIG_FIH_BATTERY
	panel_notifier_call(power_mode);
#endif

	switch(panel_id)
	{
	default:
		switch (power_mode) {
			case SDE_MODE_DPMS_ON:
			if(display->panel->panel_power_mode==PANEL_LOW_POWER_MODE){
				display->panel->pixel_early_control = true;
				pr_info("LP-->ON\n");
			}else{
				pr_info("OFF-->ON\n");
				display->panel->pixel_early_control= false;
			}
			break;
		case SDE_MODE_DPMS_LP1:
			rc = dsi_panel_set_lp1(display->panel);
			break;
		case SDE_MODE_DPMS_LP2:
			rc = dsi_panel_set_lp2(display->panel);
			break;
		case SDE_MODE_DPMS_OFF:
			if(display->panel->panel_power_mode==PANEL_LOW_POWER_MODE){
				pr_info("LP-->LP OFF(DDIC ON state)\n");
				dsi_panel_set_nolp(display->panel);
			}else{
				pr_info("ON-->OFF\n");
				display->panel->pixel_early_control= false;
			}

			break;
		default:
			rc = dsi_panel_set_nolp(display->panel);
			break;
		}
		break;
	}
	return rc;
}


int dsi_display_set_power(struct drm_connector *connector,
		int power_mode, void *disp)
{
	struct dsi_display *display = disp;
	struct mipi_dsi_device *dsi;
	int rc = 0;

	if (!display || !display->panel) {
		pr_err("invalid display/panel\n");
		return -EINVAL;
	}

	dsi = &display->panel->mipi_device;

	pr_err("Panel Power Mode (%d)\n",power_mode);
	printk("BBox::EHCS;51301:i:LCM Power Mode=%d\n", power_mode);

	rc = dsi_display_set_power_by_panel(display,dsi,power_mode,display->panel->panel_id);

	return rc;
}

