/**************************************************
 * LEDDriver.c
 * Drives LED display
 *
 * Copyright 2015 University of Strathclyde
 *
 *
 **************************************************/

#include "LedDriver.h"

//Current dial value
unsigned char ledValue_ = 0;
unsigned char bank_ = 0; //LED bank to display (toggles between 1 and 2)

#define GET_BIT(byte, bit) ((byte & (1<<bit))>>bit)


/**
* Sets the port bit
* port the port to set (i.e. P2OUT)
* bit the bit to set (0-7)
* the value (1 or 0)
*/
void SetLED(unsigned char volatile* port, unsigned char bit, unsigned char value)
{  
  *port = ((value & 0x01) ? *port | (1<<bit) : (*port & ~(1<<bit)));  
}

/**
* Calculate Port Value
* port current value
* bit the bit to set (0-7)
* the value (1 or 0)
*/
void CalculateNewPortValue(unsigned char volatile* port, unsigned char bit, unsigned char value)
{  
  *port = ((value & 0x01) ? *port | (1<<bit) : (*port & ~(1<<bit)));  
}



/**
Initialise LED Dial, setting GPIO parameters and timer
*/
void initialiseLedDial()
{
  //GPIO 2.7
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    
  //GPIO 5.1, 5.2, 5.3
  GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1  | GPIO_PIN2 | GPIO_PIN3);
    
  //GPIO 8.0
  GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0);
  
  //Set up a timer to control the LED display
  TA0CCTL0 |= CCIE; //Enable the interrupt
  //TA0EX0 left as default
  TA0CCR0 = LED_REFRESH_TIMER_COUNT; //Count value
  TA0CTL |= TASSEL_1 | ID_0 | MC_2 | TACLR | TAIE; //Aclk
  
}

/** 
Set dial value
*/
void setLedDial(unsigned char value)
{
  ledValue_ = value;
}

/**
* Set lower bank of LEDS (1 - 4)
* value The value of the 8 LEDS (1 = on)
*/
void setLowerBank(unsigned char value)
{
  unsigned char P2Temp = P2OUT;
  unsigned char P5Temp = P5OUT;
  unsigned char P8Temp = P8OUT;
  
  CalculateNewPortValue(&P8Temp,0,(value & 0xFF));        //LED 1
  CalculateNewPortValue(&P5Temp,1,((value >> 1) & 0xFF)); //LED 2
  CalculateNewPortValue(&P5Temp,2,((value >> 2) & 0xFF)); //LED 3
  CalculateNewPortValue(&P5Temp,3,((value >> 3) & 0xFF)); //LED 4
  CalculateNewPortValue(&P2Temp,7,0); //Set as low
  
  //Set in one go!
  P5OUT = P5Temp;
  P2OUT = P2Temp;
  P8OUT = P8Temp;
   
}

/**
* Set upper bank of LEDS (5 - 8)
* value The value of the 8 LEDS (1 = on)
*/
void setUpperBank(unsigned char value)
{
  
  unsigned char P2Temp = P2OUT;
  unsigned char P5Temp = P5OUT;
  unsigned char P8Temp = P8OUT;
  
  CalculateNewPortValue(&P8Temp,0,(~(value >> 7) & 0xFF)); //LED 5
  CalculateNewPortValue(&P5Temp,1,((~value >> 6) & 0xFF)); //LED 6
  CalculateNewPortValue(&P5Temp,2,(~(value >> 5) & 0xFF)); //LED 7
  CalculateNewPortValue(&P5Temp,3,(~(value >> 4) & 0xFF)); //LED 8
  CalculateNewPortValue(&P2Temp,7,1); //Set as high
  
  //Set in one go!
  P5OUT = P5Temp;
  P2OUT = P2Temp;
  P8OUT = P8Temp;

}

/** 
Refresh the display
*/
void refreshLedDial()
{
  if (0 == bank_)
    setLowerBank(ledValue_);
  else
    setUpperBank(ledValue_);

  bank_ = (~bank_);
}