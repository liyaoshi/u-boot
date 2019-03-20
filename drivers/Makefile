#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-$(CONFIG_$(SPL_)DM)		+= core/
obj-$(CONFIG_$(SPL_)CLK)	+= clk/
obj-$(CONFIG_$(SPL_)PINCTRL)	+= pinctrl/
obj-$(CONFIG_$(SPL_)RAM)	+= ram/

ifdef CONFIG_SPL_BUILD

obj-$(CONFIG_SPL_I2C_SUPPORT) += i2c/
obj-$(CONFIG_SPL_GPIO_SUPPORT) += gpio/
obj-$(CONFIG_SPL_MMC_SUPPORT) += mmc/
obj-$(CONFIG_SPL_SERIAL_SUPPORT) += serial/
obj-$(CONFIG_SPL_SPI_SUPPORT) += spi/
obj-$(CONFIG_SPL_POWER_SUPPORT) += power/ power/pmic/
obj-$(CONFIG_SPL_POWER_SUPPORT) += power/regulator/
obj-$(CONFIG_SPL_MUSB_NEW_SUPPORT) += usb/musb-new/
obj-$(CONFIG_SPL_USBETH_SUPPORT) += usb/gadget/
obj-$(CONFIG_SPL_USB_HOST_SUPPORT) += usb/host/
obj-$(CONFIG_OMAP_USB_PHY) += usb/phy/
obj-$(CONFIG_SPL_SATA_SUPPORT) += block/

ifdef CONFIG_SPL_USBETH_SUPPORT
obj-$(CONFIG_USB_DWC3_GADGET) += usb/dwc3/
obj-$(CONFIG_USB_DWC3_GADGET) += usb/gadget/udc/
endif

else

obj-$(CONFIG_DM_DEMO) += demo/
obj-y += block/
obj-$(CONFIG_CPU) += cpu/
obj-y += dfu/
obj-y += timer/
obj-y += memory/
obj-y += input/
# SOC specific infrastructure drivers.
obj-$(CONFIG_REMOTEPROC) += remoteproc/

endif
