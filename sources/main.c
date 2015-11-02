/*
 * main implementation: use this sample to create your own application
 *
 */
#include "DIY_ComDef.h"
uint8 sysOsciFre = 80;//默认晶振频率

vuint16 leftMSCnt,rightMSCnt;           //左右电机GPT值
vuint16 leftMotorSpeed,rightMotorSpeed; //左右电机速度值

int main(void)
{
    uint8 e;
	EnableInterrupts();//打开总中断
	pll_init();         //PLL初始化，时钟设置为80Mhz，可超频到144Mhz
	/*串口初始化*/
	TUartx_INIT(2);     //串口2初始化
	TUartx_INIT(1);     //串口1初始化
    TUartx_INIT(0);     //串口0初始化
    TUart0_DMAInit();
    
	/*部分引脚GPIO模式初始化*/
	MCF_GPIO_PTEPAR = 0;    //摄像头数据引脚A0-A8，配置为GPIO模式
	MCF_GPIO_DDRTE = 0;     //输入
    while(0)
    {
        TUart0_Putchar(MCF_GPIO_SETTE);
    }
	MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A15_GPIO);  //颜色模块的0号引脚
	MCF_GPIO_DDRTF |= MCF_GPIO_PTFPAR_MB_A15_GPIO;      //输出
	MCF_GPIO_PTGPAR &= ~(MCF_GPIO_PTGPAR_MB_A16_GPIO | MCF_GPIO_PTGPAR_MB_A17_GPIO | MCF_GPIO_PTGPAR_MB_A18_GPIO);
    //颜色模块的1、2、3号引脚
    MCF_GPIO_DDRTG |= MCF_GPIO_DDRTG_DDRTG0 | MCF_GPIO_DDRTG_DDRTG1 | MCF_GPIO_DDRTG_DDRTG2 | MCF_GPIO_DDRTG_DDRTG3;
    //输出

    //SCCB_WriteByte(OV7725_HOutSize,0x14);
    //SCCB_WriteByte(OV7725_VOutSize,0x1E);
    TZ_OV7725_Init();

	MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A9_GPIO | MCF_GPIO_PTFPAR_MB_A10_GPIO | MCF_GPIO_PTFPAR_MB_A11_GPIO | MCF_GPIO_PTFPAR_MB_A12_GPIO | MCF_GPIO_PTFPAR_MB_A13_GPIO | MCF_GPIO_PTFPAR_MB_A14_GPIO);
	//拨码开关1-6的引脚
	MCF_GPIO_DDRTF |= MCF_GPIO_DDRTF_DDRTF1 | MCF_GPIO_DDRTF_DDRTF2 | MCF_GPIO_DDRTF_DDRTF3 | MCF_GPIO_DDRTF_DDRTF4 | MCF_GPIO_DDRTF_DDRTF5 | MCF_GPIO_DDRTF_DDRTF6;
    //输入


	/*串口测试*/
	TUart0_Puts("Hello MCF52259 send by TUART0!\r\n");
	TUart1_Puts("Hello MCF52259 send by TUART1!\r\n");
	TUart2_Puts("Hello MCF52259 send by TUART2!\r\n");
	
	/*DMA模块初始化*/
	//TDMAx_Init(1);      //初始化DMA模块1，用于HREF获取图像数据
	//TDMAx_Init(2);      
	TDMAx_Init(3);      //初始化DMA模块3，用于PCLK获取图像数据
    //SET_DMA3_SAR(0x40100030);//DMA3源地址设为 A0-A7 的输入引脚地址
    Image_ToPC[0] = 0x01;
    Image_ToPC[1] = 0xFE;
    Image_ToPC[IMG_SIZE-2] = 0xFE;
    Image_ToPC[IMG_SIZE-1] = 0x01;
    Image_Ptr = &Image_ToPC[2];
    MCF_DMA3_SAR = (vuint32)0x40100030;
    //MCF_DMA2_BCR = 10;
    //MCF_DMA2_DCR |=  MCF_DMA_DCR_EEXT ;//外部触发
    
#if 1
	/*PWM模块   初始化*/
	TPWMx_INIT(0);  //左电机PWM初始化 左右根据接线而定
	TPWMx_INIT(2);  //右电机PWM初始化
	TPWMx_INIT(4);  //舵机PWM初始化

	/*GPT模块初始化*/
	TGPTx_Init(0);  //编码器1输入捕获模式初始化
	TGPTx_Init(1);  //编码器2输入捕获模式初始化
	TGPTx_Init(3);  //颜色传感器输入捕获模式初始化
    //TGPT0_DISINTER();   //关GPT0中断
    //TGPT1_DISINTER();   //关GPT2中断
    TGPT3_DISINTER();   //关GPT4中断

	/*外部中断初始化*/
	TEPORTx_Init(1);    //场外部中断初始化
	TEPORTx_Init(7);    //行外部中断初始化
#endif
	/*定时器初始化*/
    TPITx_Init(0);      //初始化PIT0
    TPITx_Init(1);      //初始化PIT1
    TPIT0_SetPMR(1);    //PIT0中断时间设置为1ms
    TPIT1_SetPMR(1000); //PIT1中断时间设置为1000ms
    //TPIT1_ENABLE();     //PIT1使能

	/*I2C模块(SCCB)初始化*/
    //MCF_GPIO_PASPAR |= MCF_GPIO_PASPAR_SCL0_SCL0 | MCF_GPIO_PASPAR_SDA0_SDA0;
    //MCF_GPIO_DDRAS |= MCF_GPIO_DDRAS_DDRAS0 | MCF_GPIO_DDRAS_DDRAS1 | MCF_GPIO_DDRAS_DDRAS2;

	pwmCnt = 3250;
	while(1){}{
        while(pwmCnt<=4250){
            TPWM45_SetDTY(pwmCnt);
            pwmCnt++;
            delay2();
        }
        while(pwmCnt>=3250){
            TPWM45_SetDTY(pwmCnt);
            pwmCnt--;
            delay2();
        }
	}

}