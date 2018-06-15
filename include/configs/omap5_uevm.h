/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated.
 * Sricharan R	  <r.sricharan@ti.com>
 *
 * Configuration settings for the TI EVM5430 board.
 * See ti_omap5_common.h for omap5 common settings.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_OMAP5_EVM_H
#define __CONFIG_OMAP5_EVM_H

#include <environment/ti/dfu.h>

#ifndef CONFIG_SPL_BUILD
#define PARTS_DEFAULT \
        /* Linux partitions */ \
    "uuid_disk=${uuid_gpt_disk};" \
    "name=rootfs,start=2MiB,size=-,uuid=${uuid_gpt_rootfs}\0" \
    /* Android partitions */ \
    "partitions_android=" \
    "uuid_disk=${uuid_gpt_disk};" \
    "name=xloader,start=128K,size=256K,uuid=${uuid_gpt_xloader};" \
    "name=bootloader,size=2304K,uuid=${uuid_gpt_bootloader};" \
    "name=environment,size=256K,uuid=${uuid_gpt_environment};" \
    "name=misc,size=128K,uuid=${uuid_gpt_misc};" \
    "name=reserved,size=384K,uuid=${uuid_gpt_reserved};" \
    "name=efs,size=16M,uuid=${uuid_gpt_efs};" \
    "name=crypto,size=16K,uuid=${uuid_gpt_crypto};" \
    "name=recovery,size=30M,uuid=${uuid_gpt_recovery};" \
    "name=boot,size=30M,uuid=${uuid_gpt_boot};" \
    "name=system,size=768M,uuid=${uuid_gpt_system};" \
    "name=vendor,size=256M,uuid=${uuid_gpt_vendor};" \
    "name=cache,size=256M,uuid=${uuid_gpt_cache};" \
    "name=ipu1,size=8M,uuid=${uuid_gpt_ipu1};" \
    "name=ipu2,size=8M,uuid=${uuid_gpt_ipu2};" \
    "name=dsp1,size=8M,uuid=${uuid_gpt_dsp1};" \
    "name=dsp2,size=8M,uuid=${uuid_gpt_dsp2};" \
    "name=userdata,size=-,uuid=${uuid_gpt_userdata}"

#define DFUARGS \
        "dfu_bufsiz=0x10000\0" \
    DFU_ALT_INFO_EMMC \
    DFU_ALT_INFO_RAM 

/* Fastboot */
#define CONFIG_USB_FUNCTION_FASTBOOT
#define CONFIG_CMD_FASTBOOT
#define CONFIG_FASTBOOT_BUF_ADDR    CONFIG_SYS_LOAD_ADDR
#define CONFIG_FASTBOOT_BUF_SIZE    0x2F000000
#define CONFIG_FASTBOOT_FLASH
#define CONFIG_FASTBOOT_FLASH_MMC_DEV   1
#endif

#ifdef CONFIG_SPL_BUILD
#undef CONFIG_CMD_BOOTD
#ifdef CONFIG_SPL_DFU_SUPPORT
#define CONFIG_SPL_LOAD_FIT_ADDRESS 0x80200000
#define CONFIG_SPL_ENV_SUPPORT
#define CONFIG_SPL_HASH_SUPPORT

#ifdef CONFIG_SPL_LOAD_FIT
#define DFUARGS \
        "dfu_bufsiz=0x10000\0" \
    DFU_ALT_INFO_RAM
#else
#define DFU_ALT_INFO_RAM \
        "dfu_alt_info_ram=" \
    "kernel ram 0x807fffc0 0x4000000;" \
    "fdt ram 0x80f80000 0x80000;" \
    "ramdisk ram 0x81000000 0x4000000\0"
#define DFUARGS \
        "dfu_bufsiz=0x10000\0" \
    DFU_ALT_INFO_RAM
#endif
#endif
#endif

#include <configs/ti_omap5_common.h>

#define CONFIG_CONS_INDEX		3
#define CONFIG_SYS_NS16550_COM3		UART3_BASE
#define CONFIG_BAUDRATE			115200

#define CONFIG_MISC_INIT_R
/* MMC ENV related defines */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		1	/* SLOT2: eMMC(1) */
#define CONFIG_ENV_SIZE			(128 << 10)
#define CONFIG_ENV_OFFSET		0x260000
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT

/* Enhance our eMMC support / experience. */
#define CONFIG_CMD_GPT
#define CONFIG_EFI_PARTITION
#define CONFIG_HSMMC2_8BIT
#define CONFIG_SUPPORT_EMMC_BOOT

/* Required support for the TCA642X GPIO we have on the uEVM */
#define CONFIG_TCA642X
#define CONFIG_CMD_TCA642X
#define CONFIG_SYS_I2C_TCA642X_BUS_NUM 4
#define CONFIG_SYS_I2C_TCA642X_ADDR 0x22

/* USB UHH support options */
#define CONFIG_USB_HOST
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_OMAP
#define CONFIG_USB_STORAGE
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS 3
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET

#define CONFIG_OMAP_EHCI_PHY2_RESET_GPIO 80
#define CONFIG_OMAP_EHCI_PHY3_RESET_GPIO 79

/* USB Device Firmware Update support */
#define CONFIG_USB_FUNCTION_DFU
#define CONFIG_DFU_RAM

#define CONFIG_DFU_MMC
#define CONFIG_ANDROID_BOOT_IMAGE
/* Enabled commands */

/* USB Networking options */
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_SMSC95XX

#define CONSOLEDEV		"ttyO2"

/* Max time to hold reset on this board, see doc/README.omap-reset-time */
#define CONFIG_OMAP_PLATFORM_RESET_TIME_MAX_USEC	16296

#define CONFIG_CMD_SCSI
#define CONFIG_LIBATA
#define CONFIG_SCSI_AHCI
#define CONFIG_SCSI_AHCI_PLAT
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	1
#define CONFIG_SYS_SCSI_MAX_LUN		1
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * \
						CONFIG_SYS_SCSI_MAX_LUN)

#endif /* __CONFIG_OMAP5_EVM_H */
