/* lcd_hd44780.c
 *
 * HD44780 LCD display driver functions
 *
 * Copyright 2015 - 2017 (C) Taz8du29
 * Refer to LICENSE.MD for more infos about copyright
*/


#ifndef _LCD_HD44780_C_
#define _LCD_HD44780_C_

#include "displays/lcd_hd44780.h"


/* LCD DISPLAY FUNCTIONS */

// Wait for busy bit to clear
#define lcd_wait() while ( lcd_read(BF) & 0x80 );


void lcd_init(void) {
	// Set command pins to output, then LOW
	lcd_CMD_DIR |= (1<<lcd_RS) | (1<<lcd_RW) | (1<<lcd_EN);
	lcd_CMD_PORT |= ~(1<<lcd_RS) | ~(1<<lcd_RW) | ~(1<<lcd_EN);


	// Set data port as output
	#if ( lcd_mode == 8 )
		setPortOut(lcd_DATA_DIR);
		lcd_DATA_PORT = 0x00;
	#else
		lcd_DATA_DIR |= 0xF0;
		lcd_DATA_PORT &= 0x0F;
	#endif

	_delay_us(100);

	// Write number of lines, font (5x8 dots) and mode (8 or 4 bits)
	lcd_write(CMD, (_lcd_linesCalc | _lcd_modeCalc | 0x20) );
	lcd_wait();

	// Display on, show static cursor
	lcd_write(CMD, 0x0E);
	lcd_wait();

	// Write from Left to Right, no shift
	lcd_write(CMD, 0x06);
	lcd_wait();

	// Clear the display
	lcd_write(CMD, lcd_CMD_clearDisplay);
	lcd_wait();
}


// Small pulse on lcd_EN pin
void lcd_pulse(void) {
	_delay_us(10);
	sbi(lcd_CMD_PORT, lcd_EN);
	_delay_us(10);
	cbi(lcd_CMD_PORT, lcd_EN);
}


void lcd_write(uint8_t mode, uint8_t cmd) {
	#if ( lcd_mode == 8 )
		// Whole port (8 bits/pins) as output
		setPortOut(lcd_DATA_DIR);
	#else
		// Half port (4 upper bits/pins) as output
		lcd_DATA_DIR |= 0xF0;
	#endif

	// Write mode -> RW pin low
	cbi(lcd_CMD_PORT, lcd_RW);

	if( mode ) { sbi(lcd_CMD_PORT, lcd_RS); }
	else { cbi(lcd_CMD_PORT, lcd_RS); }

	// Output data on PORTx register
	// Then pulse EN pin, that way lcd loads data
	#if ( lcd_mode == 8 )
		lcd_DATA_PORT = cmd;
		lcd_pulse();
	#else
		// Four lower bits
		lcd_DATA_PORT &= 0x0F;
		lcd_DATA_PORT |= (cmd & 0xF0);
		lcd_pulse();

		// Four upper bits
		lcd_DATA_PORT &= 0x0F;
		lcd_DATA_PORT |= ((cmd<<4) & 0xF0);
		lcd_pulse();
	#endif
}

uint8_t lcd_read(uint8_t mode) {
	#if ( lcd_mode == 8 )
		// Whole port (8 bits/pins) as input
		setPortIn(lcd_DATA_DIR);
	#else
		// Half port (4 upper bits/pins) as output
		lcd_DATA_DIR &= 0x0F;
	#endif

	// Read mode -> RW pin high
	sbi(lcd_CMD_PORT, lcd_RW);

	if ( mode ) { sbi(lcd_CMD_PORT, lcd_RS); }
	else { cbi(lcd_CMD_PORT, lcd_RS); }

	uint8_t outbyte = 0x00;

	// Pulse EN pin, that way lcd outputs data
	// then save from PINx register
	#if ( lcd_mode == 8 )
		lcd_pulse();
		outbyte |= lcd_DATA_PIN;
	#else
		lcd_pulse();
		outbyte |= (lcd_DATA_PIN & 0xF0);
		lcd_pulse();
		outbyte |= ((lcd_DATA_PIN & 0xF0)<<4);
	#endif

	return outbyte;
}


void lcd_print(uint8_t* str, uint8_t line, uint8_t col) {
	// Addresses are sent to the display by setting the instruction register
	// bits as follow : [7] = 1 & [6:0] = address
	// Pre-computed values for different lines are set below.
	const uint8_t line_list[4] = {0x80, 0xC0, 0x94, 0xD4};

	lcd_write(CMD, (line_list[ line ] + col));
		lcd_wait();

	while ( *str ) {
		lcd_write(DATA, *str++);
		lcd_wait();
	}
}


#endif	/* !_LCD_HD44780_C_ */
