/**************************************************
 * ADC.c
 * Driver for ADC dial in middle of daughter board
 *
 * Copyright 2015 University of Strathclyde
 *
 *
 **************************************************/

#include "UoS_ADC.h"

// TimerA UpMode Configuration Parameter
Timer_A_initUpModeParam initUpParam_A1 =
{
    TIMER_A_CLOCKSOURCE_ACLK,               // ACLK Clock Source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,          // ACLK/1 = 32768Hz
    0x2000,                                 // Timer period
    TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
    TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE ,   // Disable CCR0 interrupt
    TIMER_A_DO_CLEAR                        // Clear value
};

Timer_A_initCompareModeParam initCompParam =
{
    TIMER_A_CAPTURECOMPARE_REGISTER_1,        // Compare register 1
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // Disable Compare interrupt
    TIMER_A_OUTPUTMODE_RESET_SET,             // Timer output mode 7
    0x1000                                    // Compare value
};
   
/**
Initialise Using library
Using Library functions
*/
void initialiseADC_Easy()
{
  
     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P8, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

      //Initialize the ADC Module
    /*
     * Base Address for the ADC Module
     * Use Timer trigger 1 as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC_init(ADC_BASE,
        ADC_SAMPLEHOLDSOURCE_2,
        ADC_CLOCKSOURCE_ADCOSC,
        ADC_CLOCKDIVIDER_1);
    
    ADC_enable(ADC_BASE);
    
    ADC_clearInterrupt(ADC_BASE,
            ADC_COMPLETED_INTERRUPT);


    ADC_enableInterrupt(ADC_BASE,
            ADC_COMPLETED_INTERRUPT);
    
    //Configure Memory Buffer
    /*
     * Base Address for the ADC Module
     * Use input A9 POT
     * Use positive reference of AVcc
     * Use negative reference of AVss
     */
    ADC_configureMemory(ADC_BASE,
        ADC_INPUT_A9,
        ADC_VREFPOS_AVCC,
        ADC_VREFNEG_AVSS);

    //Start a single measurement
    ADC_startConversion(ADC_BASE,
                        ADC_REPEATED_SINGLECHANNEL);

    
    // TimerA1.1 (125ms ON-period) - ADC conversion trigger signal
    Timer_A_initUpMode(TIMER_A1_BASE, &initUpParam_A1);

    //Initialize compare mode to generate PWM1
    Timer_A_initCompareMode(TIMER_A1_BASE, &initCompParam);

    // Start timer A1 in up mode
    Timer_A_startCounter(TIMER_A1_BASE,
        TIMER_A_UP_MODE
        );

    // Delay for reference settling
    __delay_cycles(300000);
}
     
/**
Initialise ADC
Directly using registers
*/
void initialiseADC_Advanced()
{

    
  SYSCFG2 |= ADCPCTL9;              // Configure ADC A9 pin

  // Configure ADC10
  ADCCTL0 |= ADCSHT_2 | ADCON;      // ADCON, S&H=16 ADC clks
  ADCCTL1 |= ADCSHP;                // ADCCLK = MODOSC; sampling timer
  ADCCTL2 |= ADCRES;                // 10-bit conversion results
  ADCMCTL0 |= ADCINCH_9;            // A9 ADC input select; Vref=AVCC
  
  ADCIFG &= ~0x01;  //Clear interrupt flag

  ADCIE |= ADCIE0;                  // Enable ADC conv complete interrupt
}