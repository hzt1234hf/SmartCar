#ifndef TZ_CONTROL_VAR
#define TZ_CONTROL_VAR

extern uint8 debugCnt;//���Գ���λ��

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
extern int8 centerLineSub;
extern uint8 lineOffsetRow;


extern uint8 lineRealDis;
extern uint8 lineRealRate;

extern float lineResSteerAngle;

//�о������ʿ��Ʊ���
/*
extern int threePoint[3][3];
extern uint8 tpStartDist;
extern uint8 tpEndDist;
extern uint8 midThreePointCol1,midThreePointCol2;

extern float radiusOfCurva_1;

extern uint8 tpResSteerAngle;

//Զ�������ʿ��Ʊ���

extern int threePoint2[3][3];
extern uint8 midThreePoint2Col1,midThreePoint2Col2;
extern uint8 tp2RowCnt;
extern uint8 tp2ColCnt;

extern float radiusOfCurva_2;

extern uint8 tp2ResSteerAngle;

//Զ�������ʿ��Ʊ���

extern int threePoint3[3][3];
extern uint8 midThreePoint3Col1,midThreePoint3Col2;
extern uint8 tp3RowCnt;
extern uint8 tp3ColCnt;

extern float radiusOfCurva_3;

extern uint8 tp3ResSteerAngle;
/*�ܿ��Ʊ���*/
extern float allRadiusOfCurva[IMG_H];
extern int16 tpCol1,tpRow1;
extern int16 tpCol2,tpRow2;
extern int16 tpCol3,tpRow3;
extern int16 tpLen1,tpLen2,tpLen3;

/*�ܿ��Ʊ���2*/
extern uint8 lenOf2Screen;     //����ͼ�����ʻ�ľ���
extern int lenOf2Screen_Row; //�·�ͼ��ʼ�����ͼ���λ��
extern uint8 lenOfSpeed_Cnt;   //�ٶ�ָ���ͼ��������ȥ�·�ͼ��ָ��������ļ���
extern int lenOfSpeed_Row;   //�ٶ�ָ���ͼ������

/*�����ر���*/

extern float leftMotorBase;
extern float rightMotorBase;

extern uint16 leftMSCnt,rightMSCnt;           //���ҵ��GPTֵ\

extern uint16 leftMotorSpeed;
extern uint16 rightMotorSpeed;
extern uint16 leftMotorCntAve;     //��ƽ��ֵ
extern uint16 rightMotorCntAve;    //��ƽ��ֵ
extern uint16 leftMotorCnt[8];     //��8��ֵ
extern uint16 rightMotorCnt[8];    //��8��ֵ

extern uint8 leftCnt,rightCnt;
extern float motorSpeed;
extern float leftMSPwm;
extern float rightMSPwm;
extern int motorMSPwm;
extern float motorOffsetCnt;         //PID���ں��GPT��ֵ
extern float motorOffsetSpeed;

extern int motorCnt;
extern int motorCnt1;
extern int motorCnt2;

extern float motorOffset;      //���ݵ��ת�������ƫ��ֵ

/*�����ر���*/
extern uint16 pwmCnt;
extern float steerOffsetCnt;

/*б����ر���*/
extern float Slope[80];
extern uint8 SlopeCnt;
extern float SlopeAve;
/*�ܵĿ��Ʊ���*/
extern int lineOffsetValueCnt[IMG_H];       //�ۻ�ƫ��ֵ
extern int lineOffsetValue[IMG_H];          //ƫ��ֵ
extern int lineOffsetCnt;       //��ʱƫ��ֵ��¼����
extern uint8 PITCnt;                       //PIT�жϼ���


#endif