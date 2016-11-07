/**************************************************
 * ADC.h
 * Drives ADC
 *
 * Copyright 2015 University of Strathclyde
 *
 *
 **************************************************/
#include <msp430.h>
#include <driverlib.h>

extern void initialiseADC_Easy();  //Initialise ADC, setting GPIO parameters
extern void initialiseADC_Advanced();


//extern void setLedDial(unsigned char value); //Set dial value
//extern void refreshLedDial(); //Refresh the display