#ifndef TZ_CAMERA_SCCB
#define TZ_CAMERA_SCCB


typedef struct{

	volatile unsigned char PORT7			:	1;
	volatile unsigned char PORT6			:	1;
	volatile unsigned char PORT5			:	1;
	volatile unsigned char PORT4			:	1;
	volatile unsigned char PORT3			:	1;
	volatile unsigned char PORT2			:	1;
	volatile unsigned char PORT1			:	1;
	volatile unsigned char PORT0			:	1;

}__BITS_PANPAR_PORT;

typedef struct{

	volatile unsigned char DDR7			:	1;
	volatile unsigned char DDR6			:	1;
	volatile unsigned char DDR5			:	1;
	volatile unsigned char DDR4			:	1;
	volatile unsigned char DDR3			:	1;
	volatile unsigned char DDR2			:	1;
	volatile unsigned char DDR1			:	1;
	volatile unsigned char DDR0			:	1;

}__BITS_PANPAR_DDR;

typedef struct{

	volatile unsigned char SET7			:	1;
	volatile unsigned char SET6			:	1;
	volatile unsigned char SET5			:	1;
	volatile unsigned char SET4			:	1;
	volatile unsigned char SET3			:	1;
	volatile unsigned char SET2			:	1;
	volatile unsigned char SET1			:	1;
	volatile unsigned char SET0			:	1;

}__BITS_PANPAR_SET;

typedef struct{

	volatile unsigned char CLR7			:	1;
	volatile unsigned char CLR6			:	1;
	volatile unsigned char CLR5			:	1;
	volatile unsigned char CLR4			:	1;
	volatile unsigned char CLR3			:	1;
	volatile unsigned char CLR2			:	1;
	volatile unsigned char CLR1			:	1;
	volatile unsigned char CLR0			:	1;

}__BITS_PANPAR_CLR;
#define GPIO_PORTAN                      ((volatile __BITS_PANPAR_PORT *)(0x4010000A))
#define GPIO_DDRAN                       ((volatile __BITS_PANPAR_DDR *)(0x40100022))
#define GPIO_SETAN                       ((volatile __BITS_PANPAR_SET *)(0x4010003A))
#define GPIO_CLRAN                       ((volatile __BITS_PANPAR_SET *)(0x40100052))

#define GPIO_PORTAN_PORT0_AN0               GPIO_PORTAN->PORT0
#define GPIO_PORTAN_PORT1_AN1               GPIO_PORTAN->PORT1
#define GPIO_PORTAN_PORT2_AN2               GPIO_PORTAN->PORT2
#define GPIO_PORTAN_PORT3_AN3               GPIO_PORTAN->PORT3
#define GPIO_PORTAN_PORT4_AN4               GPIO_PORTAN->PORT4
#define GPIO_PORTAN_PORT5_AN5               GPIO_PORTAN->PORT5
#define GPIO_PORTAN_PORT6_AN6               GPIO_PORTAN->PORT6
#define GPIO_PORTAN_PORT7_AN7               GPIO_PORTAN->PORT7

#define GPIO_PORTAN_SET0_AN0               GPIO_SETAN->SET0
#define GPIO_PORTAN_SET1_AN1               GPIO_SETAN->SET1
#define GPIO_PORTAN_SET2_AN2               GPIO_SETAN->SET2
#define GPIO_PORTAN_SET3_AN3               GPIO_SETAN->SET3
#define GPIO_PORTAN_SET4_AN4               GPIO_SETAN->SET4
#define GPIO_PORTAN_SET5_AN5               GPIO_SETAN->SET5
#define GPIO_PORTAN_SET6_AN6               GPIO_SETAN->SET6
#define GPIO_PORTAN_SET7_AN7               GPIO_SETAN->SET7

#define GPIO_PORTAN_DDR0_AN0                GPIO_DDRAN->DDR0
#define GPIO_PORTAN_DDR1_AN0                GPIO_DDRAN->DDR1
#define GPIO_PORTAN_DDR2_AN0                GPIO_DDRAN->DDR2
#define GPIO_PORTAN_DDR3_AN0                GPIO_DDRAN->DDR3
#define GPIO_PORTAN_DDR4_AN0                GPIO_DDRAN->DDR4
#define GPIO_PORTAN_DDR5_AN0                GPIO_DDRAN->DDR5
#define GPIO_PORTAN_DDR6_AN0                GPIO_DDRAN->DDR6
#define GPIO_PORTAN_DDR7_AN0                GPIO_DDRAN->DDR7


#define W_SDA   GPIO_PORTAN_PORT7_AN7
#define W_SCL   GPIO_PORTAN_PORT6_AN6
#define R_SDA   GPIO_PORTAN_SET7_AN7
#define R_SCL   GPIO_PORTAN_SET6_AN6
#define DDR_SDA GPIO_PORTAN_DDR7_AN0
#define DDR_SCL GPIO_PORTAN_DDR6_AN0

/*
#define W_SDA   GPIO_PORTAN_PORT6_AN6
#define W_SCL   GPIO_PORTAN_PORT7_AN7
#define R_SDA   GPIO_PORTAN_SET6_AN6
#define R_SCL   GPIO_PORTAN_SET7_AN7
#define DDR_SDA GPIO_PORTAN_DDR6_AN0
#define DDR_SCL GPIO_PORTAN_DDR7_AN0
*/
#define SCL_H()         W_SCL=1
#define SCL_L()         W_SCL=0
#define SCL_DDR_OUT()   DDR_SCL=1
#define SCL_DDR_IN()    DDR_SCL=0

#define SDA_H()         W_SDA=1
#define SDA_L()         W_SDA=0
#define SDA_IN()        R_SDA
#define SDA_DDR_OUT()   DDR_SDA=1
#define SDA_DDR_IN()    DDR_SDA=0

//摄像头相关定义

#define ADDR_OV7725   0x42

#define DEV_ADR  ADDR_OV7725             /*设备地址定义*/

#define SCCB_DELAY()    Delay1us(1)


extern int SCCB_WriteByte( uint16 WriteAddress , uint8 SendByte);
extern int SCCB_ReadByte(uint8 *pBuffer,   uint16 length,   uint8 ReadAddress);

#define OV7725_ID        0x21

#define OV7725_GAIN      0x00
#define OV7725_BLUE      0x01
#define OV7725_RED       0x02
#define OV7725_GREEN     0x03
#define OV7725_BAVG      0x05
#define OV7725_GAVG      0x06
#define OV7725_RAVG      0x07
#define OV7725_AECH      0x08
#define OV7725_COM2      0x09
#define OV7725_PID       0x0A
#define OV7725_VER       0x0B
#define OV7725_COM3      0x0C
#define OV7725_COM4      0x0D
#define OV7725_COM5      0x0E
#define OV7725_COM6      0x0F
#define OV7725_AEC       0x10
#define OV7725_CLKRC     0x11
#define OV7725_COM7      0x12
#define OV7725_COM8      0x13
#define OV7725_COM9      0x14
#define OV7725_COM10     0x15
#define OV7725_REG16     0x16
#define OV7725_HSTART    0x17
#define OV7725_HSIZE     0x18
#define OV7725_VSTRT     0x19
#define OV7725_VSIZE     0x1A
#define OV7725_PSHFT     0x1B
#define OV7725_MIDH      0x1C
#define OV7725_MIDL      0x1D
#define OV7725_LAEC      0x1F
#define OV7725_COM11     0x20
#define OV7725_BDBase    0x22
#define OV7725_BDMStep   0x23
#define OV7725_AEW       0x24
#define OV7725_AEB       0x25
#define OV7725_VPT       0x26
#define OV7725_REG28     0x28
#define OV7725_HOutSize  0x29
#define OV7725_EXHCH     0x2A
#define OV7725_EXHCL     0x2B
#define OV7725_VOutSize  0x2C
#define OV7725_ADVFL     0x2D
#define OV7725_ADVFH     0x2E
#define OV7725_YAVE      0x2F
#define OV7725_LumHTh    0x30
#define OV7725_LumLTh    0x31
#define OV7725_HREF      0x32
#define OV7725_DM_LNL    0x33
#define OV7725_DM_LNH    0x34
#define OV7725_ADoff_B   0x35
#define OV7725_ADoff_R   0x36
#define OV7725_ADoff_Gb  0x37
#define OV7725_ADoff_Gr  0x38
#define OV7725_Off_B     0x39
#define OV7725_Off_R     0x3A
#define OV7725_Off_Gb    0x3B
#define OV7725_Off_Gr    0x3C
#define OV7725_COM12     0x3D
#define OV7725_COM13     0x3E
#define OV7725_COM14     0x3F
#define OV7725_COM16     0x41
#define OV7725_ANT_B     0x42
#define OV7725_ANT_R     0x43
#define OV7725_ANT_Gb    0x44
#define OV7725_ANT_Gr    0x45
#define OV7725_LC_CTR    0x46
#define OV7725_LC_XC     0x47
#define OV7725_LC_YC     0x48
#define OV7725_LC_COEF   0x49
#define OV7725_LC_RADI   0x4A
#define OV7725_LC_COEFB  0x4B
#define OV7725_LC_COEFR  0x4C
#define OV7725_FixGain   0x4D
#define OV7725_AREF1     0x4F
#define OV7725_AREF6     0x54
#define OV7725_UFix      0x60
#define OV7725_VFix      0x61
#define OV7725_AWBb_blk  0x62
#define OV7725_AWB_Ctrl0 0x63
#define OV7725_DSP_Ctrl1 0x64
#define OV7725_DSP_Ctrl2 0x65
#define OV7725_DSP_Ctrl3 0x66
#define OV7725_DSP_Ctrl4 0x67
#define OV7725_AWB_bias  0x68
#define OV7725_AWBCtrl1  0x69
#define OV7725_AWBCtrl2  0x6A
#define OV7725_AWBCtrl3  0x6B
#define OV7725_AWBCtrl4  0x6C
#define OV7725_AWBCtrl5  0x6D
#define OV7725_AWBCtrl6  0x6E
#define OV7725_AWBCtrl7  0x6F
#define OV7725_AWBCtrl8  0x70
#define OV7725_AWBCtrl9  0x71
#define OV7725_AWBCtrl10 0x72
#define OV7725_AWBCtrl11 0x73
#define OV7725_AWBCtrl12 0x74
#define OV7725_AWBCtrl13 0x75
#define OV7725_AWBCtrl14 0x76
#define OV7725_AWBCtrl15 0x77
#define OV7725_AWBCtrl16 0x78
#define OV7725_AWBCtrl17 0x79
#define OV7725_AWBCtrl18 0x7A
#define OV7725_AWBCtrl19 0x7B
#define OV7725_AWBCtrl20 0x7C
#define OV7725_AWBCtrl21 0x7D
#define OV7725_GAM1      0x7E
#define OV7725_GAM2      0x7F
#define OV7725_GAM3      0x80
#define OV7725_GAM4      0x81
#define OV7725_GAM5      0x82
#define OV7725_GAM6      0x83
#define OV7725_GAM7      0x84
#define OV7725_GAM8      0x85
#define OV7725_GAM9      0x86
#define OV7725_GAM10     0x87
#define OV7725_GAM11     0x88
#define OV7725_GAM12     0x89
#define OV7725_GAM13     0x8A
#define OV7725_GAM14     0x8B
#define OV7725_GAM15     0x8C
#define OV7725_SLOP      0x8D
#define OV7725_DNSTh     0x8E
#define OV7725_EDGE0     0x8F
#define OV7725_EDGE1     0x90
#define OV7725_DNSOff    0x91
#define OV7725_EDGE2     0x92
#define OV7725_EDGE3     0x93
#define OV7725_MTX1      0x94
#define OV7725_MTX2      0x95
#define OV7725_MTX3      0x96
#define OV7725_MTX4      0x97
#define OV7725_MTX5      0x98
#define OV7725_MTX6      0x99
#define OV7725_MTX_Ctrl  0x9A
#define OV7725_BRIGHT    0x9B
#define OV7725_CNST      0x9C
#define OV7725_UVADJ0    0x9E
#define OV7725_UVADJ1    0x9F
#define OV7725_SCAL0     0xA0
#define OV7725_SCAL1     0xA1
#define OV7725_SCAL2     0xA2
#define OV7725_SDE       0xA6
#define OV7725_USAT      0xA7
#define OV7725_VSAT      0xA8
#define OV7725_HUECOS    0xA9
#define OV7725_HUESIN    0xAA
#define OV7725_SIGN      0xAB
#define OV7725_DSPAuto   0xAC

#endif