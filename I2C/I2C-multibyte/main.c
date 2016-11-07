/*-----------------------------------------------------------------------------
 * SingleTact I2C Demo
 * 
 * Copyright (c) 2016 Pressure Profile Systems
 * Licensed under the MIT license. This file may not be copied, modified, or
 * distributed except according to those terms.
 * 
 * Created by Barnali Das Aug 2016
 */
//******************************************************************************
//  The USCI_B0 RX interrupt is used to know when new data has been received.
//  ACLK = default REFO ~32768Hz, MCLK = SMCLK = BRCLK = DCODIV ~1MHz.
//
//
//                                /|\  /|\
//               SingleTact       10k  10k     MSP430FR4133
//                                 |    |        master
//             -----------------   |    |   -----------------
//            |    (Pin 6)   SDA|<-|----|->|P5.2/UCB0SDA     |
//            |                 |  |       |                 |
//            |                 |  |       |                 |
//            |    (Pin 3)   SCL|<-|------>|P5.3/UCB0SCL     |
//            |                 |          |                 |
//
//******************************************************************************
#include <msp430.h>
#include <driverlib.h>
#include "Uos_ADC.h"
#include "hal_LCD.h"
#include <math.h>


//volatile unsigned char RXData;
unsigned char RXByteCtr;   
volatile unsigned char RxBuffer[2];         
volatile unsigned char tmpRX;
volatile unsigned int sensor_val;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    
    // Configure GPIO
    P5SEL0 |= BIT2 | BIT3;                   // I2C pins

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                   // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC; // I2C mode, Master mode, sync
    UCB0CTLW1 |= UCASTP_2;                  // Automatic stop generated
                                            // after UCB0TBCNT is reached
    UCB0I2CSA = 0x0004;
    UCB0CTL1 &= ~UCSWRST;
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIE;
    
    Init_LCD();
  
    displayScrollText("PPS UK I2C");
    clearLCD();
  
    LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);
    sensor_val = 0;
    while (1)   
    {   
      RXByteCtr = 0;                          // Load RX byte counter   
      while (UCB0CTL1 & UCTXSTP);
      UCB0CTL1 |= UCTXSTT;                    // I2C start condition   
      __bis_SR_register(LPM0_bits|GIE);        // Enter LPM0 w/ interrupts   
                                               // Remain in LPM0 until all data   
                                               // is RX'd   
      sensor_val = ((RxBuffer[0] << 8) + RxBuffer[1]) & 0x000003FF;
    
      showChar(((sensor_val / 1000) % 10) + 48,pos2);
      showChar(((sensor_val / 100) % 10) + 48,pos3);
      showChar(((sensor_val / 10) % 10) + 48,pos4);
      showChar(((sensor_val / 1) % 10) + 48,pos5);
    }       
}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCIB0_ISR(void)
{
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
      UCB0CTL1 |= UCTXSTT;                  // I2C start condition
      break;
    
    case USCI_I2C_UCRXIFG0:                 // Vector 24: RXIFG0
      tmpRX = UCB0RXBUF;
      if(RXByteCtr>3){                      // Collect only last two bytes
        RxBuffer[RXByteCtr-4] = tmpRX;
        
        if(RXByteCtr == 5)
          __bic_SR_register_on_exit(LPM0_bits);  // Exit LPM0
        else
          UCB0CTL1 |= UCTXSTP;              // Generate I2C stop condition
      }
      
      RXByteCtr++;                          // Increament RX byte counter                 
      break;
   
    default: break;
  }
}
