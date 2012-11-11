#define MUX_DEFAULT_OMAP4() \
	MV(CP(GPMC_AD0), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD1), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD2), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD3), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD4), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD5), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD6), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD7), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_AD8), (PTD | IEN | M0))						\
	MV(CP(GPMC_AD9), (PTD | IEN | M0))						\
	MV(CP(GPMC_AD10), (PTD | IEN | M0))						\
	MV(CP(GPMC_AD11), (PTD | IEN | M0))						\
	MV(CP(GPMC_AD12), (PTD | IEN | M0))						\
	MV(CP(GPMC_AD13), (PTD | IEN | M0))						\
	MV(CP(GPMC_AD14), (PTD | IEN | M0))						\
	MV(CP(GPMC_AD15), (PTD | IEN | M0))						\
	MV(CP(GPMC_A16), (PTU | IEN | M3))						\
	MV(CP(GPMC_A17), (PTD | M3))							\
	MV(CP(GPMC_A18), (OFF_EN | M3))							\
	MV(CP(GPMC_A19), (PTD | IEN | M3))						\
	MV(CP(GPMC_A20), (PTD | IEN | M7))						\
	MV(CP(GPMC_A21), (PTD | M3))							\
	MV(CP(GPMC_A22), (PTD | IEN | M7))						\
	MV(CP(GPMC_A23), (PTD | M3))							\
	MV(CP(GPMC_A24), (PTD | IEN | M7))						\
	MV(CP(GPMC_A25), (M7))								\
	MV(CP(GPMC_NCS0), (M0))								\
	MV(CP(GPMC_NCS1), (IEN | M3))							\
	MV(CP(GPMC_NCS2), (IEN | M3))							\
	MV(CP(GPMC_NCS3), (PTD | IEN | M7))						\
	MV(CP(GPMC_NWP), (PTD | IEN | M7))						\
	MV(CP(GPMC_CLK), (M3))								\
	MV(CP(GPMC_NADV_ALE), (M0))							\
	MV(CP(GPMC_NOE), (PTU | IEN | OFF_EN | M1))					\
	MV(CP(GPMC_NWE), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M1))			\
	MV(CP(GPMC_NBE0_CLE), (PTD | M3))						\
	MV(CP(GPMC_NBE1), (OFF_EN | M3))						\
	MV(CP(GPMC_WAIT0), (PTD | IEN | M0))						\
	MV(CP(GPMC_WAIT1), (OFF_EN | OFF_OUT_PTU | M3))					\
	MV(CP(C2C_DATA11), (PTD | IEN | M7))						\
	MV(CP(C2C_DATA12), (PTD | IEN | M7))						\
	MV(CP(C2C_DATA13), (PTD | IEN | M7))						\
	MV(CP(C2C_DATA14), (PTD | IEN | M3))						\
	MV(CP(C2C_DATA15), (PTD | M3))							\
	MV(CP(HDMI_HPD), (PTD | IEN | M3))						\
	MV(CP(HDMI_CEC), (PTD | IEN | M7))						\
	MV(CP(HDMI_DDC_SCL), (PTU | IEN | M0))						\
	MV(CP(HDMI_DDC_SDA), (PTU | IEN | M0))						\
	MV(CP(CSI21_DX0), (IEN | M0))							\
	MV(CP(CSI21_DY0), (IEN | M0))							\
	MV(CP(CSI21_DX1), (IEN | M0))							\
	MV(CP(CSI21_DY1), (IEN | M0))							\
	MV(CP(CSI21_DX2), (IEN | M0))							\
	MV(CP(CSI21_DY2), (IEN | M0))							\
	MV(CP(CSI21_DX3), (PTD | IEN | M7))						\
	MV(CP(CSI21_DY3), (PTD | IEN | M7))						\
	MV(CP(CSI21_DX4), (PTD | IEN | M7))						\
	MV(CP(CSI21_DY4), (PTD | IEN | M7))						\
	MV(CP(CSI22_DX0), (IEN | M0))							\
	MV(CP(CSI22_DY0), (IEN | M0))							\
	MV(CP(CSI22_DX1), (IEN | M0))							\
	MV(CP(CSI22_DY1), (IEN | M0))							\
	MV(CP(CAM_SHUTTER), (PTD | OFF_EN | OFF_PD | M3))				\
	MV(CP(CAM_STROBE), (OFF_EN | M3))						\
	MV(CP(CAM_GLOBALRESET), (M3))							\
	MV(CP(USBB1_ULPITLL_CLK), (PTD | IEN | OFF_WE | M1))				\
	MV(CP(USBB1_ULPITLL_STP), (IEN | M1))						\
	MV(CP(USBB1_ULPITLL_DIR), (IEN | M1))						\
	MV(CP(USBB1_ULPITLL_NXT), (OFF_EN | M1))					\
	MV(CP(USBB1_ULPITLL_DAT0), (M1))						\
	MV(CP(USBB1_ULPITLL_DAT1), (M1))						\
	MV(CP(USBB1_ULPITLL_DAT2), (M1))						\
	MV(CP(USBB1_ULPITLL_DAT3), (IEN | M1))						\
	MV(CP(USBB1_ULPITLL_DAT4), (IEN | OFF_EN | OFF_IN | OFF_PD | M4))		\
	MV(CP(USBB1_ULPITLL_DAT5), (PTD | IEN | M7))					\
	MV(CP(USBB1_ULPITLL_DAT6), (IEN | OFF_EN | OFF_IN | OFF_PD | M4))		\
	MV(CP(USBB1_ULPITLL_DAT7), (IEN | OFF_EN | OFF_IN | OFF_PD | M4))		\
	MV(CP(USBB1_HSIC_DATA), (M7))							\
	MV(CP(USBB1_HSIC_STROBE), (M7))							\
	MV(CP(USBC1_ICUSB_DP), (PTD | OFF_EN | OFF_PD | M3))				\
	MV(CP(USBC1_ICUSB_DM), (M7))							\
	MV(CP(SDMMC1_CLK), (PTU | OFF_EN | M0))						\
	MV(CP(SDMMC1_CMD), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))			\
	MV(CP(SDMMC1_DAT0), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(SDMMC1_DAT1), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(SDMMC1_DAT2), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(SDMMC1_DAT3), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(SDMMC1_DAT4), (PTD | IEN | M7))						\
	MV(CP(SDMMC1_DAT5), (PTD | IEN | M7))						\
	MV(CP(SDMMC1_DAT6), (PTD | IEN | M7))						\
	MV(CP(SDMMC1_DAT7), (PTD | IEN | M7))						\
	MV(CP(ABE_MCBSP2_CLKX), (IEN | OFF_EN | OFF_IN | OFF_PD | M0))			\
	MV(CP(ABE_MCBSP2_DR), (IEN | OFF_EN | M0))					\
	MV(CP(ABE_MCBSP2_DX), (OFF_EN | M0))						\
	MV(CP(ABE_MCBSP2_FSX), (IEN | OFF_EN | OFF_IN | OFF_PD | M0))			\
	MV(CP(ABE_MCBSP1_CLKX), (IEN | OFF_EN | OFF_IN | OFF_PD | M0))			\
	MV(CP(ABE_MCBSP1_DR), (IEN | OFF_EN | M0))					\
	MV(CP(ABE_MCBSP1_DX), (OFF_EN | M0))						\
	MV(CP(ABE_MCBSP1_FSX), (IEN | OFF_EN | OFF_IN | OFF_PD | M0))			\
	MV(CP(ABE_PDM_UL_DATA), (IEN | M0))						\
	MV(CP(ABE_PDM_DL_DATA), (PTD | IEN | M0))					\
	MV(CP(ABE_PDM_FRAME), (PTU | IEN | M0))						\
	MV(CP(ABE_PDM_LB_CLK), (PTD | IEN | M0))					\
	MV(CP(ABE_CLKS), (PTD | IEN | M0))						\
	MV(CP(ABE_DMIC_CLK1), (PTD | IEN | OFF_WE | M3))				\
	MV(CP(ABE_DMIC_DIN1), (PTD | IEN | OFF_WE | M3))				\
	MV(CP(ABE_DMIC_DIN2), (PTU | OFF_EN | M3))					\
	MV(CP(ABE_DMIC_DIN3), (PTD | M3))						\
	MV(CP(UART2_CTS), (PTU | IEN | M0))						\
	MV(CP(UART2_RTS), (PTU | IEN | M7))						\
	MV(CP(UART2_RX), (IEN | M0))							\
	MV(CP(UART2_TX), (M0))								\
	MV(CP(HDQ_SIO), (M3))								\
	MV(CP(I2C1_SCL), (IEN | M0))							\
	MV(CP(I2C1_SDA), (IEN | M0))							\
	MV(CP(I2C2_SCL), (IEN | M0))							\
	MV(CP(I2C2_SDA), (IEN | M0))							\
	MV(CP(I2C3_SCL), (IEN | M0))							\
	MV(CP(I2C3_SDA), (IEN | M0))							\
	MV(CP(I2C4_SCL), (IEN | M0))							\
	MV(CP(I2C4_SDA), (IEN | M0))							\
	MV(CP(MCSPI1_CLK), (PTD | IEN | M7))						\
	MV(CP(MCSPI1_SOMI), (PTD | IEN | M7))						\
	MV(CP(MCSPI1_SIMO), (PTD | IEN | M7))						\
	MV(CP(MCSPI1_CS0), (PTD | IEN | M7))						\
	MV(CP(MCSPI1_CS1), (PTU | IEN | OFF_WE | M1))					\
	MV(CP(MCSPI1_CS2), (PTD | IEN | M7))						\
	MV(CP(MCSPI1_CS3), (M3))							\
	MV(CP(UART3_CTS_RCTX), (M1))							\
	MV(CP(UART3_RTS_SD), (PTD | IEN | M7))						\
	MV(CP(UART3_RX_IRRX), (M1))							\
	MV(CP(UART3_TX_IRTX), (PTD | IEN | M7))						\
	MV(CP(SDMMC5_CLK), (PTU | IEN | OFF_EN | M0))					\
	MV(CP(SDMMC5_CMD), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))			\
	MV(CP(SDMMC5_DAT0), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(SDMMC5_DAT1), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(SDMMC5_DAT2), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(SDMMC5_DAT3), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | M0))		\
	MV(CP(MCSPI4_CLK), (M7))							\
	MV(CP(MCSPI4_SIMO), (M7))							\
	MV(CP(MCSPI4_SOMI), (M7))							\
	MV(CP(MCSPI4_CS0), (PTD | IEN | M7))						\
	MV(CP(UART4_RX), (PTU | IEN | M0))						\
	MV(CP(UART4_TX), (M0))								\
	MV(CP(USBB2_ULPITLL_CLK), (PTD | OFF_EN | M3))					\
	MV(CP(USBB2_ULPITLL_STP), (PTD | IEN | M3))					\
	MV(CP(USBB2_ULPITLL_DIR), (PTD | IEN | M3))					\
	MV(CP(USBB2_ULPITLL_NXT), (PTD | IEN | OFF_EN | OFF_IN | OFF_PD | OFF_WE | M3))	\
	MV(CP(USBB2_ULPITLL_DAT0), (IEN | M3))						\
	MV(CP(USBB2_ULPITLL_DAT1), (PTD | OFF_EN | OFF_IN | OFF_PD | M3))		\
	MV(CP(USBB2_ULPITLL_DAT2), (PTD | OFF_EN | OFF_IN | OFF_PD | M3))		\
	MV(CP(USBB2_ULPITLL_DAT3), (M3))						\
	MV(CP(USBB2_ULPITLL_DAT4), (PTD | M3))						\
	MV(CP(USBB2_ULPITLL_DAT5), (PTD | M3))						\
	MV(CP(USBB2_ULPITLL_DAT6), (PTD | IEN | OFF_EN | OFF_IN | OFF_WE | M3))		\
	MV(CP(USBB2_ULPITLL_DAT7), (PTD | M3))						\
	MV(CP(USBB2_HSIC_DATA), (M7))							\
	MV(CP(USBB2_HSIC_STROBE), (M7))							\
	MV(CP(UNIPRO_TX0), (OFF_WE | M1))						\
	MV(CP(UNIPRO_TY0), (OFF_IN | OFF_PD | M1))					\
	MV(CP(UNIPRO_TX1), (PTD | IEN | M7))						\
	MV(CP(UNIPRO_TY1), (PTD | IEN | M7))						\
	MV(CP(UNIPRO_TX2), (PTD | OFF_EN | OFF_PD | M3))				\
	MV(CP(UNIPRO_TY2), (OFF_EN | OFF_OUT_PTU | M3))					\
	MV(CP(UNIPRO_RX0), (PTU | IEN | OFF_IN | OFF_PD | M1))				\
	MV(CP(UNIPRO_RY0), (PTU | IEN | OFF_WE | M1))					\
	MV(CP(UNIPRO_RX1), (PTU | IEN | OFF_IN | OFF_PD | M1))				\
	MV(CP(UNIPRO_RY1), (PTD | IEN | M7))						\
	MV(CP(UNIPRO_RX2), (PTD | IEN | M7))						\
	MV(CP(UNIPRO_RY2), (PTD | IEN | M7))						\
	MV(CP(USBA0_OTG_CE), (PTD | M0))						\
	MV(CP(USBA0_OTG_DP), (M0))							\
	MV(CP(USBA0_OTG_DM), (M0))							\
	MV(CP(FREF_CLK1_OUT), (M0))							\
	MV(CP(FREF_CLK2_OUT), (M0))							\
	MV(CP(SYS_NIRQ1), (PTU | IEN | OFF_WE | M0))					\
	MV(CP(SYS_NIRQ2), (PTU | IEN | OFF_WE | M0))					\
	MV(CP(SYS_BOOT0), (M7))								\
	MV(CP(SYS_BOOT1), (IEN | M3))							\
	MV(CP(SYS_BOOT2), (IEN | M3))							\
	MV(CP(SYS_BOOT3), (M7))								\
	MV(CP(SYS_BOOT4), (IEN | M3))							\
	MV(CP(SYS_BOOT5), (M7))								\
	MV(CP(DPM_EMU0), (PTD | OFF_EN | OFF_PD | M3))					\
	MV(CP(DPM_EMU1), (PTD | OFF_EN | OFF_PD | M3))					\
	MV(CP(DPM_EMU2), (PTD | IEN | M3))						\
	MV(CP(DPM_EMU3), (PTU | M3))							\
	MV(CP(DPM_EMU4), (PTD | OFF_EN | OFF_PD | M3))					\
	MV(CP(DPM_EMU5), (IEN | OFF_WE | M3))						\
	MV(CP(DPM_EMU6), (M2))								\
	MV(CP(DPM_EMU7), (IEN | M2))							\
	MV(CP(DPM_EMU8), (M2))								\
	MV(CP(DPM_EMU9), (PTD | IEN | M2))						\
	MV(CP(DPM_EMU10), (PTU | IEN | OFF_EN | OFF_PU | OFF_IN | OFF_WE | M3))		\
	MV(CP(DPM_EMU11), (M7))								\
	MV(CP(DPM_EMU12), (PTD | M3))							\
	MV(CP(DPM_EMU13), (PTD | OFF_EN | OFF_PD | M3))					\
	MV(CP(DPM_EMU14), (PTD | OFF_EN | OFF_PD | M3))					\
	MV(CP(DPM_EMU15), (PTD | IEN | OFF_WE | M3))					\
	MV(CP(DPM_EMU16), (OFF_EN | M3))						\
	MV(CP(DPM_EMU17), (PTD | IEN | M3))						\
	MV(CP(DPM_EMU18), (M7))								\
	MV(CP(DPM_EMU19), (PTD | M3))							\
	MV(CP(CSI22_DX2), (M0))								\
	MV(CP(CSI22_DY2), (M0))								\
	MV1(WK(PAD0_SIM_IO), (M7))							\
	MV1(WK(PAD1_SIM_CLK), (M7))							\
	MV1(WK(PAD0_SIM_RESET), (M7))							\
	MV1(WK(PAD1_SIM_CD), (PTD | M3))						\
	MV1(WK(PAD0_SIM_PWRCTRL), (PTD | IEN | OFF_WE | M3))				\
	MV1(WK(PAD1_SR_SCL), (PTU | IEN | M0))						\
	MV1(WK(PAD0_SR_SDA), (PTU | IEN | M0))						\
	MV1(WK(PAD1_FREF_XTAL_IN), (M7))						\
	MV1(WK(PAD0_FREF_SLICER_IN), (M0))						\
	MV1(WK(PAD1_FREF_CLK_IOREQ), (M0))						\
	MV1(WK(PAD0_FREF_CLK0_OUT), (M3))						\
	MV1(WK(PAD1_FREF_CLK3_REQ), (PTD | M3))						\
	MV1(WK(PAD0_FREF_CLK3_OUT), (PTU | IEN | M3))					\
	MV1(WK(PAD1_FREF_CLK4_REQ), (IEN | M3))						\
	MV1(WK(PAD0_FREF_CLK4_OUT), (PTU | IEN | M3))					\
	MV1(WK(PAD1_SYS_32K), (IEN | M0))						\
	MV1(WK(PAD0_SYS_NRESPWRON), (M0))						\
	MV1(WK(PAD1_SYS_NRESWARM), (M0))						\
	MV1(WK(PAD0_SYS_PWR_REQ), (PTU | M0))						\
	MV1(WK(PAD1_SYS_PWRON_RESET), (PTU | IEN | M3))					\
	MV1(WK(PAD0_SYS_BOOT6), (M7))							\
	MV1(WK(PAD1_SYS_BOOT7), (IEN | M3))						\
	MV1(WK(PAD0_JTAG_NTRST), (PTD | IEN | M0))					\
	/* MV1(WK(0x4A31E06E ), (PTD | IEN | M0)) */					\
	MV1(WK(PAD0_JTAG_RTCK), (PTD | M0))						\
	MV1(WK(PAD1_JTAG_TMS_TMSC), (IEN | M0))						\
	MV1(WK(PAD0_JTAG_TDI), (PTU | IEN | M0))					\
	MV1(WK(PAD1_JTAG_TDO), (PTU | IEN | M0))					\

