ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
	dtbo-$(CONFIG_ARCH_SDM670) += \
		sdm670-mtp-overlay_pnx-evb.dtbo \
		sdm670-mtp-overlay_pnx-pre_evt_CN.dtbo \
		sdm670-mtp-overlay_pnx-evt_CN.dtbo \
		sdm670-mtp-overlay_pnx-evt_CN_2nd.dtbo \
		sdm670-mtp-overlay_pnx-evt_ROW.dtbo \
		sdm710-mtp-overlay_pnx-dvt_CN.dtbo \
		sdm710-mtp-overlay_pnx-dvt_IN.dtbo \
		sdm710-mtp-overlay_pnx-dvt_ROW.dtbo \
		sdm670-mtp-overlay_pnx-dvt1_CN.dtbo \
		sdm670-mtp-overlay_pnx-dvt1_IN.dtbo \
		sdm670-mtp-overlay_pnx-dvt1_ROW.dtbo \
		sdm710-mtp-overlay_pnx-dvt2_ROW.dtbo \
		sdm710-mtp-overlay_pnx-pre_pvt_CN.dtbo \
		sdm710-mtp-overlay_pnx-pvt_CN.dtbo \
		sdm710-mtp-overlay_pnx-pvt_IN.dtbo \
		sdm710-mtp-overlay_pnx-pvt_ROW.dtbo

sdm670-mtp-overlay_pnx-evb.dtbo-base := sdm670-fih.dtb
sdm670-mtp-overlay_pnx-pre_evt_CN.dtbo-base := sdm670-fih.dtb
sdm670-mtp-overlay_pnx-evt_CN.dtbo-base := sdm670-fih.dtb
sdm670-mtp-overlay_pnx-evt_CN_2nd.dtbo-base := sdm670-fih.dtb
sdm670-mtp-overlay_pnx-evt_ROW.dtbo-base := sdm670-fih.dtb
sdm710-mtp-overlay_pnx-dvt_CN.dtbo-base := sdm710-fih.dtb
sdm710-mtp-overlay_pnx-dvt_IN.dtbo-base := sdm710-fih.dtb
sdm710-mtp-overlay_pnx-dvt_ROW.dtbo-base := sdm710-fih.dtb
sdm670-mtp-overlay_pnx-dvt1_CN.dtbo-base := sdm670-fih.dtb
sdm670-mtp-overlay_pnx-dvt1_IN.dtbo-base := sdm670-fih.dtb
sdm670-mtp-overlay_pnx-dvt1_ROW.dtbo-base := sdm670-fih.dtb
sdm710-mtp-overlay_pnx-dvt2_ROW.dtbo-base := sdm710-fih.dtb
sdm710-mtp-overlay_pnx-pre_pvt_CN.dtbo-base := sdm710-fih.dtb
sdm710-mtp-overlay_pnx-pvt_CN.dtbo-base := sdm710-fih.dtb
sdm710-mtp-overlay_pnx-pvt_IN.dtbo-base := sdm710-fih.dtb
sdm710-mtp-overlay_pnx-pvt_ROW.dtbo-base := sdm710-fih.dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
