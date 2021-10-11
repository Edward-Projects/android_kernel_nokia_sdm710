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
#include "../../../fih/fih_touch.h"
extern struct fih_touch_cb touch_cb;

#define DSI_POWER_DISABLE 0
#define DSI_POWER_ENABLE 1

#define BBOX_LCM_POWER_STATUS_FAIL do {printk("BBox;%s: Power status fail!\n", __func__); printk("BBox::UEC;0::6\n");} while (0);

int dsi_panel_customize_reset_state(struct dsi_panel *panel)
{
	int rc=0,PanelID=0;

	if(!panel)
		return -EINVAL;

	pr_debug(" %s %d++++\n", __func__, __LINE__);

	PanelID = panel->panel_id;

	switch(PanelID){
		case DISPLAY_PANEL_ID_HX83112A_TIANMA_FHD_VIDEO:
		case DISPLAY_PANEL_ID_HX83112A_CTC_FHD_VIDEO:
		case DISPLAY_PANEL_ID_FT8719A_CTC_FHD_VIDEO:
		case DISPLAY_PANEL_ID_NT36672A_CTC_FHD_VIDEO:
			if(touch_cb.touch_double_tap_read==NULL){
				pr_err("Touch call back function is NULL\n");
				return -EINVAL;
			}

			if (gpio_is_valid(panel->reset_config.reset_gpio))
			{
				if (touch_cb.touch_double_tap_read() == 0)
				{
					pr_info(" %s %d, pull Reset down\n", __func__, __LINE__);
					gpio_set_value(panel->reset_config.reset_gpio, 0);
				}
				else
				{
					pr_info(" %s %d, pull Reset high\n", __func__, __LINE__);
					gpio_set_value(panel->reset_config.reset_gpio, 1);
				}
			}

		break;
		default:
			pr_info(" %s %d, default  pull Reset down\n", __func__, __LINE__);
			gpio_set_value(panel->reset_config.reset_gpio, 0);
		break;
	}
	pr_debug(" %s %d---\n", __func__, __LINE__);

	return rc;

}

int dsi_panel_customize_pwr_state(struct dsi_panel *panel,bool enable)
{
	int rc=0,PanelID=0;
	if(!panel || !panel->panel_always_on)
		return -EINVAL;

	PanelID = panel->panel_id;

	switch(PanelID){
		case DISPLAY_PANEL_ID_HX83112A_TIANMA_FHD_VIDEO:
		case DISPLAY_PANEL_ID_HX83112A_CTC_FHD_VIDEO:
		case DISPLAY_PANEL_ID_FT8719A_CTC_FHD_VIDEO:
		case DISPLAY_PANEL_ID_NT36672A_CTC_FHD_VIDEO:
			if(enable){
				if(panel->power_on_initial==DSI_POWER_DISABLE){
					rc = dsi_pwr_enable_regulator(&panel->power_info, true);
					if (rc) {
						pr_err("[%s] failed to enable vregs, rc=%d\n", panel->name, rc);
						BBOX_LCM_POWER_STATUS_FAIL;
					}
					panel->power_on_initial=DSI_POWER_ENABLE;
					pr_info("Enable LCM Power\n");
				}

			}else{
				//pr_info("LCM Power Always keep\n");
				//panel->power_on_initial=DSI_POWER_DISABLE;
				if(touch_cb.touch_double_tap_read==NULL){
					pr_err("Touch call back function is NULL\n");
					return -EINVAL;
				}
				if(touch_cb.touch_double_tap_read() == 0)
				{
					pr_info("LCM Power disable\n");
					rc = dsi_pwr_enable_regulator(&panel->power_info, false);
					if (rc) {
						pr_err("[%s] failed to enable vregs, rc=%d\n", panel->name, rc);
						BBOX_LCM_POWER_STATUS_FAIL;
					}
					panel->power_on_initial=DSI_POWER_DISABLE;
				}
				else
				{
					pr_info("LCM Power Always keep\n");
				}
			}
		break;
		default:
		break;
	}
	return rc;
}
