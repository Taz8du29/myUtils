/* glcd_ssd1306.h
 *
 * SSD1306 based graphical LCD driver
 * With I²C interface
 *
 * Copyright 2015 - 2017 (C) Taz8du29
 * Refer to LICENSE.MD for more infos about copyright
*/


#ifndef _GLCD_SSD1306_H_
#define _GLCD_SSD1306_H_


/* DEPENDENCIES */

#if !defined( _I2C_MASTER_H_ ) || !defined( _MYUTILS_H_ )
	#error This file needs the following dependencies : myUtils, i2c_master
#endif



/* SSD1306 CONSTANTS */

#define glcd_normalDisplay_CMD 0xA6
#define glcd_inverseDisplay_CMD 0xA7

#define glcd_displayOff 0xAE
#define glcd_displayOn  0xAF

//#define glcd_
//#define glcd_



/* PROTOTYPES */

static inline void glcd_init() __attribute__((always_inline));

static inline void glcd_sendCommand(uint8_t command) __attribute__((always_inline));



/* SSD1306 FUNCTIONS */
#include "glcd_ssd1306.c"


#endif  /* !_GLCD_SSD1306_H_ */
