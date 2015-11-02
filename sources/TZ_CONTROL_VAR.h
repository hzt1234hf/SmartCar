#ifndef TZ_CONTROL_VAR
#define TZ_CONTROL_VAR

extern uint8 controlMode;    //���Ʒ�ʽ
extern uint8 rowBreak;
extern uint8 controlError;// ����������������
extern uint8 controlFinsh;//
/*
    0:����
    1��������ֱ�߿������߽�������
    2������������1�������� �м�㶪ʧ����
    3������������1���߽������� �м�㶪ʧ����
*/

//������ֱ�߿��Ʊ���

extern uint8 centLineOffset;
extern uint8 lineOffsetCnt;
extern uint8 lineOffsetRow;
extern int16 lineOffsetValueCnt;

//�о�������1���Ʊ���

extern uint8 threePoint[3][2];
extern uint8 tpStartDist;
extern uint8 tpEndDist;
extern uint8 midThreePointCol1,midThreePointCol2;

//Զ��������1���Ʊ���

extern uint8 threePoint2[3][2];
extern uint8 midThreePoint2Col1,midThreePoint2Col2;
extern uint8 tp2RowCnt;
extern uint8 tp2ColCnt;


#endif