#ifndef TZ_CAMERA_VAR
#define TZ_CAMERA_VAR


#define CAMERA_W    160
#define CAMERA_W_8  20
#define CAMERA_H    120
#define CAMERA_OFFSET   8
#define CAMERA_OFFSET_8   8/8
#define IMG_SIZE    2400

#define IMG_W   160
#define IMG_W_8 20
#define IMG_H   50
#define IMG_W_HALF 80

#define WORLD_ACRA_DATATYPE     1   //��ʵ�о����ݾ�ȷ�ȶ���
#define IMG_ROWCURTTABLE_DATATYPE  1   //ͼ��->��������ת�������ݾ�ȷ�ȶ���
#define IMG_COLCURTTABLE_DATATYPE  2   //ͼ��->��������ת�������ݾ�ȷ�ȶ���
/*
DATATYPE��Ϊ��ȷ���ݺ�ģ������
    ��ȷ����ʹ��float���ͣ�ռ�ռ�󡢴����ٶ����������ھ�ȷ����
    ģ�����ݲ���uint8���ͣ�����С�������ٶȿ죬������ȷ
    1����ȷ����     2��ģ������
*/


extern vuint8 Image[CAMERA_H][CAMERA_W_8];
extern uint8 Image_ToPC[IMG_SIZE+4];
extern vuint8 Image2[50][CAMERA_W_8];
extern vuint8 ImageC[IMG_H][IMG_W];
extern uint8 *Image_Ptr;

extern vuint8 Image_Row[50];

#if (WORLD_ACRA_DATATYPE == 1)
    extern vuint8 world_ActualRange[50];
#elif (WORLD_ACRA_DATATYPE == 2)
    extern float world_ActualRange[50];
#endif

#if (IMG_COLCURTTABLE_DATATYPE == 1)
    extern uint8 colCorrectTable[50];
#elif (IMG_COLCURTTABLE_DATATYPE == 2)
    extern float colCorrectTable[50];
#endif

extern uint8 colCorDisTable[50];



extern vuint32 data,temp;
extern int32 tempa,tempb;
extern int32 tempc,tempd,tempe;
extern int16 row,col;				//�У��У�Ҳ����u v����
extern int16 u,v;

extern uint8 edgeFillMode;

extern int16 leftEdgeStart;  		//����߼�����ʼλ��
extern int16 leftEdgeEnd;      		//����߼��Ľ���λ��
extern int16 rightEdgeStart;	//�ұ��߼�����ʼλ��
extern int16 rightEdgeEnd;		//�ұ��߼��Ľ���λ��

extern int16 img_EdgeInfo[IMG_H][5];//ͼ��߽���Ϣ
    //[0]����߽� [1]���ұ߽� [2]������ [3]����ȡ���ı߽���Ϣ [4]��б��\����
extern int16 world_EdgeInfo[IMG_H][5];//����߽���Ϣ
    //[0]����߽� [1]���ұ߽� [2]������ [3]����ȡ���ı߽���Ϣ [4]��б��\����

extern int8 leftEdgeFind,rightEdgeFind;			//���ұ߽��Ƿ��ҵ���־��
extern int8 leftEdgeMissCnt,rightEdgeMissCnt;	//���ұ߽�miss��������
extern int8 leftEdgeBool,rightEdgeBool;			//���ұ߽�

extern uint8 edgeOffset;
extern uint8 edgeBothMissCnt;

extern uint8 leftLastLine[2],rightLastLine[2];
extern uint8 lastLine[2];

extern uint8 centerLine;
extern uint8 lastCenterLine;

extern int16 img_ColEdgeInfo[IMG_W][6];//ͼ��߽���Ϣ
    //[0]�����ϱ߽� [1]�����±߽� [2]������ [3]����ȡ���ı߽���Ϣ [4]��б��\���� [5]����λ��

extern int16 world_ColEdgeInfo[IMG_W][6];//����߽���Ϣ
    //[0]�����ϱ߽� [1]�����±߽� [2]������ [3]����ȡ���ı߽���Ϣ [4]��б��\���� [5]����λ��

extern uint16 imgEdgeFill4[20][4];  //����4 ����
extern uint8 imgEdgeFill4Cnt;
extern int16 imgEdgeFill4Col,imgEdgeFill4Row;

extern uint8 colEdgeEnable;
extern uint8 colBool;
extern int16 firstCol,firstRow;

extern int8 direct;
extern uint8 colEnd;
extern uint8 colEdgeCnt;

extern int16 topEdgeStart;  		//����߼�����ʼλ��
extern int16 topEdgeEnd;      					//����߼��Ľ���λ��

extern int16 bottomEdgeStart;	//�ұ��߼�����ʼλ��
extern int16 bottomEdgeEnd;		//�ұ��߼��Ľ���λ��

extern int8 topEdgeFind,bottomEdgeFind;				//���ұ߽��Ƿ��ҵ���־��
extern int8 topEdgeMissCnt,bottomEdgeMissCnt;	//���ұ߽�miss��������
extern int8 topEdgeBool,bottomEdgeBool;				//���ұ߽�
extern uint8 colCenterLine;

#if (IMG_ROWCURTTABLE_DATATYPE == 1)
extern uint8 correctTable[][160];//�ж�Ӧ��У����
#elif (IMG_ROWCURTTABLE_DATATYPE == 2)
extern float correctTable[][160];//�ж�Ӧ��У����
#endif
#endif