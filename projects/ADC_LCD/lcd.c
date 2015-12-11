/************************************

LCD Driver 4Bit
By Santiago Smith

*/

#include "ch.h"
#include "hal.h"

#define EN 0
#define RS 5

#define DB4 1
#define DB5 2
#define DB6 3
#define DB7 4


/*
 * LCD set I/Os
 */
void lcd_setio(){

  palSetPadMode(GPIOE, EN, PAL_MODE_OUTPUT_PUSHPULL); //EN
  palSetPadMode(GPIOB, RS, PAL_MODE_OUTPUT_PUSHPULL); //RS

  palSetPadMode(GPIOE, DB4, PAL_MODE_OUTPUT_PUSHPULL); //DB4
  palSetPadMode(GPIOE, DB5, PAL_MODE_OUTPUT_PUSHPULL); //DB5
  palSetPadMode(GPIOE, DB6, PAL_MODE_OUTPUT_PUSHPULL); //DB6
  palSetPadMode(GPIOE, DB7, PAL_MODE_OUTPUT_PUSHPULL); //DB7

  palWritePort(GPIOE, 0);

}



void lcd_send_nibble(uint8_t  data){
	palWritePort(GPIOE, (data<<1));
}


//Pulse Enable Pin
void lcd_pulsen(){

	//Send Pulse Enable
	palSetPad(GPIOE, EN);
	chThdSleepMicroseconds(40);
	palClearPad(GPIOE, EN);
	chThdSleepMicroseconds(1);
	
}
void lcd_sendByte(uint8_t c){

	uint8_t x;
	x = c >> 4;
	lcd_send_nibble(x);
//	chThdSleepMicroseconds(50);
	lcd_pulsen();
	
	x = c & 0x0f;
	lcd_send_nibble(x);
//	chThdSleepMicroseconds(50);
	lcd_pulsen();

}

void lcd_sendChar(uint8_t c){

	palSetPad(GPIOB, RS);
	lcd_sendByte(c);
	chThdSleepMicroseconds(50);
	palClearPad(GPIOB, RS);
}

void lcd_cmd(uint8_t c){

	palClearPad(GPIOB, RS);
	lcd_sendByte(c);
	chThdSleepMicroseconds(50);
}
/*
 * LCD init
 */
void lcd_init(){
	chThdSleepMilliseconds(100);
	lcd_cmd(0x33);
	chThdSleepMilliseconds(5);
	lcd_cmd(0x32);
	chThdSleepMilliseconds(5);
	lcd_cmd(0x28);
	lcd_cmd(0x08);
	lcd_cmd(0x01);
	chThdSleepMilliseconds(3);

	lcd_cmd(0x06);

	lcd_cmd(0x0F);
	chThdSleepMilliseconds(5);
}


void lcd_printf(const char *text)
{
 while (*text) // string ends with 0x00
 {
 lcd_sendChar(*text++); // auto-increment the array pointer
 }
}
