/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *
 * Aneesh V <aneesh@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <dm.h>
#include <spl.h>
#include <linux/compiler.h>
#include <errno.h>
#include <asm/u-boot.h>
#include <errno.h>
#include <mmc.h>
#include <image.h>
#ifdef CONFIG_SPL_ANDROID_BOOT_SUPPORT
#include <fdt_support.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

static int mmc_load_legacy(struct mmc *mmc, ulong sector,
			   struct image_header *header, bool *print_err)
{
	u32 image_size_sectors;
	unsigned long count;

	if (print_err)
		*print_err = true;

	spl_parse_image_header(header);
	/* convert size to sectors - round up */
	image_size_sectors = (spl_image.size + mmc->read_bl_len - 1) /
			     mmc->read_bl_len;

	/* Read the header too to avoid extra memcpy */
	count = mmc->block_dev.block_read(&mmc->block_dev, sector,
					  image_size_sectors,
					  (void *)(ulong)spl_image.load_addr);
	debug("read %x sectors to %x\n", image_size_sectors,
	      spl_image.load_addr);

#ifndef CONFIG_SPL_ANDROID_BOOT_SUPPORT
	if (count != image_size_sectors)
		return -EIO;
#else
	if (count == 0)
		return -1;

	/* load the ramdisk if this is an Android image */
	if (genimg_get_format(header) == IMAGE_FORMAT_ANDROID) {
		ulong ramdisk_start, ramdisk_len, ramdisk_load;
		u32 sector_ramdisk;
		int err;
		const char *s;

		printf("Loading Android ramdisk\n");

		android_image_get_ramdisk((const struct andr_img_hdr *)header,
					  &ramdisk_start, &ramdisk_len);
		ramdisk_load = android_image_get_rload(
					(const struct andr_img_hdr *)header);
		sector_ramdisk = sector +
					((ramdisk_start - (ulong)header) /
						mmc->block_dev.blksz);
		image_size_sectors = (ramdisk_len + mmc->read_bl_len - 1) /
					mmc->read_bl_len;

		count = mmc->block_dev.block_read(&mmc->block_dev,
				sector_ramdisk, image_size_sectors,
				(void *)ramdisk_load);
		if (count == 0)
			return -2;

		err = fdt_shrink_to_minimum((void *)CONFIG_SYS_SPL_ARGS_ADDR);
		if (err == 0) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
			printf("spl: fdt_shrink_to_minimum err - %d\n", err);
#endif
			if (print_err)
				*print_err = false;
			return -3;
		}
		err = fdt_chosen((void *)CONFIG_SYS_SPL_ARGS_ADDR);
		if (err != 0) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
			printf("spl: fdt_chosen err - %d\n", err);
#endif
			if (print_err)
				*print_err = false;
			return -4;
		}
		err = fdt_initrd((void *)CONFIG_SYS_SPL_ARGS_ADDR, ramdisk_load,
				 ramdisk_load + ramdisk_len);
		if (err != 0) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
			printf("spl: fdt_initrd err - %d\n", err);
#endif
			if (print_err)
				*print_err = false;
			return -5;
		}

		s = getenv("serial#");
		if (s) {
			static char data[512];
			int  nodeoffset, len;

			printf("serial# is %s\n", s);
			nodeoffset = fdt_path_offset((void *)CONFIG_SYS_SPL_ARGS_ADDR, "/chosen");
			if (nodeoffset < 0) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
				printf ("spl: fdt_path_offset err - %d\n", err);
#endif
				if (print_err)
					*print_err = false;
				return -6;
			}

			sprintf(data, "androidboot.serialno=%s ", s);
			len = strlen(data) + 1;

			err = fdt_setprop((void *)CONFIG_SYS_SPL_ARGS_ADDR,
					  nodeoffset, "bootargs", data, len);
			if (err != 0) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
				printf ("spl: fdt_setprop err - %d\n", err);
#endif
				if (print_err)
					*print_err = false;
				return -7;
			}
		}
	}
#endif

	return 0;
}

static ulong h_spl_load_read(struct spl_load_info *load, ulong sector,
			     ulong count, void *buf)
{
	struct mmc *mmc = load->dev;

	return mmc->block_dev.block_read(&mmc->block_dev, sector, count, buf);
}

static int mmc_load_image_raw_sector(struct mmc *mmc, unsigned long sector)
{
	unsigned long count;
	struct image_header *header;
	int ret = 0;
	bool print_err = true;

	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE -
					 sizeof(struct image_header));

	/* read image header to find the image size & load address */
	count = mmc->block_dev.block_read(&mmc->block_dev, sector, 1, header);
	debug("hdr read sector %lx, count=%lu\n", sector, count);
	if (count == 0) {
		ret = -EIO;
		goto end;
	}

	if (IS_ENABLED(CONFIG_SPL_LOAD_FIT) &&
	    image_get_magic(header) == FDT_MAGIC) {
		struct spl_load_info load;

		debug("Found FIT\n");
		load.dev = mmc;
		load.priv = NULL;
		load.filename = NULL;
		load.bl_len = mmc->read_bl_len;
		load.read = h_spl_load_read;
		ret = spl_load_simple_fit(&load, sector, header);
	} else {
#ifdef CONFIG_SPL_ANDROID_BOOT_SUPPORT
		if (genimg_get_format(header) == IMAGE_FORMAT_ANDROID)
			printf("Loading Android image\n");
#endif
		ret = mmc_load_legacy(mmc, sector, header, &print_err);
	}

end:
	if (ret) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		if (print_err)
			puts("mmc_load_image_raw_sector: mmc block read error"
					"\n");
#endif
		return -1;
	}

	return 0;
}

int spl_mmc_get_device_index(u32 boot_device)
{
	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		return 0;
	case BOOT_DEVICE_MMC2:
	case BOOT_DEVICE_MMC2_2:
		return 1;
	}

#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
	printf("spl: unsupported mmc boot device.\n");
#endif

	return -ENODEV;
}

static int spl_mmc_find_device(struct mmc **mmcp, u32 boot_device)
{
#ifdef CONFIG_DM_MMC
	struct udevice *dev;
#endif
	int err, mmc_dev;

	mmc_dev = spl_mmc_get_device_index(boot_device);
	if (mmc_dev < 0)
		return mmc_dev;

	err = mmc_initialize(NULL);
	if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		printf("spl: could not initialize mmc. error: %d\n", err);
#endif
		return err;
	}

#ifdef CONFIG_DM_MMC
	err = uclass_get_device(UCLASS_MMC, mmc_dev, &dev);
	if (!err)
		*mmcp = mmc_get_mmc_dev(dev);
#else
	*mmcp = find_mmc_device(mmc_dev);
	err = *mmcp ? 0 : -ENODEV;
#endif
	if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		printf("spl: could not find mmc device. error: %d\n", err);
#endif
		return err;
	}

	return 0;
}

#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION
static int mmc_load_image_raw_partition(struct mmc *mmc, int partition)
{
	disk_partition_t info;
	int err;

	err = part_get_info(&mmc->block_dev, partition, &info);
	if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		puts("spl: partition error\n");
#endif
		return -1;
	}

#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR
	return mmc_load_image_raw_sector(mmc, info.start +
					 CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR);
#else
	return mmc_load_image_raw_sector(mmc, info.start);
#endif
}
#else
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION -1
static int mmc_load_image_raw_partition(struct mmc *mmc, int partition)
{
	return -ENOSYS;
}
#endif

#ifdef CONFIG_SPL_OS_BOOT
static int mmc_load_image_raw_os(struct mmc *mmc)
{
	unsigned long count;
	int ret;
	lbaint_t sector, num_sectors;
#if defined(CONFIG_SPL_MMC_DTB_NAME) || defined(CONFIG_SPL_MMC_KERNEL_NAME)
	disk_partition_t info;
#endif

#ifdef CONFIG_SPL_MMC_DTB_NAME
	ret = part_get_info_efi_by_name(&mmc->block_dev,
					     CONFIG_SPL_MMC_DTB_NAME,
					     &info);
	if (ret) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		printf("cannot find partition: '%s'\n",
		       CONFIG_SPL_MMC_DTB_NAME);
#endif
		return -1;
	}

	sector = info.start;
	num_sectors = info.size;
#else
	sector = CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR;
	num_sectors = CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR;
#endif

	count = mmc->block_dev.block_read(&mmc->block_dev, sector, num_sectors,
		(void *) CONFIG_SYS_SPL_ARGS_ADDR);
	if (count == 0) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		puts("mmc_load_image_raw_os: mmc block read error\n");
#endif
		return -1;
	}

#ifdef CONFIG_SPL_MMC_KERNEL_NAME
	ret = part_get_info_efi_by_name(&mmc->block_dev,
					     CONFIG_SPL_MMC_KERNEL_NAME,
					     &info);
	if (ret) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		printf("cannot find partition: '%s'\n",
		       CONFIG_SPL_MMC_KERNEL_NAME);
#endif
		return -1;
	}

	sector = info.start;
#else
	sector = CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR;
#endif
	ret = mmc_load_image_raw_sector(mmc, sector);
	if (ret)
		return ret;

	if (spl_image.os != IH_OS_LINUX) {
		puts("Expected Linux image is not found. Trying to start U-boot\n");
		return -ENOENT;
	}

	return 0;
}
#else
int spl_start_uboot(void)
{
	return 1;
}
static int mmc_load_image_raw_os(struct mmc *mmc)
{
	return -ENOSYS;
}
#endif

#ifdef CONFIG_SYS_MMCSD_FS_BOOT_PARTITION
int spl_mmc_do_fs_boot(struct mmc *mmc)
{
	int err = -ENOSYS;

#ifdef CONFIG_SPL_FAT_SUPPORT
	if (!spl_start_uboot()) {
		err = spl_load_image_fat_os(&mmc->block_dev,
			CONFIG_SYS_MMCSD_FS_BOOT_PARTITION);
		if (!err)
			return err;
	}
#ifdef CONFIG_SPL_FS_LOAD_PAYLOAD_NAME
	err = spl_load_image_fat(&mmc->block_dev,
				 CONFIG_SYS_MMCSD_FS_BOOT_PARTITION,
				 CONFIG_SPL_FS_LOAD_PAYLOAD_NAME);
	if (!err)
		return err;
#endif
#endif
#ifdef CONFIG_SPL_EXT_SUPPORT
	if (!spl_start_uboot()) {
		err = spl_load_image_ext_os(&mmc->block_dev,
			CONFIG_SYS_MMCSD_FS_BOOT_PARTITION);
		if (!err)
			return err;
	}
#ifdef CONFIG_SPL_FS_LOAD_PAYLOAD_NAME
	err = spl_load_image_ext(&mmc->block_dev,
				 CONFIG_SYS_MMCSD_FS_BOOT_PARTITION,
				 CONFIG_SPL_FS_LOAD_PAYLOAD_NAME);
	if (!err)
		return err;
#endif
#endif

#if defined(CONFIG_SPL_FAT_SUPPORT) || defined(CONFIG_SPL_EXT_SUPPORT)
	err = -ENOENT;
#endif

	return err;
}
#else
int spl_mmc_do_fs_boot(struct mmc *mmc)
{
	return -ENOSYS;
}
#endif

int spl_mmc_init(struct mmc **mmc, u32 boot_device)
{
	int err = 0;

	if (boot_device == UINT_MAX)
		boot_device = spl_boot_device();

	err = spl_mmc_find_device(mmc, boot_device);
	if (err)
		return err;

	err = mmc_init(*mmc);
	if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		printf("spl: mmc init failed with error: %d\n", err);
#endif
		return err;
	}

	return 0;
}

int spl_mmc_load_image(u32 boot_device)
{
	int err = 0;
	__maybe_unused int part;
	struct mmc *mmc = NULL;
	u32 boot_mode;

	(void)boot_device; /* unused */
	err = spl_mmc_init(&mmc, boot_device);
	if (err)
		return err;

	boot_mode = spl_boot_mode();
	err = -EINVAL;
	switch (boot_mode) {
	case MMCSD_MODE_EMMCBOOT:
			/*
			 * We need to check what the partition is configured to.
			 * 1 and 2 match up to boot0 / boot1 and 7 is user data
			 * which is the first physical partition (0).
			 */
			part = (mmc->part_config >> 3) & PART_ACCESS_MASK;

			if (part == 7)
				part = 0;

			err = mmc_switch_part(0, part);
			if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
				puts("spl: mmc partition switch failed\n");
#endif
				return err;
			}
			/* Fall through */
	case MMCSD_MODE_RAW:
		debug("spl: mmc boot mode: raw\n");

		if (!spl_start_uboot()) {
			err = mmc_load_image_raw_os(mmc);
			if (!err)
				return err;
		}

		err = mmc_load_image_raw_partition(mmc,
			CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION);
		if (!err)
			return err;
#if defined(CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR)
		err = mmc_load_image_raw_sector(mmc,
			CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR);
		if (!err)
			return err;
#endif
		/* If RAW mode fails, try FS mode. */
	case MMCSD_MODE_FS:
		debug("spl: mmc boot mode: fs\n");

		err = spl_mmc_do_fs_boot(mmc);
		if (!err)
			return err;

		break;
	case MMCSD_MODE_UNDEFINED:
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
	default:
		puts("spl: mmc: wrong boot mode\n");
#endif
	}

	return err;
}
