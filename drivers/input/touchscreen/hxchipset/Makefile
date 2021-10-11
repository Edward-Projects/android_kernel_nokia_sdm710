# Makefile for the Himax touchscreen drivers.
ccflags-y := -Idrivers/gpu/drm/msm/dsi-staging
ccflags-y += -Idrivers/gpu/drm/msm

obj-$(CONFIG_TOUCHSCREEN_HIMAX_I2C)   	+= himax_platform.o himax_ic.o himax_common.o himax_debug.o
obj-$(CONFIG_TOUCHSCREEN_HIMAX_ITO_TEST)   	+= 	himax_ito_test.o
