/*
 * main implementation: use this sample to create your own application
 *
 */
#include "DIY_ComDef.h"
uint32 sysOsciFre = 128000000;//默认晶振频率
uint8 enCoder = 0;      //编码器值
uint16 colorCnt;        //获取颜色周期计数
uint16 color3Cnt[3];    //颜色计数值
uint16 colorSZCnt;      //获取颜色计数
//uint8 RGB[3];
uint8 startend = 0;

void TZ_PLLinit(uint8 mode){
    switch(mode){
        case 1:{//80M
            MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(3) | MCF_CLOCK_SYNCR_CLKSRC| MCF_CLOCK_SYNCR_PLLMODE | MCF_CLOCK_SYNCR_PLLEN ;
        }break;
        case 2:{//96M
            MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(4) | MCF_CLOCK_SYNCR_CLKSRC| MCF_CLOCK_SYNCR_PLLMODE | MCF_CLOCK_SYNCR_PLLEN ;
        }break;
        case 3:{//112M
            MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(5) | MCF_CLOCK_SYNCR_CLKSRC| MCF_CLOCK_SYNCR_PLLMODE | MCF_CLOCK_SYNCR_PLLEN ;
        }break;
        case 4:{//128M
            MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(6) | MCF_CLOCK_SYNCR_CLKSRC| MCF_CLOCK_SYNCR_PLLMODE | MCF_CLOCK_SYNCR_PLLEN ;
        }break;
        case 5:{//144M
            MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(7) | MCF_CLOCK_SYNCR_CLKSRC| MCF_CLOCK_SYNCR_PLLMODE | MCF_CLOCK_SYNCR_PLLEN ;
        }break;
    }
    while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)){}
}
#define PLL_80MHZ   1
#define PLL_96MHZ   2
#define PLL_112MHZ   3
#define PLL_128MHZ   4
#define PLL_144MHZ   5

int main(void)
{
    uint16 ab[] = {120,130,150,170,220,250,280,300,320,340,360,330,290,260,250,230,210,180,160,140,150,180,220,250,270,300,240,180,150,190};
    uint8 c = 0;
	EnableInterrupts();//打开总中断

	TZ_PLLinit( PLL_128MHZ );//PLL初始化，时钟设置为80Mhz，可超频到144Mhz

	//TADCx_Init();
    //TADC_Test();


	/*串口初始化*/
	//TUartx_INIT(2);     //串口2初始化
	TUartx_INIT(1);     //串口1初始化
    TUartx_INIT(0);     //串口0初始化
    TUart0_DMAInit();

	/*部分引脚GPIO模式初始化*/
	MCF_GPIO_PDDPAR = 0;    //摄像头数据引脚A0-A8，配置为GPIO模式
	MCF_GPIO_DDRDD = 0;     //输入

	MCF_GPIO_PTJPAR &= ~(MCF_GPIO_PTJPAR_FEC_TXD0_GPIO | MCF_GPIO_PTJPAR_FEC_TXD1_GPIO | MCF_GPIO_PTJPAR_FEC_TXD2_GPIO | MCF_GPIO_PTJPAR_FEC_TXD3_GPIO);
	//颜色模块的0、1、2、3号引脚
	//MCF_GPIO_DDRTJ |= MCF_GPIO_DDRTJ_DDRTJ2 | MCF_GPIO_DDRTJ_DDRTJ3 | MCF_GPIO_DDRTJ_DDRTJ4 | MCF_GPIO_DDRTJ_DDRTJ5;
    //输出
    //MCF_GPIO_CLRTJ &= ~(MCF_GPIO_CLRTJ_CLRTJ4 | MCF_GPIO_CLRTJ_CLRTJ5);//清空s2 s3 设置为检测红色
    //MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //置位s0 s1 设置频率为100%

	MCF_GPIO_DDRTJ &= ~(MCF_GPIO_DDRTJ_DDRTJ2 | MCF_GPIO_DDRTJ_DDRTJ3 | MCF_GPIO_DDRTJ_DDRTJ4 | MCF_GPIO_DDRTJ_DDRTJ5);
    //输入

    //SCCB_WriteByte(OV7725_HOutSize,0x14);
    //SCCB_WriteByte(OV7725_VOutSize,0x1E);
    TZ_OV7725_Init();

	MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A15_GPIO);
	MCF_GPIO_PTGPAR &= ~(MCF_GPIO_PTGPAR_MB_A16_GPIO | MCF_GPIO_PTGPAR_MB_A17_GPIO | MCF_GPIO_PTGPAR_MB_A18_GPIO | MCF_GPIO_PTGPAR_MB_A19_GPIO);
	//拨码开关1-5的引脚
	MCF_GPIO_DDRTF &= ~MCF_GPIO_DDRTF_DDRTF7;
	MCF_GPIO_DDRTG &= ~(MCF_GPIO_DDRTG_DDRTG0 | MCF_GPIO_DDRTG_DDRTG1 | MCF_GPIO_DDRTG_DDRTG2 | MCF_GPIO_DDRTG_DDRTG3);
    //输入

    enCoder |= MCF_GPIO_SETTF&MCF_GPIO_SETTF_SETTF7 ? 0x01:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG0 ? 0x02:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG1 ? 0x04:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG2 ? 0x08:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG3 ? 0x10:0;

    //sprintf(TXBuffer,"%u = %u,%u,%u,%u,%u\n",enCoder,MCF_GPIO_SETTF&MCF_GPIO_SETTF_SETTF7,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG0,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG1,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG2,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG3);
    //TUart0_Puts(TXBuffer);

    MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A9_GPIO | MCF_GPIO_PTFPAR_MB_A10_GPIO | MCF_GPIO_PTFPAR_MB_A11_GPIO | MCF_GPIO_PTFPAR_MB_A12_GPIO);
    //CD4520引脚
    MCF_GPIO_DDRTF |= (MCF_GPIO_DDRTF_DDRTF1 | MCF_GPIO_DDRTF_DDRTF2 | MCF_GPIO_DDRTF_DDRTF3 | MCF_GPIO_DDRTF_DDRTF4);
    //输出

    MCF_GPIO_PORTTF |= (MCF_GPIO_PORTTF_PORTTF1 | MCF_GPIO_PORTTF_PORTTF2);//使能CD4520 EN端
    MCF_GPIO_PORTTF &= ~(MCF_GPIO_PORTTF_PORTTF3 | MCF_GPIO_PORTTF_PORTTF4);//清零CD4520 Reset，开始计数

	/*串口测试*/
	TUart0_Puts("Hello MCF52259 send by TUART0!\r\n");
	TUart1_Puts("Hello MCF52259 send by TUART1!\r\n");
	//TUart2_Puts("Hello MCF52259 send by TUART2!\r\n");

	/*DMA模块初始化*/
	//TDMAx_Init(1);      //初始化DMA模块1，用于HREF获取图像数据
	//TDMAx_Init(2);
	TDMAx_Init(3);      //初始化DMA模块3，用于PCLK获取图像数据
    MCF_DMA3_SAR = (vuint32)0x40100044;//DMA3源地址设为 A0-A7 的输入引脚地址
    //Image_ToPC[0] = 0x01;
    //Image_ToPC[1] = 0xFE;
    //Image_ToPC[IMG_SIZE+2] = 0xFE;
    //Image_ToPC[IMG_SIZE+3] = 0x01;
    Image_Ptr = &Image_ToPC[2];

	/*PWM模块   初始化*/
	TPWMx_INIT(0);  //左电机PWM初始化 左右根据接线而定
	TPWMx_INIT(2);  //右电机PWM初始化
	TPWMx_INIT(4);  //舵机PWM初始化

	/*GPT模块初始化*/
	//TGPTx_Init(3);  //颜色传感器输入捕获模式初始化
	TGPTx_Init(0);  //编码器1输入捕获模式初始化
	TGPTx_Init(1);  //编码器2输入捕获模式初始化

    TGPT0_ENINTER();   //关GPT0中断
    TGPT1_ENINTER();   //关GPT1中断
    //TGPT3_ENINTER();   //关GPT3中断

	/*定时器初始化*/
    TPITx_Init(0);      //初始化PIT0
    TPITx_Init(1);      //初始化PIT1
    TPIT0_SetPMR(1);    //PIT0中断时间设置为1ms
    TPIT1_SetPMR(1);    //PIT1中断时间设置为1000ms
    //TPIT0_ENABLE();     //PIT1使能


	/*外部中断初始化*/
	//TEPORTx_Init(1);    //行外部中断初始化

    //baseSteerPwm = 2920;//本车
    baseSteerPwm = 2920;//带车
    baseSteerPwmMax = baseSteerPwm + 350;
    baseSteerPwmMin = baseSteerPwm - 350;
    steerPwm = baseSteerPwm;
    steerPwmf = baseSteerPwm;

    TEPORTx_Init(7);    //场外部中断初始化

	/*I2C模块(SCCB)初始化*/
    //MCF_GPIO_PASPAR |= MCF_GPIO_PASPAR_SCL0_SCL0 | MCF_GPIO_PASPAR_SDA0_SDA0;
    //MCF_GPIO_DDRAS |= MCF_GPIO_DDRAS_DDRAS0 | MCF_GPIO_DDRAS_DDRAS1 | MCF_GPIO_DDRAS_DDRAS2;

    switch(enCoder){
        case 0:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 80; variSpeed = 50;
        }break;
        case 1:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 100;variSpeed = 150;
        }break;
        case 2:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 120;variSpeed = 160;
        }break;
        case 3:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 150;variSpeed = 170;
        }break;

        case 4:{
            Kp_a = 0.5; Ki_a = 0.7; Kd_a = 0.0; baseSpeed = 80; variSpeed = 50;
        }break;
        case 5:{
            Kp_a = 0.5; Ki_a = 0.7; Kd_a = 0.0; baseSpeed = 100;variSpeed = 150;
        }break;
        case 6:{
            Kp_a = 0.5; Ki_a = 0.7; Kd_a = 0.0; baseSpeed = 120;variSpeed = 160;
        }break;
        case 7:{
            Kp_a = 0.5; Ki_a = 0.8; Kd_a = 0.0; baseSpeed = 120;variSpeed = 160;
        }break;
        case 8:{
            Kp_a = 0.5; Ki_a = 0.9; Kd_a = 0.0; baseSpeed = 120;variSpeed = 160;
        }break;
        case 9:{
            Kp_a = 0.6; Ki_a = 0.8; Kd_a = 0.0; baseSpeed = 120;variSpeed = 160;
        }break;

        case 10:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 80; variSpeed = 50;
        }break;
        case 11:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 100;variSpeed = 150;
        }break;
        case 12:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 120;variSpeed = 160;
        }break;
        case 13:{
            Kp_a = 0.5; Ki_a = 0.5; Kd_a = 0.0; baseSpeed = 150;variSpeed = 170;
        }break;

        case 14:{
            Kp_a = 0.5; Ki_a = 0.7; Kd_a = 0.0; baseSpeed = 80; variSpeed = 50;
        }break;
        case 15:{
            Kp_a = 0.5; Ki_a = 0.7; Kd_a = 0.0; baseSpeed = 100;variSpeed = 150;
        }break;
        case 16:{
            Kp_a = 0.5; Ki_a = 0.7; Kd_a = 0.0; baseSpeed = 120;variSpeed = 160;
        }break;
        case 17:{
            Kp_a = 0.5; Ki_a = 0.7; Kd_a = 0.0; baseSpeed = 150;variSpeed = 170;
        }break;
        case 18:{
            Kp_a = 0.5; Ki_a = 0.8; Kd_a = 0.0; baseSpeed = 150;variSpeed = 170;
        }break;
        case 19:{
            Kp_a = 0.5; Ki_a = 0.9; Kd_a = 0.0; baseSpeed = 150;variSpeed = 170;
        }break;

        case 20:{
            Kp_ab = 0.3; Ki_ab = 0.4; Kd_ab = 0.0; ;
            Kp_ao = 0.0; Ki_ao = 0.4; Kd_ao = 0.0; baseSpeed = 120; variSpeed = 80;
        }break;
        case 21:{
            Kp_ab = 0.4; Ki_ab = 0.5; Kd_ab = 0.0; ;
            Kp_ao = 0.0; Ki_ao = 0.3; Kd_ao = 0.0; baseSpeed = 120; variSpeed = 80;
        }break;
        case 22:{
            Kp_ab = 0.5; Ki_ab = 0.5; Kd_ab = 0.0; ;
            Kp_ao = 0.0; Ki_ao = 0.4; Kd_ao = 0.0; baseSpeed = 120; variSpeed = 80;
        }break;
        case 23:{
            Kp_ab = 0.6; Ki_ab = 0.3; Kd_ab = 0.0; ;
            Kp_ao = 0.0; Ki_ao = 0.5; Kd_ao = 0.0; baseSpeed = 120; variSpeed = 80;
        }break;
        case 24:{
            Kp_ab = 0.3; Ki_ab = 0.5; Kd_ab = 0.0; ;
            Kp_ao = 0.0; Ki_ao = 0.3; Kd_ao = 0.0; baseSpeed = 120; variSpeed = 80;
        }break;
        case 25:{
            Kp_ab = 0.3; Ki_ab = 0.5; Kd_ab = 0.0; ;
            Kp_ao = 0.0; Ki_ao = 0.3; Kd_ao = 0.0; baseSpeed = 140; variSpeed = 100;
        }break;

    }

    for(;;){}
    for(;;){
        if(MCF_GPIO_SETTJ&MCF_GPIO_SETTJ_SETTJ2){
            delay2();
            if(MCF_GPIO_SETTJ&MCF_GPIO_SETTJ_SETTJ2){
                TUart0_Puts("hw\n");
            }
        }
    }
    c = 0;
    for(;;){
        delay();
        c++;
        if(c>150)
        c = 0;
        TPWM0_SetDTY(c);
        TPWM2_SetDTY(250-c);
    }
    //电机测试代码
    SetSpeed = 100;
    for(;;){
        delay3();
        delay3();
        delay3();
        delay3();
        delay3();
        delay3();
        delay3();
        SetSpeed = ab[c++];
        if(c == 30)
            c = 0;

    }
    //舵机测试代码
	steerPwm = 2880;//正常值为3000，舵机片有一点点，因此偏移一点
    TPWM45_SetDTY(baseSteerPwm);
    while(1){
        while(steerPwm<=3230){
            TPWM45_SetDTY(steerPwm);
            steerPwm++;
            delay2();
        }
        while(steerPwm>=2510){
            TPWM45_SetDTY(steerPwm);
            steerPwm--;
            delay2();
        }
	}

}