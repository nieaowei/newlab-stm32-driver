/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.36 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : LCD_X.c
Purpose     : Port routines for STM32F10x 16-bit Interface
----------------------------------------------------------------------
*/

//#include "GUI.h"
#include "LCD_X.h"

/*********************************************************************
*
*       Hardware configuration
*
**********************************************************************
*/

#include "stm32f10x.h"

#ifndef FSMC_Bank1_NORSRAM1     /* defined in ST Peripheral Library */
  #define FSMC_Bank1_NORSRAM1      ((uint32_t)0x00000000)
#endif

/* LCD /CS is NE1 - Bank 1 of NOR/SRAM Bank 1~4                     */
#define LCD_BASE   (0x60000000UL | 0x00000000UL)
#define LCD_REG16  (*((volatile uint16_t *)(LCD_BASE  ))) 
#define LCD_DAT16  (*((volatile uint16_t *)(LCD_BASE | 0x20000)))


/**
  * @brief  Configures LCD Control lines (FSMC Pins) in alternate function mode.
  * @param  None
  * @retval None
  */
static void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
//                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                         RCC_APB2Periph_AFIO, ENABLE);
  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  /* Set PD.11(A16 (RS)) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /* Set PD.7(NE1 (LCD/CS)) as alternate function push pull - CE1(LCD /CS) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}


/**
  * @brief  Configures the Parallel interface (FSMC) for LCD(Parallel mode)
  * @param  None
  * @retval None
  */
static void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
/*-- FSMC Configuration ------------------------------------------------------*/
/*----------------------- SRAM Bank 4 ----------------------------------------*/
  /* FSMC_Bank1_NORSRAM4 configuration */
  p.FSMC_AddressSetupTime = 1;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 2;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;
  /* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable */
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  
  /* BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/*********************************************************************
*
*       Exported code
*
*********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Init
*
* Purpose:
*   This routine should be called from your application program
*   to set port pins to their initial values
*/
void LCD_X_Init(void) {

/* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();
/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();
}

/*********************************************************************
*
*       LCD_X_Write0_16
*
* Purpose:
*   Write to controller, with A16 = 0
*/
void LCD_X_Write0_16(uint16_t c) {
  LCD_REG16 = c;
}

/*********************************************************************
*
*       LCD_X_Read0_16
*
* Purpose:
*   Read from controller, with A16 = 0
*/
uint16_t LCD_X_Read0_16(void) {
  return (LCD_REG16);
}

/*********************************************************************
*
*       LCD_X_Write1_16
*
* Purpose:
*   Write to controller, with A16 = 1
*/
void LCD_X_Write1_16(uint16_t c) {
  LCD_DAT16 = c;
}

/*********************************************************************
*
*       LCD_X_Read1_16
*
* Purpose:
*   Read from controller, with A16 = 1
*/
uint16_t LCD_X_Read1_16(void) {
  return (LCD_DAT16);
}

/*********************************************************************
*
*       LCD_X_WriteM1_16
*
* Purpose:
*   Write multiple bytes to controller, with A16 = 1
*/
void LCD_X_WriteM1_16(uint16_t * pData, int NumItems) {
  for (; NumItems; NumItems--) {
    LCD_DAT16 = *pData++;
  }
}

/*********************************************************************
*
*       LCD_X_ReadM1_16
*
* Purpose:
*   Read multiple bytes from controller, with A16 = 1
*/
void LCD_X_ReadM1_16(uint16_t * pData, int NumItems) {
  for (; NumItems; NumItems--) {
    *pData++ = LCD_DAT16;
  }
}

/*************************** End of file ****************************/
