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

/*������ֱ�߿��Ʊ���*/

uint8 centLineOffset = 5;
uint8 lineOffsetCnt = 0;
uint8 lineOffsetRow = 0;
int16 lineOffsetValueCnt = 0;

uint8 lineRealDis;
uint8 lineRealRate;

float lineResSteerAngle;
/*�о������ʿ��Ʊ���*/

uint8 threePoint[3][3];
uint8 tpStartDist = 0;
uint8 tpEndDist = 0;
uint8 midThreePointCol1,midThreePointCol2;

uint8 radiusOfCurva_1;

uint8 tpResSteerAngle;

/*Զ�������ʿ��Ʊ���*/

uint8 threePoint2[3][3];
uint8 tp2StartDist = 0;
uint8 tp2EndDist = 0;
uint8 midThreePoint2Col1,midThreePoint2Col2;
uint8 tp2RowCnt = 0;
uint8 tp2ColCnt = 0;

uint8 radiusOfCurva_2;

uint8 tp2ResSteerAngle;

/*�ܿ��Ʊ���*/

/*�����ر���*/
uint16 pwmCnt;
/*�����ر���*/
uint8 leftMotorBase;
uint8 rightMotorBase;

uint16 leftMSCnt,rightMSCnt;           //���ҵ��GPTֵ

uint8 leftMotorMul;
uint8 rightMotorMul;

uint16 leftMotorSpeed;
uint16 rightMotorSpeed;

uint8 leftMotorCnt;
uint8 rightMotorCnt;

uint16 allMotorCnt;
#endif
