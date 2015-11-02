#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_EPORT
/*
*****2015.4.26 EPORTģ���д���
*****���ܣ�EPORTģ����Ҫ�����ⲿ�ж�
*****��д����������Ҫ���⣺��
    1.ע��GPIO����
    2.ע��������ã��������ػ��ǵȵȡ���
��
*/
vuint16 chang,hang;
vuint16 chang2,hang2;
uint8 bool = 1;
vuint8 Cnt_HREF = 0,Cnt_VSYN = 0;
vuint8 Cnt_Output = 0;

void TEPORTx_Init(uint8 mode){
    switch(mode){
        case 1:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ1_IRQ1;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA1_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD1;                  //����Ϊ��������
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE1;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD1;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK1;
            MCF_INTC0_ICR01 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�

        }break;
        case 3:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ3_IRQ3;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA3_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD3;                  //����Ϊ��������
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE3;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD3;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK3;
            MCF_INTC0_ICR03 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�
        }break;
        case 5:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ5_IRQ5;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA5_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD5;                  //����Ϊ��������
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE5;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD5;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK5;
            MCF_INTC0_ICR05 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�
        }break;
        case 7:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ7_IRQ7;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA7_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD7;                  //����Ϊ��������
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD7;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK7;
            MCF_INTC0_ICR07 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(1);   //�����ж����ȼ�
        }break;
    }
}
__declspec(interrupt) void EPORT7_interrupt(void){

    switch(Cnt_VSYN){
        case 1:{

            //TPIT0_ENABLE();
            MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;       //����жϱ�־λ
            MCF_DMA3_DAR = (vuint32)&Image[0];      //���������ַ
            MCF_DMA3_BCR = IMG_SIZE;                //�����С
            TPIT0_ENABLE();
/*
            color3Cnt[2] = MCF_GPT_GPTPACNT;//��ɫ
            MCF_GPT_GPTPACNT = 0;
            //��Ϊ��ɫ
            MCF_GPIO_CLRTJ &= ~(MCF_GPIO_CLRTJ_CLRTJ4 | MCF_GPIO_CLRTJ_CLRTJ5);//���s2 s3 ����Ϊ����ɫ        }
            MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //��λs0 s1 ����Ƶ��Ϊ100%
*/
        }break;
        case 2:{
            MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;      //����һ�γ��ɼ�
            MCF_GPT_GPTPACNT = 0;
        }break;
        case 3:{
            bool = 1;
            debugCnt = 0;
            TPIT1_ENABLE();

/*
            //sprintf(TXBuffer,"%u,%u,%u--",color3Cnt[0],color3Cnt[1],color3Cnt[2]);
            //TUart0_Puts(TXBuffer);
            if((color3Cnt[0]/(color3Cnt[1]+1))>3&&(color3Cnt[0]/(color3Cnt[2]+1))>3){
                startend = 1;
                //sprintf(TXBuffer,"Red\n");
                //TUart0_Puts(TXBuffer);
            }else{
                //sprintf(TXBuffer,"\n");
                //TUart0_Puts(TXBuffer);
            }
*/
            color3Cnt[0] = MCF_GPT_GPTPACNT;//��ɫ
            MCF_GPT_GPTPACNT = 0;
            //��Ϊ��ɫ
            MCF_GPIO_SETTJ |= MCF_GPIO_SETTJ_SETTJ5; //��λs3����Ϊ�����ɫ
            MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //��λs0 s1 ����Ƶ��Ϊ100%

        }break;
        case 4:{
            //sprintf(TXBuffer,"5:%u,%u\n",bool,debugCnt);
            //TUart0_Puts(TXBuffer);
            TPIT0_ENABLE();
/*
            color3Cnt[1] = MCF_GPT_GPTPACNT;//��ɫ
            MCF_GPT_GPTPACNT = 0;
            //��Ϊ��ɫ
            MCF_GPIO_SETTJ |= MCF_GPIO_SETTJ_SETTJ4; //��λs2 s3 ����Ϊ�����ɫ
            MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //��λs0 s1 ����Ƶ��Ϊ100%
*/
            if(chang >= 49){
                //showImg();
                chang = 0;
            }
            /*
            switch(colorSZCnt){
                case 0:{
                    colorSZCnt++;
                    color3Cnt[0] = MCF_GPT_GPTPACNT;//��ɫ
                    MCF_GPT_GPTPACNT = 0;
                    //��Ϊ��ɫ
                    MCF_GPIO_SETTJ |= MCF_GPIO_SETTJ_SETTJ5; //��λs3����Ϊ�����ɫ
                    MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //��λs0 s1 ����Ƶ��Ϊ100%
                }break;
                case 1:{
                    colorSZCnt++;
                    color3Cnt[1] = MCF_GPT_GPTPACNT;//��ɫ
                    MCF_GPT_GPTPACNT = 0;
                    //��Ϊ��ɫ
                    MCF_GPIO_SETTJ |= MCF_GPIO_SETTJ_SETTJ4; //��λs2 s3 ����Ϊ�����ɫ
                    MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //��λs0 s1 ����Ƶ��Ϊ100%
                }break;
                case 2:{
                    colorSZCnt = 0;
                    color3Cnt[2] = MCF_GPT_GPTPACNT;//��ɫ
                    MCF_GPT_GPTPACNT = 0;
                    //��Ϊ��ɫ
                    MCF_GPIO_CLRTJ &= ~(MCF_GPIO_CLRTJ_CLRTJ4 | MCF_GPIO_CLRTJ_CLRTJ5);//���s2 s3 ����Ϊ����ɫ        }
                    MCF_GPIO_SETTJ |= (MCF_GPIO_SETTJ_SETTJ2 | MCF_GPIO_SETTJ_SETTJ3); //��λs0 s1 ����Ƶ��Ϊ100%

                    sprintf(TXBuffer,"%u,%u,%u--",color3Cnt[0],color3Cnt[1],color3Cnt[2]);
                    TUart0_Puts(TXBuffer);
                    if((color3Cnt[0]/color3Cnt[1])>4&&(color3Cnt[0]/color3Cnt[2])>4){
                        sprintf(TXBuffer,"Red\n");
                        TUart0_Puts(TXBuffer);
                    }else{
                        sprintf(TXBuffer,"\n");
                        TUart0_Puts(TXBuffer);
                    }


                }break;
            }
            */
            Cnt_VSYN = 0;

        }break;
    }
    Cnt_VSYN++;
    chang++;

    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;    //���жϱ�־λ

}
__declspec(interrupt) void EPORT5_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF5;    //���жϱ�־λ
}
__declspec(interrupt) void EPORT3_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF3;    //���жϱ�־λ
}
__declspec(interrupt) void EPORT1_interrupt(void){
#if 0
    if(Cnt_HREF%2){
        MCF_DMA3_DAR = (vuint32)(&Image[Cnt_HREF/2][0]);
        MCF_DMA3_BCR = CAMERA_W_8 + CAMERA_OFFSET_8+1;
        MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;//����жϱ�־λ
        //TUart0_Putchar('G');
    }else{
        MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
        MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;
        //TUart0_Putchar('H');
    }
    Cnt_HREF++;
#else
    MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;       //����жϱ�־λ
    MCF_DMA3_DAR = (vuint32)(&Image[Cnt_HREF][0]);
    MCF_DMA3_BCR = CAMERA_W_8;
    MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
    //MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;
    Cnt_HREF++;
#endif
    hang ++;
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;    //���жϱ�־λ
}

void showImg(void){
#if 0
    //DMA�Զ����
    TUart0_Putchar(0x01);
    TUart0_Putchar(0xFE);

    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

    TUart0_Putchar(0xFE);
    TUart0_Putchar(0x01);
#elif 1
    //DMA�Զ����
    Image_ToPC[0] = 0x01;
    Image_ToPC[1] = 0xFE;
    Image_ToPC[IMG_SIZE+2] = 0xFE;
    Image_ToPC[IMG_SIZE+3] = 0x01;
    TUart0_DMAPutBuffer(Image_ToPC,IMG_SIZE+4);

#elif 0

    TUart0_Putchar(0);
    TUart0_Putchar(255);
    TUart0_Putchar(1);
    TUart0_Putchar(0);
    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

#elif 0       //ѭ���������
    uint8 a,b;
    TUart0_Putchar(0x01);
    TUart0_Putchar(0xFE);

    for(a = 0;a<CAMERA_H;a++){
        for(b = 0;b<CAMERA_W_8;b++)
            TUart0_Putchar(Image[a][b]);
        //TUart0_Putchar(Image[a][0]);

    }

    TUart0_Putchar(0xFE);
    TUart0_Putchar(0x01);
#endif

}
#endif