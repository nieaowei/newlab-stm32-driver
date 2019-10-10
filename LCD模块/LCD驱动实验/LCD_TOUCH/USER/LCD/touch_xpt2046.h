#ifndef __TOUCH_XPT2046_H
#define	__TOUCH_XPT2046_H

#include "stm32f10x.h"

/* 液晶坐标结构体 */
typedef	struct POINT 
{
   uint16_t x;		
   uint16_t y;
}Coordinate;

/*  校正系数结构体 */
typedef struct Parameter 
{						
long double An,  			 //注:sizeof(long double) = 8
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
}Parameter ;

extern Coordinate ScreenSample[4];
extern Coordinate DisplaySample[4];
extern Parameter touch_para ;
extern Coordinate  display ;

int tp_get_flag(void);
void tp_set_flag(int status);
int tp_calibrate(void);
void palette_init(void);
FunctionalState get_touch_point(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para );
Coordinate *tp_read_ad_xy_filter(void);
void palette_draw_point(uint16_t x, uint16_t y);

#endif /* __TOUCH_XPT2046_H */

