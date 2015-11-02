#include "DIY_ComDef.h"
 /*
*****2015.4.23 PWMģ���д���
*****���ܣ�PWMģ��ʵ���������
*****��д����������Ҫ���⣺��
    1.���������ʹ��ʱӦ��ע��Ԥ��Ƶ��ռ�ձȵ�ʱ���Ƿ������Ҫ��ֵ
    2.ע��ʱ������������ʱ�Ӳ���ϵͳʱ��
    3.����ʹ�����PWM
��
*/
#ifdef TZ_MCF52259_PWM

void TPWMx_INIT(uint8 mode){
	//MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN0_PWM0 | MCF_GPIO_PTCPAR_DTIN1_PWM2 | MCF_GPIO_PTCPAR_DTIN2_PWM4 | MCF_GPIO_PTCPAR_DTIN3_PWM6;//
	//MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC0_PWM1 | MCF_GPIO_PTAPAR_ICOC1_PWM3 | MCF_GPIO_PTAPAR_ICOC2_PWM5 | MCF_GPIO_PTAPAR_ICOC3_PWM7;

    MCF_PWM_PWMPRCLK |= MCF_PWM_PWMPRCLK_PCKA(3) | MCF_PWM_PWMPRCLK_PCKB(3);
	/** A��Bʱ������Ϊ3��Ƶ���ɵ�ʱ��Ƶ��Ϊ = 80MHZ / 2 / 2 ^ 3 = 10MHZ */
    MCF_PWM_PWMSCLA = 2;//
	/** SA����Ϊ��1��ʱ�ӻ���10MHZ */
    MCF_PWM_PWMSCLB = 2;//
	/** SB����Ϊ��1��ʱ�ӻ���10MHZ */

	switch(mode){
	    //����mode�������г�ʼ����1��3��7ʹ�����벶�����Բ�����г�ʼ����
	    //��Ҫ��ʼ������0(���1)��2(���2)��4��5(��4��ϳ�16λ�����ƶ��)��6(����)
    	case 0:{//�ҵ��
    	    MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN0_PWM0;

            MCF_PWM_PWME &= ~MCF_PWM_PWME_PWME0;        //�ر�PWM0���
            MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL0;    //��������PWMͨ��Ϊ�ɵ͵�������
            MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK0;    //����ΪA/Bʱ��
            MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE0;     //����Ϊ��߶���
            MCF_PWM_PWMCTL &= ~MCF_PWM_PWMCTL_CON01;    //ȡ���ϲ�0��1PWMģ��
            MCF_PWM_PWMCNT0 = 0;//
            MCF_PWM_PWMPER0 = 250;
            MCF_PWM_PWMDTY0 = 250;
            MCF_PWM_PWME |= MCF_PWM_PWME_PWME0;         //����PWM0���
    	}break;
    	case 2:{//����
    	    MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN1_PWM2;

            MCF_PWM_PWME &= ~MCF_PWM_PWME_PWME2;        //�ر�PWM2���
            MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL2;    //��������PWMͨ��Ϊ�ɵ͵�������
            MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK2;    //����ΪA/Bʱ��
            MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE2;     //����Ϊ��߶���
            MCF_PWM_PWMCTL &= ~MCF_PWM_PWMCTL_CON23;    //ȡ���ϲ�2��3PWMģ��
            MCF_PWM_PWMCNT2 = 0;//
            MCF_PWM_PWMPER2 = 250;
            MCF_PWM_PWMDTY2 = 0;
            MCF_PWM_PWME |= MCF_PWM_PWME_PWME2;         //����PWM2���
    	}break;
        case 4:case 5:{
	        MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN2_PWM4;//ʹ��PWM4��PWM5�˿�
	        MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC2_PWM5;

            MCF_PWM_PWME &= ~(MCF_PWM_PWME_PWME5 | MCF_PWM_PWME_PWME4);     //�ر�PWM0���
            MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL4 | MCF_PWM_PWMPOL_PPOL5;  //��������PWMͨ��Ϊ�ɸߵ�������
            MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK4 | MCF_PWM_PWMCLK_PCLK5;  //PWM4ͨ������SA/SBʱ����
            MCF_PWM_PWMPRCLK |= MCF_PWM_PWMPRCLK_PCKA(3);//Aʱ������ΪԤ��Ƶ2^7 = 10000000
            MCF_PWM_PWMCAE &= ~(MCF_PWM_PWMCAE_CAE4 | MCF_PWM_PWMCAE_CAE5); //ȫ������Ϊ��߶���
            MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_CON45;
            MCF_PWM_PWMCNT4 = 0;
            MCF_PWM_PWMCNT5 = 0;
            MCF_PWM_PWMPER4 = 40000>>8;
            MCF_PWM_PWMPER5 = 40000&0xff;
            MCF_PWM_PWMDTY4 = 3000>>8;
            MCF_PWM_PWMDTY5 = 3000&0xff;
            MCF_PWM_PWME |= MCF_PWM_PWME_PWME4|MCF_PWM_PWME_PWME5;          //����PWM4���
        }break;
        default:break;
    }
	TUart0_Puts("PWM\r\n");

}
#endif