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
MV(CP(GPMC_AD8),	M7)		/* gpio_32	*/	\
MV(CP(GPMC_AD9),	M7)		/* gpio_33	*/	\
MV(CP(GPMC_AD10),	M7)		/* gpio_34	*/	\
MV(CP(GPMC_AD11),	M7)		/* gpio_35	*/	\
MV(CP(GPMC_AD12),	M7)		/* gpio_36	*/	\
MV(CP(GPMC_AD13),	M7)		/* gpio_37	*/	\
MV(CP(GPMC_AD14),	M7)		/* gpio_38	*/	\
MV(CP(GPMC_AD15),	M7)		/* gpio_39	*/	\
MV(CP(GPMC_A16),	PTU | IEN | M3)			/* /MICROSD_DET			*/	\
MV(CP(GPMC_A17),	PTU | M3)				/* HDMI_LS_OE			*/	\
MV(CP(GPMC_A18),	PTU | IEN | M3)			/* /MICROSD_COVER_DET	*/	\
MV(CP(GPMC_A19),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* 3D_BOOST_EN */ \
MV(CP(GPMC_A20),	M7)		/* gpio_44	*/	\
MV(CP(GPMC_A21),	M7)		/* gpio_45	*/	\
MV(CP(GPMC_A22),	M7)		/* gpio_46	*/	\
MV(CP(GPMC_A23),	M7)		/* gpio_47	*/	\
MV(CP(GPMC_A24),	M7)		/* gpio_48	*/	\
MV(CP(GPMC_A25),	M7)		/* gpio_49	*/	\
MV(CP(GPMC_NCS0),	M7)		/* gpio_50	*/	\
MV(CP(GPMC_NCS1),	PTU | IEN | M3)				/* /CHG_STATUS	*/	\
MV(CP(GPMC_NCS2),	PTU | IEN | M3)				/* TOUCH_INT_N	*/	\
MV(CP(GPMC_NCS3),	M7)		/* gpio_53	*/	\
MV(CP(GPMC_NWP),	M7)		/* gpio_54	*/	\
MV(CP(GPMC_CLK),		PTD | M3)  /* IFX_USB_VBUS_EN	*/ \
MV(CP(GPMC_NADV_ALE),	M7)	/* gpio_56	*/	\
MV(CP(GPMC_NOE),	PTU | IEN | OFF_EN | OFF_OUT_PTD | M1)		/* SDMMC2_CLK	*/ \
MV(CP(GPMC_NWE),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1)	/* SDMMC2_CMD	*/ \
MV(CP(GPMC_NBE0_CLE),	M7)	/* gpio_59	*/	\
MV(CP(GPMC_NBE1),	PTU | IEN | M3)			/* HDMI_CT_CP_HPD	*/	\
MV(CP(GPMC_WAIT0),	M7)		/* gpio_61	*/	\
MV(CP(GPMC_WAIT1),	M7)		/* gpio_62	*/	\
MV(CP(C2C_DATA11),	M7)		/* gpio_100	*/	\
MV(CP(C2C_DATA12),	M7)		/* gpio_101	*/	\
MV(CP(C2C_DATA13),	M7)		/* gpio_102 */	\
MV(CP(C2C_DATA14),	M7)		/* gpio_103	*/	\
MV(CP(C2C_DATA15),		PTD | M3)			/* PROXI_LDO_EN */	\
MV(CP(HDMI_HPD),	M0)						/* HDMI_HPD		*/	\
MV(CP(HDMI_CEC),	M0)						/* HDMI_CEC		*/	\
MV(CP(HDMI_DDC_SCL),	PTU | M0)			/* HDMI_SCL		*/	\
MV(CP(HDMI_DDC_SDA),	PTU | IEN | M0)		/* HDMI_SDA		*/	\
MV(CP(CSI21_DX0),	IEN | M0)	/* PRIMARY_5M_MIPI_CLK_P	*/	\
MV(CP(CSI21_DY0),	IEN | M0)	/* PRIMARY_5M_MIPI_CLK_N	*/	\
MV(CP(CSI21_DX1),	IEN | M0)	/* PRIMARY_5M_MIPI_DATA0_P	*/	\
MV(CP(CSI21_DY1),	IEN | M0)	/* PRIMARY_5M_MIPI_DATA0_N	*/	\
MV(CP(CSI21_DX2),	IEN | M0)	/* PRIMARY_5M_MIPI_DATA1_P	*/	\
MV(CP(CSI21_DY2),	IEN | M0)	/* PRIMARY_5M_MIPI_DATA1_N	*/	\
MV(CP(CSI21_DX3),	PTD | M0)	/* VT_0.3M_MIPI_CLK_P		*/	\
MV(CP(CSI21_DY3),	PTD | M0)	/* VT_0.3M_MIPI_CLK_N		*/	\
MV(CP(CSI21_DX4),	IEN | M0)	/* VT_0.3M_MIPI_DATA0_P		*/	\
MV(CP(CSI21_DY4),	IEN | M0)	/* VT_0.3M_MIPI_DATA0_N		*/	\
MV(CP(CSI22_DX0),	IEN | M0)	/* SECONDARY_5M_MIPI_CLK_P	*/	\
MV(CP(CSI22_DY0),	IEN | M0)	/* SECONDARY_5M_MIPI_CLK_N	*/	\
MV(CP(CSI22_DX1),	IEN | M0)	/* SECONDARY_5M_MIPI_DATA0_P	*/ \
MV(CP(CSI22_DY1),	IEN | M0)	/* SECONDARY_5M_MIPI_DATA0_N	*/ \
MV(CP(CAM_SHUTTER),	IEN | M3)	/* GYRO_FSYNC	*/ \
MV(CP(CAM_STROBE),	PTD | M3)	/* FRONT_KEY_LED_EN	*/ \
MV(CP(CAM_GLOBALRESET),		PTD | M3)	/* /CHG_EN_SET	*/ \
MV(CP(USBB1_ULPITLL_CLK),	M7)		/* gpio_84	*/	\
MV(CP(USBB1_ULPITLL_STP),	M7)		/* gpio_85	*/	\
MV(CP(USBB1_ULPITLL_DIR),	M7)		/* gpio_86	*/	\
MV(CP(USBB1_ULPITLL_NXT),	M7)		/* gpio_87	*/	\
MV(CP(USBB1_ULPITLL_DAT0),	IEN | M3)										/* MP_AP_WAKE		*/	\
MV(CP(USBB1_ULPITLL_DAT1),	M3)												/* AP_MP_WAKE		*/	\
MV(CP(USBB1_ULPITLL_DAT2),	M7)		/* gpio_90	*/	\
MV(CP(USBB1_ULPITLL_DAT3),	IEN | OFF_EN | OFF_PD | OFF_IN | M3)			/* BAT_REMOVAL		*/	\
	MV(CP(USBB1_ULPITLL_DAT4) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))		/* FM_I2S_DI		*/	\
	MV(CP(USBB1_ULPITLL_DAT5) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))		/* FM_I2S_DO		*/	\
	MV(CP(USBB1_ULPITLL_DAT6) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))		/* FM_I2S_CLK		*/	\
	MV(CP(USBB1_ULPITLL_DAT7) , ( IEN | OFF_EN | OFF_PD | OFF_IN | M4))		/* FM_I2S_FSYNC		*/	\
MV(CP(USBB1_HSIC_DATA),		M7)		/* gpio_96	*/	\
MV(CP(USBB1_HSIC_STROBE),	M7)		/* gpio_97	*/	\
MV(CP(USBC1_ICUSB_DP),	PTU | OFF_EN | OFF_PU | OFF_OUT_PTU | M3)		/* LCD_CP_EN		*/	\
MV(CP(USBC1_ICUSB_DM),	PTU | OFF_EN | OFF_PU | OFF_OUT_PTU | M3)		/* CAM_SUBPN_EN		*/	\
MV(CP(SDMMC1_CLK),		PTU | OFF_EN | OFF_OUT_PTD | M0)				/* MICROSD_CLK		*/	\
MV(CP(SDMMC1_CMD),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_CMD		*/	\
MV(CP(SDMMC1_DAT0),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT0		*/	\
MV(CP(SDMMC1_DAT1),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT1		*/	\
MV(CP(SDMMC1_DAT2),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT2		*/	\
MV(CP(SDMMC1_DAT3),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MICROSD_DAT3		*/	\
MV(CP(SDMMC1_DAT4),		M7)		/* gpio_106	*/	\
MV(CP(SDMMC1_DAT5),		M7)		/* gpio_107	*/	\
MV(CP(SDMMC1_DAT6),		M7)		/* gpio_108	*/	\
MV(CP(SDMMC1_DAT7),		M7)		/* gpio_109	*/	\
	MV(CP(ABE_MCBSP2_CLKX),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* IPC_I2S_CLK	*/	\
	MV(CP(ABE_MCBSP2_DR),	IEN | OFF_EN | OFF_OUT_PTD | M0)		/* IPC_I2S_DOUT	*/	\
	MV(CP(ABE_MCBSP2_DX),	OFF_EN | OFF_OUT_PTD | M0)				/* IPC_I2S_DIN	*/	\
	MV(CP(ABE_MCBSP2_FSX),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* IPC_I2S_SYNC	*/	\
	MV(CP(ABE_MCBSP1_CLKX),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)								/* BT_PCM_CLK	*/	\
	MV(CP(ABE_MCBSP1_DR),	IEN | OFF_EN | OFF_OUT_PTD | M0)								/* BT_PCM_DIN	*/	\
	MV(CP(ABE_MCBSP1_DX),	OFF_EN | OFF_OUT_PTD | M0)				/* BT_PCM_DOUT	*/	\
	MV(CP(ABE_MCBSP1_FSX),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* BT_PCM_SYNC	*/	\
MV(CP(ABE_PDM_UL_DATA),	PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* PDM_UL_DATA	*/	\
MV(CP(ABE_PDM_DL_DATA),	PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* PDM_DL_DATA	*/	\
MV(CP(ABE_PDM_FRAME),	PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* PDM_FRAME	*/	\
MV(CP(ABE_PDM_LB_CLK),	PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* PDM_CLK		*/	\
MV(CP(ABE_CLKS),		PTD | IEN | OFF_EN | OFF_PD | OFF_IN | M0)	/* ABE_CLKS		*/	\
	MV(CP(ABE_DMIC_CLK1),	M0)			/* IPC_SRDY			*/	\
	MV(CP(ABE_DMIC_DIN1),	IEN | M0)	/* IPC_MRDY			*/	\
	MV(CP(ABE_DMIC_DIN2),	IEN | M0)	/* IPC_MODEM_SEND	*/	\
	MV(CP(ABE_DMIC_DIN3),	IEN | M0)	/* OMAP_SEND	*/	\
MV(CP(UART2_CTS),		PTU | IEN | M0)								/* /BT_UART_CTS		*/	\
MV(CP(UART2_RTS),		M0)											/* /BT_UART_RTS		*/	\
MV(CP(UART2_RX),		PTU | IEN | M0)								/* BT_UART_RXD		*/	\
MV(CP(UART2_TX),		M0)											/* BT_UART_TXD		*/	\
MV(CP(HDQ_SIO),			M3)											/* AUD_PWRON		*/	\
MV(CP(I2C1_SCL),		PTU | IEN | EN | M0)  						/* I2C1_SCL			*/	\
MV(CP(I2C1_SDA),		PTU | IEN | EN | M0)  						/* I2C1_SDA			*/	\
MV(CP(I2C2_SCL),		PTU | IEN | EN | M0)  						/* I2C2_SCL			*/	\
MV(CP(I2C2_SDA),		PTU | IEN | EN | M0)  						/* I2C2_SDA			*/	\
MV(CP(I2C3_SCL),		PTU | IEN | EN | M0)  						/* I2C3_SCL			*/	\
MV(CP(I2C3_SDA),		PTU | IEN | EN | M0)  						/* I2C3_SDA			*/	\
MV(CP(I2C4_SCL),		PTU | IEN | EN | M0)  						/* I2C4_SCL			*/	\
MV(CP(I2C4_SDA),		PTU | IEN | EN | M0)  						/* I2C4_SDA			*/	\
MV(CP(MCSPI1_CLK),		M7)		/* gpio_134	*/	\
MV(CP(MCSPI1_SOMI),		M7)		/* gpio_135	*/	\
MV(CP(MCSPI1_SIMO),		M7)		/* gpio_136	*/	\
MV(CP(MCSPI1_CS0),		M7)		/* gpio_137	*/	\
MV(CP(MCSPI1_CS1),		PTU | IEN |  M1)		/* UART1_RX_IPC		*/	\
MV(CP(MCSPI1_CS2),		PTU | OFF_EN | OFF_PD | OFF_OUT_PTU |	M3)	/* 3D_LCD_BANK_SEL	*/	\
MV(CP(MCSPI1_CS3),		PTD | 									M3)	/*	GPS_LNA_SD		*/	\
MV(CP(UART3_CTS_RCTX) , M1)									/* UART1_TX_IPC		*/	\
MV(CP(UART3_RTS_SD),	M7)		/* gpio_142	*/	\
MV(CP(UART3_RX_IRRX),	M1)											/* VIB_PWM			*/	\
MV(CP(UART3_TX_IRTX),	PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)	/* 3D_LCD_EN		*/	\
MV(CP(SDMMC5_CLK),		PTU | IEN | OFF_EN | OFF_OUT_PTD | M0)		/* WLAN_CLK			*/	\
MV(CP(SDMMC5_CMD),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)  /* WLAN_CMD			*/	\
MV(CP(SDMMC5_DAT0),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)  /* WLAN_SDIO[0]		*/	\
MV(CP(SDMMC5_DAT1),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)  /* WLAN_SDIO[1]		*/	\
MV(CP(SDMMC5_DAT2),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)  /* WLAN_SDIO[2]		*/	\
MV(CP(SDMMC5_DAT3),		PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M0)  /* WLAN_SDIO[3]		*/	\
MV(CP(MCSPI4_CLK),		IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MCSPI4_CLK		*/	\
MV(CP(MCSPI4_SIMO),		IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MCSPI4_SIMO		*/	\
MV(CP(MCSPI4_SOMI),		IEN | OFF_EN | OFF_PD | OFF_IN | M0)		/* MCSPI4_SOMI		*/	\
MV(CP(MCSPI4_CS0),		M7)		/* gpio_154	*/	\
MV(CP(UART4_RX),		IEN | M0)									/* UART4_RX_OMAP	*/	\
MV(CP(UART4_TX),		M0)											/* UART4_RX_OMAP	*/	\
MV(CP(USBB2_ULPITLL_CLK),	M7)		/* gpio_157 */	\
MV(CP(USBB2_ULPITLL_STP),	M7)		/* gpio_158	*/	\
MV(CP(USBB2_ULPITLL_DIR),	M7)		/* gpio_159	*/	\
MV(CP(USBB2_ULPITLL_NXT),	M7)		/* gpio_160	*/	\
MV(CP(USBB2_ULPITLL_DAT0),	M7)		/* gpio_161	*/	\
MV(CP(USBB2_ULPITLL_DAT1),	M7)		/* gpio_162	*/	\
MV(CP(USBB2_ULPITLL_DAT2),	M7)		/* gpio_163	*/	\
MV(CP(USBB2_ULPITLL_DAT3),	PTU | M3)									/* RESET_PMU_N		*/	\
	MV(CP(USBB2_ULPITLL_DAT4) , ( IEN | M5))  /* dispc2_data14 */ \
MV(CP(USBB2_ULPITLL_DAT5),	PTD | M3)									/* BT_EN			*/	\
MV(CP(USBB2_ULPITLL_DAT6),	PTD | IEN | M3)								/* WLAN_HOST_WAKEUP	*/	\
MV(CP(USBB2_ULPITLL_DAT7),	PTD | M3)									/* WLAN_EN			*/	\
MV(CP(USBB2_HSIC_DATA),		M7)		/* gpio_169	*/	\
MV(CP(USBB2_HSIC_STROBE),	M7)		/* gpio_170	*/	\
MV(CP(UNIPRO_TX0),		OFF_EN | OFF_PD | OFF_IN | M1)					/* KEY_C[0]			*/	\
MV(CP(UNIPRO_TY0),		OFF_EN | OFF_PD | OFF_IN | M1)					/* KEY_C[1]			*/	\
MV(CP(UNIPRO_TX1),		M7)  /* kpd_col2 */ \
MV(CP(UNIPRO_TY1),		M7)  /* kpd_col3 */ \
MV(CP(UNIPRO_TX2),		PTD	| OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* GPS_PWR_ON		*/	\
MV(CP(UNIPRO_TY2),		PTD	| OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* GPS_RESET_N		*/	\
MV(CP(UNIPRO_RX0),		PTU	| IEN |	OFF_EN | OFF_PD | OFF_IN | M1)		/* KEY_R[0]			*/	\
MV(CP(UNIPRO_RY0),		PTU | IEN |	OFF_EN | OFF_PD | OFF_IN | M1)		/* KEY_R[1]			*/	\
MV(CP(UNIPRO_RX1),		M7)		/* kpd_row2 */	\
MV(CP(UNIPRO_RY1),		M7)		/* kpd_row3 */	\
MV(CP(UNIPRO_RX2),		M7)		/* kpd_row4 */	\
MV(CP(UNIPRO_RY2),		M7)		/* kpd_row5 */	\
MV(CP(USBA0_OTG_CE),	M7)		/* usba0_otg_ce */	\
MV(CP(USBA0_OTG_DP),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)			/* USB_DP_OMAP		*/	\
MV(CP(USBA0_OTG_DM),	IEN | OFF_EN | OFF_PD | OFF_IN | M0)			/* USB_DM_OMAP		*/	\
MV(CP(FREF_CLK1_OUT),	M0)												/* 5M_MCLK			*/	\
MV(CP(FREF_CLK2_OUT),	M7)		/* fref_clk2_out */ \
MV(CP(SYS_NIRQ1),		OFF_WE | PTU | IEN | M0)									/* SYS_nIRQ1		*/	\
MV(CP(SYS_NIRQ2),		OFF_WE | PTU | IEN | M0)									/* SYS_nIRQ2		*/	\
MV(CP(SYS_BOOT0),		M7)		/* gpio_184 */ \
MV(CP(SYS_BOOT1),		IEN | M3)										/* SYS_BOOT1		*/	\
MV(CP(SYS_BOOT2),		IEN | M3)										/* SYS_BOOT2		*/	\
MV(CP(SYS_BOOT3),		M7)		/* gpio_187 */ \
MV(CP(SYS_BOOT4),		IEN | M3)										/* SYS_BOOT4		*/	\
MV(CP(SYS_BOOT5),		M7)		/* gpio_189 */ \
MV(CP(DPM_EMU0),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* UART_SW1			*/	\
MV(CP(DPM_EMU1),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* UART_SW2			*/	\
MV(CP(DPM_EMU2),		PTU | IEN	| M3	)		/* COMPASS_INT			*/	\
MV(CP(DPM_EMU3),		PTU | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* COMPASS_RESET		*/	\
MV(CP(DPM_EMU4),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* SECONDARY_5M_RESET_N	*/	\
MV(CP(DPM_EMU5),		PTU | IEN	| M3	)							/* PROXI_INT		*/	\
MV(CP(DPM_EMU6),		(PTU | M2)) 									/* GPS_UART_TXD		*/	\
MV(CP(DPM_EMU7),		(PTU | IEN | M2))								/* GPS_UART_RXD		*/	\
MV(CP(DPM_EMU8),		(PTD | M2))  									/* GPS_UART_RTS_N	*/	\
MV(CP(DPM_EMU9),		(PTD | IEN | M2))								/* GPS_UART_CTS_N	*/	\
MV(CP(DPM_EMU10),		M7)		/* gpio_21	*/	\
MV(CP(DPM_EMU11),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* VT_CAM_PWDN			*/	\
MV(CP(DPM_EMU12),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* VT_CAM_RESET			*/	\
MV(CP(DPM_EMU13),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* PRIMARY_5M_RESET_N	*/	\
MV(CP(DPM_EMU14),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* VIB_EN				*/	\
MV(CP(DPM_EMU15),		PTU | IEN | M3)									/* CP_CRASH_INT			*/	\
MV(CP(DPM_EMU16),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* LCD_EN				*/	\
MV(CP(DPM_EMU17),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* MOVIE_MODE_EN		*/	\
MV(CP(DPM_EMU18),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* DSI2_TE0				*/	\
MV(CP(DPM_EMU19),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)		/* FLASH_EN				*/	\
MV1(WK(PAD0_SIM_IO),	M7)		/* gpio_wk0	*/	\
MV1(WK(PAD1_SIM_CLK),			PTU | IEN | M3)								/* GYRO_INT			*/	\
MV1(WK(PAD0_SIM_RESET),	M7)		/* gpio_wk2	*/	\
MV1(WK(PAD1_SIM_CD),			PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)	/* /IFX_PWR_ON_SW	*/	\
MV1(WK(PAD0_SIM_PWRCTRL),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)	/* GSM_TXON_IND		*/	\
MV1(WK(PAD1_SR_SCL),			PTU | IEN | M0)  							/* SRI2C_SCL		*/	\
MV1(WK(PAD0_SR_SDA),			PTU | IEN | M0)								/* SRI2C_SDA		*/	\
MV1(WK(PAD1_FREF_XTAL_IN),		M7)		/* freq_xtal_in	*/	\
MV1(WK(PAD0_FREF_SLICER_IN),	M0)											/* OMAP_CLK_IN		*/	\
MV1(WK(PAD1_FREF_CLK_IOREQ),	M0)											/* OMAP_CLK_REQ		*/	\
MV1(WK(PAD0_FREF_CLK0_OUT),		M2)											/* SYS_DRM_MSEC		*/	\
MV1(WK(PAD1_FREF_CLK3_REQ),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)	/* LCD_RESET_N		*/	\
MV1(WK(PAD0_FREF_CLK3_OUT),		M0)											/* VT_0.3M_MCLK		*/	\
MV1(WK(PAD1_FREF_CLK4_REQ),		PTU | IEN | M3)								/* /MUIC_INT		*/	\
MV1(WK(PAD0_FREF_CLK4_OUT),		M7)		/* gpio_wk8	*/	\
MV1(WK(PAD1_SYS_32K),			IEN | M0)									/* CLK32_MAIN		*/	\
MV1(WK(PAD0_SYS_NRESPWRON),		M0)											/* SYS_nRESPWNON	*/	\
MV1(WK(PAD1_SYS_NRESWARM),		M0)											/* SYS_nRESWARM		*/	\
MV1(WK(PAD0_SYS_PWR_REQ),		PTU | M0)									/* SYS_PWRREQ		*/	\
MV1(WK(PAD1_SYS_PWRON_RESET),	PTU | IEN | M3)								/* MOTION_INT		*/	\
MV1(WK(PAD0_SYS_BOOT6),			M7)		/* gpio_wk9 */ \
MV1(WK(PAD1_SYS_BOOT7),			IEN | M3)									/* SYS_BOOT7		*/

#if	0
//	MV1(WK(PAD0_JTAG_NTRST) , ( IEN | M0))  /* jtag_ntrst */ \
	MV1(WK(PAD1_JTAG_TCK) , ( IEN | M0))  /* jtag_tck */ \
	MV1(WK(PAD0_JTAG_RTCK) , ( M0))  /* jtag_rtck */ \
	MV1(WK(PAD1_JTAG_TMS_TMSC) , ( IEN | M0))  /* jtag_tms_tmsc */ \
	MV1(WK(PAD0_JTAG_TDI) , ( IEN | M0))  /* jtag_tdi */ \
	MV1(WK(PAD1_JTAG_TDO) , ( M0))  /* jtag_tdo */ 
#endif
