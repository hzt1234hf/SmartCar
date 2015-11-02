#include "DIY_ComDef.h"
 /*
*****2015.4.23 PWM模块编写完毕
*****功能：PWM模块实现输出脉冲
*****编写中遇到的主要问题：（
    1.单个和组合使用时应该注意预分频与占空比的时间是否符合需要的值
    2.注意时钟输入是总线时钟不是系统时钟
    3.合理使用组合PWM
）
*/
#ifdef TZ_MCF52259_PWM

void TPWMx_INIT(uint8 mode){
	//MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN0_PWM0 | MCF_GPIO_PTCPAR_DTIN1_PWM2 | MCF_GPIO_PTCPAR_DTIN2_PWM4 | MCF_GPIO_PTCPAR_DTIN3_PWM6;//
	//MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC0_PWM1 | MCF_GPIO_PTAPAR_ICOC1_PWM3 | MCF_GPIO_PTAPAR_ICOC2_PWM5 | MCF_GPIO_PTAPAR_ICOC3_PWM7;

    MCF_PWM_PWMPRCLK |= MCF_PWM_PWMPRCLK_PCKA(3) | MCF_PWM_PWMPRCLK_PCKB(3);
	/** A和B时钟设置为3分频，可得时钟频率为 = 80MHZ / 2 / 2 ^ 3 = 10MHZ */
    MCF_PWM_PWMSCLA = 2;//
	/** SA设置为除1，时钟还是10MHZ */
    MCF_PWM_PWMSCLB = 2;//
	/** SB设置为除1，时钟还是10MHZ */

	switch(mode){
	    //根据mode变量进行初始化，1、3、7使用输入捕获所以不会进行初始化，
	    //需要初始化的是0(电机1)、2(电机2)、4、5(和4组合成16位，控制舵机)、6(保留)
    	case 0:{//右电机
    	    MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN0_PWM0;

            MCF_PWM_PWME &= ~MCF_PWM_PWME_PWME0;        //关闭PWM0输出
            MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL0;    //设置所有PWM通道为由低到高脉冲
            MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK0;    //设置为A/B时钟
            MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE0;     //设置为左边对齐
            MCF_PWM_PWMCTL &= ~MCF_PWM_PWMCTL_CON01;    //取消合并0、1PWM模块
            MCF_PWM_PWMCNT0 = 0;//
            MCF_PWM_PWMPER0 = 250;
            MCF_PWM_PWMDTY0 = 80;
            MCF_PWM_PWME |= MCF_PWM_PWME_PWME0;         //启动PWM0输出
    	}break;
    	case 2:{//左电机
    	    MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN1_PWM2;

            MCF_PWM_PWME &= ~MCF_PWM_PWME_PWME2;        //关闭PWM2输出
            MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL2;    //设置所有PWM通道为由低到高脉冲
            MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK2;    //设置为A/B时钟
            MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE2;     //设置为左边对齐
            MCF_PWM_PWMCTL &= ~MCF_PWM_PWMCTL_CON23;    //取消合并2、3PWM模块
            MCF_PWM_PWMCNT2 = 0;//
            MCF_PWM_PWMPER2 = 250;
            MCF_PWM_PWMDTY2 = 80;
            MCF_PWM_PWME |= MCF_PWM_PWME_PWME2;         //启动PWM2输出
    	}break;
        case 4:case 5:{
	        MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN2_PWM4;//使能PWM4和PWM5端口
	        MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC2_PWM5;

            MCF_PWM_PWME &= ~(MCF_PWM_PWME_PWME5 | MCF_PWM_PWME_PWME4);     //关闭PWM0输出
            MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL4 | MCF_PWM_PWMPOL_PPOL5;  //设置所有PWM通道为由高到低脉冲
            MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK4 | MCF_PWM_PWMCLK_PCLK5;  //PWM4通道都用SA/SB时钟组
            MCF_PWM_PWMPRCLK |= MCF_PWM_PWMPRCLK_PCKA(3);//A时钟设置为预分频2^7 = 10000000
            MCF_PWM_PWMCAE &= ~(MCF_PWM_PWMCAE_CAE4 | MCF_PWM_PWMCAE_CAE5); //全部设置为左边对齐
            MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_CON45;
            MCF_PWM_PWMCNT4 = 0;
            MCF_PWM_PWMCNT5 = 0;
            MCF_PWM_PWMPER4 = 40000>>8;
            MCF_PWM_PWMPER5 = 40000&0xff;
            MCF_PWM_PWMDTY4 = 3000>>8;
            MCF_PWM_PWMDTY5 = 3000&0xff;
            MCF_PWM_PWME |= MCF_PWM_PWME_PWME4|MCF_PWM_PWME_PWME5;          //启动PWM4输出
        }break;
        default:break;
    }
	TUart0_Puts("PWM\r\n");

}
#endif