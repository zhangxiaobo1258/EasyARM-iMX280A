/*
 * Freescale MX28EVK board
 *
 * (C) Copyright 2011 Freescale Semiconductor, Inc.
 *
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
 *
 * Based on m28evk.c:
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux-mx28.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <linux/mii.h>
#include <miiphy.h>
#include <netdev.h>
#include <errno.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Functions
 */
int board_early_init_f(void)
{
	/* IO0 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK0, 480000);
	/* IO1 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK1, 480000);

	/* SSP0 clock at 96MHz */
	mxs_set_sspclk(MXC_SSPCLK0, 96000, 0);
	/* SSP2 clock at 160MHz */
	mxs_set_sspclk(MXC_SSPCLK2, 160000, 0);

#ifdef	CONFIG_CMD_USB
	mxs_iomux_setup_pad(MX28_PAD_SSP2_SS1__USB1_OVERCURRENT);
	mxs_iomux_setup_pad(MX28_PAD_AUART2_RX__GPIO_3_8 |
			MXS_PAD_4MA | MXS_PAD_3V3 | MXS_PAD_NOPULL);
	gpio_direction_output(MX28_PAD_AUART2_RX__GPIO_3_8, 1);
#endif

	/* Power on LCD */
	/*gpio_direction_output(MX28_PAD_LCD_RESET__GPIO_3_30, 1);*/

	/* Set contrast to maximum */
	/*gpio_direction_output(MX28_PAD_PWM2__GPIO_3_18, 1);*/

	return 0;
}

int dram_init(void)
{
	return mxs_dram_init();
}

int board_init(void)
{
	/* Adress of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

#ifdef	CONFIG_CMD_MMC
static int mx28evk_mmc_wp(int id)
{
	if (id != 0) {
		printf("MXS MMC: Invalid card selected (card id = %d)\n", id);
		return 1;
	}

	return gpio_get_value(MX28_PAD_GPMI_CE1N__GPIO_0_17);
}

int board_mmc_init(bd_t *bis)
{
	/* Configure WP as input */
	/*gpio_direction_input(MX28_PAD_SSP1_SCK__GPIO_2_12);*/
	mxs_iomux_setup_pad(MX28_PAD_SSP0_DATA0__SSP0_D0 | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_SSP0_DATA1__SSP0_D1 | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_SSP0_DATA2__SSP0_D2 | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_SSP0_DATA3__SSP0_D3 | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_SSP0_CMD__SSP0_CMD| MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_SSP0_DETECT__SSP0_CARD_DETECT| MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_NOPULL);
	mxs_iomux_setup_pad(MX28_PAD_SSP0_SCK__SSP0_SCK| MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_NOPULL);
	mxs_iomux_setup_pad(MX28_PAD_GPMI_CE1N__GPIO_0_17);
	gpio_direction_input(MX28_PAD_GPMI_CE1N__GPIO_0_17);
	/* Configure MMC0 Power Enable */
	/*gpio_direction_output(MX28_PAD_PWM3__GPIO_3_28, 0);*/

	return mxsmmc_initialize(bis, 0, mx28evk_mmc_wp, NULL);
}
#endif

#ifdef	CONFIG_CMD_NET

int board_eth_init(bd_t *bis)
{
	struct mxs_clkctrl_regs *clkctrl_regs =
		(struct mxs_clkctrl_regs *)MXS_CLKCTRL_BASE;
	struct eth_device *dev;
	int ret;

	ret = cpu_eth_init(bis);
	if (ret)
		return ret;

	/* MX28EVK uses ENET_CLK PAD to drive FEC clock */
	writel(CLKCTRL_ENET_TIME_SEL_RMII_CLK | CLKCTRL_ENET_CLK_OUT_EN,
	       &clkctrl_regs->hw_clkctrl_enet);

	mxs_iomux_setup_pad(MX28_PAD_ENET0_MDC__ENET0_MDC | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_MDIO__ENET0_MDIO | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_RX_EN__ENET0_RX_EN | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_TX_EN__ENET0_TX_EN| MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_RXD0__ENET0_RXD0 | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_RXD1__ENET0_RXD1 | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_TXD0__ENET0_TXD0  | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_TXD1__ENET0_TXD1 | MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_ENET_CLK__CLKCTRL_ENET| MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	mxs_iomux_setup_pad(MX28_PAD_LCD_RS__GPIO_1_26| MXS_PAD_3V3 | MXS_PAD_8MA | MXS_PAD_PULLUP);
	/* Power-on FECs */
	/*gpio_direction_output(MX28_PAD_SSP1_DATA3__GPIO_2_15, 0);*/

	/* Reset FEC PHYs */
	/*gpio_direction_output(MX28_PAD_ENET0_RX_CLK__GPIO_4_13, 0);
	udelay(200);
	gpio_set_value(MX28_PAD_ENET0_RX_CLK__GPIO_4_13, 1);*/
	gpio_direction_output(MX28_PAD_LCD_RS__GPIO_1_26, 0);
	udelay(200);
	gpio_set_value(MX28_PAD_LCD_RS__GPIO_1_26, 1);

	ret = fecmxc_initialize_multi(bis, 0, 5, MXS_ENET0_BASE);
	if (ret) {
		puts("FEC MXS: Unable to init FEC0\n");
		return ret;
	}
	/*
	ret = fecmxc_initialize_multi(bis, 1, 3, MXS_ENET1_BASE);
	if (ret) {
		puts("FEC MXS: Unable to init FEC1\n");
		return ret;
	}
*/
	dev = eth_get_dev_by_name("FEC0");
	if (!dev) {
		puts("FEC MXS: Unable to get FEC0 device entry\n");
		return -EINVAL;
	}

/*	dev = eth_get_dev_by_name("FEC1");
	if (!dev) {
		puts("FEC MXS: Unable to get FEC1 device entry\n");
		return -EINVAL;
	}
*/
	return ret;
}

#endif
