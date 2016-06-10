/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *
 * Ravi B <ravibabu@ti.com>
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
#include <watchdog.h>
#include <console.h>
#include <g_dnl.h>
#include <usb.h>
#include <dfu.h>
#include <environment.h>

static int run_dfu(int usb_index, char *interface, char *devstring)
{
	int ret, i = 0;

	ret = dfu_init_env_entities(interface, devstring);
	if (ret)
		goto done;

	ret = CMD_RET_SUCCESS;

	board_usb_init(usb_index, USB_INIT_DEVICE);
	g_dnl_clear_detach();
	g_dnl_register("usb_dnl_dfu");

	while (1) {
		if (g_dnl_detach()) {
			/*
			 * Check if USB bus reset is performed after detach,
			 * which indicates that -R switch has been passed to
			 * dfu-util. In this case reboot the device
			 */
			if (dfu_usb_get_reset())
				goto exit;

			/*
			 * This extra number of usb_gadget_handle_interrupts()
			 * calls is necessary to assure correct transmission
			 * completion with dfu-util
			 */
			if (++i == 10000)
				goto exit;
		}
		if (ctrlc())
			goto exit;

		if (dfu_get_defer_flush()) {
			/*
			 * Call to usb_gadget_handle_interrupts() is necessary
			 * to act on ZLP OUT transaction from HOST PC after
			 * transmitting the whole file.
			 *
			 * If this ZLP OUT packet is NAK'ed, the HOST libusb
			 * function fails after timeout (by default it is set to
			 * 5 seconds). In such situation the dfu-util program
			 * exits with error message.
			 */
			usb_gadget_handle_interrupts(usb_index);
			ret = dfu_flush(dfu_get_defer_flush(), NULL, 0, 0);
			dfu_set_defer_flush(NULL);
			if (ret) {
				error("Deferred dfu_flush() failed!");
				goto exit;
			}
		}

		WATCHDOG_RESET();
		usb_gadget_handle_interrupts(usb_index);
	}
exit:
	g_dnl_unregister();
	board_usb_cleanup(usb_index, USB_INIT_DEVICE);
done:
	dfu_free_entities();
	g_dnl_clear_detach();

	return ret;
}

int spl_dfu_cmd(int usbctrl, char *dfu_alt_info, char *interface, char *devstr)
{
	char *str_env;
	int ret;

	/* set default environment */
	set_default_env(0);
	str_env = getenv(dfu_alt_info);
	if (!str_env) {
		error("\"dfu_alt_info\" env variable not defined!\n");
		return -EINVAL;
	}

	ret = setenv("dfu_alt_info", str_env);
	if (ret) {
		error("unable to set env variable \"dfu_alt_info\"!\n");
		return -EINVAL;
	}

	/* invoke dfu command */
	return run_dfu(usbctrl, interface, devstr);
}
