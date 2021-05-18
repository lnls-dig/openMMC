/*
 * Si5341 Rev B Configuration Register Export Header File
 *
 * This file represents a series of Silicon Labs Si5341 Rev B
 * register writes that can be performed to load a single configuration
 * on a device. It was created by a Silicon Labs ClockBuilder Pro
 * export tool.
 *
 * Part:                                               Si5341 Rev B
 * Design ID:                                          AFCZ
 * Includes Pre/Post Download Control Register Writes: Yes
 * Created By:                                         ClockBuilder Pro v2.38 [2019-10-31]
 * Timestamp:                                          2020-01-17 17:15:45 GMT+01:00
 *
 * A complete design report corresponding to this export is included at the end
 * of this header file.
 *
 */

#ifndef SI5341_REVB_REG_CONFIG_HEADER
#define SI5341_REVB_REG_CONFIG_HEADER

#define SI5341_REVB_REG_CONFIG_NUM_REGS             285

typedef struct
{
    unsigned int address; /* 16-bit register address */
    unsigned char value; /* 8-bit register data */

} si5341_revb_register_t;

si5341_revb_register_t const si5341_revb_registers[SI5341_REVB_REG_CONFIG_NUM_REGS] =
{

    /* Start configuration preamble */
    { 0x0B24, 0xD8 },
    { 0x0B25, 0x00 },
    /* Rev B stuck divider fix */
    { 0x0502, 0x01 },
    { 0x0505, 0x03 },
    { 0x0957, 0x1F },
    { 0x0B4E, 0x1A },
    /* End configuration preamble */

    /* Delay 300 msec */
    /*    Delay is worst case time for device to complete any calibration */
    /*    that is running due to device state change previous to this script */
    /*    being processed. */

    /* Start configuration registers */
    { 0x000B, 0x74 },
    { 0x0017, 0x10 },
    { 0x0018, 0xFE },
    { 0x0021, 0x09 },
    { 0x0022, 0x00 },
    { 0x002B, 0x02 },
    { 0x002C, 0x31 },
    { 0x002D, 0x01 },
    { 0x002E, 0x8D },
    { 0x002F, 0x00 },
    { 0x0030, 0x00 },
    { 0x0031, 0x00 },
    { 0x0032, 0x00 },
    { 0x0033, 0x00 },
    { 0x0034, 0x00 },
    { 0x0035, 0x00 },
    { 0x0036, 0x8D },
    { 0x0037, 0x00 },
    { 0x0038, 0x00 },
    { 0x0039, 0x00 },
    { 0x003A, 0x00 },
    { 0x003B, 0x00 },
    { 0x003C, 0x00 },
    { 0x003D, 0x00 },
    { 0x0041, 0x06 },
    { 0x0042, 0x00 },
    { 0x0043, 0x00 },
    { 0x0044, 0x00 },
    { 0x009E, 0x00 },
    { 0x0102, 0x01 },
    { 0x0108, 0x06 },
    { 0x0109, 0x09 },
    { 0x010A, 0x3D },
    { 0x010B, 0x01 },
    { 0x010D, 0x06 },
    { 0x010E, 0x09 },
    { 0x010F, 0x3D },
    { 0x0110, 0x01 },
    { 0x0112, 0x02 },
    { 0x0113, 0x09 },
    { 0x0114, 0x3B },
    { 0x0115, 0x02 },
    { 0x0117, 0x02 },
    { 0x0118, 0x09 },
    { 0x0119, 0x3B },
    { 0x011A, 0x01 },
    { 0x011C, 0x02 },
    { 0x011D, 0x09 },
    { 0x011E, 0x3D },
    { 0x011F, 0x00 },
    { 0x0121, 0x02 },
    { 0x0122, 0x09 },
    { 0x0123, 0x3B },
    { 0x0124, 0x00 },
    { 0x0126, 0x02 },
    { 0x0127, 0x09 },
    { 0x0128, 0x3B },
    { 0x0129, 0x01 },
    { 0x012B, 0x02 },
    { 0x012C, 0x09 },
    { 0x012D, 0x3B },
    { 0x012E, 0x00 },
    { 0x0130, 0x02 },
    { 0x0131, 0x09 },
    { 0x0132, 0x3B },
    { 0x0133, 0x02 },
    { 0x013A, 0x02 },
    { 0x013B, 0x09 },
    { 0x013C, 0x3B },
    { 0x013D, 0x00 },
    { 0x013F, 0x00 },
    { 0x0140, 0x00 },
    { 0x0141, 0x40 },
    { 0x0202, 0x00 },
    { 0x0203, 0x00 },
    { 0x0204, 0x00 },
    { 0x0205, 0x00 },
    { 0x0206, 0x00 },
    { 0x0208, 0x02 },
    { 0x0209, 0x00 },
    { 0x020A, 0x00 },
    { 0x020B, 0x00 },
    { 0x020C, 0x00 },
    { 0x020D, 0x00 },
    { 0x020E, 0x01 },
    { 0x020F, 0x00 },
    { 0x0210, 0x00 },
    { 0x0211, 0x00 },
    { 0x0212, 0x00 },
    { 0x0213, 0x00 },
    { 0x0214, 0x00 },
    { 0x0215, 0x00 },
    { 0x0216, 0x00 },
    { 0x0217, 0x00 },
    { 0x0218, 0x00 },
    { 0x0219, 0x00 },
    { 0x021A, 0x00 },
    { 0x021B, 0x00 },
    { 0x021C, 0x00 },
    { 0x021D, 0x00 },
    { 0x021E, 0x00 },
    { 0x021F, 0x00 },
    { 0x0220, 0x00 },
    { 0x0221, 0x00 },
    { 0x0222, 0x00 },
    { 0x0223, 0x00 },
    { 0x0224, 0x00 },
    { 0x0225, 0x00 },
    { 0x0226, 0x00 },
    { 0x0227, 0x00 },
    { 0x0228, 0x00 },
    { 0x0229, 0x00 },
    { 0x022A, 0x00 },
    { 0x022B, 0x00 },
    { 0x022C, 0x00 },
    { 0x022D, 0x00 },
    { 0x022E, 0x00 },
    { 0x022F, 0x00 },
    { 0x0235, 0x00 },
    { 0x0236, 0x00 },
    { 0x0237, 0x00 },
    { 0x0238, 0x80 },
    { 0x0239, 0x89 },
    { 0x023A, 0x00 },
    { 0x023B, 0x00 },
    { 0x023C, 0x00 },
    { 0x023D, 0x00 },
    { 0x023E, 0x80 },
    { 0x024A, 0x00 },
    { 0x024B, 0x00 },
    { 0x024C, 0x00 },
    { 0x024D, 0x00 },
    { 0x024E, 0x00 },
    { 0x024F, 0x00 },
    { 0x0250, 0x0B },
    { 0x0251, 0x00 },
    { 0x0252, 0x00 },
    { 0x0253, 0x01 },
    { 0x0254, 0x00 },
    { 0x0255, 0x00 },
    { 0x0256, 0x04 },
    { 0x0257, 0x00 },
    { 0x0258, 0x00 },
    { 0x0259, 0x03 },
    { 0x025A, 0x00 },
    { 0x025B, 0x00 },
    { 0x025C, 0x02 },
    { 0x025D, 0x00 },
    { 0x025E, 0x00 },
    { 0x025F, 0x04 },
    { 0x0260, 0x00 },
    { 0x0261, 0x00 },
    { 0x0262, 0x0C },
    { 0x0263, 0x00 },
    { 0x0264, 0x00 },
    { 0x0268, 0x04 },
    { 0x0269, 0x00 },
    { 0x026A, 0x00 },
    { 0x026B, 0x41 },
    { 0x026C, 0x46 },
    { 0x026D, 0x43 },
    { 0x026E, 0x5A },
    { 0x026F, 0x00 },
    { 0x0270, 0x00 },
    { 0x0271, 0x00 },
    { 0x0272, 0x00 },
    { 0x0302, 0x00 },
    { 0x0303, 0x00 },
    { 0x0304, 0x00 },
    { 0x0305, 0x80 },
    { 0x0306, 0x05 },
    { 0x0307, 0x00 },
    { 0x0308, 0x00 },
    { 0x0309, 0x00 },
    { 0x030A, 0x00 },
    { 0x030B, 0x80 },
    { 0x030C, 0x00 },
    { 0x030D, 0x00 },
    { 0x030E, 0x00 },
    { 0x030F, 0x00 },
    { 0x0310, 0x30 },
    { 0x0311, 0x11 },
    { 0x0312, 0x00 },
    { 0x0313, 0x00 },
    { 0x0314, 0x00 },
    { 0x0315, 0x00 },
    { 0x0316, 0xC0 },
    { 0x0317, 0x00 },
    { 0x0318, 0x00 },
    { 0x0319, 0x00 },
    { 0x031A, 0xC0 },
    { 0x031B, 0xB6 },
    { 0x031C, 0x06 },
    { 0x031D, 0x00 },
    { 0x031E, 0x00 },
    { 0x031F, 0x00 },
    { 0x0320, 0x00 },
    { 0x0321, 0x9C },
    { 0x0322, 0x00 },
    { 0x0323, 0x00 },
    { 0x0324, 0x00 },
    { 0x0325, 0x00 },
    { 0x0326, 0x00 },
    { 0x0327, 0x00 },
    { 0x0328, 0x00 },
    { 0x0329, 0x00 },
    { 0x032A, 0x00 },
    { 0x032B, 0x00 },
    { 0x032C, 0x00 },
    { 0x032D, 0x00 },
    { 0x032E, 0x00 },
    { 0x032F, 0x00 },
    { 0x0330, 0x00 },
    { 0x0331, 0x00 },
    { 0x0332, 0x00 },
    { 0x0333, 0x00 },
    { 0x0334, 0x00 },
    { 0x0335, 0x00 },
    { 0x0336, 0x00 },
    { 0x0337, 0x00 },
    { 0x0338, 0x00 },
    { 0x0339, 0x1F },
    { 0x033B, 0x00 },
    { 0x033C, 0x00 },
    { 0x033D, 0x00 },
    { 0x033E, 0x00 },
    { 0x033F, 0x00 },
    { 0x0340, 0x00 },
    { 0x0341, 0x00 },
    { 0x0342, 0x00 },
    { 0x0343, 0x00 },
    { 0x0344, 0x00 },
    { 0x0345, 0x00 },
    { 0x0346, 0x00 },
    { 0x0347, 0x00 },
    { 0x0348, 0x00 },
    { 0x0349, 0x00 },
    { 0x034A, 0x00 },
    { 0x034B, 0x00 },
    { 0x034C, 0x00 },
    { 0x034D, 0x00 },
    { 0x034E, 0x00 },
    { 0x034F, 0x00 },
    { 0x0350, 0x00 },
    { 0x0351, 0x00 },
    { 0x0352, 0x00 },
    { 0x0353, 0x00 },
    { 0x0354, 0x00 },
    { 0x0355, 0x00 },
    { 0x0356, 0x00 },
    { 0x0357, 0x00 },
    { 0x0358, 0x00 },
    { 0x0359, 0x00 },
    { 0x035A, 0x00 },
    { 0x035B, 0x00 },
    { 0x035C, 0x00 },
    { 0x035D, 0x00 },
    { 0x035E, 0x00 },
    { 0x035F, 0x00 },
    { 0x0360, 0x00 },
    { 0x0361, 0x00 },
    { 0x0362, 0x00 },
    { 0x0802, 0x00 },
    { 0x0803, 0x00 },
    { 0x0804, 0x00 },
    { 0x090E, 0x00 },
    { 0x091C, 0x04 },
    { 0x0943, 0x01 },
    { 0x0949, 0x01 },
    { 0x094A, 0x10 },
    { 0x0A02, 0x00 },
    { 0x0A03, 0x07 },
    { 0x0A04, 0x01 },
    { 0x0A05, 0x07 },
    { 0x0B44, 0x0F },
    { 0x0B4A, 0x18 },
    /* End configuration registers */

    /* Start configuration postamble */
    { 0x001C, 0x01 },
    { 0x0B24, 0xDB },
    { 0x0B25, 0x02 },
    /* End configuration postamble */

};

/*
 * Design Report
 *
 * Overview
 * ========
 * Part:               Si5341AB Rev B
 * Project File:       C:\Users\kmacias\Desktop\shared\exar_afcz\Si5341-RevB-AFCZ-Project.slabtimeproj
 * Design ID:          AFCZ
 * Created By:         ClockBuilder Pro v2.38 [2019-10-31]
 * Timestamp:          2020-01-17 17:15:45 GMT+01:00
 *
 * Design Rule Check
 * =================
 * Errors:
 * - No errors
 *
 * Warnings:
 * - OUT2 [52 MHz] and OUT3 [150 MHz] may have coupling [1]
 * - OUT5 [156.25 MHz] and OUT6 [100 MHz] may have coupling [1]
 * - OUT8 [48 MHz] and OUT7 [125 MHz] may have coupling [1]
 * - OUT8 [48 MHz] and OUT9 [125 MHz] may have coupling [1]
 * - Revision B is not recommended for new designs
 *
 * Footnotes:
 * [1] To avoid coupling in outputs, Silicon Labs recommends the following:
 *
 * - Avoid adjacent frequency values that are close. CBPro uses an output's integration bandwidth (IBW) to determine whether two adjacent frequencies are too close. An IBW of 20 MHz is used for frequencies 80 MHz and larger. Lower frequencies will use IBW of OUT/4. CBPro will flag fundamental coupling and coupling up to the fourth harmonic, where coupling frequency = Absolute(OUTa*x - OUTb*y) for all combinations of x and y 1 through 4. If any one of these values is less than or equal to the IBW, the output is flagged as having possible coupling.
 * - Adjacent frequency values that are integer multiples of one another are okay and these outputs should be grouped accordingly. For example, a 155.52 MHz and 622.08 MHz (155.52 x 4) can be adjacent.
 * - Unused outputs can be used to separate clock outputs that might otherwise interfere with one another.
 *
 * Silicon Labs recommends you validate your design's jitter performance using an Evaluation Board. You can request a custom phase noise report for your design from CBPro's design dashboard.
 *
 * Device Grade
 * ============
 * Maximum Output Frequency: 300 MHz
 * Frequency Synthesis Mode: Fractional
 * Frequency Plan Grade:     B
 * Minimum Base OPN:         Si5341B*
 *
 * Base       Output Clock         Supported Frequency Synthesis Modes
 * OPN Grade  Frequency Range      (Typical Jitter)
 * ---------  -------------------  --------------------------------------------
 * Si5341A    100 Hz to 712.5 MHz  Integer (< 100 fs) and fractional (< 150 fs)
 * Si5341B*   100 Hz to 350 MHz    "
 * Si5341C    100 Hz to 712.5 MHz  Integer only (< 100 fs)
 * Si5341D    100 Hz to 350 MHz    "
 *
 * * Based on your calculated frequency plan, a Si5341B grade device is
 * sufficient for your design. For more in-system configuration flexibility
 * (higher frequencies and/or to enable fractional synthesis), consider
 * selecting device grade Si5341A when specifying an ordering part number (OPN)
 * for your application. See the datasheet Ordering Guide for more information.
 *
 * Design
 * ======
 * Host Interface:
 *    I/O Power Supply: VDDA (3.3V)
 *    SPI Mode: 4-Wire
 *    I2C Address Range: 116d to 119d / 0x74 to 0x77 (selected via A0/A1 pins)
 *
 * Inputs:
 *    XAXB: Unused
 *     IN0: 100 MHz
 *          Standard
 *     IN1: Unused
 *     IN2: Unused
 *   FB_IN: Unused
 *
 * Outputs:
 *    OUT0: 300 MHz
 *          Enabled, LVDS 1.8 V
 *    OUT1: 300 MHz
 *          Enabled, LVDS 1.8 V
 *    OUT2: 52 MHz
 *          Enabled, LVDS 2.5 V
 *    OUT3: 150 MHz
 *          Enabled, LVDS 2.5 V
 *    OUT4: 125 MHz
 *          Enabled, LVDS 1.8 V
 *    OUT5: 156.25 MHz
 *          Enabled, LVDS 2.5 V
 *    OUT6: 100 MHz
 *          Enabled, LVDS 2.5 V
 *    OUT7: 125 MHz
 *          Enabled, LVDS 2.5 V
 *    OUT8: 48 MHz
 *          Enabled, LVDS 2.5 V
 *    OUT9: 125 MHz
 *          Enabled, LVDS 2.5 V
 *
 * Frequency Plan
 * ==============
 * Priority: maximize the number of low jitter outputs
 *
 * Fpfd = 50 MHz
 * Fvco = 13.75 GHz
 * Fms0 = 1.25 GHz
 * Fms1 = 600 MHz
 * Fms2 = 1.248 GHz
 *
 * P dividers:
 *    P0  = 2
 *    P1  = Unused
 *    P2  = Unused
 *    P3  = Unused
 *    Pxaxb = Unused
 *
 * M = 275
 * N dividers:
 *    N0:
 *       Value: 11
 *       OUT4: 125 MHz
 *       OUT5: 156.25 MHz [ 156 + 1/4 MHz ]
 *       OUT7: 125 MHz
 *       OUT9: 125 MHz
 *    N1:
 *       Value: 22.9166666666666666... [ 22 + 11/12 ]
 *       OUT0: 300 MHz
 *       OUT1: 300 MHz
 *       OUT3: 150 MHz
 *       OUT6: 100 MHz
 *    N2:
 *       Value: 11.0176282051282051... [ 11 + 11/624 ]
 *       OUT2: 52 MHz
 *       OUT8: 48 MHz
 *    N3:
 *       Unused
 *    N4:
 *       Unused
 *
 * R dividers:
 *    R0 = 2
 *    R1 = 2
 *    R2 = 24
 *    R3 = 4
 *    R4 = 10
 *    R5 = 8
 *    R6 = 6
 *    R7 = 10
 *    R8 = 26
 *    R9 = 10
 *
 * Dividers listed above show effective values. These values are translated to register settings by ClockBuilder Pro. For the actual register values, see below. Refer to the Family Reference Manual for information on registers related to frequency plan.
 *
 * Digitally Controlled Oscillator (DCO)
 * =====================================
 * Mode: FINC/FDEC
 *
 * N0: DCO Disabled
 *
 * N1: DCO Disabled
 *
 * N2: DCO Disabled
 *
 * N3: DCO Disabled
 *
 * N4: DCO Disabled
 *
 * Estimated Power & Junction Temperature
 * ======================================
 * Assumptions:
 *
 * Revision: B
 * VDD:      1.8 V
 * Ta:       25 °C
 * Theta-JA: 18.3 °C/W
 * Airflow:  2 m/s
 *
 * Total Power: 1.081 W, On Chip Power: 1.021 W, Tj: 44 °C
 *
 *          Frequency  Format   Voltage   Current     Power
 *         ----------  ------  --------  --------  --------
 * VDD                            1.8 V  188.3 mA    339 mW
 * VDDA                           3.3 V  116.4 mA    384 mW
 * VDDO0      300 MHz  LVDS       1.8 V   16.4 mA     29 mW
 * VDDO1      300 MHz  LVDS       1.8 V   16.4 mA     29 mW
 * VDDO2       52 MHz  LVDS       2.5 V   15.2 mA     38 mW
 * VDDO3      150 MHz  LVDS       2.5 V   15.7 mA     39 mW
 * VDDO4      125 MHz  LVDS       1.8 V   15.6 mA     28 mW
 * VDDO5   156.25 MHz  LVDS       2.5 V   15.7 mA     39 mW
 * VDDO6      100 MHz  LVDS       2.5 V   15.5 mA     39 mW
 * VDDO7      125 MHz  LVDS       2.5 V   15.6 mA     39 mW
 * VDDO8       48 MHz  LVDS       2.5 V   15.2 mA     38 mW
 * VDDO9      125 MHz  LVDS       2.5 V   15.6 mA     39 mW
 *                                       --------  --------
 *                                Total  461.5 mA   1.081 W
 *
 * Note:
 *
 * -Tj is junction temperature. Tj must be less than 125 °C (on Si5341 Revision B) for device to comply with datasheet specifications. Tj = Ta + Theta_JA*On_Chip_Power.
 * -Overall power includes on-chip power dissipation and adds differential load power dissipation to estimate total power requirements.
 * -Above are estimates only: power and temperature should be measured on your PCB.
 * -Selection of appropriate Theta-JA is required for most accurate estimate. Ideally, select 'User Specified Theta-JA' and enter a Theta-JA value based on the thermal properties of your PCB.
 *
 * Settings
 * ========
 *
 * Location      Setting Name         Decimal Value      Hex Value
 * ------------  -------------------  -----------------  -----------------
 * 0x000B[6:0]   I2C_ADDR             116                0x74
 * 0x0017[0]     SYSINCAL_INTR_MSK    0                  0x0
 * 0x0017[1]     LOSXAXB_INTR_MSK     0                  0x0
 * 0x0017[2]     LOSREF_INTR_MSK      0                  0x0
 * 0x0017[3]     LOL_INTR_MSK         0                  0x0
 * 0x0017[5]     SMB_TMOUT_INTR_MSK   0                  0x0
 * 0x0018[3:0]   LOSIN_INTR_MSK       14                 0xE
 * 0x0021[0]     IN_SEL_REGCTRL       1                  0x1
 * 0x0021[2:1]   IN_SEL               0                  0x0
 * 0x0022[1]     OE                   0                  0x0
 * 0x002B[3]     SPI_3WIRE            0                  0x0
 * 0x002B[5]     AUTO_NDIV_UPDATE     0                  0x0
 * 0x002C[3:0]   LOS_EN               1                  0x1
 * 0x002C[4]     LOSXAXB_DIS          1                  0x1
 * 0x002D[1:0]   LOS0_VAL_TIME        1                  0x1
 * 0x002D[3:2]   LOS1_VAL_TIME        0                  0x0
 * 0x002D[5:4]   LOS2_VAL_TIME        0                  0x0
 * 0x002D[7:6]   LOS3_VAL_TIME        0                  0x0
 * 0x002E[15:0]  LOS0_TRG_THR         141                0x008D
 * 0x0030[15:0]  LOS1_TRG_THR         0                  0x0000
 * 0x0032[15:0]  LOS2_TRG_THR         0                  0x0000
 * 0x0034[15:0]  LOS3_TRG_THR         0                  0x0000
 * 0x0036[15:0]  LOS0_CLR_THR         141                0x008D
 * 0x0038[15:0]  LOS1_CLR_THR         0                  0x0000
 * 0x003A[15:0]  LOS2_CLR_THR         0                  0x0000
 * 0x003C[15:0]  LOS3_CLR_THR         0                  0x0000
 * 0x0041[4:0]   LOS0_DIV_SEL         6                  0x06
 * 0x0042[4:0]   LOS1_DIV_SEL         0                  0x00
 * 0x0043[4:0]   LOS2_DIV_SEL         0                  0x00
 * 0x0044[4:0]   LOS3_DIV_SEL         0                  0x00
 * 0x009E[7:4]   LOL_SET_THR          0                  0x0
 * 0x0102[0]     OUTALL_DISABLE_LOW   1                  0x1
 * 0x0108[0]     OUT0_PDN             0                  0x0
 * 0x0108[1]     OUT0_OE              1                  0x1
 * 0x0108[2]     OUT0_RDIV_FORCE2     1                  0x1
 * 0x0109[2:0]   OUT0_FORMAT          1                  0x1
 * 0x0109[3]     OUT0_SYNC_EN         1                  0x1
 * 0x0109[5:4]   OUT0_DIS_STATE       0                  0x0
 * 0x0109[7:6]   OUT0_CMOS_DRV        0                  0x0
 * 0x010A[3:0]   OUT0_CM              13                 0xD
 * 0x010A[6:4]   OUT0_AMPL            3                  0x3
 * 0x010B[2:0]   OUT0_MUX_SEL         1                  0x1
 * 0x010B[7:6]   OUT0_INV             0                  0x0
 * 0x010D[0]     OUT1_PDN             0                  0x0
 * 0x010D[1]     OUT1_OE              1                  0x1
 * 0x010D[2]     OUT1_RDIV_FORCE2     1                  0x1
 * 0x010E[2:0]   OUT1_FORMAT          1                  0x1
 * 0x010E[3]     OUT1_SYNC_EN         1                  0x1
 * 0x010E[5:4]   OUT1_DIS_STATE       0                  0x0
 * 0x010E[7:6]   OUT1_CMOS_DRV        0                  0x0
 * 0x010F[3:0]   OUT1_CM              13                 0xD
 * 0x010F[6:4]   OUT1_AMPL            3                  0x3
 * 0x0110[2:0]   OUT1_MUX_SEL         1                  0x1
 * 0x0110[7:6]   OUT1_INV             0                  0x0
 * 0x0112[0]     OUT2_PDN             0                  0x0
 * 0x0112[1]     OUT2_OE              1                  0x1
 * 0x0112[2]     OUT2_RDIV_FORCE2     0                  0x0
 * 0x0113[2:0]   OUT2_FORMAT          1                  0x1
 * 0x0113[3]     OUT2_SYNC_EN         1                  0x1
 * 0x0113[5:4]   OUT2_DIS_STATE       0                  0x0
 * 0x0113[7:6]   OUT2_CMOS_DRV        0                  0x0
 * 0x0114[3:0]   OUT2_CM              11                 0xB
 * 0x0114[6:4]   OUT2_AMPL            3                  0x3
 * 0x0115[2:0]   OUT2_MUX_SEL         2                  0x2
 * 0x0115[7:6]   OUT2_INV             0                  0x0
 * 0x0117[0]     OUT3_PDN             0                  0x0
 * 0x0117[1]     OUT3_OE              1                  0x1
 * 0x0117[2]     OUT3_RDIV_FORCE2     0                  0x0
 * 0x0118[2:0]   OUT3_FORMAT          1                  0x1
 * 0x0118[3]     OUT3_SYNC_EN         1                  0x1
 * 0x0118[5:4]   OUT3_DIS_STATE       0                  0x0
 * 0x0118[7:6]   OUT3_CMOS_DRV        0                  0x0
 * 0x0119[3:0]   OUT3_CM              11                 0xB
 * 0x0119[6:4]   OUT3_AMPL            3                  0x3
 * 0x011A[2:0]   OUT3_MUX_SEL         1                  0x1
 * 0x011A[7:6]   OUT3_INV             0                  0x0
 * 0x011C[0]     OUT4_PDN             0                  0x0
 * 0x011C[1]     OUT4_OE              1                  0x1
 * 0x011C[2]     OUT4_RDIV_FORCE2     0                  0x0
 * 0x011D[2:0]   OUT4_FORMAT          1                  0x1
 * 0x011D[3]     OUT4_SYNC_EN         1                  0x1
 * 0x011D[5:4]   OUT4_DIS_STATE       0                  0x0
 * 0x011D[7:6]   OUT4_CMOS_DRV        0                  0x0
 * 0x011E[3:0]   OUT4_CM              13                 0xD
 * 0x011E[6:4]   OUT4_AMPL            3                  0x3
 * 0x011F[2:0]   OUT4_MUX_SEL         0                  0x0
 * 0x011F[7:6]   OUT4_INV             0                  0x0
 * 0x0121[0]     OUT5_PDN             0                  0x0
 * 0x0121[1]     OUT5_OE              1                  0x1
 * 0x0121[2]     OUT5_RDIV_FORCE2     0                  0x0
 * 0x0122[2:0]   OUT5_FORMAT          1                  0x1
 * 0x0122[3]     OUT5_SYNC_EN         1                  0x1
 * 0x0122[5:4]   OUT5_DIS_STATE       0                  0x0
 * 0x0122[7:6]   OUT5_CMOS_DRV        0                  0x0
 * 0x0123[3:0]   OUT5_CM              11                 0xB
 * 0x0123[6:4]   OUT5_AMPL            3                  0x3
 * 0x0124[2:0]   OUT5_MUX_SEL         0                  0x0
 * 0x0124[7:6]   OUT5_INV             0                  0x0
 * 0x0126[0]     OUT6_PDN             0                  0x0
 * 0x0126[1]     OUT6_OE              1                  0x1
 * 0x0126[2]     OUT6_RDIV_FORCE2     0                  0x0
 * 0x0127[2:0]   OUT6_FORMAT          1                  0x1
 * 0x0127[3]     OUT6_SYNC_EN         1                  0x1
 * 0x0127[5:4]   OUT6_DIS_STATE       0                  0x0
 * 0x0127[7:6]   OUT6_CMOS_DRV        0                  0x0
 * 0x0128[3:0]   OUT6_CM              11                 0xB
 * 0x0128[6:4]   OUT6_AMPL            3                  0x3
 * 0x0129[2:0]   OUT6_MUX_SEL         1                  0x1
 * 0x0129[7:6]   OUT6_INV             0                  0x0
 * 0x012B[0]     OUT7_PDN             0                  0x0
 * 0x012B[1]     OUT7_OE              1                  0x1
 * 0x012B[2]     OUT7_RDIV_FORCE2     0                  0x0
 * 0x012C[2:0]   OUT7_FORMAT          1                  0x1
 * 0x012C[3]     OUT7_SYNC_EN         1                  0x1
 * 0x012C[5:4]   OUT7_DIS_STATE       0                  0x0
 * 0x012C[7:6]   OUT7_CMOS_DRV        0                  0x0
 * 0x012D[3:0]   OUT7_CM              11                 0xB
 * 0x012D[6:4]   OUT7_AMPL            3                  0x3
 * 0x012E[2:0]   OUT7_MUX_SEL         0                  0x0
 * 0x012E[7:6]   OUT7_INV             0                  0x0
 * 0x0130[0]     OUT8_PDN             0                  0x0
 * 0x0130[1]     OUT8_OE              1                  0x1
 * 0x0130[2]     OUT8_RDIV_FORCE2     0                  0x0
 * 0x0131[2:0]   OUT8_FORMAT          1                  0x1
 * 0x0131[3]     OUT8_SYNC_EN         1                  0x1
 * 0x0131[5:4]   OUT8_DIS_STATE       0                  0x0
 * 0x0131[7:6]   OUT8_CMOS_DRV        0                  0x0
 * 0x0132[3:0]   OUT8_CM              11                 0xB
 * 0x0132[6:4]   OUT8_AMPL            3                  0x3
 * 0x0133[2:0]   OUT8_MUX_SEL         2                  0x2
 * 0x0133[7:6]   OUT8_INV             0                  0x0
 * 0x013A[0]     OUT9_PDN             0                  0x0
 * 0x013A[1]     OUT9_OE              1                  0x1
 * 0x013A[2]     OUT9_RDIV_FORCE2     0                  0x0
 * 0x013B[2:0]   OUT9_FORMAT          1                  0x1
 * 0x013B[3]     OUT9_SYNC_EN         1                  0x1
 * 0x013B[5:4]   OUT9_DIS_STATE       0                  0x0
 * 0x013B[7:6]   OUT9_CMOS_DRV        0                  0x0
 * 0x013C[3:0]   OUT9_CM              11                 0xB
 * 0x013C[6:4]   OUT9_AMPL            3                  0x3
 * 0x013D[2:0]   OUT9_MUX_SEL         0                  0x0
 * 0x013D[7:6]   OUT9_INV             0                  0x0
 * 0x013F[11:0]  OUTX_ALWAYS_ON       0                  0x000
 * 0x0141[5]     OUT_DIS_LOL_MSK      0                  0x0
 * 0x0141[7]     OUT_DIS_MSK_LOS_PFD  0                  0x0
 * 0x0202[31:0]  XAXB_FREQ_OFFSET     0                  0x00000000
 * 0x0206[1:0]   PXAXB                0                  0x0
 * 0x0208[47:0]  P0                   2                  0x000000000002
 * 0x020E[31:0]  P0_SET               1                  0x00000001
 * 0x0212[47:0]  P1                   0                  0x000000000000
 * 0x0218[31:0]  P1_SET               0                  0x00000000
 * 0x021C[47:0]  P2                   0                  0x000000000000
 * 0x0222[31:0]  P2_SET               0                  0x00000000
 * 0x0226[47:0]  P3                   0                  0x000000000000
 * 0x022C[31:0]  P3_SET               0                  0x00000000
 * 0x0235[43:0]  M_NUM                590558003200       0x08980000000
 * 0x023B[31:0]  M_DEN                2147483648         0x80000000
 * 0x024A[23:0]  R0_REG               0                  0x000000
 * 0x024D[23:0]  R1_REG               0                  0x000000
 * 0x0250[23:0]  R2_REG               11                 0x00000B
 * 0x0253[23:0]  R3_REG               1                  0x000001
 * 0x0256[23:0]  R4_REG               4                  0x000004
 * 0x0259[23:0]  R5_REG               3                  0x000003
 * 0x025C[23:0]  R6_REG               2                  0x000002
 * 0x025F[23:0]  R7_REG               4                  0x000004
 * 0x0262[23:0]  R8_REG               12                 0x00000C
 * 0x0268[23:0]  R9_REG               4                  0x000004
 * 0x026B[7:0]   DESIGN_ID0           65                 0x41
 * 0x026C[7:0]   DESIGN_ID1           70                 0x46
 * 0x026D[7:0]   DESIGN_ID2           67                 0x43
 * 0x026E[7:0]   DESIGN_ID3           90                 0x5A
 * 0x026F[7:0]   DESIGN_ID4           0                  0x00
 * 0x0270[7:0]   DESIGN_ID5           0                  0x00
 * 0x0271[7:0]   DESIGN_ID6           0                  0x00
 * 0x0272[7:0]   DESIGN_ID7           0                  0x00
 * 0x0302[43:0]  N0_NUM               23622320128        0x00580000000
 * 0x0308[31:0]  N0_DEN               2147483648         0x80000000
 * 0x030C[0]     N0_UPDATE            0                  0x0
 * 0x030D[43:0]  N1_NUM               73819750400        0x01130000000
 * 0x0313[31:0]  N1_DEN               3221225472         0xC0000000
 * 0x0317[0]     N1_UPDATE            0                  0x0
 * 0x0318[43:0]  N2_NUM               28835840000        0x006B6C00000
 * 0x031E[31:0]  N2_DEN               2617245696         0x9C000000
 * 0x0322[0]     N2_UPDATE            0                  0x0
 * 0x0323[43:0]  N3_NUM               0                  0x00000000000
 * 0x0329[31:0]  N3_DEN               0                  0x00000000
 * 0x032D[0]     N3_UPDATE            0                  0x0
 * 0x032E[43:0]  N4_NUM               0                  0x00000000000
 * 0x0334[31:0]  N4_DEN               0                  0x00000000
 * 0x0338[0]     N4_UPDATE            0                  0x0
 * 0x0338[1]     N_UPDATE             0                  0x0
 * 0x0339[4:0]   N_FSTEP_MSK          31                 0x1F
 * 0x033B[43:0]  N0_FSTEPW            0                  0x00000000000
 * 0x0341[43:0]  N1_FSTEPW            0                  0x00000000000
 * 0x0347[43:0]  N2_FSTEPW            0                  0x00000000000
 * 0x034D[43:0]  N3_FSTEPW            0                  0x00000000000
 * 0x0353[43:0]  N4_FSTEPW            0                  0x00000000000
 * 0x0359[15:0]  N0_DELAY             0                  0x0000
 * 0x035B[15:0]  N1_DELAY             0                  0x0000
 * 0x035D[15:0]  N2_DELAY             0                  0x0000
 * 0x035F[15:0]  N3_DELAY             0                  0x0000
 * 0x0361[15:0]  N4_DELAY             0                  0x0000
 * 0x0802[15:0]  FIXREGSA0            0                  0x0000
 * 0x0804[7:0]   FIXREGSD0            0                  0x00
 * 0x090E[0]     XAXB_EXTCLK_EN       0                  0x0
 * 0x090E[1]     XAXB_PDNB            0                  0x0
 * 0x091C[2:0]   ZDM_EN               4                  0x4
 * 0x0943[0]     IO_VDD_SEL           1                  0x1
 * 0x0949[3:0]   IN_EN                1                  0x1
 * 0x0949[7:4]   IN_PULSED_CMOS_EN    0                  0x0
 * 0x094A[7:4]   INX_TO_PFD_EN        1                  0x1
 * 0x0A02[4:0]   N_ADD_0P5            0                  0x00
 * 0x0A03[4:0]   N_CLK_TO_OUTX_EN     7                  0x07
 * 0x0A04[4:0]   N_PIBYP              1                  0x01
 * 0x0A05[4:0]   N_PDNB               7                  0x07
 * 0x0B44[3:0]   PDIV_ENB             15                 0xF
 * 0x0B4A[4:0]   N_CLK_DIS            24                 0x18
 *
 *
 */

#endif
