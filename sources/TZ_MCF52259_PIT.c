#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT
#include "TZ_CAMERA_VAR.c"
/*
*****2015.4.21 PITģ���д���
*****���ܣ�PIT�ɱ�̶�ʱ��ģ����ж�ʱ
*****��д����������Ҫ���⣺��
    1.Ԥ��Ƶ����MCF_PIT0_PMR����ֵ��Ҫע�����
��
*/
uint8 ImageC[CAMERA_H][CAMERA_W] = {0};
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
#if 0
    vuint8 row;
    vuint16 left_Offset_Cnt = 0,right_Offset_Cnt = 0;
    vuint8 left_Offset_Max = 0,right_Offset_Max = 0;
    for(row = 119;;row--){
        if(Image_Edge[row][2] != 0){
            if(Image_Edge[row][2] < 80){
                left_Offset_Cnt += (80 - Image_Edge[row][2]);
                if((80 - Image_Edge[row][2]) > left_Offset_Max)
                    left_Offset_Max = (uint8)(80 - Image_Edge[row][2]);
            }else{
                right_Offset_Cnt += (Image_Edge[row][2] - 80);
                if((Image_Edge[row][2] - 80) > right_Offset_Max)
                    right_Offset_Max = (uint8)(Image_Edge[row][2] - 80);
            }
        }
        if(row == 0)
            break;
    }

    if(left_Offset_Cnt>right_Offset_Cnt){
        pwmCnt = 3750 - (left_Offset_Cnt+right_Offset_Cnt)/3*(left_Offset_Cnt/(float)(left_Offset_Cnt+right_Offset_Cnt));
        TPWM45_SetDTY(pwmCnt);
    }else{
        pwmCnt = 3750 + (left_Offset_Cnt+right_Offset_Cnt)/3*(right_Offset_Cnt/(float)(left_Offset_Cnt+right_Offset_Cnt));
        TPWM45_SetDTY(pwmCnt);
    }
    //sprintf(TXBuffer,"%u,%u,%u\r\n",pwmCnt,left_Offset_Cnt,right_Offset_Cnt);
    //TUart0_Puts(TXBuffer);
#elif 1
    vuint8 row;
    uint8 pitch;
    float ii;
    for(row = 118,pitch = Image_Edge[row+1][2];row >=90;row--){
        pitch = (pitch + Image_Edge[row][2]) /2;
    }
    if(pitch>CAMERA_W/2&&pwmCnt<4250){
        pwmCnt+=15;
    }else if(pitch<CAMERA_W/2&&pwmCnt>3150){
        pwmCnt-=15;
    }else{
        pwmCnt = 3700;
    }
    TPWM45_SetDTY(pwmCnt);
#endif
    //TUart0_Puts("PIT0!\r\n");
    MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־

}
__declspec(interrupt:0) void TPIT1_interrupt(void){

#define EDGE_OFFSET 2
/*ͼ�������ȡ����һ����ʼλ��ƫ��ֵ*/
#define EDGE_INVALID 255
/*������Чֵ���壬���벻Ϊͼ���е�ȡֵ*/
#define IMG_ANALYSE_MODE    11
/*
1:8λ���ȫͼʵ����ȡ����
2:8λɨ��ʵ����ȡ����
3:8λѡ��ɨ��ʵ����ȡ����
4:uint8����ɨ�跨��ȡ����
5:uint8������ȡ����
11:��λ����ʾƫ��У�����ͼ��
12:��λ����ʾԭʼ�ɼ���ͼ��
0:����ģʽ�������������ֵ��������ͼ����
*/

#if (IMG_ANALYSE_MODE==1)
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
#elif  (IMG_ANALYSE_MODE==2)
/***********************************/
/*      8λɨ��ʵ����ȡ����         */
/***********************************/
    vuint8 data;
    int8 row,col;
    int8 leftJPStart,rightJPStart;      //JP��Jump Point����㣬����������ұ��߼�����ʼλ��
    int8 leftJPEnd,rightJPEnd;      //JP��Jump Point����㣬����������ұ��߼�����ʼλ��
    vuint8 edgeMiss = 0;        //����־����

    for(row = 0; row<120; row++){
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
            Image_Ptr[row * 20 + col] = 0;
        }
        Image_Ptr[row * 20 + col] = 0;
        Image[row][col] = data;
    }
    leftJPStart = 9;
    rightJPStart = 10;
    leftJPEnd = 0;
    rightJPEnd = 19;
    for(row = 119;;row--){
        Image_Edge[row][0] = EDGE_INVALID;
        Image_Edge[row][1] = EDGE_INVALID;
        Image_Edge[row][2] = EDGE_INVALID;
        for(col = leftJPStart;col>=0;){
            if(Image[row][col] == 255){
                if(edgeMiss){                   //�������־����λ���ظ������һ��
                    Image_Edge[row][0] = EDGE_INVALID;
                    edgeMiss = 0;               //�������־��
                    break;
                }else{
                    leftJPStart = rightJPStart;           //������������������ô�����ʼ�����ұ���ʼ�����
                    edgeMiss = 1;               //��λ����־��
                    continue;                   //�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
                }
            }else if(Image[row][col] == 0){
                if(col > 0 && Image[row][col-1] == 0xff){
                    Image_Ptr[row * 20 + col-1] = 0x01;
                    leftJPStart = (int8)(col + EDGE_OFFSET);
                    Image_Edge[row][0] = (uint8)(col * 8 -1);
                    break;
                }
            }else{
                switch(Image[row][col]){
                    case 0xFE:{Image_Ptr[row * 20 + col] = 0x02;Image_Edge[row][0] = (uint8)(col * 8 + 7);}break;
                    case 0xFC:{Image_Ptr[row * 20 + col] = 0x04;Image_Edge[row][0] = (uint8)(col * 8 + 6);}break;
                    case 0xF8:{Image_Ptr[row * 20 + col] = 0x08;Image_Edge[row][0] = (uint8)(col * 8 + 5);}break;
                    case 0xF0:{Image_Ptr[row * 20 + col] = 0x10;Image_Edge[row][0] = (uint8)(col * 8 + 4);}break;
                    case 0xE0:{Image_Ptr[row * 20 + col] = 0x20;Image_Edge[row][0] = (uint8)(col * 8 + 3);}break;
                    case 0xC0:{Image_Ptr[row * 20 + col] = 0x40;Image_Edge[row][0] = (uint8)(col * 8 + 2);}break;
                    case 0x80:{Image_Ptr[row * 20 + col] = 0x80;Image_Edge[row][0] = (uint8)(col * 8 + 1);}break;
                }
                leftJPStart = (int8)(col + EDGE_OFFSET);
                break;
            }
            col--;
        }
        for(col = rightJPStart;col<=19;){
            if(Image[row][col] == 255){
                if(edgeMiss){                   //�������־����λ���ظ������һ��
                    Image_Edge[row][1] = EDGE_INVALID;
                    edgeMiss = 0;               //�������־��
                    break;
                }else{
                    rightJPStart = leftJPStart;           //������������������ô�ұ���ʼ���������ʼ�����
                    edgeMiss = 1;               //��λ����־��
                    continue;                   //�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
                }
            }else if(Image[row][col] == 0){
                if(col < 19 && Image[row][col+1] == 0xff){
                    Image_Ptr[row * 20 + col+1] = 0x80;
                    rightJPStart = (int8)(col - EDGE_OFFSET);
                    Image_Edge[row][1] = (uint8)(col * 8 + 9);
                    break;
                }
            }else{
                switch(Image[row][col]){
                    case 0x01:{Image_Ptr[row * 20 + col] = 0x01;Image_Edge[row][1] = (uint8)(col * 8 + 7);}break;
                    case 0x03:{Image_Ptr[row * 20 + col] = 0x02;Image_Edge[row][1] = (uint8)(col * 8 + 6);}break;
                    case 0x07:{Image_Ptr[row * 20 + col] = 0x04;Image_Edge[row][1] = (uint8)(col * 8 + 5);}break;
                    case 0x0F:{Image_Ptr[row * 20 + col] = 0x08;Image_Edge[row][1] = (uint8)(col * 8 + 4);}break;
                    case 0x1F:{Image_Ptr[row * 20 + col] = 0x10;Image_Edge[row][1] = (uint8)(col * 8 + 3);}break;
                    case 0x3F:{Image_Ptr[row * 20 + col] = 0x20;Image_Edge[row][1] = (uint8)(col * 8 + 2);}break;
                    case 0x7F:{Image_Ptr[row * 20 + col] = 0x40;Image_Edge[row][1] = (uint8)(col * 8 + 1);}break;
                }
                rightJPStart = (int8)(col - EDGE_OFFSET);
                break;
            }
            col++;
        }

#if 1
        if(Image_Edge[row][0] == EDGE_INVALID || Image_Edge[row][1] == EDGE_INVALID ){
            //Image_Edge[row][2] = CAMERA_W/2;
            Image_Edge[row][2] = 0;
            //sprintf(TXBuffer,"%u,%u,%u\r\n",row,Image_Edge[row][0],Image_Edge[row][1]);
            //TUart0_Puts(TXBuffer);
        }else{
            Image_Edge[row][2] = (uint8)((Image_Edge[row][0] + Image_Edge[row][1]) / 2);
            Image_Ptr[row * 20 + Image_Edge[row][2] / 8] = (uint8)(1 << (8 - Image_Edge[row][2] % 8));
            //sprintf(TXBuffer,"%u,%u,%u,%u\r\n",row,Image_Edge[row][0],Image_Edge[row][1],Image_Edge[row][2]);
            //TUart0_Puts(TXBuffer);
        }
#endif
        //Image_Ptr[row * 20 + 9] |= 0x01;
        if(row == 0)//��rowΪ��ʱ���˳�ѭ��
            break;
        if( rightJPStart < leftJPStart ){//��֤���ҿ�ʼ�����н���
            rightJPStart = (uint8)((rightJPStart + leftJPStart)/2);
            leftJPStart = rightJPStart;
        }
        rightJPEnd = (uint8)(rightJPStart + 3 * EDGE_OFFSET);
        if(rightJPEnd>19)rightJPEnd = 19;
        leftJPEnd = (uint8)(leftJPStart - 3 * EDGE_OFFSET);
        if(leftJPEnd<0)rightJPEnd = 0;
    }
#elif  (IMG_ANALYSE_MODE==3)
/***********************************/
/*    8λѡ��ɨ��ʵ����ȡ����       */
/***********************************/
    vuint8 data;
    vuint8 row,col;
    vuint8 leftJPStart,rightJPStart;
    vuint8 edgeMiss = 0;        //����־����
    vuint8 judgeMode = 0;

    for(row = 0; row<120; row++){   //ͼ��ƫ�ƻ�ԭ
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
            Image_Ptr[row * 20 + col] = 0;
        }
        Image[row][col] = data;
    }
    leftJPStart = 9;
    rightJPStart = 10;
    for(row = 119;;row--){
        Image_Edge[row][0] = 0;
        Image_Edge[row][1] = 0;
        Image_Edge[row][2] = 0;
        switch(judgeMode){
            case 0:{
                for(col = leftJPStart;col>=0;){
                    if(0&&Image[row][col] == 255){
                        if(edgeMiss){                   //�������־����λ���ظ������һ��
                            Image_Edge[row][0] = EDGE_INVALID;
                            edgeMiss = 0;               //�������־��
                            break;
                        }else{
                            leftJPStart = rightJPStart;           //������������������ô�����ʵ�����ұ���ʼ�����
                            edgeMiss = 1;               //��λ����־��
                            continue;                   //�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
                        }
                    }else if(Image[row][col] == 0){
                        if(col > 0 && Image[row][col-1] == 255){
                            Image_Ptr[row * 20 + col-1] = 0x01;
                            leftJPStart = col + EDGE_OFFSET;
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
                        leftJPStart = col + EDGE_OFFSET;
                        break;
                    }
                    col--;
                }
            }break;
            case 1:{
                for(col = rightJPStart;col<=19;){
                    if(0&&Image[row][col] == 255){
                        if(edgeMiss){                   //�������־����λ���ظ������һ��
                            Image_Edge[row][1] = EDGE_INVALID;
                            edgeMiss = 0;               //�������־��
                            break;
                        }else{
                            rightJPStart = leftJPStart;           //������������������ô�ұ���ʼ���������ʼ�����
                            edgeMiss = 1;               //��λ����־��
                            continue;                   //�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
                        }
                    }else if(Image[row][col] == 0){
                        if(col < 19 && Image[row][col+1] == 255){
                            Image_Ptr[row * 20 + col+1] = 0x80;
                            rightJPStart = col - EDGE_OFFSET;
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
                        rightJPStart = col - EDGE_OFFSET;
                        break;
                    }
                    col++;
                }
            }break;
        }
#if 1
        Image_Edge[row][2] = (Image_Edge[row][0] + Image_Edge[row][1]) / 2;
        Image_Ptr[row * 20 + Image_Edge[row][2] / 8] = (1 << (8 - Image_Edge[row][2] % 8));
#endif
        if(row == 0)
            break;
        if( rightJPStart < leftJPStart ){//��֤���ҿ�ʼ�����н���
            rightJPStart = (rightJPStart + leftJPStart)/2;
            leftJPStart = rightJPStart;
        }
    }
#elif (IMG_ANALYSE_MODE==4)
/***********************************/
/*     uint8����ɨ�跨��ȡ����   */
/***********************************/
    vuint8 data;
    int8 row,col;
    int8 leftJPStart,rightJPStart;  //JP��Jump Point����㣬����������ұ��߼�����ʼλ��
    int8 leftJPEnd,rightJPEnd;      //JP��Jump Point����㣬����������ұ��߼��Ľ���λ��
    vuint8 leftFindBool = 0,rightFindBool = 0;
    vuint8 edgeStart[2][2] = {0};
    vuint8 edgeMiss = 0;        //����־����
    vuint8 edgeMode = 0;
    for(row = 0; row<120; row++){
        data = Image[row][0];
        for(col = 0;col < 19; col++){
          Image[row][col] = Image[row][col + 1];
          if(Image[row][col]&0x01)  ImageC[row][col*8+7] = 1; else ImageC[row][col*8+7] = 0;
          if(Image[row][col]&0x02)  ImageC[row][col*8+6] = 1; else ImageC[row][col*8+6] = 0;
          if(Image[row][col]&0x04)  ImageC[row][col*8+5] = 1; else ImageC[row][col*8+5] = 0;
          if(Image[row][col]&0x08)  ImageC[row][col*8+4] = 1; else ImageC[row][col*8+4] = 0;
          if(Image[row][col]&0x10)  ImageC[row][col*8+3] = 1; else ImageC[row][col*8+3] = 0;
          if(Image[row][col]&0x20)  ImageC[row][col*8+2] = 1; else ImageC[row][col*8+2] = 0;
          if(Image[row][col]&0x40)  ImageC[row][col*8+1] = 1; else ImageC[row][col*8+1] = 0;
          if(Image[row][col]&0x80)  ImageC[row][col*8+0] = 1; else ImageC[row][col*8+0] = 0;
        }
        Image[row][col] = data;
        if(Image[row][col]&0x01)  ImageC[row][col*8+7] = 1; else ImageC[row][col*8+7] = 0;
        if(Image[row][col]&0x02)  ImageC[row][col*8+6] = 1; else ImageC[row][col*8+6] = 0;
        if(Image[row][col]&0x04)  ImageC[row][col*8+5] = 1; else ImageC[row][col*8+5] = 0;
        if(Image[row][col]&0x08)  ImageC[row][col*8+4] = 1; else ImageC[row][col*8+4] = 0;
        if(Image[row][col]&0x10)  ImageC[row][col*8+3] = 1; else ImageC[row][col*8+3] = 0;
        if(Image[row][col]&0x20)  ImageC[row][col*8+2] = 1; else ImageC[row][col*8+2] = 0;
        if(Image[row][col]&0x40)  ImageC[row][col*8+1] = 1; else ImageC[row][col*8+1] = 0;
        if(Image[row][col]&0x80)  ImageC[row][col*8+0] = 1; else ImageC[row][col*8+0] = 0;
    }
#if 0
    for(row = 0; row<120; row++){
        for(col = 0;col < 20; col++){
        	Image_Ptr[row * 20 + col] = 0x00;
          if(ImageC[row][col*8+7]) Image_Ptr[row * 20 + col] |= 0x01;else Image_Ptr[row * 20 + col] &= ~0x01;
          if(ImageC[row][col*8+6]) Image_Ptr[row * 20 + col] |= 0x02;else Image_Ptr[row * 20 + col] &= ~0x02;
          if(ImageC[row][col*8+5]) Image_Ptr[row * 20 + col] |= 0x04;else Image_Ptr[row * 20 + col] &= ~0x04;
          if(ImageC[row][col*8+4]) Image_Ptr[row * 20 + col] |= 0x08;else Image_Ptr[row * 20 + col] &= ~0x08;
          if(ImageC[row][col*8+3]) Image_Ptr[row * 20 + col] |= 0x10;else Image_Ptr[row * 20 + col] &= ~0x10;
          if(ImageC[row][col*8+2]) Image_Ptr[row * 20 + col] |= 0x20;else Image_Ptr[row * 20 + col] &= ~0x20;
          if(ImageC[row][col*8+1]) Image_Ptr[row * 20 + col] |= 0x40;else Image_Ptr[row * 20 + col] &= ~0x40;
          if(ImageC[row][col*8+0]) Image_Ptr[row * 20 + col] |= 0x80;else Image_Ptr[row * 20 + col] &= ~0x80;
        }
    }
#endif
#if 1
    for(row = 0; row<120; row++){
        for(col = 0;col < 20; col++){
        	Image_Ptr[row * 20 + col] = 0x00;
        }
    }
	leftFindBool = 1;
	rightFindBool = 1;
    for(row = 119;;row--){
        for(col = leftJPStart;leftFindBool==0&&col>=1;){
            if(Image[row][col] == 255){
                if(edgeMiss){                   	//�������־����λ���ظ������һ��
                    edgeMiss = 0;               	//�������־��
                    break;
                }else{
                    rightJPStart = leftJPStart;		//������������������ô�ұ���ʼ���������ʼ�����
                    edgeMiss = 1;               	//��λ����־��
                    continue;                   	//�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
                }
            }else if(Image[row][col] == 0){
		            if(col > 0 && Image[row][col-1] == 255){
		            		edgeStart[0][0] = row;
		            		edgeStart[0][1] = col*8 - 1;
		                break;
		            }else if(col == 0){
		                break;
		            }
            }else{
            	edgeStart[0][0] = row;
                switch(Image[row][col]){
                    case 0xFE:{Image_Ptr[row * 20 + col] = 0x02;edgeStart[0][1] = (uint8)(col * 8 + 7);}break;
                    case 0xFC:{Image_Ptr[row * 20 + col] = 0x04;edgeStart[0][1] = (uint8)(col * 8 + 6);}break;
                    case 0xF8:{Image_Ptr[row * 20 + col] = 0x08;edgeStart[0][1] = (uint8)(col * 8 + 5);}break;
                    case 0xF0:{Image_Ptr[row * 20 + col] = 0x10;edgeStart[0][1] = (uint8)(col * 8 + 4);}break;
                    case 0xE0:{Image_Ptr[row * 20 + col] = 0x20;edgeStart[0][1] = (uint8)(col * 8 + 3);}break;
                    case 0xC0:{Image_Ptr[row * 20 + col] = 0x40;edgeStart[0][1] = (uint8)(col * 8 + 2);}break;
                    case 0x80:{Image_Ptr[row * 20 + col] = 0x80;edgeStart[0][1] = (uint8)(col * 8 + 1);}break;
                }
                leftJPStart = (int8)(col + EDGE_OFFSET);
                break;
            }
            col--;
        }
        for(col = rightJPStart;rightFindBool&&col<=19;){
            if(Image[row][col] == 255){
                if(edgeMiss){                   //�������־����λ���ظ������һ��
                    edgeMiss = 0;               //�������־��
                    break;
                }else{
                    rightJPStart = leftJPStart;           //������������������ô�ұ���ʼ���������ʼ�����
                    edgeMiss = 1;               //��λ����־��
                    continue;                   //�Թ��˴�ѭ��������col��û�иı䣬�Ӷ�ʵ�����¼����һ��
                }
            }else if(Image[row][col] == 0){
                if(col < 19 && Image[row][col+1] == 0xff){
            		edgeStart[1][0] = row;
		            edgeStart[1][1] = col*8 + 9;
		               break;
                }else if(col == 19){
                	break;
                }
            }else{
            	edgeStart[1][0] = row;
                switch(Image[row][col]){
                    case 0x01:{Image_Ptr[row * 20 + col] = 0x01;edgeStart[1][1] = (uint8)(col * 8 + 7);}break;
                    case 0x03:{Image_Ptr[row * 20 + col] = 0x02;edgeStart[1][1] = (uint8)(col * 8 + 6);}break;
                    case 0x07:{Image_Ptr[row * 20 + col] = 0x04;edgeStart[1][1] = (uint8)(col * 8 + 5);}break;
                    case 0x0F:{Image_Ptr[row * 20 + col] = 0x08;edgeStart[1][1] = (uint8)(col * 8 + 4);}break;
                    case 0x1F:{Image_Ptr[row * 20 + col] = 0x10;edgeStart[1][1] = (uint8)(col * 8 + 3);}break;
                    case 0x3F:{Image_Ptr[row * 20 + col] = 0x20;edgeStart[1][1] = (uint8)(col * 8 + 2);}break;
                    case 0x7F:{Image_Ptr[row * 20 + col] = 0x40;edgeStart[1][1] = (uint8)(col * 8 + 1);}break;
                }
                rightJPStart = (int8)(col - EDGE_OFFSET);
                break;
            }
            col++;
        }
        if(rightFindBool&&leftFindBool)
        	break;
    }
    row = edgeStart[0][0] + 1;
    col = edgeStart[0][1];
    for(;(col>0||col<160)&&(row>0&&row<120);){
    		if(ImageC[row][col] == 0){
    				if(ImageC[row][col+1] == 0 && ImageC[row][col-1] == 1){
    						if(ImageC[row+1][col] == 0 && ImageC[row-1][col] == 1){
    								Image_Ptr[row * 20 + col/8] = 1<<(8 - col%8);
    								edgeMode = 1;//�������
    								row++;
    						}else if(ImageC[row+1][col] == 1 && ImageC[row-1][col] == 0){
    								Image_Ptr[row * 20 + col/8] = 1<<(8 - col%8);
    								edgeMode = 2;//�������
    								row++;
    								if(col>0)
    									col--;
    						}else if(ImageC[row+1][col] == 0 && ImageC[row-1][col] == 0){
    								Image_Ptr[row * 20 + col/8] = 1<<(8 - col%8);
    								edgeMode = 2;//ֱ������
    								row++;
    								if(col>0)
    									col--;
    						}else{
    							TUart0_Putchar('d');
    						}
    				}else if(ImageC[row][col+1] == 0 && ImageC[row][col-1] == 0){
    						if(ImageC[row+1][col] == 1 && ImageC[row+1][col+1] == 1){
    								Image_Ptr[row * 20 + col/8] = 1<<(8 - col%8);
    								edgeMode = 3;
    								col++;
    						}else if(ImageC[row-1][col] == 1 && ImageC[row-1][col+1] == 1){
    								Image_Ptr[row * 20 + col/8] = 1<<(8 - col%8);
    								edgeMode = 4;
    								col++;
    						}else{
    							TUart0_Putchar('e');
    						}
    				}
    		}else{
    				col++;
    				TUart0_Putchar('f');
    		}
    }
#endif
#elif (IMG_ANALYSE_MODE==5)
/***********************************/
/*     uint8����ɨ�跨��ȡ����   */
/***********************************/

    vuint8 data;
    int16 row,col;											//�У��У�Ҳ����u v����
    
    int16 leftEdgeStart = CAMERA_W/2;  		//����߼�����ʼλ��
    int16 leftEdgeEnd = 0;      					//����߼��Ľ���λ��
    
    int16 rightEdgeStart = CAMERA_W/2+1;	//�ұ��߼�����ʼλ��
    int16 rightEdgeEnd = CAMERA_W - 1;		//�ұ��߼��Ľ���λ��
    
    uint8 img_EdgeInfo[CAMERA_H][5] = {0};//ͼ��߽���Ϣ 
    //[0]����߽� [1]���ұ߽� [2]������ [3]����ȡ���ı߽���Ϣ [4]��б��\����
    
    int8 leftEdgeFind = 0,rightEdgeFind = 0;				//���ұ߽��Ƿ��ҵ���־��
    int8 leftEdgeMissCnt = 0,rightEdgeMissCnt = 0;	//���ұ߽�miss��������
    int8 leftEdgeBool = 1,rightEdgeBool = 1;				//���ұ߽�
    
    uint8 leftJPMissLine = 0,rightJPMissLine = 0;
    uint8 leftJPMissLast[2],rightJPMissLast[2];
    
    //bit��ԭΪchar��
    for(row = 0; row<120; row++){
        data = Image[row][0];
        for(col = 0;col < 19; col++){
          Image[row][col] = Image[row][col + 1];
          if(Image[row][col]&0x01)  ImageC[row][col*8+7] = 1; else ImageC[row][col*8+7] = 0;
          if(Image[row][col]&0x02)  ImageC[row][col*8+6] = 1; else ImageC[row][col*8+6] = 0;
          if(Image[row][col]&0x04)  ImageC[row][col*8+5] = 1; else ImageC[row][col*8+5] = 0;
          if(Image[row][col]&0x08)  ImageC[row][col*8+4] = 1; else ImageC[row][col*8+4] = 0;
          if(Image[row][col]&0x10)  ImageC[row][col*8+3] = 1; else ImageC[row][col*8+3] = 0;
          if(Image[row][col]&0x20)  ImageC[row][col*8+2] = 1; else ImageC[row][col*8+2] = 0;
          if(Image[row][col]&0x40)  ImageC[row][col*8+1] = 1; else ImageC[row][col*8+1] = 0;
          if(Image[row][col]&0x80)  ImageC[row][col*8+0] = 1; else ImageC[row][col*8+0] = 0;
        }
        Image[row][col] = data;
        if(Image[row][col]&0x01)  ImageC[row][col*8+7] = 1; else ImageC[row][col*8+7] = 0;
        if(Image[row][col]&0x02)  ImageC[row][col*8+6] = 1; else ImageC[row][col*8+6] = 0;
        if(Image[row][col]&0x04)  ImageC[row][col*8+5] = 1; else ImageC[row][col*8+5] = 0;
        if(Image[row][col]&0x08)  ImageC[row][col*8+4] = 1; else ImageC[row][col*8+4] = 0;
        if(Image[row][col]&0x10)  ImageC[row][col*8+3] = 1; else ImageC[row][col*8+3] = 0;
        if(Image[row][col]&0x20)  ImageC[row][col*8+2] = 1; else ImageC[row][col*8+2] = 0;
        if(Image[row][col]&0x40)  ImageC[row][col*8+1] = 1; else ImageC[row][col*8+1] = 0;
        if(Image[row][col]&0x80)  ImageC[row][col*8+0] = 1; else ImageC[row][col*8+0] = 0;
    }
#if 0
    for(row = 0; row<120; row++){
        for(col = 0;col < 20; col++){
        	Image_Ptr[row * 20 + col] = 0x00;
          if(ImageC[row][col*8+7]) Image_Ptr[row * 20 + col] |= 0x01;else Image_Ptr[row * 20 + col] &= ~0x01;
          if(ImageC[row][col*8+6]) Image_Ptr[row * 20 + col] |= 0x02;else Image_Ptr[row * 20 + col] &= ~0x02;
          if(ImageC[row][col*8+5]) Image_Ptr[row * 20 + col] |= 0x04;else Image_Ptr[row * 20 + col] &= ~0x04;
          if(ImageC[row][col*8+4]) Image_Ptr[row * 20 + col] |= 0x08;else Image_Ptr[row * 20 + col] &= ~0x08;
          if(ImageC[row][col*8+3]) Image_Ptr[row * 20 + col] |= 0x10;else Image_Ptr[row * 20 + col] &= ~0x10;
          if(ImageC[row][col*8+2]) Image_Ptr[row * 20 + col] |= 0x20;else Image_Ptr[row * 20 + col] &= ~0x20;
          if(ImageC[row][col*8+1]) Image_Ptr[row * 20 + col] |= 0x40;else Image_Ptr[row * 20 + col] &= ~0x40;
          if(ImageC[row][col*8+0]) Image_Ptr[row * 20 + col] |= 0x80;else Image_Ptr[row * 20 + col] &= ~0x80;
        }
    }
#endif
#if 1
    for(row = 0; row<120; row++){
        for(col = 0;col < 20; col++){
        	Image_Ptr[row * 20 + col] = 0x00;
        }
    }

    for(row = 119;row>=0;row --){
        leftJPFind = 0;
        rightJPFind = 0;
        Image_Edge[row][0] = 0;
        Image_Edge[row][1] = CAMERA_W;
        Image_Edge[row][2] = 0;
        Image_Edge[row][3] = 0;
        Image_Edge[row][4] = 0;
        for(col = leftJPStart;leftJPMissBool&&col>leftJPEnd;col--){
            if(ImageC[row][col] == 0 && ImageC[row][col-1] == 1 ){
                Image_Edge[row][0] = col-1;
                leftJPFind = 1;
                leftJPStart = col + 8;
                leftJPEnd = col - 8;
                if(col != leftJPMissLast[1]){
                    Image_Edge[leftJPMissLast[0]][3] = ((leftJPMissLast[0] - row)*(leftJPMissLast[0] - row) + (leftJPMissLast[1] - col)*(leftJPMissLast[1] - col));
                    leftJPMissLast[0] = row;
                    leftJPMissLast[1] = col;
                }
                if(leftJPEnd<0)
                    leftJPEnd = 0;
                break;
            }
        }
        for(col = rightJPStart;rightJPMissBool&&col<rightJPEnd;col++){
            if(ImageC[row][col] == 0 && ImageC[row][col+1] == 1 ){
                Image_Edge[row][1] = col+1;
                rightJPFind = 1;
                rightJPStart = col - 8;
                rightJPEnd = col + 8;
                if(col != rightJPMissLast[1]){
                    Image_Edge[rightJPMissLast[0]][4] = ((rightJPMissLast[0] - row)*(rightJPMissLast[0] - row) + (rightJPMissLast[1] - col)*(rightJPMissLast[1] - col));
                    rightJPMissLast[0] = row;
                    rightJPMissLast[1] = col;
                }
                if(rightJPEnd>CAMERA_W)
                    rightJPEnd = CAMERA_W-1;
                break;
            }
        }
        if(leftJPStart>rightJPStart){
            leftJPStart = (leftJPStart + rightJPStart)/2;
            rightJPStart = leftJPStart + 1;
        }
        if(leftJPFind && rightJPFind){
            Image_Edge[row][2] = (Image_Edge[row][0] + Image_Edge[row][1])/2;
            Image_Ptr[20 * row + Image_Edge[row][0] / 8] = 1 << (7 - Image_Edge[row][0] % 8);
            Image_Ptr[20 * row + Image_Edge[row][1] / 8] = 1 << (7 - Image_Edge[row][1] % 8);
            //Image_Ptr[20 * row + Image_Edge[row][2] / 8] = 1 << (7 - Image_Edge[row][2] % 8);
            leftJPMissCnt = 0;
            rightJPMissCnt = 0;

        }else if(leftJPFind || rightJPFind){
            if(leftJPFind == 0){
                leftJPMissCnt++;
                Image_Ptr[20 * row + Image_Edge[row][1] / 8] = 1 << (7 - Image_Edge[row][1] % 8);
            }if(rightJPFind == 0){
                rightJPMissCnt++;
                Image_Ptr[20 * row + Image_Edge[row][0] / 8] = 1 << (7 - Image_Edge[row][0] % 8);
            }
        }else{
            leftJPMissCnt++;
            rightJPMissCnt++;
        }
        if(leftJPMissBool&&leftJPMissCnt>5){
            leftJPMissBool = 0;
            leftJPMissLine = row;
        }
        if(rightJPMissBool&&rightJPMissCnt>5){
            rightJPMissBool = 0;
            rightJPMissLine = row;
        }
        if(!rightJPMissBool && !leftJPMissBool){
            if(leftJPMissLine > rightJPMissLine){
                if(leftJPMissLine>110 && leftJPMissLine - rightJPMissLine > 50 && leftJPMissLine - rightJPMissLine < 60 )
                    TPWM45_SetDTY(3150);
            }else if(leftJPMissLine < rightJPMissLine){
                if(rightJPMissLine >110 && rightJPMissLine - leftJPMissLine > 50 && rightJPMissLine - leftJPMissLine < 60 )
                    TPWM45_SetDTY(4250);
            }else if(leftJPMissLine - rightJPMissLine <10 || rightJPMissLine - leftJPMissLine <10 ){
                leftJPMissBool = 1;
                rightJPMissBool = 1;
            }
            //sprintf(TXBuffer,"%u,%u-",leftJPMissLine,rightJPMissLine);
            //TUart0_Puts(TXBuffer);
            break;
        }
    }


    for(row = 118; row>1; row--){
        if(row<rightJPMissLine+7 || row<leftJPMissLine+7)
            break;
        if(Image_Edge[row][2] == 0 && Image_Edge[row-1][2] != 0){
            if( Image_Edge[row-1][2] != 0 ){
                Image_Edge[row][2] = ( Image_Edge[row-1][2] + Image_Edge[row+1][2] )/2;
            }else if( Image_Edge[row-2][3] != 0 ){
                Image_Edge[row][2] = ( Image_Edge[row-2][2] + Image_Edge[row+1][2] )/2;
            }else
                break;
        }else{
            Image_Edge[row][2] = ( Image_Edge[row-1][2] + Image_Edge[row+1][2] )/2;
        }
        Image_Ptr[20 * row + Image_Edge[row][2] / 8] = 1 << (7 - Image_Edge[row][2] % 8);
    }
#endif
#elif (IMG_ANALYSE_MODE==11)
/***********************************/
/*     ��λ����ʾƫ��У�����ͼ��   */
/***********************************/
    vuint8 data;
    vuint8 row,col;

    for(row = 0; row<120; row++){   //ͼ��ƫ�ƻ�ԭ
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
        }
        Image[row][col] = data;
    }
#elif (IMG_ANALYSE_MODE==12)
/***********************************/
/*     ��λ����ʾԭʼ�ɼ���ͼ��     */
/***********************************/
    uint8 row,col,col2,data;
    for(row = 0; row<120; row++){
        for(col = 0,col2 = 1;col < 20; col++,col2++){
            Image_Ptr[row * 20 + col] = Image[row][col2];
        }
    }
#elif (IMG_ANALYSE_MODE==0)

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
#endif

    //TUart0_Puts("PIT1!\r\n");
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־
    bool = 0;
}
#endif