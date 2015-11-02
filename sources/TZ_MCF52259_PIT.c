#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT
/*
*****2015.4.21 PITģ���д���
*****���ܣ�PIT�ɱ�̶�ʱ��ģ����ж�ʱ
*****��д����������Ҫ���⣺��
    1.Ԥ��Ƶ����MCF_PIT0_PMR����ֵ��Ҫע�����
��
*/

void TPITx_Init(uint8 mode){
    switch(mode){
        default:case 0:{
            MCF_PIT0_PCSR = 0;
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT0_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            MCF_PIT0_PMR = 19531 * 1;   //���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK55;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR55 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN;
        }break;
        case 1:{
            MCF_PIT1_PCSR = 0;
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT1_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            MCF_PIT1_PMR = 19531 * 1;   //���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK56;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR56 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN;
        }break;
    }
}
__declspec(interrupt:0) void TPIT0_interrupt(void){

    //TUart0_Puts("PIT0!\r\n");
    MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־

}
__declspec(interrupt:0) void TPIT1_interrupt(void){

#if 0
/***********************************/
/*      8λ���ȫͼʵ����ȡ����     */
/***********************************/
    uint8 row,col,col2,data;
    for(row = 0; row<120; row++){
        for(col = 0,col2 = 1;col < 20; col++,col2++){
            if(col2>=20)
                col2 = 0;
            data = Image[row][col2];
            if(data==255){
                if(Image[row][col2+1] !=0)
                    Image_Ptr[row * 20 + col] = 0;
                else
                    Image_Ptr[row * 20 + col] = 0x01;
            }else if(data==0){
                if(Image[row][col2+1] !=255)
                    Image_Ptr[row * 20 + col] = 0;
                else
                    Image[row][col2+1] = 0x80;
            }else{
                switch(data){
                    case 0x01:Image_Ptr[row * 20 + col] = 0x01;break;
                    case 0xFE:case 0x03:Image_Ptr[row * 20 + col] = 0x02;break;
                    case 0xFC:case 0x07:Image_Ptr[row * 20 + col] = 0x04;break;
                    case 0xF8:case 0x0F:Image_Ptr[row * 20 + col] = 0x08;break;
                    case 0xF0:case 0x1F:Image_Ptr[row * 20 + col] = 0x10;break;
                    case 0xE0:case 0x3F:Image_Ptr[row * 20 + col] = 0x20;break;
                    case 0xC0:case 0x7F:Image_Ptr[row * 20 + col] = 0x40;break;
                    case 0x80:Image_Ptr[row * 20 + col] = 0x80;break;
                    default:Image_Ptr[row * 20 + col] = 0xff;
                }
            }
        }
    }
    for(row = 0;row<40;row++){
        Image_Ptr[row * 60 + 9] |= 0x01;
        Image_Ptr[row * 60 + 10] |= 0x80;
    }
#elif 1
/***********************************/
/*      8λɨ��ʵ����ȡ����         */
/***********************************/
    vuint8 data;
    vuint8 row,col;
    vuint8 leftJP,rightJP;
    for(row = 0; row<120; row++){
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
            Image_Ptr[row * 20 + col] = 0;
        }
        Image[row][col] = data;
    }
    leftJP = 9;
    rightJP = 10;
    for(row = 119;;row--){
        for(col = leftJP;col>=0;){
            if(0&&Image[row][col] == 255){
                leftJP = rightJP;           //������������������ô�����ʵ�����ұ���ʼ�����
                continue;                   //�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
            }else if(Image[row][col] == 0){
                if(col > 0 && Image[row][col-1] == 255){
                    Image_Ptr[row * 20 + col-1] = 0x01;
                    leftJP = col + 4;
                    Image_Edge[row][0] = col * 8 -1;
                    break;
                }else if(col == 0){
                    //Image_Ptr[row * 20 + col] = 0x01;
                    break;
                }
            }else{
                switch(Image[row][col]){
                    case 0xFE:{Image_Ptr[row * 20 + col] = 0x02;Image_Edge[row][0] = col * 8 + 7;}break;
                    case 0xFC:{Image_Ptr[row * 20 + col] = 0x04;Image_Edge[row][0] = col * 8 + 6;}break;
                    case 0xF8:{Image_Ptr[row * 20 + col] = 0x08;Image_Edge[row][0] = col * 8 + 5;}break;
                    case 0xF0:{Image_Ptr[row * 20 + col] = 0x10;Image_Edge[row][0] = col * 8 + 4;}break;
                    case 0xE0:{Image_Ptr[row * 20 + col] = 0x20;Image_Edge[row][0] = col * 8 + 3;}break;
                    case 0xC0:{Image_Ptr[row * 20 + col] = 0x40;Image_Edge[row][0] = col * 8 + 2;}break;
                    case 0x80:{Image_Ptr[row * 20 + col] = 0x80;Image_Edge[row][0] = col * 8 + 1;}break;
                }
                break;
                leftJP = col + 4;
            }
            col--;
        }
        for(col = rightJP;col<=19;){
            if(0&&Image[row][col] == 255){
                rightJP = leftJP;           //������������������ô�ұ���ʼ���������ʼ�����
                continue;                   //�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
            }else if(Image[row][col] == 0){
                if(col < 19 && Image[row][col+1] == 255){
                    Image_Ptr[row * 20 + col+1] = 0x80;
                    rightJP = col - 1;
                    break;
                }else if(col == 19){
                    break;
                }
            }else{
                switch(Image[row][col]){
                    case 0x01:{Image_Ptr[row * 20 + col] = 0x01;Image_Edge[row][1] = col * 8 + 7;}break;
                    case 0x03:{Image_Ptr[row * 20 + col] = 0x02;Image_Edge[row][1] = col * 8 + 6;}break;
                    case 0x07:{Image_Ptr[row * 20 + col] = 0x04;Image_Edge[row][1] = col * 8 + 5;}break;
                    case 0x0F:{Image_Ptr[row * 20 + col] = 0x08;Image_Edge[row][1] = col * 8 + 4;}break;
                    case 0x1F:{Image_Ptr[row * 20 + col] = 0x10;Image_Edge[row][1] = col * 8 + 3;}break;
                    case 0x3F:{Image_Ptr[row * 20 + col] = 0x20;Image_Edge[row][1] = col * 8 + 2;}break;
                    case 0x7F:{Image_Ptr[row * 20 + col] = 0x40;Image_Edge[row][1] = col * 8 + 1;}break;
                }
                break;
                rightJP = col - 1;
            }
            col++;
        }
#if 1
        Image_Edge[row][2] = (Image_Edge[row][0] + Image_Edge[row][1]) / 2;
        Image_Ptr[row * 20 + Image_Edge[row][2] / 8] = (1 << (8 - Image_Edge[row][2] % 8));
#endif
        if(row == 0)
            break;
        if( rightJP < leftJP ){//��֤���ҿ�ʼ�����н���
            rightJP = (rightJP + leftJP)/2;
            leftJP = rightJP;
        }
    }
#elif 0
/***********************************/
/*     ��λ����ʾƫ��У�����ͼ��   */
/***********************************/
    vuint8 data;
    vuint8 row,col;
    for(row = 0; row<120; row++){
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
        }
        Image[row][col] = data;
    }
#elif 0
/***********************************/
/*     ��λ����ʾԭʼ�ɼ���ͼ��     */
/***********************************/
    uint8 row,col,col2,data;
    for(row = 0; row<120; row++){
        for(col = 0,col2 = 1;col < 20; col++,col2++){
            Image_Ptr[row * 20 + col] = Image[row][col2];
        }
    }
#endif
#if 0
{
    TUart0_Puts("PIT1!\r\n");
    sprintf(TXBuffer,"%u,%u,%u,%u--",chang,hang,hang2,chang2);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u,%u,%u,%u,%u,%u---",MCF_DTIM3_DTCR,MCF_DMA3_SAR,MCF_DMA3_DAR,Image,&Image[1],MCF_DMA3_BCR&0xffffff);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u\r\n",&(Image[Cnt_HREF]));
    TUart0_Puts(TXBuffer);
    hang = 0;
    hang2 = 0;
    chang = 0;
    chang2 = 0;
    leftMSCnt = 0;
    rightMSCnt = 0;
}
#endif

    //TUart0_Puts("PIT1!\r\n");
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־
}
#endif