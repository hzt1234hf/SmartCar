    #include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT
/*
*****2015.4.21 PITģ���д���
*****���ܣ�PIT�ɱ�̶�ʱ��ģ����ж�ʱ
*****��д����������Ҫ���⣺��
    1.Ԥ��Ƶ����MCF_PIT0_PMR����ֵ��Ҫע�����
��
*/

/*
���ڴ˲��ִ��뾭�����У�ǣ���������Ż����������Ż��󲿷ֵط�������˼����ò�ͨ���׶�
������Ҫ�ڴ���������ͺ��ڽ��д����Ż�
�Ż����ԣ�
1��a=a+1 a=a-1  һ���Ӽ�����ı�Ϊ ����++ �Լ�--
2��a = a% 8  ��������ı�Ϊ������    a = a & 7
3��b = a^2  ƽ�������Ϊ�˷�����    b = a * a
4��b=a* 2 b=a/ 2 �˳������Ϊ������λ���� b = a << 1 b = a >> 1
5������ѭ���� �Լ��� �� ������ ������
6��do while ���� while
7���������㶼���ò��
*/
void TPITx_Init(uint8 mode){
    switch(mode){
        default:case 0:{
            MCF_PIT0_PCSR = 0;
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT0_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(TPIT0_PRECLK);
            MCF_PIT0_PMR = TPIT0_ZQ * 1000;   //���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK55;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR55 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN;
        }break;
        case 1:{
            MCF_PIT1_PCSR = 0;
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT1_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(TPIT1_PRECLK);
            MCF_PIT1_PMR = TPIT1_ZQ * 1000;   //���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK56;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR56 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN;
        }break;
    }
}
__declspec(interrupt:0) void TPIT0_interrupt(void){
#if 1
    switch(controlMode){
        case 3:{
            pwmCnt = lineResSteerAngle*lineRealRate + 300/radiusOfCurva_1 + 200/radiusOfCurva_1 ;
            allMotorCnt = 5000;
        }break;
        case 2:{
            pwmCnt = lineResSteerAngle*5*lineRealRate + 300/radiusOfCurva_1;
            allMotorCnt = 3000;
        }break;
        case 1:{
            pwmCnt = lineResSteerAngle*11*lineRealRate;
            allMotorCnt = 1000;
        }break;
    }

    if(rightMotorSpeed <allMotorCnt)
        rightMotorCnt-=5;
    else
        rightMotorCnt+=5;
    if(leftMotorSpeed <allMotorCnt)
        leftMotorCnt-=5;
    else
        leftMotorCnt+=5;

    TPWM0_SetDTY(leftMotorCnt);
    TPWM1_SetDTY(rightMotorCnt);
    TPWM45_SetDTY(pwmCnt);
#elif 0
#endif
    MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־

}
/*
unsigned int TZ_sqrt(uint16 x){
    uint16 ans = 0,p = 0x80;
    while(p!=0)
    {
        ans+=p;
        if(ans*ans>x)
        {
            ans-=p;
        }
        p=(uint16)(p*0.5);
    }
    return(ans);
}
*/
unsigned int sqrt_16(unsigned long M)
{
    unsigned int N, i;
    unsigned long tmp, ttp;   // �����ѭ������
    if (M == 0)               // �����������������ҲΪ0
        return 0;
    N = 0;
    tmp = (M >> 30);          // ��ȡ���λ��B[m-1]
    M <<= 2;
    if (tmp > 1)              // ���λΪ1
    {
        N ++;                 // �����ǰλΪ1������ΪĬ�ϵ�0
        tmp -= N;
    }
    for (i=15; i>0; i--)      // ��ʣ���15λ
    {
        N <<= 1;              // ����һλ
        tmp <<= 2;
        tmp += (M >> 30);     // ����
        ttp = N;
        ttp = (ttp<<1)+1;
        M <<= 2;
        if (tmp >= ttp)       // �������
        {
            tmp -= ttp;
            N ++;
        }
    }
    return N;
}

void TPIT1_interrupt2(void){
#define EDGE_OFFSET 2
/*ͼ�������ȡ����һ����ʼλ��ƫ��ֵ*/
#define EDGE_INVALID 255
/*������Чֵ���壬���벻Ϊͼ���е�ȡֵ*/
#define IMG_ANALYSE_MODE    13
/*
5:uint8������ȡ����
11:��λ����ʾƫ��У�����ͼ��
12:��λ����ʾԭʼ�ɼ���ͼ��
13:��λ����ʾƫ��У������вɼ���ͼ��
14:��λ����ʾƫ��У������вɼ���ͼ��(���ַ�ʽѡ��)
0:����ģʽ�������������ֵ��������ͼ����
*/


#if (IMG_ANALYSE_MODE==11)
/***********************************/
/*     ��λ����ʾƫ��У�����ͼ��   */
/***********************************/

    for(row = 0; row<120; row++){   //ͼ��ƫ�ƻ�ԭ
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
        }
        Image[row][col] = data;
    }
#elif (IMG_ANALYSE_MODE==13)
/*********************************************/
/*     ��λ����ʾƫ��У������вɼ���ͼ��     */
/********************************************/
    vuint8 *ptr;
    for(row = 0; 0&&row<50; row++){   //ͼ��ƫ�ƻ�ԭ
        data = Image[Image_Row[row]][0];
        for(col = 0;col < 19; col++){
            Image_Ptr[Image_Row[row] * 20 + col] = Image[Image_Row[row]][col + 1];
        }
        Image_Ptr[Image_Row[row] * 20 + col] = data;
    }
    for(u = 0; 0&&u<CAMERA_H;u++){
        for(v = 0;v<CAMERA_W_8;v++){
            Image_Ptr[u * 20 + v] = 0;
        }
    }

    for(u = 0; u<IMG_H; u++){
        ptr = &ImageC[u][1];
        for(v = 1;v < IMG_W_8; v++){
            data = Image[Image_Row[u]][v];
            Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            ptr++;
        }
        data = Image[Image_Row[u]][0];
        Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
    }
#endif
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־
    bool = 0;

}
__declspec(interrupt:0)  void TPIT1_interrupt(void){

    vuint8 *ptr;

    for(u = 0; u<IMG_H; u++){
        ptr = &ImageC[u][1];
        for(v = 1;v < IMG_W_8; v++){
            data = Image[Image_Row[u]][v];
            Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            ptr++;
        }
        data = Image[Image_Row[u]][0];
        Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
    }

#if 1

    //img_EdgeInfo[CAMERA_H][5] = {0};//ͼ��߽���Ϣ
    //[0]����߽� [1]���ұ߽� [2]������ [3]����ȡ���ı߽���Ϣ [4]��б��\����

    leftEdgeBool = 1;rightEdgeBool = 1;			//���ұ߽�

    colEdgeEnable = 0;

    edgeFillMode = 0;
    leftEdgeMissCnt = 0;rightEdgeMissCnt = 0;	//���ұ߽翪ʼmiss��������

    leftEdgeStart = IMG_W_HALF;  	//����߼�����ʼλ��
    leftEdgeEnd = 0;      		    //����߼��Ľ���λ��
    rightEdgeStart = IMG_W_HALF+1; //�ұ��߼�����ʼλ��
    rightEdgeEnd = IMG_W - 1;	    //�ұ��߼��Ľ���λ��

    centerLine = IMG_W_HALF+1;//��������

    leftEdgeFind = 0;   //������ҵ���־������
    rightEdgeFind = 0;  //�ұ����ҵ���־������

    for(row = IMG_H-1;row>=0;row --){

        img_EdgeInfo[row][0] = 0;
        img_EdgeInfo[row][1] = 0;
        img_EdgeInfo[row][2] = 0;
        img_EdgeInfo[row][3] = 0;
        img_EdgeInfo[row][4] = 0;
        world_EdgeInfo[row][0] = 0;
        world_EdgeInfo[row][1] = 0;
        world_EdgeInfo[row][2] = 0;
        world_EdgeInfo[row][3] = 0;
        world_EdgeInfo[row][4] = 0;

        //Image_Ptr[Image_Row[row] * 20 + leftEdgeStart/8] |= 1<<(7-leftEdgeStart&0x07);
        //Image_Ptr[Image_Row[row] * 20 + rightEdgeStart/8] |= 1<<(7-rightEdgeStart&0x07);

        if(ImageC[row][centerLine] == 0x80){    //���м���Ϊ���ߣ����ʾ��⵽��β
            img_EdgeInfo[row][3] = 5;           //��ǰ��������Ϊ5������ѭ��
            break;
        }
        for(col = leftEdgeStart;col>leftEdgeEnd;col--){     //�����Ѱ��
            if((ImageC[row][col]&0x80) && ImageC[row][col+1] == 0 ){
                img_EdgeInfo[row][0] = col;                         //��ȡͼ��߽���Ϣ
                world_EdgeInfo[row][0] = correctTable[row][col];    //��ȡ����߽���Ϣ

                //������������ߵ�����
                leftLastLine[0] = row;
                leftLastLine[1] = col;

                leftEdgeFind = 1;   //����߱�־����λ
                break;
            }else if(ImageC[row][col+1]&0x80){//����߿�ʼʱ��ʧ����ʱδ���
                leftEdgeStart = centerLine;
            }
        }
        for(col = rightEdgeStart;col<rightEdgeEnd;col++){   //�ұ���Ѱ��
            if((ImageC[row][col]&0x80) && ImageC[row][col-1] == 0 ){
                img_EdgeInfo[row][1] = col;                         //��ȡͼ��߽���Ϣ
                world_EdgeInfo[row][1] = correctTable[row][col];    //��ȡ����߽���Ϣ

                //���������ұ��ߵ�����
                rightLastLine[0] = row;
                rightLastLine[1] = col;

                rightEdgeFind = 1;  //�ұ��߱�־����λ
                break;
            }else if(ImageC[row][col-1]&0x80){//�ұ��߿�ʼʱ��ʧ����ʱδ���
                rightEdgeStart = centerLine;
            }
        }

        if(leftEdgeFind&&rightEdgeFind){

        	img_EdgeInfo[row][2] = (img_EdgeInfo[row][0] + img_EdgeInfo[row][1])*0.5;       //��ȡͼ����������
        	world_EdgeInfo[row][2] = (world_EdgeInfo[row][0] + world_EdgeInfo[row][1])*0.5; //��ȡ������������

        	centerLine = img_EdgeInfo[row][2];  //�ϴ�������Ϊͼ����������

        	//Image_Ptr[Image_Row[row] * 20 + img_EdgeInfo[row][2]/8] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
        	//Image_Ptr[Image_Row[row] * 20 + world_EdgeInfo[row][2]/8] |= 1<<(7-world_EdgeInfo[row][2]&0x07);

            leftEdgeStart =   img_EdgeInfo[row][0] + 15;    //�ı���һ���������Ѱ��ʼλ��
            leftEdgeEnd   =   img_EdgeInfo[row][0] - 5;    //�ı���һ���������Ѱ����λ��
            rightEdgeStart =  img_EdgeInfo[row][1] - 15;    //�ı���һ���ұ�����Ѱ��ʼλ��
            rightEdgeEnd  =   img_EdgeInfo[row][1] + 5;    //�ı���һ���ұ�����Ѱ����λ��

            img_EdgeInfo[row][3] = 1;   //����������Ϊ �����Ҷ�����


        }else if(leftEdgeFind){
            //temp = rightLastLine[1];

        	//img_EdgeInfo[row][2] = (img_EdgeInfo[row][0] + temp)*0.5;
        	//world_EdgeInfo[row][2] = (world_EdgeInfo[row][0] + world_EdgeInfo[temp][1])*0.5;

            leftEdgeStart = img_EdgeInfo[row][0] + 15;    //�ı���һ���������Ѱ��ʼλ��
            leftEdgeEnd =   img_EdgeInfo[row][0] - 5;    //�ı���һ���������Ѱ����λ��

            img_EdgeInfo[row][3] = 2;   //����������Ϊ �����

            leftEdgeMissCnt++;

        }else if(rightEdgeFind){
            //temp = leftLastLine[1];

        	//img_EdgeInfo[row][2] = (img_EdgeInfo[row][1] + leftLastLine[1])*0.5;
        	//world_EdgeInfo[row][2] = (world_EdgeInfo[temp][0] + world_EdgeInfo[row][1])*0.5;

            rightEdgeStart = img_EdgeInfo[row][1] - 15;     //�ı���һ���ұ�����Ѱ��ʼλ��
            rightEdgeEnd =   img_EdgeInfo[row][1] + 5;     //�ı���һ���ұ�����Ѱ����λ��

            img_EdgeInfo[row][3] = 3;   //����������Ϊ �ҿ���

            rightEdgeMissCnt++;

        }else{
            leftEdgeStart = centerLine;     //������һ���������Ѱ��ʼλ��
            rightEdgeStart = centerLine+1;  //������һ���ұ�����Ѱ��ʼλ��

            /*
            if(leftEdgeStart-leftEdgeEnd>15)
                leftEdgeEnd++;
            if(rightEdgeStart-rightEdgeEnd>15)
                rightEdgeEnd--;
            */

            img_EdgeInfo[row][3] = 4;   //����������Ϊ ���߶�ʧ

        }

        if(row == IMG_H-6){
            if(leftEdgeMissCnt == 5&&!rightEdgeMissCnt){
                edgeFillMode = 1;
            }else if(rightEdgeMissCnt == 5&&!leftEdgeMissCnt){
                edgeFillMode = 1;
            }
        }

        /*У�����ұ��߿�ʼ����λ�ñ�������ֹ��������߽�*/
        if(leftEdgeStart>=CAMERA_W){
            leftEdgeStart = CAMERA_W - 1;
        }
        if(rightEdgeStart < 0){
            rightEdgeStart = 0;
        }
        if(leftEdgeEnd < 0){
            leftEdgeEnd = 0;
            leftEdgeStart = leftEdgeEnd + 20;
        }
        if(rightEdgeEnd >= CAMERA_W){
            rightEdgeEnd = CAMERA_W - 1;
            rightEdgeStart = rightEdgeEnd - 20;
        }

        leftEdgeFind = 0;   //���־λ����
        rightEdgeFind = 0;  //�ұ�־λ����

    }

#endif
#if 1
    imgEdgeFill4Cnt = 0;
    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;
    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;
    for(row = IMG_H-2;row>0;row--){     //���ߴ���ѭ��

        if(img_EdgeInfo[row-1][2]&&img_EdgeInfo[row+1][2]){     //�������߶�����ʱ���б����˲�

            img_EdgeInfo[row][2] = (img_EdgeInfo[row-1][2] + img_EdgeInfo[row+1][2])*0.5;   //�����˲�
            world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];               //���»�ȡ��ȡ����߽���Ϣ

            edgeFillMode = 1;
            lastCenterLine = img_EdgeInfo[row][2];
        }else if(edgeFillMode && img_EdgeInfo[row][3] == 2 || img_EdgeInfo[row][3] == 3){   //��һ���߿���ʱ�������߲���
            if(img_EdgeInfo[row][3] == 2){
                //temp = world_EdgeInfo[row][1];
                img_EdgeInfo[row][2] = img_EdgeInfo[row][0] + colCorrectTable[row]*14;   //��ȡ��ȡ����߽���Ϣ������ʵ����������Ȼ�ȡ����
                world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];       //��ȡ��ȡ����߽���Ϣ

                if(img_EdgeInfo[row][2]<IMG_W)  //��������Ƿ���ʾ��ͼ��������
                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

            }else if(img_EdgeInfo[row][3] == 3){
                //temp = world_EdgeInfo[row][2];
                img_EdgeInfo[row][2] = img_EdgeInfo[row][1] - colCorrectTable[row]*14;   //��ȡ��ȡ����߽���Ϣ������ʵ����������Ȼ�ȡ����
                world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];       //��ȡ��ȡ����߽���Ϣ

                if(img_EdgeInfo[row][2]>0)      //��������Ƿ���ʾ��ͼ��������
                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

            }
        }else if(img_EdgeInfo[row][3] == 4){    //�������߶�������ʱ���м�¼���Ա��ж��Ƿ���Ҫ����
            if(!imgEdgeFill4[imgEdgeFill4Cnt][0]){  //��¼��ʼλ��
                if(img_EdgeInfo[row+1][3]!=4){
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = row+1;
                    imgEdgeFill4[imgEdgeFill4Cnt][1] = img_EdgeInfo[row+1][2];
                }
                if(row >= IMG_H-2){       //��ʼ�߽����
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = IMG_H-2;
                    imgEdgeFill4[imgEdgeFill4Cnt][1] = IMG_W_HALF;
                }
            }else if(!imgEdgeFill4[imgEdgeFill4Cnt][2]){
                if(img_EdgeInfo[row-1][3]!=4){
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = row-1;
                    //imgEdgeFill4[imgEdgeFill4Cnt][3] = img_EdgeInfo[row-1][2];

                    imgEdgeFill4Cnt++;
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;
                }
                if(row <=1){              //�����߽����
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = 1;
                    imgEdgeFill4[imgEdgeFill4Cnt][3] = IMG_W_HALF;

                    imgEdgeFill4Cnt++;
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;
                }
            }
            edgeFillMode = 1;
        }else if(img_EdgeInfo[row][3] == 5&&img_EdgeInfo[row+2][3] != 1&&row>5){
            if(leftLastLine[0] > rightLastLine[0]){
                v = leftLastLine[1];
                u = (leftLastLine[0]+row)*0.5;
                topEdgeStart = u;
                bottomEdgeStart = u;
                topEdgeEnd = row - 5;
                bottomEdgeEnd = leftLastLine[0] + 5;
                direct = -1;
                colEnd = 0;
                colEdgeEnable = 1;

            }else if(leftLastLine[0] < rightLastLine[0]){
                v = rightLastLine[1];
                u = (rightLastLine[0]+row)*0.5;
                topEdgeStart = u;
                bottomEdgeStart = u;
                topEdgeEnd = row - 5;
                bottomEdgeEnd = rightLastLine[0] + 5;
                direct = 1;
                colEnd = IMG_W-1;
                colEdgeEnable = 1;
            }
        }
        if(img_EdgeInfo[row][2] !=0)
            Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
    }
    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

#endif

#if 1
    while(imgEdgeFill4Cnt){
        imgEdgeFill4Cnt--;
        v = imgEdgeFill4[imgEdgeFill4Cnt][1];

        imgEdgeFill4Row = imgEdgeFill4[imgEdgeFill4Cnt][0] - imgEdgeFill4[imgEdgeFill4Cnt][2];
        if(imgEdgeFill4[imgEdgeFill4Cnt][2]==1)
                imgEdgeFill4Col = imgEdgeFill4[imgEdgeFill4Cnt][1] - imgEdgeFill4[imgEdgeFill4Cnt][3];
        else
                imgEdgeFill4Col = img_EdgeInfo[imgEdgeFill4[imgEdgeFill4Cnt][2]][2] - imgEdgeFill4[imgEdgeFill4Cnt][1];

        for(row = imgEdgeFill4[imgEdgeFill4Cnt][0] ;row > imgEdgeFill4[imgEdgeFill4Cnt][2] ;row--){
        	img_EdgeInfo[row][2] = (uint16)(v + ((float)(imgEdgeFill4[imgEdgeFill4Cnt][0]  - row))/imgEdgeFill4Row * imgEdgeFill4Col);
            world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//��ȡ����߽���Ϣ
            Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

        }
    }
#endif

#if 1

    if(colEdgeEnable)
    {
        colEdgeCnt = 0;
        colCenterLine = u;
        colBool = 0;

        for(col = v;col!=colEnd;col+=direct){

            img_ColEdgeInfo[colEdgeCnt][0] = 0;
            img_ColEdgeInfo[colEdgeCnt][1] = 0;
            img_ColEdgeInfo[colEdgeCnt][2] = 0;
            img_ColEdgeInfo[colEdgeCnt][3] = 0;
            img_ColEdgeInfo[colEdgeCnt][4] = 0;
            img_ColEdgeInfo[colEdgeCnt][5] = col;
            world_ColEdgeInfo[colEdgeCnt][0] = 0;
            world_ColEdgeInfo[colEdgeCnt][1] = 0;
            world_ColEdgeInfo[colEdgeCnt][2] = 0;
            world_ColEdgeInfo[colEdgeCnt][3] = 0;
            world_ColEdgeInfo[colEdgeCnt][4] = 0;
            world_ColEdgeInfo[colEdgeCnt][5] = col;

            if(ImageC[colCenterLine][col] == 0x80){
                img_ColEdgeInfo[colEdgeCnt][3] = 5;
                break;
            }

        	//Image_Ptr[Image_Row[topEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[bottomEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
            for(row = topEdgeStart;row>topEdgeEnd;row--){
                if((ImageC[row][col]&0x80) && ImageC[row+1][col] == 0 ){
                    img_ColEdgeInfo[colEdgeCnt][0] = row;         //��ȡͼ��߽���Ϣ
                    world_ColEdgeInfo[colEdgeCnt][0] = correctTable[row][col];//��ȡ����߽���Ϣ

                    topEdgeFind = 1;
                    break;

                }
            }
            for(row = bottomEdgeStart;row<bottomEdgeEnd;row++){
                if((ImageC[row][col]&0x80) && ImageC[row-1][col] == 0 ){
                    img_ColEdgeInfo[colEdgeCnt][1] = row;         //��ȡͼ��߽���Ϣ
                    world_ColEdgeInfo[colEdgeCnt][1] = correctTable[row][col];//��ȡ����߽���Ϣ

                    bottomEdgeFind = 1;
                    break;

                }
            }
            if(topEdgeFind&&bottomEdgeFind){

                img_ColEdgeInfo[colEdgeCnt][2] = (uint8)((img_ColEdgeInfo[colEdgeCnt][0]+img_ColEdgeInfo[colEdgeCnt][1])*0.5);
                world_ColEdgeInfo[colEdgeCnt][2] = correctTable[img_ColEdgeInfo[colEdgeCnt][2]][col];//��ȡ����߽���Ϣ

        	    Image_Ptr[Image_Row[img_ColEdgeInfo[colEdgeCnt][2]] * 20 + (uint8)(img_ColEdgeInfo[colEdgeCnt][5]/8)] |= 1<<(7-img_ColEdgeInfo[colEdgeCnt][5]&0x07);
                colCenterLine = img_ColEdgeInfo[colEdgeCnt][2];

                if(colBool == 0){
                    firstRow = img_ColEdgeInfo[colEdgeCnt][2];
                    firstCol = col;
                    colBool = 1;
                }

                topEdgeStart =   img_ColEdgeInfo[colEdgeCnt][0] + 5;
                topEdgeEnd   =   img_ColEdgeInfo[colEdgeCnt][0] - 5;
                bottomEdgeStart =  img_ColEdgeInfo[colEdgeCnt][1] - 5;
                bottomEdgeEnd  =   img_ColEdgeInfo[colEdgeCnt][1] + 5;

                img_ColEdgeInfo[colEdgeCnt][3] = 1;

                colEdgeCnt++;
            }else if(topEdgeFind){
                topEdgeStart =   img_ColEdgeInfo[colEdgeCnt][0] + 5;
                topEdgeEnd   =   img_ColEdgeInfo[colEdgeCnt][0] - 5;
                bottomEdgeStart = topEdgeStart;

                img_ColEdgeInfo[colEdgeCnt][3] = 2;

                colEdgeCnt++;
            }else if(bottomEdgeFind){
                bottomEdgeStart =  img_ColEdgeInfo[colEdgeCnt][1] - 5;
                bottomEdgeEnd  =   img_ColEdgeInfo[colEdgeCnt][1] + 5;
                topEdgeStart = bottomEdgeStart;

                img_ColEdgeInfo[colEdgeCnt][3] = 3;

                colEdgeCnt++;
            }else{
                img_ColEdgeInfo[colEdgeCnt][3] = 4;
                colEdgeCnt++;
            }

            if(topEdgeStart>=40){
                topEdgeStart = 40;
            }
            if(bottomEdgeStart < 0){
                bottomEdgeStart = 0;
            }
            if(topEdgeEnd < 0){
                topEdgeEnd = 0;
                topEdgeStart = topEdgeEnd + 10;
            }
            if(bottomEdgeEnd >= 40){
                bottomEdgeEnd = 40;
                bottomEdgeStart = bottomEdgeEnd - 10;
            }

            topEdgeFind = 0;
            bottomEdgeFind = 0;

        }
            sprintf(TXBuffer,"[%u]",colEdgeEnable);
            TUart0_Puts(TXBuffer);

        if(colBool){

            if(direct == 1){
                temp = rightLastLine[0] - firstRow;
                data = firstCol - img_EdgeInfo[rightLastLine[0]][2];
                //sprintf(TXBuffer,"[%d+%d]",temp,data);
                //TUart0_Puts(TXBuffer);
                for(row = rightLastLine[0];row >= rightLastLine[0]-temp;row--){
                	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[rightLastLine[0]][2] + ((float)(rightLastLine[0] - row))/(temp+1) * data);
                    world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//��ȡ����߽���Ϣ

                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
                    //sprintf(TXBuffer,"[%d]\n",img_EdgeInfo[row][2]);
                    //TUart0_Puts(TXBuffer);
                }
            }else{
                temp = leftLastLine[0] - firstRow;
                data = img_EdgeInfo[rightLastLine[0]][2] - firstCol;
                for(row = leftLastLine[0];row>topEdgeEnd;row--){
                	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[leftLastLine[0]][2] + ((float)(leftLastLine[0] - row))/(temp+1) * data);
                    world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//��ȡ����߽���Ϣ

                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
                }
            }
        }

    }


#endif

#if 1
//���³��ֵĿ��Ʊ�����������Control�ļ���
    controlMode = 0;
    rowBreak = 0;
    controlError = 0;
    controlFinsh = 0;

    lineOffsetRow = 50;
    lineOffsetCnt = 0;
    lineOffsetValueCnt = 0;

    tpStartDist = 0;
    tpEndDist = 0;

    tp2RowCnt = 0;
    tp2ColCnt = 0;

    for(row = IMG_H-1;controlError == 0&&controlFinsh == 0&&row>0;row --){
        if(img_EdgeInfo[row][3] == 5 )
            rowBreak = 1;
        switch(controlMode){
            case 0:{
                if(rowBreak == 0 && img_EdgeInfo[row][2] != 0){
                    temp = img_EdgeInfo[row][2] - IMG_W_HALF + centLineOffset;
                    if(temp < (centLineOffset<<1)){
                        lineOffsetRow = row;
                        lineOffsetCnt ++;
                        lineOffsetValueCnt += (temp - centLineOffset);
                    }else{
                        lineOffsetRow = row;
                        lineOffsetCnt ++;

                        controlMode++;    //�ı���Ʒ�ʽ

                        threePoint[1][0] = 0;
                        threePoint[2][0] = 0;

                        threePoint[0][0] = row;//��ֵ��������
                        threePoint[0][1] = img_EdgeInfo[row][2];
                        tpStartDist = world_ActualRange[row];


                    }
                }else if(rowBreak){
                    controlError = 1;
                    break;
                }
            }break;
            case 1:{
                if(lineOffsetRow < 20)
                    break;
                if(img_EdgeInfo[row][2] != 0){
                    threePoint[2][0] = row;
                    threePoint[2][1] = img_EdgeInfo[row][2];
                    if( (world_ActualRange[row] - tpStartDist) > 80){
                        midThreePointCol1 = (threePoint[2][0] + threePoint[0][0])*0.5;
                        midThreePointCol2 = midThreePointCol1+1;
                        do{
                            if(img_EdgeInfo[midThreePointCol1][2] != 0){
                                threePoint[1][0] = midThreePointCol1;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol1][2];
                                break;
                            }else{
                                midThreePointCol1--;
                            }
                            if(img_EdgeInfo[midThreePointCol2][2] != 0){
                                threePoint[1][0] = midThreePointCol2;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol2][2];
                                break;
                            }else{
                                midThreePointCol2++;
                            }
                        }while(midThreePointCol1 != threePoint[2][0] && midThreePointCol2 != threePoint[0][0]);
                        if(midThreePointCol1 == threePoint[2][0] || midThreePointCol2 == threePoint[0][0]){
                            controlError = 2;
                            break;
                        }else if(world_ActualRange[row]<150){
                            threePoint2[1][0] = 0;
                            threePoint2[2][0] = 0;

                            threePoint2[0][0] = row;//��ֵ��������
                            threePoint2[0][1] = img_EdgeInfo[row][2];
                            controlMode ++;
                        }else{
                            controlFinsh = 1;
                        }
                    }
                }else if(rowBreak){
                    if(threePoint[2][0] != 0){
                        midThreePointCol1 = (threePoint[2][0] + threePoint[0][0])*0.5;
                        midThreePointCol2 = midThreePointCol1+1;
                        do{
                            if(img_EdgeInfo[midThreePointCol1][2] != 0){
                                threePoint[1][0] = midThreePointCol1;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol1][2];
                                break;
                            }else{
                                midThreePointCol1--;
                            }
                            if(img_EdgeInfo[midThreePointCol2][2] != 0){
                                threePoint[1][0] = midThreePointCol2;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol2][2];
                                break;
                            }else{
                                midThreePointCol2++;
                            }
                        }while(midThreePointCol1 != threePoint[2][0] && midThreePointCol2 != threePoint[0][0]);
                        if(midThreePointCol1 == threePoint[2][0] || midThreePointCol2 == threePoint[0][0]){
                            controlError = 3;
                            break;
                        }
                    }
                    break;
                }
            }break;
            case 2:{
                tp2RowCnt++;
                if(img_EdgeInfo[row][2] != 0){
                    threePoint2[2][0] = row;
                    threePoint2[2][1] = img_EdgeInfo[row][2];
                }else if(rowBreak){
                    temp = 0;
                    if(img_ColEdgeInfo[temp][2] != 0){
                        while(img_ColEdgeInfo[temp][2] !=0 && img_ColEdgeInfo[temp][2] !=5){
                            tp2ColCnt++;
                            temp++;
                        }
                        if(tp2ColCnt>tp2RowCnt){
                            threePoint2[2][0] = img_ColEdgeInfo[temp-1][2];
                            threePoint2[2][1] = img_ColEdgeInfo[temp-1][5];
                            threePoint2[1][0] = img_ColEdgeInfo[0][2];
                            threePoint2[1][1] = img_ColEdgeInfo[0][5];
                            controlFinsh = 1;
                            controlMode++;

                        }else if(tp2ColCnt<=tp2RowCnt){
                            threePoint2[1][0] = threePoint2[2][0];
                            threePoint2[1][1] = threePoint2[2][1];
                            threePoint2[2][0] = img_ColEdgeInfo[temp-1][2];
                            threePoint2[2][1] = img_ColEdgeInfo[temp-1][5];
                            controlFinsh = 1;
                            controlMode++;
                        }
                    }else{
                        midThreePoint2Col1 = (threePoint2[2][0] + threePoint2[0][0])*0.5;
                        midThreePoint2Col2 = midThreePoint2Col1+1;
                        do{
                            if(img_EdgeInfo[midThreePoint2Col1][2] != 0){
                                threePoint2[1][0] = midThreePoint2Col1;
                                threePoint2[1][1] = img_EdgeInfo[midThreePoint2Col1][2];
                                break;
                            }else{
                                midThreePoint2Col1--;
                            }
                            if(img_EdgeInfo[midThreePoint2Col2][2] != 0){
                                threePoint2[1][0] = midThreePoint2Col2;
                                threePoint2[1][1] = img_EdgeInfo[midThreePoint2Col2][2];
                                break;
                            }else{
                                midThreePoint2Col2++;
                            }
                        }while(midThreePoint2Col1 != threePoint2[2][0] && midThreePoint2Col2 != threePoint2[0][0]);
                        if(midThreePoint2Col1 == threePoint2[2][0] || midThreePoint2Col2 == threePoint2[0][0]){
                            controlError = 4;
                            break;
                        }else{
                            controlFinsh = 1;
                            controlMode++;
                        }

                    }
                }
            }break;
        }
    }
#endif
#if 1
    switch(controlMode){
        case 3:{
            tempa = threePoint2[0][0];
            tempb = threePoint2[0][1];
            temp = ((threePoint2[1][0]-tempa)*(threePoint2[2][1]-tempb) - (threePoint2[2][0]-tempa)*(threePoint2[1][1]-tempb))*0.5;
            if(temp == 0){
                controlMode = 2;
                break;
            }
            tempa = threePoint2[1][0] - threePoint2[0][0];
            tempb = threePoint2[1][1] - threePoint2[0][1];
            threePoint2[0][2] = sqrt_16(tempa*tempa+tempb*tempb);
            tempa = threePoint2[2][0] - threePoint2[1][0];
            tempb = threePoint2[2][1] - threePoint2[1][1];
            threePoint2[1][2] = sqrt_16(tempa*tempa+tempb*tempb);
            tempa = threePoint2[2][0] - threePoint2[0][0];
            tempb = threePoint2[2][1] - threePoint2[0][1];
            threePoint2[2][2] = sqrt_16(tempa*tempa+tempb*tempb);

            radiusOfCurva_2 = threePoint2[0][2] * threePoint2[1][2] * threePoint2[2][2] / temp * 0.25;
            //tp2ResSteerAngle = 90 / radiusOfCurva_2;
        }
        case 2:{
            tempa = threePoint[0][0];
            tempb = threePoint[0][1];
            temp = ((threePoint[1][0]-tempa)*(threePoint[2][1]-tempb) - (threePoint[2][0]-tempa)*(threePoint[1][1]-tempb))*0.5;
            if(temp <= 1){
                temp = ((threePoint[1][0]-IMG_W_HALF)*(threePoint[2][1]-tempb) - (threePoint[2][0]-IMG_W_HALF)*(threePoint[1][1]-tempb))*0.5;
                tempa = threePoint[1][0] - IMG_W_HALF;
                tempb = threePoint[1][1] - threePoint[0][1];
                threePoint[0][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - threePoint[1][0];
                tempb = threePoint[2][1] - threePoint[1][1];
                threePoint[1][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - IMG_W_HALF;
                tempb = threePoint[2][1] - threePoint[0][1];
                threePoint[2][2] = sqrt_16(tempa*tempa+tempb*tempb);
            }else{
                tempa = threePoint[1][0] - threePoint[0][0];
                tempb = threePoint[1][1] - threePoint[0][1];
                threePoint[0][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - threePoint[1][0];
                tempb = threePoint[2][1] - threePoint[1][1];
                threePoint[1][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - threePoint[0][0];
                tempb = threePoint[2][1] - threePoint[0][1];
                threePoint[2][2] = sqrt_16(tempa*tempa+tempb*tempb);

            }

            radiusOfCurva_1 = threePoint[0][2] * threePoint[1][2] * threePoint[2][2] / temp*0.25;
            //tpResSteerAngle = 90 / radiusOfCurva_1;
        }
        case 1:{
            if((50-lineOffsetRow-lineOffsetCnt)>5){
                lineRealDis = world_ActualRange[lineOffsetRow];
                lineRealRate = 50-lineOffsetRow;

                lineResSteerAngle = ((float)lineOffsetValueCnt/lineRealRate*colCorDisTable[lineOffsetRow])/lineRealDis*28.647;
            }else{
                lineRealDis = world_ActualRange[50-lineOffsetCnt];
                lineRealRate = lineOffsetCnt;
                lineResSteerAngle = ((float)lineOffsetValueCnt/lineRealRate*colCorDisTable[50-lineOffsetCnt])/lineRealDis*28.647;
            }
        }break;
    }
#endif
    //TUart0_Puts("PIT1!\r\n");
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־
    bool = 0;
}
#endif