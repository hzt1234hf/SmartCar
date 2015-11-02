/*
 * main implementation: use this sample to create your own application
 *
 */
#include "DIY_ComDef.h"
uint8 sysOsciFre = 80;//Ĭ�Ͼ���Ƶ��

vuint16 leftMSCnt,rightMSCnt;           //���ҵ��GPTֵ
vuint16 leftMotorSpeed,rightMotorSpeed; //���ҵ���ٶ�ֵ

int main(void)
{
    uint8 e,f;
	EnableInterrupts();//�����ж�
	pll_init();         //PLL��ʼ����ʱ������Ϊ80Mhz���ɳ�Ƶ��144Mhz
	//TADCx_Init();

	/*���ڳ�ʼ��*/
	TUartx_INIT(2);     //����2��ʼ��
	TUartx_INIT(1);     //����1��ʼ��
    TUartx_INIT(0);     //����0��ʼ��
    TUart0_DMAInit();

	/*��������GPIOģʽ��ʼ��*/
	MCF_GPIO_PTEPAR = 0;    //����ͷ��������A0-A8������ΪGPIOģʽ
	MCF_GPIO_DDRTE = 0;     //����

	MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A15_GPIO);  //��ɫģ���0������
	MCF_GPIO_DDRTF |= MCF_GPIO_PTFPAR_MB_A15_GPIO;      //���
	MCF_GPIO_PTGPAR &= ~(MCF_GPIO_PTGPAR_MB_A16_GPIO | MCF_GPIO_PTGPAR_MB_A17_GPIO | MCF_GPIO_PTGPAR_MB_A18_GPIO);
    //��ɫģ���1��2��3������
    MCF_GPIO_DDRTG |= MCF_GPIO_DDRTG_DDRTG0 | MCF_GPIO_DDRTG_DDRTG1 | MCF_GPIO_DDRTG_DDRTG2 | MCF_GPIO_DDRTG_DDRTG3;
    //���

    //SCCB_WriteByte(OV7725_HOutSize,0x14);
    //SCCB_WriteByte(OV7725_VOutSize,0x1E);
    TZ_OV7725_Init();

	MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A9_GPIO | MCF_GPIO_PTFPAR_MB_A10_GPIO | MCF_GPIO_PTFPAR_MB_A11_GPIO | MCF_GPIO_PTFPAR_MB_A12_GPIO | MCF_GPIO_PTFPAR_MB_A13_GPIO | MCF_GPIO_PTFPAR_MB_A14_GPIO);
	//���뿪��1-6������
	MCF_GPIO_DDRTF |= MCF_GPIO_DDRTF_DDRTF1 | MCF_GPIO_DDRTF_DDRTF2 | MCF_GPIO_DDRTF_DDRTF3 | MCF_GPIO_DDRTF_DDRTF4 | MCF_GPIO_DDRTF_DDRTF5 | MCF_GPIO_DDRTF_DDRTF6;
    //����


	/*���ڲ���*/
	TUart0_Puts("Hello MCF52259 send by TUART0!\r\n");
	TUart1_Puts("Hello MCF52259 send by TUART1!\r\n");
	TUart2_Puts("Hello MCF52259 send by TUART2!\r\n");

	/*DMAģ���ʼ��*/
	//TDMAx_Init(1);      //��ʼ��DMAģ��1������HREF��ȡͼ������
	//TDMAx_Init(2);
	TDMAx_Init(3);      //��ʼ��DMAģ��3������PCLK��ȡͼ������
    MCF_DMA3_SAR = (vuint32)0x40100030;//DMA3Դ��ַ��Ϊ A0-A7 ���������ŵ�ַ
    //Image_ToPC[0] = 0x01;
    //Image_ToPC[1] = 0xFE;
    //Image_ToPC[IMG_SIZE+2] = 0xFE;
    //Image_ToPC[IMG_SIZE+3] = 0x01;
    Image_Ptr = &Image_ToPC[2];


#if 1
	/*PWMģ��   ��ʼ��*/
	TPWMx_INIT(0);  //����PWM��ʼ�� ���Ҹ��ݽ��߶���
	TPWMx_INIT(2);  //�ҵ��PWM��ʼ��
	TPWMx_INIT(4);  //���PWM��ʼ��

	/*GPTģ���ʼ��*/
	TGPTx_Init(0);  //������1���벶��ģʽ��ʼ��
	TGPTx_Init(1);  //������2���벶��ģʽ��ʼ��
	TGPTx_Init(3);  //��ɫ���������벶��ģʽ��ʼ��
    //TGPT0_DISINTER();   //��GPT0�ж�
    //TGPT1_DISINTER();   //��GPT2�ж�
    TGPT3_DISINTER();   //��GPT4�ж�

	/*�ⲿ�жϳ�ʼ��*/
	TEPORTx_Init(1);    //���ⲿ�жϳ�ʼ��
	//TEPORTx_Init(7);    //���ⲿ�жϳ�ʼ��
#endif
	/*��ʱ����ʼ��*/
    TPITx_Init(0);      //��ʼ��PIT0
    TPITx_Init(1);      //��ʼ��PIT1
    TPIT0_SetPMR(1);    //PIT0�ж�ʱ������Ϊ1ms
    TPIT1_SetPMR(1); //PIT1�ж�ʱ������Ϊ1000ms
    //TPIT1_ENABLE();     //PIT1ʹ��

	/*I2Cģ��(SCCB)��ʼ��*/
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