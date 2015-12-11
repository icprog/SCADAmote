/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "lcd.c"




/*
* Defines for single scan conversion
*/
#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      4

/*
* Buffer for single conversion
*/
static adcsample_t samples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

/*
* ADC conversion group.
* Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
* Channels:    IN15 = PC5 = Morpho Nucleo CN10-pin 6.
*/
static const ADCConversionGroup adcgrpcfg1 = {
  FALSE,                                 // circular buffer mode
  ADC_GRP1_NUM_CHANNELS,                 // Number of the analog channels
  NULL,                                  // Callback function (not needed here)
  NULL,                                // Error callback
  0,                                     // CR1
  ADC_CR2_SWSTART,                       // CR2
  ADC_SMPR2_SMP_AN0(ADC_SAMPLE_3),      // sample times ch10-18
  0,                                     // sample times ch0-9
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),// SQR1: Conversion group sequence 13...16 + sequence length
  0,                                     // SQR2: Conversion group sequence 7...12
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0)       // SQR3: Conversion group sequence 1...6
};


/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palSetPad(GPIOC, GPIOC_PIN13);       /* Orange.  */
    chThdSleepMilliseconds(1000);
    palClearPad(GPIOC, GPIOC_PIN13);     /* Orange.  */
    chThdSleepMilliseconds(1000);
  }
}


/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
  osalSysEnable();

  palSetPadMode(GPIOC, 13, PAL_MODE_OUTPUT_PUSHPULL);

lcd_setio();
lcd_init();


/*
   * Initializes the ADC driver 1 and enable the thermal sensor.
   * The pin PC0 on the port GPIOC is programmed as analog input.
   */
  adcStart(&ADCD1, NULL);
  adcSTM32EnableTSVREFE();
  palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);



  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);


  adcsample_t avg_ch1;
  while (true) {
	  
/***
 * ADC Conversion
 * */
 
   adcConvert(&ADCD1, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
   
   	/* Calculates the average values from the ADC samples.*/
	avg_ch1 = (samples1[0] + samples1[1] + samples1[2] + samples1[3]) / 4;
 
 
 /***
  * 
  * 
  * 
  * */
	lcd_printf("El taller de RF");

  for(uint8_t i =0 ; i<=25; i++){
	 lcd_cmd(0x14); 
  }

	lcd_sendChar(avg_ch1);

chThdSleepMilliseconds(1000);
	lcd_cmd(0x01);
	lcd_cmd(0x02);//return cursor to home position
  }
}
