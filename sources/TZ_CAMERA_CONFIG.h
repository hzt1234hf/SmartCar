#ifndef TZ_CAMERA_CONFIG
#define TZ_CAMERA_CONFIG

#define CAMERA_W    160
#define CAMERA_W_8  20
#define CAMERA_H    120
#define CAMERA_OFFSET   8
#define CAMERA_OFFSET_8   8/8
#define IMG_SIZE    2400

extern void TZ_OV7725_Init();

extern vuint8 Image[CAMERA_H][CAMERA_W_8];
extern vuint8 Image_Edge[CAMERA_H][3];
extern vuint8 Image_ToPC[IMG_SIZE+4];
extern vuint8 *Image_Ptr;

/*
typedef struct Bit2Char{

    vuint8 B2C    :1  ;

}Bit2Char;

typedef struct Img_Bit2Char{

#if (CAMERA_W == 80)
    Bit2Char img[80];
#elif (CAMERA_W == 160)
    Bit2Char img[160];
#elif (CAMERA_W == 240)
    Bit2Char img[240];
#elif (CAMERA_W == 320)
    Bit2Char img[320];
#endif

}Img_Bit2Char;

typedef struct{

    Img_Bit2Char data[120];

}Img_Char;
*/

#endif
