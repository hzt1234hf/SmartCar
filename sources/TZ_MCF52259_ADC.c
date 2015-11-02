#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_ADC

int16 ADC0_Data;
int16 ADC1_Data;
int16 ADC2_Data;
int16 ADC3_Data;
int16 ADC4_Data;
int16 ADC5_Data;
int16 ADC6_Data;
int16 ADC7_Data;

void TADCx_Init(){
	MCF_GPIO_PANPAR = MCF_GPIO_PANPAR_AN0_AN0;// | MCF_GPIO_PANPAR_AN1_AN1 | MCF_GPIO_PANPAR_AN2_AN2;
	/*                  停止运行            全部为单端模式                  顺序一次            扫描结束中断使能*/
	MCF_ADC_CTRL1 = MCF_ADC_CTRL1_STOP0 | MCF_ADC_CTRL1_CHNCFG(0) | MCF_ADC_CTRL1_SMODE(0)|MCF_ADC_CTRL1_EOSIE0;
	MCF_ADC_CTRL2 = MCF_ADC_CTRL2_DIV(7);   //  配置ADC时钟频率  ADC时钟频率 = fsys/(2*(DIV+1)) = 80MHz/256 = 312.5kHz
	MCF_ADC_ADZCC = 0;                      //  过零全部禁止
    /* 通道设置 全部为默认对应引脚 */
	MCF_ADC_ADLST1 = MCF_ADC_ADLST1_SAMPLE0(0) | MCF_ADC_ADLST1_SAMPLE1(1) | MCF_ADC_ADLST1_SAMPLE2(2) | MCF_ADC_ADLST1_SAMPLE3(3);
	MCF_ADC_ADLST2 = MCF_ADC_ADLST2_SAMPLE4(4) | MCF_ADC_ADLST2_SAMPLE5(5) | MCF_ADC_ADLST2_SAMPLE6(6) | MCF_ADC_ADLST2_SAMPLE7(7);
	MCF_ADC_ADSDIS &= ~(MCF_ADC_ADSDIS_DS0);// | MCF_ADC_ADSDIS_DS1 | MCF_ADC_ADSDIS_DS2);//使能AN0 AN1 AN2
	MCF_ADC_ADSTAT = MCF_ADC_ADSTAT_EOSI0 | MCF_ADC_ADSTAT_EOSI1;

	MCF_ADC_ADLSTAT = 0XFFFF;           //全部设置为小于阀值
	/*  上下限寄存器  */
	MCF_ADC_ADLLMT(0) = MCF_ADC_ADLLMT_LLMT(0);
	MCF_ADC_ADLLMT(1) = MCF_ADC_ADLLMT_LLMT(0);
	MCF_ADC_ADLLMT(2) = MCF_ADC_ADLLMT_LLMT(0);
	MCF_ADC_ADLLMT(3) = MCF_ADC_ADLLMT_LLMT(0);
	MCF_ADC_ADLLMT(4) = MCF_ADC_ADLLMT_LLMT(0);
	MCF_ADC_ADLLMT(5) = MCF_ADC_ADLLMT_LLMT(0);
	MCF_ADC_ADLLMT(6) = MCF_ADC_ADLLMT_LLMT(0);
	MCF_ADC_ADLLMT(7) = MCF_ADC_ADLLMT_LLMT(0);

	MCF_ADC_ADHLMT(0) = MCF_ADC_ADHLMT_HLMT(0xFFFF);
	MCF_ADC_ADHLMT(1) = MCF_ADC_ADHLMT_HLMT(0xFFFF);
	MCF_ADC_ADHLMT(2) = MCF_ADC_ADHLMT_HLMT(0xFFFF);
	MCF_ADC_ADHLMT(3) = MCF_ADC_ADHLMT_HLMT(0xFFFF);
	MCF_ADC_ADHLMT(4) = MCF_ADC_ADHLMT_HLMT(0xFFFF);
	MCF_ADC_ADHLMT(5) = MCF_ADC_ADHLMT_HLMT(0xFFFF);
	MCF_ADC_ADHLMT(6) = MCF_ADC_ADHLMT_HLMT(0xFFFF);
	MCF_ADC_ADHLMT(7) = MCF_ADC_ADHLMT_HLMT(0xFFFF);

	/*  偏移量寄存器  */
	MCF_ADC_ADOFS(0) = MCF_ADC_ADOFS_OFFSET(0);
	MCF_ADC_ADOFS(1) = MCF_ADC_ADOFS_OFFSET(0);
	MCF_ADC_ADOFS(2) = MCF_ADC_ADOFS_OFFSET(0);
	MCF_ADC_ADOFS(3) = MCF_ADC_ADOFS_OFFSET(0);
	MCF_ADC_ADOFS(4) = MCF_ADC_ADOFS_OFFSET(0);
	MCF_ADC_ADOFS(5) = MCF_ADC_ADOFS_OFFSET(0);
	MCF_ADC_ADOFS(6) = MCF_ADC_ADOFS_OFFSET(0);
	MCF_ADC_ADOFS(7) = MCF_ADC_ADOFS_OFFSET(0);

	MCF_ADC_ADZCSTAT = 0;//零点不改变标志位

	//  禁止自动低功耗模式与自动掉电模式
	MCF_ADC_POWER &= ~MCF_ADC_POWER_ASB;
	MCF_ADC_POWER &= ~MCF_ADC_POWER_APD;

    // 开启中断
	MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK49;
	MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;
	MCF_INTC0_ICR49 = MCF_INTC_ICR_IP(5) + MCF_INTC_ICR_IL(3);

	MCF_ADC_POWER &=~ MCF_ADC_POWER_PD0;
	while(MCF_ADC_POWER & MCF_ADC_POWER_PSTS0){}

}

void TADC_Test(void){
    MCF_ADC_CTRL1 &= ~MCF_ADC_CTRL1_STOP0;
    MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0;
}

__declspec(interrupt:0) void TADCA_interrupt(void){
    //ADC0_Data = (MCF_ADC_ADRSLT0>>3)&0x0fff;
    //Img_GAMvalue = ADC0_Data * (float)255/4095 - 30;
    //ADC1_Data = (MCF_ADC_ADRSLT1>>3)&0x0fff;
    //ADC2_Data = (MCF_ADC_ADRSLT2>>3)&0x0fff;
    //sprintf(TXBuffer,"%u,%u--\r\n",ADC0_Data,Img_GAMvalue);
    TUart0_Puts(TXBuffer);
    MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_EOSI0;
}
__declspec(interrupt:0) void TADCB_interrupt(void){
    MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_EOSI1;
}
__declspec(interrupt:0) void TADCINT_interrupt(void){
    MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_ZCI | MCF_ADC_ADSTAT_HLMTI | MCF_ADC_ADSTAT_LLMTI;
}

#endif