#
# (C) Copyright 2000-2007
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

ifdef CONFIG_DM_USB
obj-$(CONFIG_CMD_USB) += usb-uclass.o
obj-$(CONFIG_SANDBOX) += usb-sandbox.o
endif

# ohci
obj-$(CONFIG_USB_OHCI_NEW) += ohci-hcd.o
obj-$(CONFIG_USB_OHCI_DA8XX) += ohci-da8xx.o
obj-$(CONFIG_USB_SL811HS) += sl811-hcd.o
obj-$(CONFIG_USB_OHCI_GENERIC) += ohci-generic.o

# echi
obj-$(CONFIG_USB_EHCI) += ehci-hcd.o
obj-$(CONFIG_USB_EHCI_ARMADA100) += ehci-armada100.o utmi-armada100.o
obj-$(CONFIG_USB_EHCI_ATMEL) += ehci-atmel.o
ifdef CONFIG_MPC512X
obj-$(CONFIG_USB_EHCI_FSL) += ehci-mpc512x.o
else
obj-$(CONFIG_USB_EHCI_FSL) += ehci-fsl.o
endif
obj-$(CONFIG_USB_EHCI_FARADAY) += ehci-faraday.o
obj-$(CONFIG_USB_EHCI_GENERIC) += ehci-generic.o
obj-$(CONFIG_USB_EHCI_OMAP) += ehci-omap.o
obj-$(CONFIG_USB_EHCI_MSM) += ehci-msm.o
obj-$(CONFIG_USB_EHCI_SPEAR) += ehci-spear.o
obj-$(CONFIG_USB_EHCI_VCT) += ehci-vct.o

# xhci
obj-$(CONFIG_USB_XHCI) += xhci.o xhci-mem.o xhci-ring.o
obj-$(CONFIG_USB_XHCI_DWC3) += xhci-dwc3.o
obj-$(CONFIG_USB_XHCI_OMAP) += xhci-omap.o

# designware
obj-$(CONFIG_USB_DWC2) += dwc2.o
