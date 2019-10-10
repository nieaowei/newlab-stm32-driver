/*-----------------------------------------------------------------------------
 * 文件说明：
 * 该文件是Board_GLCD.h的副本，区别在于本文件Board_GLCDx.h的相关参数用于竖屏操作，
 * 而Board_GLCDx.h用于横屏操作。
 *
 *
 *
 *
 *----------------------------------------------------------------------------*/

#ifndef __BOARD_GLCDx_H
#define __BOARD_GLCDx_H

#include <stdint.h>
#include <stdbool.h>

#include "Board_GLCD.h"

extern int32_t  GLCDx_Initialize          (void);
extern int32_t  GLCDx_Uninitialize        (void);
extern int32_t  GLCDx_SetForegroundColor  (uint32_t color);
extern int32_t  GLCDx_SetBackgroundColor  (uint32_t color);
extern int32_t  GLCDx_ClearScreen         (void);
extern int32_t  GLCDx_SetFont             (GLCD_FONT *font);
extern int32_t  GLCDx_DrawPixel           (uint32_t x, uint32_t y);
extern int32_t  GLCDx_DrawHLine           (uint32_t x, uint32_t y, uint32_t length);
extern int32_t  GLCDx_DrawVLine           (uint32_t x, uint32_t y, uint32_t length);
extern int32_t  GLCDx_DrawRectangle       (uint32_t x, uint32_t y, uint32_t width, uint32_t height);
extern int32_t  GLCDx_DrawFullRectangle	 (uint32_t x, uint32_t y, uint32_t width, uint32_t height);
extern int32_t  GLCDx_DrawChar            (uint32_t x, uint32_t y, int32_t  ch);
extern int32_t  GLCDx_DrawString          (uint32_t x, uint32_t y, const char *str);
extern int32_t  GLCDx_DrawBargraph        (uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t val);
extern int32_t  GLCDx_DrawBitmap          (uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap);
extern int32_t  GLCDx_VScroll             (uint32_t dy);
extern int32_t  GLCDx_FrameBufferAccess   (bool enable);
extern uint32_t GLCDx_FrameBufferAddress  (void);
extern void GLCDx_SetWindow (uint32_t x, uint32_t y, uint32_t w, uint32_t h);

#endif /* __BOARD_GLCDx_H */
