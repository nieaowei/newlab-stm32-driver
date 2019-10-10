/*-----------------------------------------------------------------------------
 * 文件说明：
 * 该文件是GLCD_Config.h的副本，区别在于本文件GLCD_ConfigX.h的相关参数用于竖屏操作，
 * 而GLCD_Config.h用于横屏操作。
 *
 *
 *
 *
 *----------------------------------------------------------------------------*/

#ifndef __GLCD_CONFIGX_H
#define __GLCD_CONFIGX_H

/*---------------------- Graphic LCD orientation configuration ---------------*/
#ifndef GLCDx_MIRROR_X
#define GLCDx_MIRROR_X   0               /* Mirror X axis = 1:yes, 0:no */
#endif
#ifndef GLCDx_MIRROR_Y
#define GLCDx_MIRROR_Y   0               /* Mirror Y axis = 1:yes, 0:no */
#endif
#ifndef GLCDx_SWAP_XY
#define GLCDx_SWAP_XY    0               /* Swap X&Y axis = 1:yes, 0:no */
#endif

/*---------------------- Graphic LCD physical definitions --------------------*/
#define GLCDx_SIZE_X     240             /* Screen size X (in pixels) */
#define GLCDx_SIZE_Y     320             /* Screen size Y (in pixels) */
#define GLCDx_BPP        16              /* Bits per pixel            */

#if    (GLCDx_SWAP_XY)
#define GLCDx_WIDTH      GLCDx_SIZE_Y     /* Screen Width  (in pixels) */
#define GLCDx_HEIGHT     GLCDx_SIZE_X     /* Screen Height (in pixels) */
#else
#define GLCDx_WIDTH      GLCDx_SIZE_X     /* Screen Width  (in pixels) */
#define GLCDx_HEIGHT     GLCDx_SIZE_Y     /* Screen Height (in pixels) */
#endif

/*---------------------- Graphic LCD color definitions -----------------------*/
/* Color coding (16-bit): 
     15..11 = R4..0 (Red)
     10..5  = G5..0 (Green)
      4..0  = B4..0 (Blue)
*/

/* GLCD RGB color definitions                            */
#define GLCDx_COLOR_BLACK        0x0000  /*   0,   0,   0 */
#define GLCDx_COLOR_NAVY         0x000F  /*   0,   0, 128 */
#define GLCDx_COLOR_DARK_GREEN   0x03E0  /*   0, 128,   0 */
#define GLCDx_COLOR_DARK_CYAN    0x03EF  /*   0, 128, 128 */
#define GLCDx_COLOR_MAROON       0x7800  /* 128,   0,   0 */
#define GLCDx_COLOR_PURPLE       0x780F  /* 128,   0, 128 */
#define GLCDx_COLOR_OLIVE        0x7BE0  /* 128, 128,   0 */
#define GLCDx_COLOR_LIGHT_GREY   0xC618  /* 192, 192, 192 */
#define GLCDx_COLOR_DARK_GREY    0x7BEF  /* 128, 128, 128 */
#define GLCDx_COLOR_BLUE         0x001F  /*   0,   0, 255 */
#define GLCDx_COLOR_GREEN        0x07E0  /*   0, 255,   0 */
#define GLCDx_COLOR_CYAN         0x07FF  /*   0, 255, 255 */
#define GLCDx_COLOR_RED          0xF800  /* 255,   0,   0 */
#define GLCDx_COLOR_MAGENTA      0xF81F  /* 255,   0, 255 */
#define GLCDx_COLOR_YELLOW       0xFFE0  /* 255, 255, 0   */
#define GLCDx_COLOR_WHITE        0xFFFF  /* 255, 255, 255 */

#endif /* __GLCD_CONFIGX_H */
