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

extern uint8 lineRealDis;
extern uint8 lineRealRate;

extern float lineResSteerAngle;

//�о������ʿ��Ʊ���

extern uint8 threePoint[3][3];
extern uint8 tpStartDist;
extern uint8 tpEndDist;
extern uint8 midThreePointCol1,midThreePointCol2;

extern uint8 radiusOfCurva_1;

extern uint8 tpResSteerAngle;

//Զ�������ʿ��Ʊ���

extern uint8 threePoint2[3][3];
extern uint8 midThreePoint2Col1,midThreePoint2Col2;
extern uint8 tp2RowCnt;
extern uint8 tp2ColCnt;

extern uint8 radiusOfCurva_2;

extern uint8 tp2ResSteerAngle;

/*�����ر���*/

extern uint8 leftMotorBase;
extern uint8 rightMotorBase;

extern uint16 leftMSCnt,rightMSCnt;           //���ҵ��GPTֵ\

extern uint8 leftMotorMul;
extern uint8 rightMotorMul;

extern uint16 leftMotorSpeed;   //���ҵ���ٶ�ֵ
extern uint16 rightMotorSpeed;  //���ҵ���ٶ�ֵ

extern uint8 leftMotorCnt;
extern uint8 rightMotorCnt;

extern uint16 allMotorCnt;

/*�����ر���*/
extern uint16 pwmCnt;

#endif