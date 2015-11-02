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

#define WORLD_ACRA_DATATYPE     1   //真实行距数据精确度定义
#define IMG_ROWCURTTABLE_DATATYPE  2   //图像->世界坐标转换表数据精确度定义
#define IMG_COLCURTTABLE_DATATYPE  2   //图像->世界坐标转换表数据精确度定义
/*
DATATYPE分为精确数据和模糊数据
    精确数据使用float类型，占空间大、处理速度慢，适用于精确控制
    模糊数据采用uint8类型，空闲小，处理速度快，但不精确
    1：精确数据     2：模糊数据
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
extern uint8 lastRow;
extern int row,col;				//行，列，也可用u v代替
extern int rowt,colt;
extern int range;
extern int u,v;

extern uint8 edgeFillMode;

extern int leftEdgeStart;  		//左边线检测的起始位置
extern int leftEdgeEnd;      		//左边线检测的结束位置
extern int leftEdgeOffset;

extern int rightEdgeStart;	//右边线检测的起始位置
extern int rightEdgeEnd;		//右边线检测的结束位置
extern int rightEdgeOffset;

extern int edgetempa,edgetempb;

extern int img_EdgeInfo[IMG_H][5];//图像边界信息
    //[0]：左边界 [1]：右边界 [2]：中线 [3]：获取到的边界信息 [4]：斜率\曲率
extern float world_EdgeInfo[IMG_H];//世界边界信息
    //[0]：左边界 [1]：右边界 [2]：中线 [3]：获取到的边界信息 [4]：斜率\曲率

extern int16 img_EdgeTemp; //临时边界变量，用于临时存储
extern int16 img_EdgeTempRow;  //临时边界行数变量，用于临时存储
extern double img_EdgeRowRatio;

extern int img_BuxianRow;
extern int img_BuxianEndRow;
extern uint8 img_BuxianCnt;
extern uint8 img_BuxianBool;


extern int8 leftEdgeFind,rightEdgeFind;			//左右边界是否找到标志量
extern uint8 leftEdgeMissCnt,rightEdgeMissCnt;	//左右边界miss个数计数
extern int8 leftEdgeBool,rightEdgeBool;			//左右边界

extern uint8 edgeOffset;
extern uint8 edgeBothMissCnt;
extern uint8 edgeMissBool;

extern int16 leftLastLine[2],rightLastLine[2];
extern int16 bothLastLine[2];
extern int16 lastLine[2];

extern int centerLine;
extern uint8 lastCenterLine;

extern int img_ColEdgeInfo[IMG_W][6];//图像边界信息
    //[0]：行上边界 [1]：行下边界 [2]：中线 [3]：获取到的边界信息 [4]：斜率\曲率 [5]：列位置

extern float world_ColEdgeInfo[IMG_W];//世界边界信息
    //[0]：行上边界 [1]：行下边界 [2]：中线 [3]：获取到的边界信息 [4]：斜率\曲率 [5]：列位置

extern int imgEdgeFill4[10][4];  //类型4 补线
extern uint8 imgEdgeFill4Cnt;
extern int16 imgEdgeFill4Col,imgEdgeFill4Row;

extern uint8 colEdgeEnable;
extern uint8 colBool;
extern int16 firstCol,firstRow;

extern int16 direct;
extern uint8 colEnd;
extern uint8 colEdgeCnt;

extern int16 topEdgeStart;  		//左边线检测的起始位置
extern int16 topEdgeEnd;      					//左边线检测的结束位置

extern int16 bottomEdgeStart;	//右边线检测的起始位置
extern int16 bottomEdgeEnd;		//右边线检测的结束位置

extern uint16 top_bottomOffset;    //上下界限值

extern int8 topEdgeFind,bottomEdgeFind;				//左右边界是否找到标志量
extern int8 topEdgeMissCnt,bottomEdgeMissCnt;	//左右边界miss个数计数
extern int8 topEdgeBool,bottomEdgeBool;				//左右边界
extern uint8 colCenterLine;

#if (IMG_ROWCURTTABLE_DATATYPE == 1)
extern uint8 correctTable[][160];//行对应的校正表
#elif (IMG_ROWCURTTABLE_DATATYPE == 2)
extern float correctTable[][160];//行对应的校正表
#endif
#endif