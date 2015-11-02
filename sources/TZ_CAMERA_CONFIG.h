#ifndef TZ_CAMERA_CONFIG
#define TZ_CAMERA_CONFIG

#define CAMERA_W    160
#define CAMERA_W_8  20
#define CAMERA_H    120
#define CAMERA_OFFSET   8
#define CAMERA_OFFSET_8   8/8
#define IMG_SIZE    2400

extern vuint8 Image[CAMERA_H][CAMERA_W_8];
extern vuint8 Image_ToPC[IMG_SIZE+4];
extern vuint8 *Image_Ptr;

extern void TZ_OV7725_Init();

#endif
