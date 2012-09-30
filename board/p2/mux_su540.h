#define MUX_DEFAULT_OMAP4() \
	MV(CP(GPMC_AD0), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat0 */	\
	MV(CP(GPMC_AD1), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat1 */	\
	MV(CP(GPMC_AD2), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat2 */	\
	MV(CP(GPMC_AD3), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat3 */	\
	MV(CP(GPMC_AD4), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat4 */	\
	MV(CP(GPMC_AD5), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat5 */	\
	MV(CP(GPMC_AD6), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat6 */	\
	MV(CP(GPMC_AD7), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_dat7 */	\
	MV(CP(GPMC_AD8), (PTD | M0)) 	\
	MV(CP(GPMC_AD9), (PTD | M0)) 	\
	MV(CP(GPMC_AD10), (PTD | M0)) 	\
	MV(CP(GPMC_AD11), (PTD | M0)) 	\
	MV(CP(GPMC_AD12), (PTD | M0)) 	\
	MV(CP(GPMC_AD13), (PTD | M0)) 	\
	MV(CP(GPMC_AD14), (PTD | M0)) 	\
	MV(CP(GPMC_AD15), (PTD | M0)) 	\
	MV(CP(GPMC_A16), (PTU | IEN | M3)) /* gpio_40 */	\
	MV(CP(GPMC_A17), (IEN | M3)) /* gpio_41 */	\
	MV(CP(GPMC_A18), (IEN | OFF_EN | M3)) /* gpio_42 */	\
	MV(CP(GPMC_A19), (PTD | M3)) /* gpio_43 */	\
	MV(CP(GPMC_A20), (PTU | IEN | M3)) /* gpio_44 */	\
	MV(CP(GPMC_A21), (IEN | M3)) /* gpio_45 */	\
	MV(CP(GPMC_A22), (PTD | M7)) /* safe_mode */	\
	MV(CP(GPMC_A23), (IEN | M3)) /* gpio_47 */	\
	MV(CP(GPMC_A24), (IEN | M3)) /* gpio_48 */	\
	MV(CP(GPMC_A25), (IEN | OFF_EN | M3)) /* gpio_49 */	\
	MV(CP(GPMC_NCS0), (IEN | M3)) /* gpio_50 */	\
	MV(CP(GPMC_NCS1), (IEN | M3)) /* gpio_51 */	\
	MV(CP(GPMC_NCS2), (IEN | M3)) /* gpio_52 */	\
	MV(CP(GPMC_NCS3), (PTD | M7)) /* safe_mode */	\
	MV(CP(GPMC_NWP), (IEN | M3)) /* gpio_54 */	\
	MV(CP(GPMC_CLK), (IEN | M3)) /* gpio_55 */	\
	MV(CP(GPMC_NADV_ALE), (IEN | M0)) 	\
	MV(CP(GPMC_NOE), (PTU | IEN | OFF_EN | M1)) /* sdmmc2_clk */	\
	MV(CP(GPMC_NWE), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1)) /* sdmmc2_cmd */	\
	MV(CP(GPMC_NBE0_CLE), (IEN | OFF_EN | M3)) /* gpio_59 */	\
	MV(CP(GPMC_NBE1), (IEN | OFF_EN | M3)) /* gpio_60 */	\
	MV(CP(GPMC_WAIT0), (PTD | M0)) 	\
	MV(CP(GPMC_WAIT1), (IEN | OFF_EN | OFF_OUT_PTU | M3)) /* gpio_62 */	\
	MV(CP(C2C_DATA11), (PTD | M7)) /* safe_mode */	\
	MV(CP(C2C_DATA12), (PTD | M7)) /* safe_mode */	\
	MV(CP(C2C_DATA13), (PTD | M7)) /* safe_mode */	\
	MV(CP(C2C_DATA14), (PTD | M3)) /* gpio_103 */	\
	MV(CP(C2C_DATA15), (IEN | M3)) /* gpio_104 */	\
	MV(CP(HDMI_HPD), (PTD | M3)) /* gpio_63 */	\
	MV(CP(HDMI_CEC), (PTD | M7)) /* safe_mode */	\
	MV(CP(HDMI_DDC_SCL), (PTU | IEN | M0)) 	\
	MV(CP(HDMI_DDC_SDA), (PTU | IEN | M0)) 	\
	MV(CP(CSI21_DX0), (IEN | M0)) 	\
	MV(CP(CSI21_DY0), (IEN | M0)) 	\
	MV(CP(CSI21_DX1), (IEN | M0)) 	\
	MV(CP(CSI21_DY1), (IEN | M0)) 	\
	MV(CP(CSI21_DX2), (IEN | M0)) 	\
	MV(CP(CSI21_DY2), (IEN | M0)) 	\
	MV(CP(CSI21_DX3), (PTD | M7)) /* safe_mode */	\
	MV(CP(CSI21_DY3), (PTD | M7)) /* safe_mode */	\
	MV(CP(CSI21_DX4), (PTD | M7)) /* safe_mode */	\
	MV(CP(CSI21_DY4), (PTD | M7)) /* safe_mode */	\
	MV(CP(CSI22_DX0), (IEN | M0)) 	\
	MV(CP(CSI22_DY0), (IEN | M0)) 	\
	MV(CP(CSI22_DX1), (IEN | M0)) 	\
	MV(CP(CSI22_DY1), (IEN | M0)) 	\
	MV(CP(CAM_SHUTTER), (IEN | M3)) /* gpio_81 */	\
	MV(CP(CAM_STROBE), (IEN | OFF_EN | M3)) /* gpio_82 */	\
	MV(CP(CAM_GLOBALRESET), (IEN | M3)) /* gpio_83 */	\
	MV(CP(USBB1_ULPITLL_CLK), (PTD | M1)) /* hsi1_cawake */	\
	MV(CP(USBB1_ULPITLL_STP), (IEN | M1)) /* hsi1_cadata */	\
	MV(CP(USBB1_ULPITLL_DIR), (IEN | M1)) /* hsi1_caflag */	\
	MV(CP(USBB1_ULPITLL_NXT), (IEN | OFF_EN | M1)) /* hsi1_acready */	\
	MV(CP(USBB1_ULPITLL_DAT0), (IEN | M1)) /* hsi1_acwake */	\
	MV(CP(USBB1_ULPITLL_DAT1), (IEN | M1)) /* hsi1_acdata */	\
	MV(CP(USBB1_ULPITLL_DAT2), (IEN | M1)) /* hsi1_acflag */	\
	MV(CP(USBB1_ULPITLL_DAT3), (IEN | M1)) /* hsi1_caready */	\
	MV(CP(USBB1_ULPITLL_DAT4), (IEN | OFF_EN | OFF_PD | OFF_IN | M4)) /* usbb1_ulpiphy_dat4 */	\
	MV(CP(USBB1_ULPITLL_DAT5), (PTD | M7)) /* safe_mode */	\
	MV(CP(USBB1_ULPITLL_DAT6), (IEN | OFF_EN | OFF_PD | OFF_IN | M4)) /* usbb1_ulpiphy_dat6 */	\
	MV(CP(USBB1_ULPITLL_DAT7), (IEN | OFF_EN | OFF_PD | OFF_IN | M4)) /* usbb1_ulpiphy_dat7 */	\
	MV(CP(USBB1_HSIC_DATA), (IEN | M7)) /* safe_mode */	\
	MV(CP(USBB1_HSIC_STROBE), (IEN | M7)) /* safe_mode */	\
	MV(CP(USBC1_ICUSB_DP), (IEN | OFF_EN | M3)) /* gpio_98 */	\
	MV(CP(USBC1_ICUSB_DM), (IEN | M7)) /* safe_mode */	\
	MV(CP(SDMMC1_CLK), (IEN | OFF_EN | M0)) 	\
	MV(CP(SDMMC1_CMD), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC1_DAT0), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC1_DAT1), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC1_DAT2), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC1_DAT3), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC1_DAT4), (PTD | M7)) /* safe_mode */	\
	MV(CP(SDMMC1_DAT5), (PTD | M7)) /* safe_mode */	\
	MV(CP(SDMMC1_DAT6), (PTD | M7)) /* safe_mode */	\
	MV(CP(SDMMC1_DAT7), (PTD | M7)) /* safe_mode */	\
	MV(CP(ABE_MCBSP2_CLKX), (IEN | OFF_EN | OFF_PD | OFF_IN | M0)) 	\
	MV(CP(ABE_MCBSP2_DR), (IEN | OFF_EN | M0)) 	\
	MV(CP(ABE_MCBSP2_DX), (IEN | OFF_EN | M0)) 	\
	MV(CP(ABE_MCBSP2_FSX), (IEN | OFF_EN | OFF_PD | OFF_IN | M0)) 	\
	MV(CP(ABE_MCBSP1_CLKX), (IEN | OFF_EN | OFF_PD | OFF_IN | M0)) 	\
	MV(CP(ABE_MCBSP1_DR), (IEN | OFF_EN | M0)) 	\
	MV(CP(ABE_MCBSP1_DX), (IEN | OFF_EN | M0)) 	\
	MV(CP(ABE_MCBSP1_FSX), (IEN | OFF_EN | OFF_PD | OFF_IN | M0)) 	\
	MV(CP(ABE_PDM_UL_DATA), (IEN | M0)) 	\
	MV(CP(ABE_PDM_DL_DATA), (PTD | M0)) 	\
	MV(CP(ABE_PDM_FRAME), (PTU | IEN | M0)) 	\
	MV(CP(ABE_PDM_LB_CLK), (PTD | M0)) 	\
	MV(CP(ABE_CLKS), (PTD | M0)) 	\
	MV(CP(ABE_DMIC_CLK1), (PTD | M3)) /* gpio_119 */	\
	MV(CP(ABE_DMIC_DIN1), (PTD | M3)) /* gpio_120 */	\
	MV(CP(ABE_DMIC_DIN2), (IEN | OFF_EN | M3)) /* gpio_121 */	\
	MV(CP(ABE_DMIC_DIN3), (IEN | M3)) /* gpio_122 */	\
	MV(CP(UART2_CTS), (PTU | IEN | M0)) 	\
	MV(CP(UART2_RTS), (PTU | IEN | M7)) /* safe_mode */	\
	MV(CP(UART2_RX), (IEN | M0)) 	\
	MV(CP(UART2_TX), (IEN | M0)) 	\
	MV(CP(HDQ_SIO), (IEN | M3)) /* gpio_127 */	\
	MV(CP(I2C1_SCL), (IEN | M0)) 	\
	MV(CP(I2C1_SDA), (IEN | M0)) 	\
	MV(CP(I2C2_SCL), (IEN | M0)) 	\
	MV(CP(I2C2_SDA), (IEN | M0)) 	\
	MV(CP(I2C3_SCL), (IEN | M0)) 	\
	MV(CP(I2C3_SDA), (IEN | M0)) 	\
	MV(CP(I2C4_SCL), (IEN | M0)) 	\
	MV(CP(I2C4_SDA), (IEN | M0)) 	\
	MV(CP(MCSPI1_CLK), (PTD | M0)) 	\
	MV(CP(MCSPI1_SOMI), (PTD | M0)) 	\
	MV(CP(MCSPI1_SIMO), (PTD | M0)) 	\
	MV(CP(MCSPI1_CS0), (PTD | M0)) 	\
	MV(CP(MCSPI1_CS1), (PTU | IEN | M1)) /* uart1_rx */	\
	MV(CP(MCSPI1_CS2), (PTD | M7)) /* safe_mode */	\
	MV(CP(MCSPI1_CS3), (IEN | M3)) /* gpio_140 */	\
	MV(CP(UART3_CTS_RCTX), (IEN | M1)) /* uart1_tx */	\
	MV(CP(UART3_RTS_SD), (PTD | M7)) /* safe_mode */	\
	MV(CP(UART3_RX_IRRX), (IEN | M1)) /* dmtimer8_pwm_evt */	\
	MV(CP(UART3_TX_IRTX), (PTD | M7)) /* safe_mode */	\
	MV(CP(SDMMC5_CLK), (PTU | IEN | OFF_EN | M0)) 	\
	MV(CP(SDMMC5_CMD), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC5_DAT0), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC5_DAT1), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC5_DAT2), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(SDMMC5_DAT3), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0)) 	\
	MV(CP(MCSPI4_CLK), (IEN | M7)) /* safe_mode */	\
	MV(CP(MCSPI4_SIMO), (IEN | M7)) /* safe_mode */	\
	MV(CP(MCSPI4_SOMI), (IEN | M7)) /* safe_mode */	\
	MV(CP(MCSPI4_CS0), (PTD | M7)) /* safe_mode */	\
	MV(CP(UART4_RX), (PTU | IEN | M0)) 	\
	MV(CP(UART4_TX), (IEN | M0)) 	\
	MV(CP(USBB2_ULPITLL_CLK), (IEN | OFF_EN | M3)) /* gpio_157 */	\
	MV(CP(USBB2_ULPITLL_STP), (PTD | M3)) /* gpio_158 */	\
	MV(CP(USBB2_ULPITLL_DIR), (PTD | M3)) /* gpio_159 */	\
	MV(CP(USBB2_ULPITLL_NXT), (PTD | OFF_EN | OFF_PD | OFF_IN | M3)) /* gpio_160 */	\
	MV(CP(USBB2_ULPITLL_DAT0), (IEN | M3)) /* gpio_161 */	\
	MV(CP(USBB2_ULPITLL_DAT1), (IEN | OFF_EN | OFF_PD | OFF_IN | M3)) /* gpio_162 */	\
	MV(CP(USBB2_ULPITLL_DAT2), (IEN | OFF_EN | OFF_PD | OFF_IN | M3)) /* gpio_163 */	\
	MV(CP(USBB2_ULPITLL_DAT3), (IEN | M3)) /* gpio_164 */	\
	MV(CP(USBB2_ULPITLL_DAT4), (IEN | M3)) /* gpio_165 */	\
	MV(CP(USBB2_ULPITLL_DAT5), (IEN | M3)) /* gpio_166 */	\
	MV(CP(USBB2_ULPITLL_DAT6), (PTD | M3)) /* gpio_167 */	\
	MV(CP(USBB2_ULPITLL_DAT7), (IEN | M3)) /* gpio_168 */	\
	MV(CP(USBB2_HSIC_DATA), (IEN | M7)) /* safe_mode */	\
	MV(CP(USBB2_HSIC_STROBE), (IEN | M7)) /* safe_mode */	\
	MV(CP(UNIPRO_TX0), (IEN | M1)) /* kpd_col0 */	\
	MV(CP(UNIPRO_TY0), (IEN | M1)) /* kpd_col1 */	\
	MV(CP(UNIPRO_TX1), (PTD | M7)) /* safe_mode */	\
	MV(CP(UNIPRO_TY1), (PTD | M7)) /* safe_mode */	\
	MV(CP(UNIPRO_TX2), (IEN | OFF_EN | M3)) /* gpio_0 */	\
	MV(CP(UNIPRO_TY2), (IEN | OFF_EN | OFF_OUT_PTU | M3)) /* gpio_1 */	\
	MV(CP(UNIPRO_RX0), (PTU | IEN | M1)) /* kpd_row0 */	\
	MV(CP(UNIPRO_RY0), (PTU | IEN | M1)) /* kpd_row1 */	\
	MV(CP(UNIPRO_RX1), (PTU | IEN | M1)) /* kpd_row2 */	\
	MV(CP(UNIPRO_RY1), (PTD | M7)) /* safe_mode */	\
	MV(CP(UNIPRO_RX2), (PTD | M7)) /* safe_mode */	\
	MV(CP(UNIPRO_RY2), (PTD | M7)) /* safe_mode */	\
	MV(CP(USBA0_OTG_CE), (IEN | M0)) 	\
	MV(CP(USBA0_OTG_DP), (IEN | M0)) 	\
	MV(CP(USBA0_OTG_DM), (IEN | M0)) 	\
	MV(CP(FREF_CLK1_OUT), (IEN | M0)) 	\
	MV(CP(FREF_CLK2_OUT), (IEN | M0)) 	\
	MV(CP(SYS_NIRQ1), (PTU | IEN | M0)) 	\
	MV(CP(SYS_NIRQ2), (PTU | IEN | M0)) 	\
	MV(CP(SYS_BOOT0), (IEN | M7)) /* safe_mode */	\
	MV(CP(SYS_BOOT1), (IEN | M3)) /* gpio_185 */	\
	MV(CP(SYS_BOOT2), (IEN | M3)) /* gpio_186 */	\
	MV(CP(SYS_BOOT3), (IEN | M7)) /* safe_mode */	\
	MV(CP(SYS_BOOT4), (IEN | M3)) /* gpio_188 */	\
	MV(CP(SYS_BOOT5), (IEN | M7)) /* safe_mode */	\
	MV(CP(DPM_EMU0), (IEN | OFF_EN | M3)) /* gpio_11 */	\
	MV(CP(DPM_EMU1), (IEN | OFF_EN | M3)) /* gpio_12 */	\
	MV(CP(DPM_EMU2), (PTD | M3)) /* gpio_13 */	\
	MV(CP(DPM_EMU3), (IEN | M3)) /* gpio_14 */	\
	MV(CP(DPM_EMU4), (IEN | OFF_EN | M3)) /* gpio_15 */	\
	MV(CP(DPM_EMU5), (IEN | M3)) /* gpio_16 */	\
	MV(CP(DPM_EMU6), (IEN | M2)) /* uart3_tx_irtx */	\
	MV(CP(DPM_EMU7), (IEN | M2)) /* uart3_rx_irrx */	\
	MV(CP(DPM_EMU8), (IEN | M2)) /* uart3_rts_sd */	\
	MV(CP(DPM_EMU9), (PTD | M2)) /* uart3_cts_rctx */	\
	MV(CP(DPM_EMU10), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M3)) /* gpio_21 */	\
	MV(CP(DPM_EMU11), (IEN | M7)) /* safe_mode */	\
	MV(CP(DPM_EMU12), (IEN | M3)) /* gpio_23 */	\
	MV(CP(DPM_EMU13), (IEN | OFF_EN | M3)) /* gpio_24 */	\
	MV(CP(DPM_EMU14), (IEN | OFF_EN | M3)) /* gpio_25 */	\
	MV(CP(DPM_EMU15), (PTD | M3)) /* gpio_26 */	\
	MV(CP(DPM_EMU16), (IEN | OFF_EN | M3)) /* gpio_27 */	\
	MV(CP(DPM_EMU17), (PTD | M3)) /* gpio_28 */	\
	MV(CP(DPM_EMU18), (IEN | M7)) /* safe_mode */	\
	MV(CP(DPM_EMU19), (IEN | M3)) /* gpio_191 */	\
	MV(CP(CSI22_DX2), (IEN | M0)) 	\
	MV(CP(CSI22_DY2), (IEN | M0))	\
	MV1(WK(PAD0_SIM_IO), (IEN | M7)) /* safe_mode */	\
	MV1(WK(PAD1_SIM_CLK), (IEN | M7)) /* safe_mode */	\
	MV1(WK(PAD0_SIM_RESET), (IEN | M7)) /* safe_mode */	\
	MV1(WK(PAD1_SIM_CD), (IEN | M3)) /* gpio_wk3 */	\
	MV1(WK(PAD0_SIM_PWRCTRL), (PTD | M3)) /* gpio_wk4 */	\
	MV1(WK(PAD1_SR_SCL), (PTU | IEN | M0)) 	\
	MV1(WK(PAD0_SR_SDA), (PTU | IEN | M0)) 	\
	MV1(WK(PAD1_FREF_XTAL_IN), (IEN | M7)) /* (null) */	\
	MV1(WK(PAD0_FREF_SLICER_IN), (IEN | M0)) 	\
	MV1(WK(PAD1_FREF_CLK_IOREQ), (IEN | M0)) 	\
	MV1(WK(PAD0_FREF_CLK0_OUT), (IEN | M3)) /* gpio_wk6 */	\
	MV1(WK(PAD1_FREF_CLK3_REQ), (IEN | M3)) /* gpio_wk30 */	\
	MV1(WK(PAD0_FREF_CLK3_OUT), (PTU | IEN | M3)) /* gpio_wk31 */	\
	MV1(WK(PAD1_FREF_CLK4_REQ), (IEN | M3)) /* gpio_wk7 */	\
	MV1(WK(PAD0_FREF_CLK4_OUT), (PTU | IEN | M3)) /* gpio_wk8 */	\
	MV1(WK(PAD1_SYS_32K), (IEN | M0)) 	\
	MV1(WK(PAD0_SYS_NRESPWRON), (IEN | M0)) 	\
	MV1(WK(PAD1_SYS_NRESWARM), (IEN | M0)) 	\
	MV1(WK(PAD0_SYS_PWR_REQ), (IEN | M0)) 	\
	MV1(WK(PAD1_SYS_PWRON_RESET), (PTU | IEN | M3)) /* gpio_wk29 */	\
	MV1(WK(PAD0_SYS_BOOT6), (IEN | M7)) /* safe_mode */	\
	MV1(WK(PAD1_SYS_BOOT7), (IEN | M3)) /* gpio_wk10 */	\

#if 0
	MV1(WK(PAD0_JTAG_NTRST), (PTD | M0)) 	\
	MV1(WK(PAD1_JTAG_TCK), (PTD | M0)) 	\
	MV1(WK(PAD0_JTAG_RTCK), (IEN | M0)) 	\
	MV1(WK(PAD1_JTAG_TMS_TMSC), (IEN | M0)) 	\
	MV1(WK(PAD0_JTAG_TDI), (PTU | IEN | M0)) 	\
	MV1(WK(PAD1_JTAG_TDO), (PTU | IEN | M0)) 	\

#endif
