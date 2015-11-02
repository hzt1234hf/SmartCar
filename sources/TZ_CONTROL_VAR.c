#include "DIY_ComDef.h"
#ifdef TZ_CONTROL_VAR

uint8 debugCnt;//���Գ���λ��

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


uint8 lineRealDis;
uint8 lineRealRate;

float lineResSteerAngle;
/*�о������ʿ��Ʊ���*/
/*
int threePoint[3][3];
uint8 tpStartDist = 0;
uint8 tpEndDist = 0;
uint8 midThreePointCol1,midThreePointCol2;

float radiusOfCurva_1;

uint8 tpResSteerAngle;

/*Զ�������ʿ��Ʊ���*/
/*
int threePoint2[3][3];
uint8 tp2StartDist = 0;
uint8 tp2EndDist = 0;
uint8 midThreePoint2Col1,midThreePoint2Col2;
uint8 tp2RowCnt = 0;
uint8 tp2ColCnt = 0;

float radiusOfCurva_2;

uint8 tp2ResSteerAngle;

/*Զ�������ʿ��Ʊ���*/
/*
int threePoint3[3][3];
uint8 tp3StartDist = 0;
uint8 tp3EndDist = 0;
uint8 midThreePoint3Col1,midThreePoint3Col2;
uint8 tp3RowCnt = 0;
uint8 tp3ColCnt = 0;

float radiusOfCurva_3;

uint8 tp3ResSteerAngle;

/*�ܿ��Ʊ���*/
float allRadiusOfCurva[IMG_H];
int16 tpCol1,tpRow1;
int16 tpCol2,tpRow2;
int16 tpCol3,tpRow3;
int16 tpLen1,tpLen2,tpLen3;

uint8 centLineOffset = 5;
uint8 lineOffsetRow = 0;

/*�ܿ��Ʊ���2*/
uint8 lenOf2Screen;     //����ͼ�����ʻ�ľ���
int lenOf2Screen_Row; //�·�ͼ��ʼ�����ͼ���λ��
uint8 lenOfSpeed_Cnt;   //�ٶ�ָ���ͼ��������ȥ�·�ͼ��ָ��������ļ���
int lenOfSpeed_Row;   //�ٶ�ָ���ͼ������



/*�����ر���*/
uint16 baseSpeed = 160;       //�����ٶ�

float leftMotorBase;        //��GPT��������׼ʱ��
float rightMotorBase;       //��GPT��������׼ʱ��

uint16 leftMSCnt,rightMSCnt;    //���ҵ��GPTֵ

uint8 leftCnt,rightCnt;     //���ҵ��GPT����ֵ

uint16 leftMotorCntAve;     //GPT��ƽ��ֵ
uint16 rightMotorCntAve;    //GPT��ƽ��ֵ
uint16 leftMotorCnt[8];     //GPT��8��ֵ
uint16 rightMotorCnt[8];    //GPT��8��ֵ

float motorSpeed;          //��ǰ�ٶ�ֵ ��λ(cm/s)
uint16 leftMotorSpeed;      //��ǰ����ʵ�ٶ�
uint16 rightMotorSpeed;         //��ǰ����ʵ�ٶ�
float leftMSPwm;               //������ǰPWMֵ
float rightMSPwm;              //�ҵ����ǰPWMֵ
float motorOffsetSpeed;        //PID���ں�����ʵ�ٶȵĲ�ֵ
float motorOffsetCnt;         //PID���ں��GPT��ֵ
int motorMSPwm;             //PID���ں��ܵĵ��PWMֵ

float motorOffset;      //���ݵ��ת�������ƫ��ֵ

int motorCnt;
int motorCnt1;
int motorCnt2;

/*�����ر���*/
uint16 pwmCnt;              //��ǰ���PWMֵ
float steerOffsetCnt;       //�����Ķ��ƫ��ֵ

/*б����ر���*/
float Slope[80];
uint8 SlopeCnt;
float SlopeAve;

/*PID��ر���*/

/*�ܵĿ��Ʊ���*/
int lineOffsetValueCnt[IMG_H];          //�ۻ�ƫ��ֵ
int lineOffsetValue[IMG_H];             //ƫ��ֵ
int lineOffsetCnt = 0;                  //��ʱƫ��ֵ��¼����
uint8 PITCnt = 0;                       //PIT�жϼ���

#endif
