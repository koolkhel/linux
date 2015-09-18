/*
 *  Board-specific setup code for the AT91SAM9M10G45 Evaluation Kit family
 *
 *  Covers: * AT91SAM9G45-EKES  board
 *          * AT91SAM9M10G45-EK board
 *
 *  Copyright (C) 2009 Atmel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/can/platform/mcp251x.h>
#include <linux/lis3lv02d.h>
#include <linux/spi/spi.h>
#include <linux/fb.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/leds.h>
#include <linux/clk.h>
#include <linux/atmel-mci.h>

#include <linux/platform_data/at91_adc.h>

#include <linux/platform_data/at91_adc.h>

#include <mach/hardware.h>
#include <video/atmel_lcdc.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>


#include <mach/at91sam9_smc.h>
#include <mach/system_rev.h>

#include "at91_aic.h"
#include "at91_shdwc.h"
#include "board.h"
#include "sam9_smc.h"
#include "generic.h"

#include <linux/indigo-gpioperiph.h>
#include <mach/indigo-revisions.h>

static struct gpio_peripheral indigo_all_peripherals [DEVICE_2_2 + 1][INDIGO_PERIPH_COUNT] = {
	{ /* UNKNOWN DEVICE */
	},
	{ /* revision DEVICE_STARTERKIT -- 1 */
	},
	{ /* revision DEVICE_1_0 -- 2 */
	},
	{ /* revision DEVICE_1_1 -- 3 */
	},
	{ /* revision DEVICE_2_0 -- 4 */
	},
	{ /* revision DEVICE_2_1 -- 5 */
		{
			.kind = INDIGO_PERIPH_KIND_GPS,
			.name = "gps",
			.description = "NV08C-CSM GNSS",
			.setup = gps_nv08c_csm_setup,
			.active = false,
			.pins = {
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "NET_ANT",
					.description = "1 соответствует подключению ко входу активной антенны, 0 – Отсутствию",
					.pin_no = AT91_PIN_PA31,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_RESET,
					.schematics_name = "RST_GPS",
					.description = "reset pin, active is low",
					.pin_no = AT91_PIN_PB15,
					.flags = GPIOF_DIR_OUT_INIT_HIGH
				},
				{
					.function = INDIGO_FUNCTION_POWER,
					.schematics_name = "EN_GPS",
					.description = "enable GNSS module",
					.pin_no = AT91_PIN_PD3,
					.flags = GPIOF_DIR_OUT_INIT_LOW | GPIOF_ACTIVE_LOW
				}
			}
		},
		{
			.kind = INDIGO_PERIPH_KIND_POWER,
			.name = "power",
			.description = "BQ24032a",
			.setup = indigo_do_nothing_setup,
			.active = 1,
			.pins = {
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "STAT1",
					.description = "stat 1"/*"Статус режиме работы:precharge in progress(S1-ON, S2-ON), fast charge in progress(S1-ON, S2-OFF), Charge done(S1-OFF, S2-ON),Charge suspend(S1-OFF, S2-OFF)."*/,
					.pin_no = AT91_PIN_PD19,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "STAT2",
					.description = "stat 2"/*"Статус режиме работы:precharge in progress(S1-ON, S2-ON), fast charge in progress(S1-ON, S2-OFF), Charge done(S1-OFF, S2-ON),Charge suspend(S1-OFF, S2-OFF)."*/,
					.pin_no = AT91_PIN_PD11,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "Acpg",
					.description = "состояние внешнего источника питания. 0 - хорошо, 1 - плохо",
					.pin_no = AT91_PIN_PD30,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP | GPIOF_POLLABLE | GPIOF_DEGLITCH
				},
				/* --------------------------- debug pins here ----------------------- */
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT1",
					.description = "",
					.pin_no = AT91_PIN_PC20,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT2",
					.description = "",
					.pin_no = AT91_PIN_PC12,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT3",
					.description = "",
					.pin_no = AT91_PIN_PC22,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT4",
					.description = "",
					.pin_no = AT91_PIN_PC17,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
			}
		},
		{
			.kind = INDIGO_PERIPH_KIND_GSM,
			.name = "gsm",
			.description = "Sim900 GSM",
			.setup = gsm_sim900_setup,
			.active = 1,
			.pins = {
				{
					.function = INDIGO_FUNCTION_STATUS,
					.schematics_name = "STATUS_GSM",
					.description = "Sim900 status pin",
					.pin_no = AT91_PIN_PC31,
					.flags = GPIOF_DIR_IN | GPIOF_DEGLITCH | GPIOF_ACTIVE_HIGH | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_PWRKEY,
					.schematics_name = "PWRkey",
					.description = "Sim900 power key pin",
					.pin_no = AT91_PIN_PD22,
					.flags = GPIOF_DIR_OUT_INIT_LOW | GPIOF_ACTIVE_LOW
				},
				{
					.function = INDIGO_FUNCTION_POWER,
					.schematics_name = "EN_GSM",
					.description = "Sim900 power key pin",
					.pin_no = AT91_PIN_PD21,
					.flags = GPIOF_DIR_OUT_INIT_LOW | GPIOF_ACTIVE_LOW,
				}

			}
		}
	},
	{ /* revision DEVICE_STARTERKIT_9G45 -- 6 */
	},
	{ /* revision DEVICE_2_2 -- 7 */
		{
			.kind = INDIGO_PERIPH_KIND_GPS,
			.name = "gps",
			.description = "NV08C-CSM GNSS",
			.setup = gps_nv08c_csm_setup,
			.active = false,
			.pins = {
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "NET_ANT",
					.description = "1 соответствует подключению ко входу активной антенны, 0 – Отсутствию",
					.pin_no = AT91_PIN_PB3,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_RESET,
					.schematics_name = "RST_GPS",
					.description = "reset pin, active is low",
					.pin_no = AT91_PIN_PA31,
					.flags = GPIOF_DIR_OUT_INIT_HIGH
				},
				{
					.function = INDIGO_FUNCTION_POWER,
					.schematics_name = "EN_GPS",
					.description = "enable GNSS module",
					.pin_no = AT91_PIN_PD3,
					.flags = GPIOF_DIR_OUT_INIT_LOW | GPIOF_ACTIVE_LOW
				}
			}
		},
		{
			.kind = INDIGO_PERIPH_KIND_POWER,
			.name = "power",
			.description = "BQ24032a",
			.setup = indigo_do_nothing_setup,
			.active = false,
			.pins = {
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "STAT1",
					.description = "stat 1"/*"Статус режиме работы:precharge in progress(S1-ON, S2-ON), fast charge in progress(S1-ON, S2-OFF), Charge done(S1-OFF, S2-ON),Charge suspend(S1-OFF, S2-OFF)."*/,
					.pin_no = AT91_PIN_PD19,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "STAT2",
					.description = "stat 2"/*"Статус режиме работы:precharge in progress(S1-ON, S2-ON), fast charge in progress(S1-ON, S2-OFF), Charge done(S1-OFF, S2-ON),Charge suspend(S1-OFF, S2-OFF)."*/,
					.pin_no = AT91_PIN_PD11,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "Acpg",
					.description = "состояние внешнего источника питания. 0 - хорошо, 1 - плохо",
					.pin_no = AT91_PIN_PD30,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP | GPIOF_POLLABLE | GPIOF_DEGLITCH
				},
				/* --------------------------- debug pins here ----------------------- */
			}
		},
		{
			.kind = INDIGO_PERIPH_KIND_GSM,
			.name = "gsm",
			.description = "Sim900 GSM",
			.setup = gsm_sim900_setup,
			.active = false,
			.pins = {
				{
					.function = INDIGO_FUNCTION_STATUS,
					.schematics_name = "STATUS_GSM",
					.description = "Sim900 status pin",
					.pin_no = AT91_PIN_PC7,
					.flags = GPIOF_DIR_IN | GPIOF_DEGLITCH | GPIOF_ACTIVE_HIGH | GPIOF_PULLUP
				},
				{
					.function = INDIGO_FUNCTION_PWRKEY,
					.schematics_name = "PWRkey",
					.description = "Sim900 power key pin",
					.pin_no = AT91_PIN_PC31,
					.flags = GPIOF_DIR_OUT_INIT_LOW | GPIOF_ACTIVE_LOW
				},
				{
					.function = INDIGO_FUNCTION_POWER,
					.schematics_name = "EN_GSM",
					.description = "Sim900 power key pin",
					.pin_no = AT91_PIN_PC27,
					.flags = GPIOF_DIR_OUT_INIT_LOW | GPIOF_ACTIVE_LOW,
				}

			}
		},
		{
			.kind = INDIGO_PERIPH_KIND_UNKNOWN,
			.name = "io",
			.description = "Various IO",
			.setup = indigo_do_nothing_setup,
			.active = true,
			.pins = {
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT1",
					.description = "",
					.pin_no = AT91_PIN_PC20,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT2",
					.description = "",
					.pin_no = AT91_PIN_PC12,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT3",
					.description = "",
					.pin_no = AT91_PIN_PC22,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "OUT4",
					.description = "",
					.pin_no = AT91_PIN_PC17,
					.flags = GPIOF_DIR_OUT_INIT_LOW
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "uC_in1",
					.description = "",
					.pin_no = AT91_PIN_PC25,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP | GPIOF_POLLABLE | GPIOF_DEGLITCH
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "uC_in2",
					.description = "",
					.pin_no = AT91_PIN_PC28,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP | GPIOF_POLLABLE | GPIOF_DEGLITCH
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "uC_in3",
					.description = "",
					.pin_no = AT91_PIN_PC30,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP | GPIOF_POLLABLE | GPIOF_DEGLITCH
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "uC_in4",
					.description = "",
					.pin_no = AT91_PIN_PC29,
					.flags = GPIOF_DIR_IN | GPIOF_PULLUP | GPIOF_POLLABLE | GPIOF_DEGLITCH
				},
				{
					.function = INDIGO_FUNCTION_NO_FUNCTION,
					.schematics_name = "POWER_ON_USB",
					.description = "",
					.pin_no = AT91_PIN_PA9,
					.flags = GPIOF_DIR_OUT_INIT_HIGH
				},
			}
		}
	}
};

static void __init ek_init_early(void)
{
	/* Initialize processor: 12.000 MHz crystal */
	at91_initialize(12000000);
}

/*
 * USB HS Host port (common to OHCI & EHCI)
 */
static struct at91_usbh_data __initdata ek_usbh_hs_data = {
	.ports		= 2,
	.vbus_pin	= {-EINVAL, -EINVAL},
	.overcurrent_pin= {-EINVAL, -EINVAL},
};

int setup_can_device(struct spi_device *device);

static struct mcp251x_platform_data mcp251x_info = {
	.oscillator_frequency = 8 * 1000 * 1000,
	.irq_flags            = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
	.board_specific_setup = setup_can_device,
	.transceiver_enable   = NULL,
	.power_enable         = NULL,
};

/*
 * SPI devices.
 */
static struct spi_board_info ek_spi_devices[] = {
	{	/* CAN */
		.modalias	= "mcp2515",
		.chip_select	= 0,
		.max_speed_hz	= 8 * 1000 * 1000,
		.bus_num	= 1,
		.platform_data  = &mcp251x_info,
		.irq = 0
	},
	{
		.modalias      = "l3gd20", // gyro
		.chip_select   = 1,
		.max_speed_hz  = 8 * 1000 * 1000,
		.bus_num       = 1,
		.platform_data = 0,
		.irq           = 0,
	},
	{
		.modalias      = "lis3lv02d_spi", // accelerometer
		.chip_select   = 2,
		.max_speed_hz  = 8 * 1000 * 1000,
		.bus_num       = 1,
		.platform_data = 0,
		.irq           = 0,
	}
};

void setup_spi_devices(void)
{
	int pin = -1;
	// CAN
	ek_spi_devices[0].irq = gpio_to_irq(AT91_PIN_PA26);

	// GYRO
	pin = AT91_PIN_PB1;
	ek_spi_devices[1].irq = gpio_to_irq(pin);
	if (gpio_request(pin, "L3GD20 INT")) {
		printk(KERN_ERR "L3GD20 INT failed to request INT\n");
		BUG();
	}

	gpio_direction_input(pin);
	at91_set_deglitch(pin, 1);

	// ACCELEROMETER
	pin = AT91_PIN_PA22;
	ek_spi_devices[2].irq = gpio_to_irq(pin);
	if (gpio_request(pin, "lis3lv02d INT")) {
		printk(KERN_ERR "lis3lv02d failed to request INT\n");
		BUG();
	}

	gpio_direction_input(pin);
	at91_set_deglitch(pin, 1);
}


/* called before probing CAN chip */
int setup_can_device(struct spi_device *device)
{
	int reset_pin = -1;
	int interrupt_pin = -1;
	int phy_mode_pin = -1;
	int phy_shutdown = -1;

	switch (indigo_revision) {
	case DEVICE_2_1:
		ek_spi_devices[0].bus_num = 0;
		reset_pin = AT91_PIN_PA27;
		interrupt_pin = AT91_PIN_PD12;
		phy_mode_pin = AT91_PIN_PA18;
		break;
	case DEVICE_2_2:
		ek_spi_devices[0].bus_num = 1;
		reset_pin = AT91_PIN_PA28;
		interrupt_pin = AT91_PIN_PA26;
		phy_mode_pin = AT91_PIN_PA29;
		phy_shutdown = AT91_PIN_PB2;
		break;
	default:
		break;
	}

	if (gpio_request(reset_pin, "MCP251x RESET")) {
		printk(KERN_ERR "MAC251x failed to request RESET\n");
		return -EBUSY;
	}

	/* set reset pin to inactive state */
	gpio_direction_output(reset_pin, 1);

	if (gpio_request(phy_mode_pin, "MAX3051 RS")) {
		printk(KERN_ERR "MAX3051 failed to request MODE\n");
		return -EBUSY;
	}

	/* enable phy highspeed operation */
	gpio_direction_output(phy_mode_pin, 0);

	if (gpio_request(interrupt_pin, "MCP251x INT")) {
		printk(KERN_ERR "MAC251x failed to request INT\n");
		return -EBUSY;
	}

	gpio_direction_input(interrupt_pin);
	at91_set_deglitch(interrupt_pin, 1);


	if (gpio_request(phy_shutdown, "CAN PHY shutdown pin")) {
		printk(KERN_ERR "CAN PHY shutdown pin failed\n");
		return -EBUSY;
	} else {
		/* set reset pin to inactive state */
		gpio_direction_output(phy_shutdown, 0);
	}

	return 0;
}


/*
 * MCI (SD/MMC)
 */
static struct mci_platform_data __initdata mci0_data = {
	.slot[0] = {
		.bus_width	= 4,
		.detect_pin	= AT91_PIN_PB0,
		.wp_pin		= -EINVAL,
	},
};


/*
 * NAND flash
 */
static struct mtd_partition __initdata ek_nand_partition[] = {
	{
		.name	= "System Images",
		.offset	= 0,
		.size	= SZ_32M,
	},
	{
		.name	= "Root partitions",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
	#if 0
	{
		.name	= "Volatile data",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
	#endif
};

/* det_pin is not connected */
static struct atmel_nand_data __initdata ek_nand_data = {
	.ale		= 21, // не совсем понял, зачем нужны
	.cle		= 22,
	.rdy_pin	= AT91_PIN_PC8,
	.enable_pin	= AT91_PIN_PC14,
	.det_pin	= -EINVAL,
	.ecc_mode       = NAND_ECC_SOFT,
	.on_flash_bbt   = 0,
	.parts		= ek_nand_partition,
	.num_parts	= ARRAY_SIZE(ek_nand_partition),
};

/* timings for Samsung K9F2G08U0C 2Gbit */
static struct sam9_smc_config __initdata ek_nand_smc_config = {
	.ncs_read_setup		= 0,
	.nrd_setup		= 1,
	.ncs_write_setup	= 0,
	.nwe_setup		= 1,

	.ncs_read_pulse		= 2,
	.nrd_pulse		= 2,
	.ncs_write_pulse	= 4,
	.nwe_pulse		= 2,

	.read_cycle		= 4,
	.write_cycle		= 4,

	.mode			= AT91_SMC_READMODE | AT91_SMC_WRITEMODE | AT91_SMC_EXNWMODE_DISABLE,
	.tdf_cycles		= 2,
};

static void __init ek_add_device_nand(void)
{
	ek_nand_smc_config.mode |= AT91_SMC_DBW_8;

	/* configure chip-select 3 (NAND) */
	sam9_smc_configure(0, 3, &ek_nand_smc_config);
	at91_add_device_nand(&ek_nand_data);
}


/*
 * LCD Controller
 */
#if defined(CONFIG_FB_ATMEL) || defined(CONFIG_FB_ATMEL_MODULE)
static struct fb_videomode at91_tft_vga_modes[] = {
	{
		.name           = "SK",
		.refresh	= 60,
		.xres		= 800,		.yres		= 480,
		.pixclock	= KHZ2PICOS(29232),// (800 + 40 + 40 + 48) * (480 + 29 + 13 + 3) * 60

		.left_margin	= 40,		.right_margin	= 40,
		.upper_margin	= 29,		.lower_margin	= 13,
		.hsync_len	= 48,		.vsync_len	= 3,

		.sync		= 0,
		.vmode		= FB_VMODE_NONINTERLACED,
	},
};

static struct fb_monspecs at91fb_default_monspecs = {
	.manufacturer	= "Ingo",
	.monitor        = "Dev2.1DS",

	.modedb		= at91_tft_vga_modes,
	.modedb_len	= ARRAY_SIZE(at91_tft_vga_modes),
	.hfmin		= 15000,
	.hfmax		= 64000,
	.vfmin		= 50,
	.vfmax		= 150,
};

#define AT91SAM9G45_DEFAULT_LCDCON2 	(ATMEL_LCDC_MEMOR_LITTLE \
					| ATMEL_LCDC_DISTYPE_TFT	\
					| ATMEL_LCDC_CLKMOD_ALWAYSACTIVE)

/* Driver datas */
static struct atmel_lcdfb_info __initdata ek_lcdc_data = {
	.lcdcon_is_backlight		= true,
//	.default_bpp			= 32,
	.default_bpp			= 16,
	.default_dmacon			= ATMEL_LCDC_DMAEN,
	.default_lcdcon2		= AT91SAM9G45_DEFAULT_LCDCON2,
	.default_monspecs		= &at91fb_default_monspecs,
	.guard_time			= 15,
	.lcd_wiring_mode		= ATMEL_LCDC_WIRING_RGB,
	.smem_len			= 0,
};

#else
static struct atmel_lcdfb_info __initdata ek_lcdc_data;
#endif

/*
 * ADCs
 */
static struct at91_adc_data ek_adc_data = {
	.channels_used = BIT(4) | BIT(5) | BIT(6) | BIT(7),
	.use_external_triggers = true,
	.vref = 3300,
};

/*
 * GPIO Buttons
 */
#if defined(CONFIG_KEYBOARD_GPIO) || defined(CONFIG_KEYBOARD_GPIO_MODULE)
static struct gpio_keys_button ek_buttons[] = {
	{
		.code		= KEY_LEFT,
		.gpio		= AT91_PIN_PA23,
		.desc		= "KEY1",
		.debounce_interval = 50,
	},
	{
		.code		= KEY_RIGHT,
		.gpio		= AT91_PIN_PD2,
		.desc		= "KEY2",
		.debounce_interval = 50,
	},
	{
		.code		= KEY_UP,
		.gpio		= AT91_PIN_PA24,
		.desc		= "KEY3",
		.debounce_interval = 50,
	},
	{
		.code		= KEY_DOWN,
		.gpio		= AT91_PIN_PA26,
		.desc		= "KEY4",
		.debounce_interval = 50,
	},
	{
		.code		= KEY_ENTER,
		.gpio		= AT91_PIN_PE31,
		.desc		= "KEY5",
		.debounce_interval = 50,
	},
	{	/* BP1, "leftclick" */
		.code		= KEY_ESC,
		.gpio		= AT91_PIN_PB14,
		.desc		= "KEY6",
		.debounce_interval = 50,
	},

};

static void setup_keys(void) {
	switch (indigo_revision) {
	case DEVICE_2_2:
		ek_buttons[4].code = KEY_ENTER;
		ek_buttons[4].gpio = AT91_PIN_PA11;

		ek_buttons[5].code = KEY_ESC;
		ek_buttons[5].gpio = AT91_PIN_PA18;

		ek_buttons[3].code = KEY_DOWN;
		ek_buttons[3].gpio = AT91_PIN_PD14;

		ek_buttons[1].code = KEY_RIGHT;
		ek_buttons[1].gpio = AT91_PIN_PD10;

		ek_buttons[2].code = KEY_UP;
		ek_buttons[2].gpio = AT91_PIN_PB25;

		ek_buttons[0].code = KEY_LEFT;
		ek_buttons[0].gpio = AT91_PIN_PB28;
		break;
	default:
		break;
	}
}

static struct gpio_keys_platform_data ek_button_data = {
	.buttons	= ek_buttons,
	.nbuttons	= ARRAY_SIZE(ek_buttons),
};

static struct platform_device ek_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &ek_button_data,
	}
};

static void __init ek_add_device_buttons(void)
{
	int i;

	if (indigo_revision == DEVICE_2_1) {
		for (i = 0; i < ARRAY_SIZE(ek_buttons); i++) {
			/* на 2.1 кнопки физически подтянуты вниз, не делаем pullup */
			at91_set_GPIO_periph(ek_buttons[i].gpio, 0);
			at91_set_deglitch(ek_buttons[i].gpio, 1);
		}
	} else {
		// а на 2.2 вверх
	}

	platform_device_register(&ek_button_device);
}
#else
static void __init ek_add_device_buttons(void) {}
#endif

static struct gpio_led ek_leds[] = {
        {       /* "are we still alive" */
                .name                   = "alive",
                .gpio                   = AT91_PIN_PA27,
                .default_trigger        = "heartbeat",
        },
};

/*
 * AC97
 */
static struct ac97c_platform_data ek_ac97_data = {
	.reset_pin	= AT91_PIN_PC21,
};

static unsigned long int device_mask(int kind) {
	unsigned long int result = 0;
	if (kind == 0) {
		printk(KERN_ERR "this would surely lead to memory corruption\n");
		BUG();
	} else if (kind > 0 && kind < 31) {
		set_bit(32 - kind, &result);
	}
	return result;
}

static int find_device_by_kind(enum indigo_gpioperiph_kind_t kind, struct gpio_peripheral peripherals[3]) {
	int i = 0;
	int index = -1;
	for (i = 0; i < 3; i++) {
		if (peripherals[i].kind == kind) {
			index = i;
			goto out;
		}
	}

	printk(KERN_ERR "couldn't find kind %d\n", kind);
	BUG();

out:
	return index;

}

enum general_peripherals {
	p_usb = 1 << 16,
	p_spi = 1 << 17,
	p_mci = 1 << 18,
	p_lcd = 1 << 19,
	p_ac97 = 1 << 20,
	p_rs485 = 1 << 21,
};

static int setup_rs485(void) {
	int receiver_enable_pin = -1;
	int driver_enable_pin = -1;

	switch (indigo_revision) {
	case DEVICE_2_1:
		receiver_enable_pin = AT91_PIN_PA22;

		/*
		   DE is part of corresponding UART
		   driver_enable_pin = AT91_PIN_PA23;
		*/
		break;
	case DEVICE_2_2:
		//receiver_enable_pin = AT91_PIN_PA24;
		//driver_enable_pin   = AT91_PIN_PA23;
	default:
		break;
	}

	if (receiver_enable_pin != -1) {
		if (gpio_request(receiver_enable_pin, "ADM3485 RE")) {
			printk(KERN_ERR "ADM3485 failed to request RE\n");
			return -EBUSY;
		}

		/* set reset pin to inactive state */
		gpio_direction_output(receiver_enable_pin, 0);
	}

	if (driver_enable_pin != -1) {
		if (gpio_request(driver_enable_pin, "ADM3485 DE")) {
			printk(KERN_ERR "ADM3485 failed to request DE\n");
			return -EBUSY;
		}

		gpio_direction_output(driver_enable_pin, 1);
	}

	return 0;
}

void handle_device_versions(void)
{
	switch (indigo_revision) {
	case DEVICE_2_1:
		break;
	case DEVICE_2_2:
		break;
	}
}

static void __init ek_board_init(void)
{
	int kind = 0, j = 0;

	handle_device_versions();

	/* DGBU on ttyS0. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* ttyS1 == GSM */
	at91_register_uart(AT91SAM9G45_ID_US1, 1, ATMEL_UART_CTS | ATMEL_UART_RTS);

	/* ttyS2 == GPS */
	at91_register_uart(AT91SAM9G45_ID_US2, 2, 0);

	/* ttyS3 = external RS232 */
	at91_register_uart(AT91SAM9G45_ID_US0, 3, 0);

	/* ttyS4 == external RS485 */
	at91_register_uart(AT91SAM9G45_ID_US3, 4, ATMEL_UART_RTS);

	/* Serial */
	at91_add_device_serial();

	if (indigo_hardware & p_rs485)
		setup_rs485();
	else
		printk(KERN_INFO "not enabling RS-485 transceiver, fix system_rev\n");

	/* USB HS Host */
	if (indigo_hardware & p_usb) {
		at91_add_device_usbh_ehci(&ek_usbh_hs_data);
		at91_add_device_usbh_ohci(&ek_usbh_hs_data);
	} else
		printk(KERN_INFO "not enabling USB, fix system_rev\n");

        /* SPI */
	setup_spi_devices();
	if (indigo_hardware & p_spi)
		at91_add_device_spi(ek_spi_devices, ARRAY_SIZE(ek_spi_devices));
	else
		printk(KERN_INFO "not enabling SPI (CAN, etc), fix system rev\n");

        /* MMC */
	if (indigo_hardware & p_mci)
		at91_add_device_mci(0, &mci0_data);
	else
		printk(KERN_INFO "not enabling MMC, fix system rev\n");
	/* NAND */
	ek_add_device_nand();
	/* LCD Controller */
	if (indigo_hardware & p_lcd) {
		at91_add_device_lcdc(&ek_lcdc_data);
	} else
		printk(KERN_INFO "not enabling LCD, fix system rev\n");

	/* FIXME conditional enabling */
	/* ADC */
	at91_add_device_adc(&ek_adc_data);

        /* Push Buttons */
	setup_keys();
	ek_add_device_buttons();

        /* LEDs */
	at91_gpio_leds(ek_leds, ARRAY_SIZE(ek_leds));
	/* AC97 */
	if (indigo_hardware & p_ac97)
		at91_add_device_ac97(&ek_ac97_data);
	else
		printk(KERN_INFO "not enabling AC97, fix system rev\n");

#ifdef CONFIG_INDIGO_GPIO_PERIPH
	for (kind = 1; kind < 4; kind++) { /* 1, 2, 3 literally here */
		if (indigo_hardware & device_mask(kind)) {
			j = find_device_by_kind(kind, indigo_all_peripherals[indigo_revision]);
			if (j != -1)
				indigo_all_peripherals[indigo_revision][j].active = true;
			else
				printk(KERN_ERR "unknown periph kind: %d\n", kind);

		}
	}
	indigo_gpio_peripheral_init(indigo_all_peripherals[indigo_revision]);
#endif

}

MACHINE_START(AT91SAM9G45EKES, "Indigo Tracker 2.1")
	/* Maintainer: Atmel */
	.timer		= &at91sam926x_timer,
	.map_io		= at91_map_io,
	.handle_irq	= at91_aic_handle_irq,
	.init_early	= ek_init_early,
	.init_irq	= at91_init_irq_default,
	.init_machine	= ek_board_init,
MACHINE_END
