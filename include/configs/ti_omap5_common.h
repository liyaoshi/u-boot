/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated.
 * Sricharan R	  <r.sricharan@ti.com>
 *
 * Derived from OMAP4 done by:
 *	Aneesh V <aneesh@ti.com>
 *
 * TI OMAP5 AND DRA7XX common configuration settings
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * For more details, please see the technical documents listed at
 * http://www.ti.com/product/omap5432
 */

#ifndef __CONFIG_TI_OMAP5_COMMON_H
#define __CONFIG_TI_OMAP5_COMMON_H

/* Common ARM Erratas */
#define CONFIG_ARM_ERRATA_798870

#define CONFIG_SYS_CACHELINE_SIZE	64

/* Use General purpose timer 1 */
#define CONFIG_SYS_TIMERBASE		GPT2_BASE

/*
 * For the DDR timing information we can either dynamically determine
 * the timings to use or use pre-determined timings (based on using the
 * dynamic method.  Default to the static timing infomation.
 */
#define CONFIG_SYS_EMIF_PRECALCULATED_TIMING_REGS
#ifndef CONFIG_SYS_EMIF_PRECALCULATED_TIMING_REGS
#define CONFIG_SYS_AUTOMATIC_SDRAM_DETECTION
#define CONFIG_SYS_DEFAULT_LPDDR2_TIMINGS
#endif

#define CONFIG_PALMAS_POWER

#include <asm/arch/cpu.h>
#include <asm/arch/omap.h>

#include <configs/ti_armv7_omap.h>

/*
 * Hardware drivers
 */
#define CONFIG_SYS_NS16550_CLK		48000000
#if defined(CONFIG_SPL_BUILD) || !defined(CONFIG_DM_SERIAL)
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#endif

/*
 * Environment setup
 */
#ifndef PARTS_DEFAULT
#define PARTS_DEFAULT
#endif

#ifndef DFUARGS
#define DFUARGS
#endif

#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_LINUX_BOOT_ENV \
	DEFAULT_MMC_TI_ARGS \
	DEFAULT_FIT_TI_ARGS \
	"console=" CONSOLEDEV ",115200n8\0" \
	"fdtfile=undefined\0" \
	"reboot_image=boot\0" \
	"boot_os=0\0" \
	"bootpart=0:2\0" \
	"bootdir=/boot\0" \
	"bootfile=zImage\0" \
	"usbtty=cdc_acm\0" \
	"vram=16M\0" \
	"partitions=" PARTS_DEFAULT "\0" \
	"optargs=\0" \
	"dofastboot=0\0" \
	"loadbootscript=fatload mmc ${mmcdev} ${loadaddr} boot.scr\0" \
	"bootscript=echo Running bootscript from mmc${mmcdev} ...; " \
		"source ${loadaddr}\0" \
	"loadimage=load mmc ${bootpart} ${loadaddr} ${bootdir}/${bootfile}\0" \
	"mmcboot=" \
		"if mmc dev ${mmcdev}; then " \
			"setenv devtype mmc; " \
			"if mmc rescan; then " \
				"echo SD/MMC found on device ${mmcdev};" \
				"if run loadimage; then " \
					"run loadfdt; " \
					"echo Booting from mmc${mmcdev} ...; " \
					"run args_mmc; " \
					"bootz ${loadaddr} - ${fdtaddr}; " \
				"fi; " \
			"fi; " \
		"fi;\0" \
	"emmc_android_boot=" \
		"setenv eval_bootargs setenv bootargs $bootargs; " \
		"run eval_bootargs; " \
		"setenv mmcdev 1; " \
		"setenv fdt_part 3; " \
		"setenv boot_part 9; " \
		"if test $reboot_image = recovery; then " \
			"setenv boot_part 8; " \
			"setenv reboot_image boot; saveenv; fi;" \
		"setenv machid fe6; " \
		"mmc dev $mmcdev; " \
		"mmc rescan; " \
		"part start mmc ${mmcdev} ${fdt_part} fdt_start; " \
		"part size mmc ${mmcdev} ${fdt_part} fdt_size; " \
		"part start mmc ${mmcdev} ${boot_part} boot_start; " \
		"part size mmc ${mmcdev} ${boot_part} boot_size; " \
		"mmc read ${fdtaddr} ${fdt_start} ${fdt_size}; " \
		"mmc read ${loadaddr} ${boot_start} ${boot_size}; " \
		"echo Booting from eMMC ...; " \
		"bootm $loadaddr $loadaddr $fdtaddr;\0" \
	"findfdt="\
		"if test $board_name = omap5_uevm; then " \
			"setenv fdtfile omap5-uevm.dtb; fi; " \
		"if test $board_name = dra7xx; then " \
			"setenv fdtfile dra7-evm.dtb; fi;" \
		"if test $board_name = dra72x-revc; then " \
			"setenv fdtfile dra72-evm-revc.dtb; fi;" \
		"if test $board_name = dra72x; then " \
			"setenv fdtfile dra72-evm.dtb; fi;" \
		"if test $board_name = dra71x; then " \
			"setenv fdtfile dra71-evm.dtb; fi;" \
		"if test $board_name = dra76x; then " \
			"setenv fdtfile dra76-evm.dtb; fi;" \
		"if test $board_name = beagle_x15; then " \
			"setenv fdtfile am57xx-beagle-x15.dtb; fi;" \
		"if test $board_name = beagle_x15_revb1; then " \
			"setenv fdtfile am57xx-beagle-x15-revb1.dtb; fi;" \
		"if test $board_name = am57xx_evm; then " \
			"setenv fdtfile am57xx-evm.dtb; fi;" \
		"if test $board_name = am57xx_evm_reva3; then " \
			"setenv fdtfile am57xx-evm-reva3.dtb; fi;" \
		"if test $board_name = am572x_idk && test $idk_lcd = no; then " \
			"setenv fdtfile am572x-idk.dtb; fi;" \
		"if test $board_name = am572x_idk && test $idk_lcd = osd101t2045; then " \
			"setenv fdtfile am572x-idk-lcd-osd.dtb; fi;" \
		"if test $board_name = am572x_idk && test $idk_lcd = osd101t2587; then " \
			"setenv fdtfile am572x-idk-lcd-osd101t2587.dtb; fi;" \
		"if test $board_name = am571x_idk && test $idk_lcd = no; then " \
			"setenv fdtfile am571x-idk.dtb; fi;" \
		"if test $board_name = am571x_idk && test $idk_lcd = osd101t2045; then " \
			"setenv fdtfile am571x-idk-lcd-osd.dtb; fi;" \
		"if test $board_name = am571x_idk && test $idk_lcd = osd101t2587; then " \
			"setenv fdtfile am571x-idk-lcd-osd101t2587.dtb; fi;" \
		"if test $fdtfile = undefined; then " \
			"echo WARNING: Could not determine device tree to use; fi; \0" \
	DFUARGS \
	NETARGS \

#ifndef CONFIG_BOOTARGS_BOARD
#define CONFIG_BOOTARGS_BOARD
#endif
#define CONFIG_BOOTARGS  "androidboot.serialno=${serial#} " \
	CONFIG_BOOTARGS_BOARD

#ifndef CONFIG_FASTBOOT_USB_DEV
#define CONFIG_FASTBOOT_USB_DEV 0
#endif
#define CONFIG_BOOTCOMMAND \
	"if test ${dofastboot} -eq 1; then " \
		"echo Boot fastboot requested, resetting dofastboot ...;" \
		"setenv dofastboot 0; saveenv;" \
		"echo Booting into fastboot ...; " \
		"fastboot " __stringify(CONFIG_FASTBOOT_USB_DEV) "; " \
	"fi;" \
	"if test ${boot_fit} -eq 1; then "	\
		"run update_to_fit;"	\
	"fi;"	\
	"run findfdt; " \
	"run envboot; " \
	"run mmcboot;" \
	"run emmc_android_boot; " \
	""

/*
 * SPL related defines.  The Public RAM memory map the ROM defines the
 * area between 0x40300000 and 0x4031E000 as a download area for OMAP5.
 * On DRA7xx/AM57XX the download area is between 0x40300000 and 0x4037E000.
 * We set CONFIG_SPL_DISPLAY_PRINT to have omap_rev_string() called and
 * print some information.
 */
#ifdef CONFIG_TI_SECURE_DEVICE
/*
 * For memory booting on HS parts, the first 4KB of the internal RAM is
 * reserved for secure world use and the flash loader image is
 * preceded by a secure certificate. The SPL will therefore run in internal
 * RAM from address 0x40301350 (0x40300000+0x1000(reserved)+0x350(cert)).
 */
#define CONFIG_SECURE_BOOT_SRAM 0x1000
#define CONFIG_SPL_TEXT_BASE	0x40301350
/* If no specific start address is specified then the secure EMIF
 * region will be placed at the end of the DDR space. In order to prevent
 * the main u-boot relocation from clobbering that memory and causing a
 * firewall violation, we tell u-boot that memory is protected RAM (PRAM)
 */
#if (CONFIG_TI_SECURE_EMIF_REGION_START == 0)
#define CONFIG_PRAM (CONFIG_TI_SECURE_EMIF_TOTAL_REGION_SIZE) >> 10
#endif
#else
/*
 * For all booting on GP parts, the flash loader image is
 * downloaded into internal RAM at address 0x40300000.
 */
#define CONFIG_SPL_TEXT_BASE	0x40300000
#endif

/* DRA7xx/AM57xx have 512K of SRAM, OMAP5 only 128K */
#define CONFIG_SPL_DISPLAY_PRINT
#define CONFIG_SPL_LDSCRIPT "$(CPUDIR)/omap-common/u-boot-spl.lds"
#define CONFIG_SYS_SPL_ARGS_ADDR	(CONFIG_SYS_SDRAM_BASE + \
					 (128 << 20))

#ifdef CONFIG_NAND
#define CONFIG_SPL_NAND_AM33XX_BCH	/* ELM support */
#endif

/*
 * Disable MMC DM for SPL build and can be re-enabled after adding
 * DM support in SPL
 */
#ifdef CONFIG_SPL_BUILD
#undef CONFIG_DM_MMC
#undef CONFIG_TIMER
#undef CONFIG_DM_ETH
#endif

#define MAX_REMOTECORE_BIN_SIZE (8*0x100000)

/* Define the address to which the IPU1 binary is
 * loaded from persistent storage
 */
#define IPU1_LOAD_ADDR		(0xa0fff000)

/*
 * Set load address  for each core 8 MB after load
 * address for the previous core
 */
#define IPU2_LOAD_ADDR		(IPU1_LOAD_ADDR+MAX_REMOTECORE_BIN_SIZE)
#define DSP1_LOAD_ADDR		(IPU2_LOAD_ADDR+MAX_REMOTECORE_BIN_SIZE)
#define DSP2_LOAD_ADDR		(DSP1_LOAD_ADDR+MAX_REMOTECORE_BIN_SIZE)

/* Define the GPT partition names only when they are used.
 * This prevents warnings of invalid GPT table when loading
 * binaries from FAT partition.
 */
#ifdef CONFIG_LATE_ATTACH_GPT_PART
#define CONFIG_MMC_IPU1_PART_NAME "ipu1"
#define CONFIG_MMC_IPU2_PART_NAME "ipu2"
#define CONFIG_MMC_DSP1_PART_NAME "dsp1"
#define CONFIG_MMC_DSP2_PART_NAME "dsp2"
#endif

/* Error code to indicate that SPL failed to load a remotecore */
#define SPL_CORE_LOAD_ERR_ID (0xFF00)

#endif /* __CONFIG_TI_OMAP5_COMMON_H */
