/*
 * IEN  - Input Enable
 * IDIS - Input Disable
 * PTD  - Pull type Down
 * PTU  - Pull type Up
 * DIS  - Pull type selection is inactive
 * EN   - Pull type selection is active
 * M0   - Mode 0
 * The commented string gives the final mux configuration for that pin
 */

#define MUX_DEFAULT_OMAP4() \
MV(CP(GPMC_AD0),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[0]	*/	\
MV(CP(GPMC_AD1),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[1]	*/	\
MV(CP(GPMC_AD2),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[2]	*/	\
MV(CP(GPMC_AD3),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[3]	*/	\
MV(CP(GPMC_AD4),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[4]	*/	\
MV(CP(GPMC_AD5),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[5]	*/	\
MV(CP(GPMC_AD6),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[6]	*/	\
MV(CP(GPMC_AD7),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)  /* SDMMC2_DAT[7]	*/	\
	MV(CP(GPMC_AD8) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M3))  /* gpio_32 */ \
	MV(CP(GPMC_AD9) , ( PTU | IEN | M3))  /* gpio_33 */ \
	MV(CP(GPMC_AD10) , ( PTU | IEN | M3))  /* gpio_34 */ \
	MV(CP(GPMC_AD11) , ( PTU | IEN | M3))  /* gpio_35 */ \
	MV(CP(GPMC_AD12) , ( PTU | IEN | M3))  /* gpio_36 */ \
	MV(CP(GPMC_AD13) , ( PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3))  /* gpio_37 */ \
	MV(CP(GPMC_AD14) , ( PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3))  /* gpio_38 */ \
	MV(CP(GPMC_AD15) , ( PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3))  /* gpio_39 */ \
	MV(CP(GPMC_A16) , ( M3))  /* gpio_40 */ \
	MV(CP(GPMC_A17) , ( PTD | M3))  /* gpio_41 */ \
	MV(CP(GPMC_A18) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_row6 */ \
MV(CP(GPMC_A19),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* 3D_BOOST_EN */ \
	MV(CP(GPMC_A20) , ( IEN | M3))  /* gpio_44 */ \
	MV(CP(GPMC_A21) , ( M3))  /* gpio_45 */ \
MV(CP(GPMC_A22) , ( PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3))  /* SMPL_CHG */ \
	MV(CP(GPMC_A23) , ( OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_col7 */ \
	MV(CP(GPMC_A24) , ( PTD | M3))  /* gpio_48 */ \
	MV(CP(GPMC_A25) , ( PTD | M3))  /* gpio_49 */ \
	MV(CP(GPMC_NCS0) , ( M3))  /* gpio_50 */ \
	MV(CP(GPMC_NCS1) , ( IEN | M3))  /* gpio_51 */ \
MV(CP(GPMC_NCS2),		PTU | IEN | M3)				/* TOUCH_INT_N	*/	\
	MV(CP(GPMC_NCS3) , ( IEN | M3))  /* gpio_53 */ \
	MV(CP(GPMC_NWP) , ( M3))  /* gpio_54 */ \
	MV(CP(GPMC_CLK) , ( PTD | M3))  /* gpio_55 */ \
	MV(CP(GPMC_NADV_ALE) , ( M3))  /* gpio_56 */ \
	MV(CP(GPMC_NOE) , ( PTU | IEN | OFF_EN | OFF_OUT_PTD | M1))  /* sdmmc2_clk */ \
	MV(CP(GPMC_NWE) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1))  /* sdmmc2_cmd */ \
	MV(CP(GPMC_NBE0_CLE) , ( M3))  /* gpio_59 */ \
	MV(CP(GPMC_NBE1) , ( PTD | M3))  /* gpio_60 */ \
	MV(CP(GPMC_WAIT0) , ( PTU | IEN | M3))  /* gpio_61 */ \
	MV(CP(GPMC_WAIT1) , ( IEN | M3))  /* gpio_62 */ \
	MV(CP(C2C_DATA11) , ( PTD | M3))  /* gpio_100 */ \
	MV(CP(C2C_DATA12) , ( M1))  /* dsi1_te0 */ \
	MV(CP(C2C_DATA13) , ( PTD | M3))  /* gpio_102 */ \
	MV(CP(C2C_DATA14) , ( M1))  /* dsi2_te0 */ \
MV(CP(C2C_DATA15),		(PTD | M3))			/*	PROXI_LDO_EN - gpio_104 */ \
	MV(CP(HDMI_HPD) , ( M0))  /* hdmi_hpd */ \
	MV(CP(HDMI_CEC) , ( M0))  /* hdmi_cec */ \
	MV(CP(HDMI_DDC_SCL) , ( PTU | M0))  /* hdmi_ddc_scl */ \
	MV(CP(HDMI_DDC_SDA) , ( PTU | IEN | M0))  /* hdmi_ddc_sda */ \
	MV(CP(CSI21_DX0) , ( IEN | M0))  /* csi21_dx0 */ \
	MV(CP(CSI21_DY0) , ( IEN | M0))  /* csi21_dy0 */ \
	MV(CP(CSI21_DX1) , ( IEN | M0))  /* csi21_dx1 */ \
	MV(CP(CSI21_DY1) , ( IEN | M0))  /* csi21_dy1 */ \
	MV(CP(CSI21_DX2) , ( IEN | M0))  /* csi21_dx2 */ \
	MV(CP(CSI21_DY2) , ( IEN | M0))  /* csi21_dy2 */ \
	MV(CP(CSI21_DX3) , ( PTD | M7))  /* csi21_dx3 */ \
	MV(CP(CSI21_DY3) , ( PTD | M7))  /* csi21_dy3 */ \
	MV(CP(CSI21_DX4) , ( PTD | OFF_EN | OFF_PD | OFF_IN | M7))  /* csi21_dx4 */ \
	MV(CP(CSI21_DY4) , ( PTD | OFF_EN | OFF_PD | OFF_IN | M7))  /* csi21_dy4 */ \
	MV(CP(CSI22_DX0) , ( IEN | M0))  /* csi22_dx0 */ \
	MV(CP(CSI22_DY0) , ( IEN | M0))  /* csi22_dy0 */ \
	MV(CP(CSI22_DX1) , ( IEN | M0))  /* csi22_dx1 */ \
	MV(CP(CSI22_DY1) , ( IEN | M0))  /* csi22_dy1 */ \
	MV(CP(CAM_SHUTTER) , ( OFF_EN | OFF_PD | OFF_OUT_PTD | M0))  /* cam_shutter */ \
	MV(CP(CAM_STROBE) , (  PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3))  /* FRONT_KEY_LED_EN cam_strobe */ \
	MV(CP(CAM_GLOBALRESET) , ( PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3))  /* gpio_83 */ \
	MV(CP(USBB1_ULPITLL_CLK) , ( PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_clk */ \
	MV(CP(USBB1_ULPITLL_STP) , ( OFF_EN | OFF_OUT_PTD | M4))  /* usbb1_ulpiphy_stp */ \
	MV(CP(USBB1_ULPITLL_DIR) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dir */ \
	MV(CP(USBB1_ULPITLL_NXT) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_nxt */ \
	MV(CP(USBB1_ULPITLL_DAT0) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat0 */ \
	MV(CP(USBB1_ULPITLL_DAT1) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat1 */ \
	MV(CP(USBB1_ULPITLL_DAT2) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat2 */ \
	MV(CP(USBB1_ULPITLL_DAT3) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat3 */ \
	MV(CP(USBB1_ULPITLL_DAT4) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat4 */ \
	MV(CP(USBB1_ULPITLL_DAT5) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat5 */ \
	MV(CP(USBB1_ULPITLL_DAT6) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat6 */ \
	MV(CP(USBB1_ULPITLL_DAT7) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))  /* usbb1_ulpiphy_dat7 */ \
	MV(CP(USBB1_HSIC_DATA) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* usbb1_hsic_data */ \
	MV(CP(USBB1_HSIC_STROBE) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* usbb1_hsic_strobe */ \
MV(CP(USBC1_ICUSB_DP),	PTU | OFF_EN | OFF_PU | OFF_OUT_PTU | M3)		/* LCD_CP_EN		*/	\
MV(CP(USBC1_ICUSB_DM),	PTU | OFF_EN | OFF_PU | OFF_OUT_PTU | M3)		/* CAM_SUBPN_EN		*/	\
	MV(CP(SDMMC1_CLK) , ( PTU | OFF_EN | OFF_OUT_PTD | M0))  /* sdmmc1_clk */ \
	MV(CP(SDMMC1_CMD) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc1_cmd */ \
MV(CP(SDMMC1_DAT0),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT0	*/	\
MV(CP(SDMMC1_DAT1),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT1	*/	\
MV(CP(SDMMC1_DAT2),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT2	*/	\
MV(CP(SDMMC1_DAT3),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT3	*/	\
MV(CP(SDMMC1_DAT4),		PTU | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* LCD_EN		*/	\
	MV(CP(SDMMC1_DAT5) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc1_dat5 */ \
	MV(CP(SDMMC1_DAT6) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc1_dat6 */ \
	MV(CP(SDMMC1_DAT7) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc1_dat7 */ \
	MV(CP(ABE_MCBSP2_CLKX) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_mcbsp2_clkx */ \
	MV(CP(ABE_MCBSP2_DR) , ( IEN | OFF_EN | OFF_OUT_PTD | M0))  /* abe_mcbsp2_dr */ \
	MV(CP(ABE_MCBSP2_DX) , ( OFF_EN | OFF_OUT_PTD | M0))  /* abe_mcbsp2_dx */ \
	MV(CP(ABE_MCBSP2_FSX) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_mcbsp2_fsx */ \
	MV(CP(ABE_MCBSP1_CLKX) , ( IEN | M1))  /* abe_slimbus1_clock */ \
	MV(CP(ABE_MCBSP1_DR) , ( IEN | M1))  /* abe_slimbus1_data */ \
	MV(CP(ABE_MCBSP1_DX) , ( OFF_EN | OFF_OUT_PTD | M0))  /* abe_mcbsp1_dx */ \
	MV(CP(ABE_MCBSP1_FSX) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_mcbsp1_fsx */ \
	MV(CP(ABE_PDM_UL_DATA) , ( PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_pdm_ul_data */ \
	MV(CP(ABE_PDM_DL_DATA) , ( PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_pdm_dl_data */ \
	MV(CP(ABE_PDM_FRAME) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_pdm_frame */ \
	MV(CP(ABE_PDM_LB_CLK) , ( PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_pdm_lb_clk */ \
	MV(CP(ABE_CLKS) , ( PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* abe_clks */ \
	MV(CP(ABE_DMIC_CLK1) , ( M0))  /* abe_dmic_clk1 */ \
	MV(CP(ABE_DMIC_DIN1) , ( IEN | M0))  /* abe_dmic_din1 */ \
	MV(CP(ABE_DMIC_DIN2) , ( IEN | M0))  /* abe_dmic_din2 */ \
	MV(CP(ABE_DMIC_DIN3) , ( IEN | M0))  /* abe_dmic_din3 */ \
	MV(CP(UART2_CTS) , ( PTU | IEN | M0))  /* uart2_cts */ \
	MV(CP(UART2_RTS) , ( M0))  /* uart2_rts */ \
	MV(CP(UART2_RX) , ( PTU | IEN | M0))  /* uart2_rx */ \
	MV(CP(UART2_TX) , ( M0))  /* uart2_tx */ \
	MV(CP(HDQ_SIO) , ( M3))  /* gpio_127 */ \
MV(CP(I2C1_SCL),	PTU | IEN | EN | M0)  /* I2C1_SCL */ \
MV(CP(I2C1_SDA),	PTU | IEN | EN | M0)  /* I2C1_SDA */ \
MV(CP(I2C2_SCL),	PTU | IEN | EN | M0)  /* I2C2_SCL */ \
MV(CP(I2C2_SDA),	PTU | IEN | EN | M0)  /* I2C2_SDA */ \
MV(CP(I2C3_SCL),	PTU | IEN | EN | M0)  /* I2C3_SCL */ \
MV(CP(I2C3_SDA),	PTU | IEN | EN | M0)  /* I2C3_SDA */ \
MV(CP(I2C4_SCL),	PTU | IEN | EN | M0)  /* I2C4_SCL */ \
MV(CP(I2C4_SDA),	PTU | IEN | EN | M0)  /* I2C4_SDA */ \
	MV(CP(MCSPI1_CLK) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi1_clk */ \
	MV(CP(MCSPI1_SOMI) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi1_somi */ \
	MV(CP(MCSPI1_SIMO) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi1_simo */ \
	MV(CP(MCSPI1_CS0) , ( PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi1_cs0 */ \
MV(CP(MCSPI1_CS1),		PTU | IEN |  M1)		/* UART1_RX_IPC		*/	\
MV(CP(MCSPI1_CS2),		PTU | OFF_EN | OFF_PD | OFF_OUT_PTU |	M3)		/* 3D_LCD_BANK_SEL	*/	\
MV(CP(MCSPI1_CS3),		PTD | 									M3)		/*	GPS_LNA_SD		*/	\
MV(CP(UART3_CTS_RCTX) , M1)									/* UART1_TX_IPC		*/	\
MV(CP(UART3_RTS_SD),	M7)												/* NC				*/	\
MV(CP(UART3_RX_IRRX),	M7)												/* NC				*/	\
MV(CP(UART3_TX_IRTX),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* 3D_LCD_EN		*/	\
	MV(CP(SDMMC5_CLK) , ( PTU | IEN | OFF_EN | OFF_OUT_PTD | M0))  /* sdmmc5_clk */ \
	MV(CP(SDMMC5_CMD) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc5_cmd */ \
	MV(CP(SDMMC5_DAT0) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc5_dat0 */ \
	MV(CP(SDMMC5_DAT1) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc5_dat1 */ \
	MV(CP(SDMMC5_DAT2) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc5_dat2 */ \
	MV(CP(SDMMC5_DAT3) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* sdmmc5_dat3 */ \
	MV(CP(MCSPI4_CLK) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi4_clk */ \
	MV(CP(MCSPI4_SIMO) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi4_simo */ \
	MV(CP(MCSPI4_SOMI) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi4_somi */ \
	MV(CP(MCSPI4_CS0) , ( PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0))  /* mcspi4_cs0 */ \
MV(CP(UART4_RX),		IEN | M0)		/* UART4_RX_OMAP	*/	\
MV(CP(UART4_TX),		M0)				/* UART4_RX_OMAP	*/	\
	MV(CP(USBB2_ULPITLL_CLK) , ( IEN | M3))  /* gpio_157 */ \
	MV(CP(USBB2_ULPITLL_STP) , ( IEN | M5))  /* dispc2_data23 */ \
	MV(CP(USBB2_ULPITLL_DIR) , ( IEN | M5))  /* dispc2_data22 */ \
	MV(CP(USBB2_ULPITLL_NXT) , ( IEN | M5))  /* dispc2_data21 */ \
	MV(CP(USBB2_ULPITLL_DAT0) , ( IEN | M5))  /* dispc2_data20 */ \
	MV(CP(USBB2_ULPITLL_DAT1) , ( IEN | M5))  /* dispc2_data19 */ \
	MV(CP(USBB2_ULPITLL_DAT2) , ( IEN | M5))  /* dispc2_data18 */ \
	MV(CP(USBB2_ULPITLL_DAT3) , ( PTD | M3))  /* dispc2_data15     MV(CP(USBB2_ULPITLL_DAT3) , ( IEN | M5))    */ \
	MV(CP(USBB2_ULPITLL_DAT4) , ( IEN | M5))  /* dispc2_data14 */ \
	MV(CP(USBB2_ULPITLL_DAT5) , ( IEN | M5))  /* dispc2_data13 */ \
	MV(CP(USBB2_ULPITLL_DAT6) , ( IEN | M5))  /* dispc2_data12 */ \
	MV(CP(USBB2_ULPITLL_DAT7) , ( IEN | M5))  /* dispc2_data11 */ \
	MV(CP(USBB2_HSIC_DATA) , ( PTD | OFF_EN | OFF_OUT_PTU | M3))  /* gpio_169 */ \
	MV(CP(USBB2_HSIC_STROBE) , ( PTD | OFF_EN | OFF_OUT_PTU | M3))  /* gpio_170 */ \
MV(CP(UNIPRO_TX0),		OFF_EN | OFF_PD | OFF_IN | M1)  /* kpd_col0 */ \
MV(CP(UNIPRO_TY0),		OFF_EN | OFF_PD | OFF_IN | M1)  /* kpd_col1 */ \
	MV(CP(UNIPRO_TX1) , ( OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_col2 */ \
	MV(CP(UNIPRO_TY1) , ( OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_col3 */ \
MV(CP(UNIPRO_TX2),		(PTD	|	OFF_EN	|	OFF_PD	| OFF_OUT_PTD				| M3))	/* GPS_PWR_ON		*/ \
MV(CP(UNIPRO_TY2),		(PTD	|	OFF_EN	|	OFF_PD	| OFF_OUT_PTD				| M3))	/* GPS_RESET_N		*/ \
MV(CP(UNIPRO_RX0),		(PTU	|	IEN		|	OFF_EN	| OFF_PD		| OFF_IN	| M1))	/* KEY_R[0]			*/ \
MV(CP(UNIPRO_RY0),		(PTU	|	IEN		|	OFF_EN	| OFF_PD		| OFF_IN	| M1))	/* KEY_R[1]			*/ \
	MV(CP(UNIPRO_RX1) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_row2 */ \
	MV(CP(UNIPRO_RY1) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_row3 */ \
	MV(CP(UNIPRO_RX2) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_row4 */ \
	MV(CP(UNIPRO_RY2) , ( PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1))  /* kpd_row5 */ \
	MV(CP(USBA0_OTG_CE) , ( PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M0))  /* usba0_otg_ce */ \
MV(CP(USBA0_OTG_DP),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* USB_DP_OMAP	*/	\
MV(CP(USBA0_OTG_DM),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* USB_DM_OMAP	*/	\
	MV(CP(FREF_CLK1_OUT) , ( M0))  /* fref_clk1_out */ \
	MV(CP(FREF_CLK2_OUT) , ( M0))  /* fref_clk2_out */ \
	MV(CP(SYS_NIRQ1) , ( PTU | IEN | M0))  /* sys_nirq1 */ \
	MV(CP(SYS_NIRQ2) , ( PTU | IEN | M0))  /* sys_nirq2 */ \
	MV(CP(SYS_BOOT0) , ( PTU | IEN | M3))  /* gpio_184 */ \
	MV(CP(SYS_BOOT1) , ( M3))  /* gpio_185 */ \
	MV(CP(SYS_BOOT2) , ( PTD | IEN | M3))  /* gpio_186 */ \
	MV(CP(SYS_BOOT3) , ( M3))  /* gpio_187 */ \
	MV(CP(SYS_BOOT4) , ( M3))  /* gpio_188 */ \
	MV(CP(SYS_BOOT5) , ( PTD | IEN | M3))  /* gpio_189 */ \
MV(CP(DPM_EMU0),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* UART_SW1			*/	\
MV(CP(DPM_EMU1),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* UART_SW2			*/	\
MV(CP(DPM_EMU2),	PTU | IEN	| M3	)		/* COMPASS_INT			*/	\
MV(CP(DPM_EMU3),	PTU | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* COMPASS_RESET		*/	\
MV(CP(DPM_EMU4),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* SECONDARY_5M_RESET_N	*/	\
MV(CP(DPM_EMU5),	PTU | IEN	| M3	)		/* PROXI_INT		*/	\
MV(CP(DPM_EMU6),		(PTU | M2)) 			/* GPS_UART_TXD		*/	\
MV(CP(DPM_EMU7),		(PTU | IEN | M2))		/* GPS_UART_RXD		*/	\
MV(CP(DPM_EMU8),		(PTD | M2))  			/* GPS_UART_RTS_N	*/	\
MV(CP(DPM_EMU9),		(PTD | IEN | M2))		/* GPS_UART_CTS_N	*/	\
MV(CP(DPM_EMU10),		PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M3)		/* EAR_SENSE		*/	\
MV(CP(DPM_EMU11),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* VT_CAM_PWDN		*/	\
MV(CP(DPM_EMU12),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* VT_CAM_RESET		*/	\
MV(CP(DPM_EMU13),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* PRIMARY_5M_RESET_N */ \
	MV(CP(DPM_EMU14) , ( IEN | M5))  /* dispc2_data5 */ \
	MV(CP(DPM_EMU15) , ( IEN | M5))  /* dispc2_data4 */ \
MV(CP(DPM_EMU16),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* DSI1_TE0			*/	\	
MV(CP(DPM_EMU17),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* MOVIE_MODE_EN	*/	\
MV(CP(DPM_EMU18),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* FLASH_EN			*/	\
MV(CP(DPM_EMU19),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* GPS_GSM_CTL		*/	\
	MV1(WK(PAD0_SIM_IO) , ( IEN | M0))  /* sim_io */ \
MV1(WK(PAD1_SIM_CLK),		PTU | IEN | M3)					/* GYRO_INT		*/	\
MV1(WK(PAD0_SIM_RESET),		PTU | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)	/* IFX_PWR_ON_SW	*/	\
MV1(WK(PAD1_SIM_CD),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)	/* IFX_PWR_ON_SW	*/	\
MV1(WK(PAD0_SIM_PWRCTRL),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)	/* GSM_TXON_IND		*/	\
	MV1(WK(PAD1_SR_SCL) , ( PTU | IEN | M0))  /* sr_scl */ \
	MV1(WK(PAD0_SR_SDA) , ( PTU | IEN | M0))  /* sr_sda */ \
	MV1(WK(PAD1_FREF_XTAL_IN) , ( M0))  /* # */ \
	MV1(WK(PAD0_FREF_SLICER_IN) , ( M0))  /* fref_slicer_in */ \
	MV1(WK(PAD1_FREF_CLK_IOREQ) , ( M0))  /* fref_clk_ioreq */ \
	MV1(WK(PAD0_FREF_CLK0_OUT) , ( M2))  /* sys_drm_msecure */ \
MV1(WK(PAD1_FREF_CLK3_REQ),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)  /* LCD_RESET_N */ \
	MV1(WK(PAD0_FREF_CLK3_OUT) , ( M0))  /* fref_clk3_out */ \
	MV1(WK(PAD1_FREF_CLK4_REQ) , ( PTU | IEN | M0))  /* # */ \
	MV1(WK(PAD0_FREF_CLK4_OUT) , ( M0))  /* # */ \
	MV1(WK(PAD1_SYS_32K) , ( IEN | M0))  /* sys_32k */ \
	MV1(WK(PAD0_SYS_NRESPWRON) , ( M0))  /* sys_nrespwron */ \
	MV1(WK(PAD1_SYS_NRESWARM) , ( M0))  /* sys_nreswarm */ \
	MV1(WK(PAD0_SYS_PWR_REQ) , ( PTU | M0))  /* sys_pwr_req */ \
MV1(WK(PAD1_SYS_PWRON_RESET), PTU | IEN | M3)		/* MOTION_INT	*/ \
	MV1(WK(PAD0_SYS_BOOT6) , ( IEN | M3))  /* gpio_wk9 */ \
	MV1(WK(PAD1_SYS_BOOT7) , ( IEN | M3))  /* gpio_wk10 */ \
//	MV1(WK(PAD0_JTAG_NTRST) , ( IEN | M0))  /* jtag_ntrst */ \
	MV1(WK(PAD1_JTAG_TCK) , ( IEN | M0))  /* jtag_tck */ \
	MV1(WK(PAD0_JTAG_RTCK) , ( M0))  /* jtag_rtck */ \
	MV1(WK(PAD1_JTAG_TMS_TMSC) , ( IEN | M0))  /* jtag_tms_tmsc */ \
	MV1(WK(PAD0_JTAG_TDI) , ( IEN | M0))  /* jtag_tdi */ \
	MV1(WK(PAD1_JTAG_TDO) , ( M0))  /* jtag_tdo */ 
