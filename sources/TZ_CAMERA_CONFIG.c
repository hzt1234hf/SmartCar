#include "DIY_ComDef.h"
#ifdef TZ_CAMERA_CONFIG

/*
*****2015.5.15 鹰眼摄像头配置模块
*****功能：对鹰眼摄像头进行配置
*****编写中遇到的主要问题：（
    1.需要注意每个寄存器的作用
）
*/

void TZ_OV7725_Init(){

    MCF_GPIO_PANPAR = 0;//设为GPIO口
    MCF_GPIO_DDRAN = 0xff;//全为输出口

    //寄存器，寄存器值次
    SCCB_WriteByte(OV7725_COM4         , 0xC1);
    SCCB_WriteByte(OV7725_CLKRC        , 0x00);

    //SCCB_WriteByte(OV7725_CLKRC        , 0x00);
    SCCB_WriteByte(OV7725_COM2         , 0x03);
    SCCB_WriteByte(OV7725_COM3         , 0xD0);
    SCCB_WriteByte(OV7725_COM7         , 0x40);
    SCCB_WriteByte(OV7725_HSTART       , 0x3F);
    SCCB_WriteByte(OV7725_HSIZE        , 0x50);
    SCCB_WriteByte(OV7725_VSTRT        , 0x03);
    SCCB_WriteByte(OV7725_VSIZE        , 0x78);
    SCCB_WriteByte(OV7725_HREF         , 0x00);
    SCCB_WriteByte(OV7725_SCAL0        , 0x0A);
    SCCB_WriteByte(OV7725_AWB_Ctrl0    , 0xE0);
    SCCB_WriteByte(OV7725_DSPAuto      , 0xff);
    SCCB_WriteByte(OV7725_DSP_Ctrl2    , 0x0C);
    SCCB_WriteByte(OV7725_DSP_Ctrl3    , 0x00);
    SCCB_WriteByte(OV7725_DSP_Ctrl4    , 0x00);
/*

150fps情况下，经过计算80*60的图像每个PCLK为1s/150fps/(80*60) = 1388.88ns，由于鹰眼经过了硬件二值化，
所以一个PCLK脉冲输出8个像素的二值化值，所以实际PCLK应该如下：

  图像大小       帧数     真实PCLK周期  二值化PCLK周期    HREF周期    VSNY周期
  80 * 60       150fps      1388.88ns     11111.11ns      111.11us    6.666ms
  160*120       150fps      347.222ns      2777.77ns      55.55us     6.666ms
  240*180       150fps      154.321ns      1234.56ns      37.03us     6.666ms
  320*240       150fps      86.8055ns      694.444ns      27.77us     6.666ms
  640*480       60fps       54.2534ns      694.444ns      34.72us     16.66ms

*/

#if (CAMERA_W == 80)
    SCCB_WriteByte(OV7725_HOutSize     , 0x14);
#elif (CAMERA_W == 160)
    SCCB_WriteByte(OV7725_HOutSize     , 0x28);
#elif (CAMERA_W == 240)
    SCCB_WriteByte(OV7725_HOutSize     , 0x3c);
#elif (CAMERA_W == 320)
    SCCB_WriteByte(OV7725_HOutSize     , 0x50);
#else

#endif

#if (CAMERA_H == 60 )
    SCCB_WriteByte(OV7725_VOutSize     , 0x1E);
#elif (CAMERA_H == 120 )
    SCCB_WriteByte(OV7725_VOutSize     , 0x3c);
#elif (CAMERA_H == 180 )
    SCCB_WriteByte(OV7725_VOutSize     , 0x5a);
#elif (CAMERA_H == 240 )
    SCCB_WriteByte(OV7725_VOutSize     , 0x78);
#else

#endif

    SCCB_WriteByte(OV7725_EXHCH        , 0x00);
    SCCB_WriteByte(OV7725_GAM1         , 0x0c);
    SCCB_WriteByte(OV7725_GAM2         , 0x16);
    SCCB_WriteByte(OV7725_GAM3         , 0x2a);
    SCCB_WriteByte(OV7725_GAM4         , 0x4e);
    SCCB_WriteByte(OV7725_GAM5         , 0x61);
    SCCB_WriteByte(OV7725_GAM6         , 0x6f);
    SCCB_WriteByte(OV7725_GAM7         , 0x7b);
    SCCB_WriteByte(OV7725_GAM8         , 0x86);
    SCCB_WriteByte(OV7725_GAM9         , 0x8e);
    SCCB_WriteByte(OV7725_GAM10        , 0x97);
    SCCB_WriteByte(OV7725_GAM11        , 0xa4);
    SCCB_WriteByte(OV7725_GAM12        , 0xaf);
    SCCB_WriteByte(OV7725_GAM13        , 0xc5);
    SCCB_WriteByte(OV7725_GAM14        , 0xd7);
    SCCB_WriteByte(OV7725_GAM15        , 0xe8);
    SCCB_WriteByte(OV7725_SLOP         , 0x20);
    SCCB_WriteByte(OV7725_LC_RADI      , 0x00);
    SCCB_WriteByte(OV7725_LC_COEF      , 0x13);
    SCCB_WriteByte(OV7725_LC_XC        , 0x08);
    SCCB_WriteByte(OV7725_LC_COEFB     , 0x14);
    SCCB_WriteByte(OV7725_LC_COEFR     , 0x17);
    SCCB_WriteByte(OV7725_LC_CTR       , 0x05);
    SCCB_WriteByte(OV7725_BDBase       , 0x99);
    SCCB_WriteByte(OV7725_BDMStep      , 0x03);
    SCCB_WriteByte(OV7725_SDE          , 0x04);
    SCCB_WriteByte(OV7725_BRIGHT       , 0x00);

    SCCB_WriteByte(OV7725_CNST         , 50);

    SCCB_WriteByte(OV7725_SIGN         , 0x06);
    SCCB_WriteByte(OV7725_UVADJ0       , 0x11);
    SCCB_WriteByte(OV7725_UVADJ1       , 0x02);
}
#endif