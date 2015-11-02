#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_GPT
/*
*****2015.4.25 GPTģ���д���
*****���ܣ���GPTģ����Ҫ�������岶��
*****��д����������Ҫ���⣺��
    1.����Ҫ����Ϊ���벶��ģʽ
    2.ʹ��ʱҪע������岶��ģ����һ��оƬ�Ĳ�һ��(��һ�μ����ֹ��ܵ����岶��)
    3.ע��Ҫ���ú����Ԥ��Ƶ
��
*/
uint16 leftSSSum = 0,rightSSSum = 0;
uint8 leftSpeedBool,rightSpeedBool;
void TGPTx_Init(uint8 mode){
    switch(mode){
        case 0:{
            MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC0_ICOC0;

            MCF_GPT_GPTIOS &= ~MCF_GPT_GPTIOS_IOS0;                 //����IOCO0�˿�Ϊ���벶��ģʽ
            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_TFFCA;	            //�����Զ�����жϱ�־
            MCF_GPT_GPTCTL2 |= MCF_GPT_GPTCTL2_INPUT0_RISING;       //����Ϊ�½��ؼ��
            MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI0;                     //ʹ�����벶��˿�0���ж�
            /*           ����Ԥ��Ƶ pre = sys/2/��2^x��,����xΪ3*/
            MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(5);

            leftMotorBase = (float)(64*1000000.0/sysOsciFre); //128����Ԥ��Ƶ

            MCF_GPT_GPTFLG1 = 0;                                    //�����жϱ�־�Ĵ���
            MCF_GPT_GPTDDR &= ~MCF_GPT_GPTDDR_DDRT0;                //����������Ϊ����ģʽ

            MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK44;                //�����ж�Դ��Ϊ44��ʵ��λ��Ϊ44+64
            MCF_INTC0_ICR44 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�

            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN;                   //ʹ�����벶��0
        }break;
        case 1:{
            MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC1_ICOC1;

            MCF_GPT_GPTIOS &= ~MCF_GPT_GPTIOS_IOS1;                 //����IOCO1�˿�Ϊ���벶��ģʽ
            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_TFFCA;	            //�����Զ�����жϱ�־
            MCF_GPT_GPTCTL2 |= MCF_GPT_GPTCTL2_INPUT1_RISING;       //����Ϊ�½��ؼ��
            MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI1;                     //ʹ�����벶��˿�0���ж�
            /*           ����Ԥ��Ƶ pre = sys/2/��2^x��,����xΪ3*/
            MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(5);

            rightMotorBase = (float)(64*1000000.0/sysOsciFre); //128����Ԥ��Ƶ

            MCF_GPT_GPTFLG1 = 0;                                    //�����жϱ�־�Ĵ���
            MCF_GPT_GPTDDR &= ~MCF_GPT_GPTDDR_DDRT1;                //����������Ϊ����ģʽ

            MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK45;                //�����ж�Դ��Ϊ44��ʵ��λ��Ϊ44+64
            MCF_INTC0_ICR45 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�

            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN;                   //ʹ�����벶��0
        }break;
        case 3:{

            MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC3_ICOC3;

            MCF_GPT_GPTIOS &= ~MCF_GPT_GPTIOS_IOS3;                 //����IOCO3�˿�Ϊ���벶��ģʽ
            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_TFFCA;	            //�����Զ�����жϱ�־
            //MCF_GPT_GPTCTL2 |= MCF_GPT_GPTCTL2_INPUT3_RISING;       //����Ϊ�½��ؼ��
            //MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI3;                     //ʹ�����벶��˿�0���ж�
            //           ����Ԥ��Ƶ pre = sys/2/��2^x��,����xΪ3
            //MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(5);

            //MCF_GPT_GPTFLG1 = 0;                                    //�����жϱ�־�Ĵ���
            MCF_GPT_GPTDDR &= ~MCF_GPT_GPTDDR_DDRT3;                //����������Ϊ����ģʽ
	        MCF_GPT_GPTCTL1 = 0x00; //������ģʽ
	        MCF_GPT_GPTCTL2 = 0x00; //�������ģʽ

            //MCF_GPT_GPTPACTL = MCF_GPT_GPTPACTL_PAMOD;
        	MCF_GPT_GPTPACTL |= MCF_GPT_GPTPACTL_PAE;//0x00|MCF_GPT_GPTPACTL_PAE;//���������ۼ�

            //MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK47;                //�����ж�Դ��Ϊ44��ʵ��λ��Ϊ44+64
            //MCF_INTC0_ICR47 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�

            //MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN;                   //ʹ�����벶��0
        }break;
    }
}
__declspec(interrupt) void TGPT0_interrupt(void){

    //MCF_GPIO_PORTTF |= MCF_GPIO_PORTTF_PORTTF3;//��λCD4520 Reset������Q0-Q3
    rightMotorCnt[rightCnt&0x07] = MCF_GPT_GPTC0 - rightMSCnt;
    rightSpeedBool = 0;
    //rightSSSum = (int16)((rightMotorCnt[0]+rightMotorCnt[1]+rightMotorCnt[2]+rightMotorCnt[3]+rightMotorCnt[4]+rightMotorCnt[5]+rightMotorCnt[6]+rightMotorCnt[7])>>3);
    //sprintf(TXBuffer,"right:\t%u,%u\n",(uint16)(MCF_GPT_GPTC0 - rightMSCnt),(uint16)(1206147.179/rightMotorCnt[rightCnt&0x07]));
    //sprintf(TXBuffer,"right:\t%u,%um/s,%u\n",(uint16)(MCF_GPT_GPTC0 - rightMSCnt),(uint16)(1206147.179/rightMotorCnt[rightCnt&0x07]),rightMSPwm);
    //TUart0_Puts(TXBuffer);
    rightCnt++;
    rightMSCnt = MCF_GPT_GPTC0;

    //MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF0;
    //sprintf(TXBuffer,"right:\t%u\n",(rightMotorCnt[0]+rightMotorCnt[1]+rightMotorCnt[2]+rightMotorCnt[3]+rightMotorCnt[4]+rightMotorCnt[5]+rightMotorCnt[6]+rightMotorCnt[7])/8);
    //TUart0_Puts(TXBuffer);

    //MCF_GPIO_PORTTF &= !MCF_GPIO_PORTTF_PORTTF3;//����CD4520 Reset����ʼ����
}
__declspec(interrupt) void TGPT1_interrupt(void){

    //MCF_GPIO_PORTTF |= MCF_GPIO_PORTTF_PORTTF3;//��λCD4520 Reset������Q0-Q3
    leftMotorCnt[leftCnt&0x07] = MCF_GPT_GPTC1 - leftMSCnt;
    leftSpeedBool = 0;
    //leftSSSum = (uint16)((leftMotorCnt[0]+leftMotorCnt[1]+leftMotorCnt[2]+leftMotorCnt[3]+leftMotorCnt[4]+leftMotorCnt[5]+leftMotorCnt[6]+leftMotorCnt[7])>>3);
    //leftSSSum = (leftMotorCnt[0]+leftMotorCnt[1]+leftMotorCnt[2]+leftMotorCnt[3]+leftMotorCnt[4]+leftMotorCnt[5]+leftMotorCnt[6]+leftMotorCnt[7])>>3;
    //sprintf(TXBuffer,"left:\t%u,%u,%u\n",(uint16)(MCF_GPT_GPTC1 - leftMSCnt),(uint16)(94248.0/(leftMotorCnt[leftCnt&0x07] * leftMotorBase)),leftSSSum);
    //TUart0_Puts(TXBuffer);
    leftCnt++;
    leftMSCnt = MCF_GPT_GPTC1;
    //MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF1;
    //sprintf(TXBuffer,"left:\t%u\n",(leftMotorCnt[0]+leftMotorCnt[1]+leftMotorCnt[2]+leftMotorCnt[3]+leftMotorCnt[4]+leftMotorCnt[5]+leftMotorCnt[6]+leftMotorCnt[7])/8);
    //TUart0_Puts(TXBuffer);

    //MCF_GPIO_PORTTF &= !MCF_GPIO_PORTTF_PORTTF3;//����CD4520 Reset����ʼ����

}
__declspec(interrupt) void TGPT3_interrupt(void){
/*
    colorLastCnt = MCF_GPT_GPTC3;
    colorCnt++;
*/
    colorCnt++;
    MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF3;
    //colorSenser[colorCnt&0x07] = MCF_GPT_GPTC3 - colorLastCnt;
    //colorLastCnt = MCF_GPT_GPTC3;
    //sprintf(TXBuffer,"%u,%u\n",MCF_GPT_GPTC3,colorSenser[colorCnt&0x07]);
    //TUart0_Puts(TXBuffer);

}
#endif