/*
 * main implementation: use this sample to create your own application
 *
 */
#include "DIY_ComDef.h"
uint32 sysOsciFre = 128000000;//Ĭ�Ͼ���Ƶ��
uint8 enCoder = 0;      //������ֵ
uint16 colorCnt;        //��ȡ��ɫ���ڼ���
uint16 color3Cnt[3];    //��ɫ����ֵ
uint16 colorSZCnt;      //��ȡ��ɫ����
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
	EnableInterrupts();//�����ж�

	TZ_PLLinit( PLL_128MHZ );//PLL��ʼ����ʱ������Ϊ80Mhz���ɳ�Ƶ��144Mhz

	//TADCx_Init();
    //TADC_Test();


	/*���ڳ�ʼ��*/
	//TUartx_INIT(2);     //����2��ʼ��
	//TUartx_INIT(1);     //����1��ʼ��
    TUartx_INIT(0);     //����0��ʼ��
    TUart0_DMAInit();

	/*��������GPIOģʽ��ʼ��*/
	MCF_GPIO_PDDPAR = 0;    //����ͷ��������A0-A8������ΪGPIOģʽ
	MCF_GPIO_DDRDD = 0;     //����

	MCF_GPIO_PTJPAR &= ~(MCF_GPIO_PTJPAR_FEC_TXD0_GPIO | MCF_GPIO_PTJPAR_FEC_TXD1_GPIO | MCF_GPIO_PTJPAR_FEC_TXD2_GPIO | MCF_GPIO_PTJPAR_FEC_TXD3_GPIO);
	//��ɫģ���0��1��2��3������
	MCF_GPIO_DDRTJ |= MCF_GPIO_DDRTJ_DDRTJ2 | MCF_GPIO_DDRTJ_DDRTJ3 | MCF_GPIO_DDRTJ_DDRTJ4 | MCF_GPIO_DDRTJ_DDRTJ5;
    //���
    MCF_GPIO_CLRTJ &= ~(MCF_GPIO_CLRTJ_CLRTJ4 | MCF_GPIO_CLRTJ_CLRTJ5);//���s2 s3 ����Ϊ����ɫ
    MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //��λs0 s1 ����Ƶ��Ϊ100%

    //SCCB_WriteByte(OV7725_HOutSize,0x14);
    //SCCB_WriteByte(OV7725_VOutSize,0x1E);
    TZ_OV7725_Init();

	MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A15_GPIO);
	MCF_GPIO_PTGPAR &= ~(MCF_GPIO_PTGPAR_MB_A16_GPIO | MCF_GPIO_PTGPAR_MB_A17_GPIO | MCF_GPIO_PTGPAR_MB_A18_GPIO | MCF_GPIO_PTGPAR_MB_A19_GPIO);
	//���뿪��1-5������
	MCF_GPIO_DDRTF &= ~MCF_GPIO_DDRTF_DDRTF7;
	MCF_GPIO_DDRTG &= ~(MCF_GPIO_DDRTG_DDRTG0 | MCF_GPIO_DDRTG_DDRTG1 | MCF_GPIO_DDRTG_DDRTG2 | MCF_GPIO_DDRTG_DDRTG3);
    //����

    enCoder |= MCF_GPIO_SETTF&MCF_GPIO_SETTF_SETTF7 ? 0x01:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG0 ? 0x02:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG1 ? 0x04:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG2 ? 0x08:0;
    enCoder |= MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG3 ? 0x10:0;
    
    //sprintf(TXBuffer,"%u = %u,%u,%u,%u,%u\n",enCoder,MCF_GPIO_SETTF&MCF_GPIO_SETTF_SETTF7,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG0,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG1,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG2,MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG3);
    //TUart0_Puts(TXBuffer);

    MCF_GPIO_PTFPAR &= ~(MCF_GPIO_PTFPAR_MB_A9_GPIO | MCF_GPIO_PTFPAR_MB_A10_GPIO | MCF_GPIO_PTFPAR_MB_A11_GPIO | MCF_GPIO_PTFPAR_MB_A12_GPIO);
    //CD4520����
    MCF_GPIO_DDRTF |= (MCF_GPIO_DDRTF_DDRTF1 | MCF_GPIO_DDRTF_DDRTF2 | MCF_GPIO_DDRTF_DDRTF3 | MCF_GPIO_DDRTF_DDRTF4);
    //���

    MCF_GPIO_PORTTF |= (MCF_GPIO_PORTTF_PORTTF1 | MCF_GPIO_PORTTF_PORTTF2);//ʹ��CD4520 EN��
    MCF_GPIO_PORTTF &= ~(MCF_GPIO_PORTTF_PORTTF3 | MCF_GPIO_PORTTF_PORTTF4);//����CD4520 Reset����ʼ����

	/*���ڲ���*/
	TUart0_Puts("Hello MCF52259 send by TUART0!\r\n");
	//TUart1_Puts("Hello MCF52259 send by TUART1!\r\n");
	//TUart2_Puts("Hello MCF52259 send by TUART2!\r\n");

	/*DMAģ���ʼ��*/
	//TDMAx_Init(1);      //��ʼ��DMAģ��1������HREF��ȡͼ������
	//TDMAx_Init(2);
	TDMAx_Init(3);      //��ʼ��DMAģ��3������PCLK��ȡͼ������
    MCF_DMA3_SAR = (vuint32)0x40100044;//DMA3Դ��ַ��Ϊ A0-A7 ���������ŵ�ַ
    //Image_ToPC[0] = 0x01;
    //Image_ToPC[1] = 0xFE;
    //Image_ToPC[IMG_SIZE+2] = 0xFE;
    //Image_ToPC[IMG_SIZE+3] = 0x01;
    Image_Ptr = &Image_ToPC[2];

	/*PWMģ��   ��ʼ��*/
	TPWMx_INIT(0);  //����PWM��ʼ�� ���Ҹ��ݽ��߶���
	TPWMx_INIT(2);  //�ҵ��PWM��ʼ��
	TPWMx_INIT(4);  //���PWM��ʼ��

	/*GPTģ���ʼ��*/
	//TGPTx_Init(3);  //��ɫ���������벶��ģʽ��ʼ��
	TGPTx_Init(0);  //������1���벶��ģʽ��ʼ��
	TGPTx_Init(1);  //������2���벶��ģʽ��ʼ��
	
    TGPT0_ENINTER();   //��GPT0�ж�
    TGPT1_ENINTER();   //��GPT1�ж�
    //TGPT3_ENINTER();   //��GPT3�ж�

	/*��ʱ����ʼ��*/
    TPITx_Init(0);      //��ʼ��PIT0
    TPITx_Init(1);      //��ʼ��PIT1
    TPIT0_SetPMR(1);    //PIT0�ж�ʱ������Ϊ1ms
    TPIT1_SetPMR(1);    //PIT1�ж�ʱ������Ϊ1000ms
    //TPIT0_ENABLE();     //PIT1ʹ��


	/*�ⲿ�жϳ�ʼ��*/
	//TEPORTx_Init(1);    //���ⲿ�жϳ�ʼ��

    //TEPORTx_Init(7);    //���ⲿ�жϳ�ʼ��
    
	/*I2Cģ��(SCCB)��ʼ��*/
    //MCF_GPIO_PASPAR |= MCF_GPIO_PASPAR_SCL0_SCL0 | MCF_GPIO_PASPAR_SDA0_SDA0;
    //MCF_GPIO_DDRAS |= MCF_GPIO_DDRAS_DDRAS0 | MCF_GPIO_DDRAS_DDRAS1 | MCF_GPIO_DDRAS_DDRAS2;
    for(;;){}
    //������Դ���
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
    //������Դ���
	pwmCnt = 2880;//����ֵΪ3000�����Ƭ��һ��㣬���ƫ��һ��
    TPWM45_SetDTY(pwmCnt);
    while(1){}{
        while(pwmCnt<=3230){
            TPWM45_SetDTY(pwmCnt);
            pwmCnt++;
            delay2();
        }
        while(pwmCnt>=2510){
            TPWM45_SetDTY(pwmCnt);
            pwmCnt--;
            delay2();
        }
	}

}