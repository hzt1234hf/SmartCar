#include "DIY_ComDef.h"
#ifdef TZ_CONTROL_VAR

uint8 controlMode = 0;    //���Ʒ�ʽ
uint8 rowBreak = 0;
uint8 controlError = 0;// ����������������
uint8 controlFinsh = 0;//

/*
    0:����
    1��������ֱ�߿������߽�������
    2������������1�������� �м�㶪ʧ����
    3������������1���߽������� �м�㶪ʧ����
*/

//������ֱ�߿��Ʊ���

uint8 centLineOffset = 5;
uint8 lineOffsetCnt = 0;
uint8 lineOffsetRow = 0;
int16 lineOffsetValueCnt = 0;

//�о�������1���Ʊ���

uint8 threePoint[3][2];
uint8 tpStartDist = 0;
uint8 tpEndDist = 0;
uint8 midThreePointCol1,midThreePointCol2;
//Զ��������1���Ʊ���

uint8 threePoint2[3][2];
uint8 tp2StartDist = 0;
uint8 tp2EndDist = 0;
uint8 midThreePoint2Col1,midThreePoint2Col2;
uint8 tp2RowCnt = 0;
uint8 tp2ColCnt = 0;

#endif
