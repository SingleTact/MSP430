/**************************************************
 * Analogue demo for SingleTact
 *
 * Copyright 2016 PPS
 * Barnali Das
 *
 **************************************************/
#include <msp430.h>
#include <driverlib.h>
#include "TI_USCI_I2C_slave.h"
#include "hal_LCD.h"
#include <math.h>

unsigned char SW1_interruptFlag_ = 0;
volatile unsigned int analogue_val_ = 0;
volatile char inMainLoop_ = 0;

#define MAIN_LOOP_SLEEP 10

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{
  //It is SW1
    switch(__even_in_range(P1IV,P1IV_P1IFG7))  
    {
      case P1IV_P1IFG3:
        SW1_interruptFlag_ = 1;
        GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3);
      break;
      
    default:
      GPIO_clearInterrupt(GPIO_PORT_P1, 0xFF);
      break;
    }  
}

// ADC interrupt service routine
#pragma vector=ADC_VECTOR           // these two lines are used by IAR and CCC
__interrupt void ADC_ISR(void)
{
  switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
  {
    case ADCIV_ADCIFG:              // we're ready for a reading
        analogue_val_ = (ADCMEM0 & 0x03FF)*3.2;
        ADCCTL0 |= ADCENC | ADCSC;          // Sampling and conversion start
    break;
  }
}

//On the compare of TA0CCTL0
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMERA0_ISR0(void) //Flag cleared automatically
{
   /* 
    Not used
    */
      
}

//Timer general interrupt
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMERA0_ISR1(void) 
{
  switch(__even_in_range(TA0IV,10)) //Clears the flag 
  {
    //Not used
  }
}

int main(void)
{
  //Default MCLK = 1MHz
  
  unsigned int i = 0;
  unsigned int dialGoingCW = 1;

  WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

  Init_LCD();
  initialiseADC_Easy();
  //initialiseADC_Advanced();
 
  // Configure button S1 interrupt
  GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN3, GPIO_HIGH_TO_LOW_TRANSITION);
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN3);  
  GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3);
  GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN3);  
  
  // Disable the GPIO power-on default high-impedance mode
  // to activate previously configured port settings
  PMM_unlockLPM5();
  
  __enable_interrupt();
  
  displayScrollText("PPS UK");
  clearLCD();
  
  LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);

  ADCCTL0 |= ADCENC | ADCSC;          // Sampling and conversion start
  
 
  while(1)
  {
    inMainLoop_ = 1;   
    showChar(((analogue_val_ / 1000) % 10) + 48,pos1);
    showChar(((analogue_val_ / 100) % 10) + 48,pos2);
    showChar(((analogue_val_ / 10) % 10) + 48,pos3);
    showChar(((analogue_val_ / 1) % 10) + 48,pos4);  
    showChar('M',pos5);
    showChar('V',pos6);
    
  }
}
