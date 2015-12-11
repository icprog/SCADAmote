/**
* 
* @file ChibiOS_3/myprogsNucleoF401RE/ADC2/main.c
* 
* @brief buttonUser on => ADC (single) conversion on PC5 pin (send on serial terminal via USB)
* @details error at compilation with chprintf.h. It was not in ../os/hal/include
*  and no more in ../os/various. Copy it from /os/hal/lib/streams and add to Makefile:
* $(CHIBIOS)/os/hal/lib/streams/chprintf.c (line 105)
*
* In order to use the PAL driver the HAL_USE_PAL option must be enabled in halconf.h.
* You must enable too, the ADC subsystem in halconf.h
*
* In mcuconf.h, set:
* STM32_ADC_USE_ADC1       TRUE
* STM32_SERIAL_USE_USART2    TRUE
*/ 
#include "ch.h" 
#include "hal.h" 
#include "chprintf.h"

/*
* Defines for single scan conversion
*/
#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      2048*2*4

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

int main(void) { 

  halInit(); 
  chSysInit();
    
/*
  * ADC init
  */
  palSetPadMode(GPIOA, 0, PAL_MODE_INPUT_ANALOG);
  adcStart(&ADCD1, NULL); 

  /* 
   * Activates the serial driver 2 using the driver default configuration: 
   * 38400 bauds, 8bits, 1 stop-bit, no parity 
   */ 
    sdStart(&SD2, NULL);
  palSetPadMode(GPIOC, 5, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(7));
    
  /* 
   * Normal main() thread activity, in this demo it does nothing except 
   * sleeping in a loop and check the button state. 
   */ 
  while (TRUE) 
   { 

/*
* single analog conversion
* converts ADC_GRP1_BUF_DEPTH samples and averages them
* with ADC_GRP1_BUF_DEPTH==2048 this gives around 14 bit precision
* even though the ADC hardware has only 12 bits internal precision
* also at 2048 samples the 14 bit are nearly noise free, while
* each sample is very noisy.
* WARNING: If you average to many samples, the variable containing
*  the sum may overflow. That means that your readings will be wrong.
* However: With the ADC delivering a max value of 2^12, with uint32_t
*  you can average 2^20 = 1048576 samples.
*/

  uint32_t sum=0;
  unsigned int i;

  adcConvert(&ADCD1, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);

  //prints the first measured value
  chprintf((BaseSequentialStream *)&SD2, "Measured: %d  ", samples1[0]*16);
  sum=0;
  for (i=0;i<ADC_GRP1_BUF_DEPTH;i++){
      sum += samples1[i];
  }
  //prints the averaged value with two digits precision
  chprintf((BaseSequentialStream *)&SD2, "%U\r\n", sum/(ADC_GRP1_BUF_DEPTH/16));

  chThdSleepMilliseconds(500);
}
}
