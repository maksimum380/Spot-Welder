/*
 * File:   spotwelder.c
 * Author: nva1773@gmail.com
 *
 * Created on 06/02/2023
 * "Created in MPLAB X IDE v5.30"
 */

// Configuration Bit Settings
#pragma config FOSC = INTRCIO // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4 pin, I/O function on GP5 pin)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disable)
#pragma config PWRTE = ON // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF // Data Code Protection bit (Data memory code protection is disabled)

// Files include
#define _XTAL_FREQ 4000000 // for debuger
#include <xc.h> // access to SFRs
#include <pic12f675.h>
#include "header.h"
#include "typedef.h"

// dealy for start remoute pulse ---------------------------------------------------
// without compensation of the execution time of the function Interrupt() = 22 mks 
//                                   0mks, 7ms  , 10ms, 13ms, 16ms, 19ms, 21ms, 24ms
const unsigned int table_pulse[8] = {65535,58558,55558,52558,49558,46558,44558,41558};

// mode to 7-segment LED -----------------------------------------------------------
// mode OFF = 0, MAX = 7
const unsigned char table_bin[8] = {0x00,0x04,0x10,0x14,0x20,0x24,0x30,0x34};
#define MODE_OFF 0
#define MODE_MAX 7

// Init EEPROM
// __EEPROM_DATA(0x11, 0x77, 0xAA); // or
// __eeprom char table[3]={0x11,0x77,0xAA};
// example read eeprom: 
// unsigned char value;
// value = eeprom_read(0x01); // using function // or
// value = EEPROM_READ(0x01); // using macros

// Prototypes
void InitCPU(void);
void InitVAR(void);

// Variable
__bit isReadADC, isStartWelding, isDelayOn, isDelayOff;
unsigned char adc, mode, pins, delay;
unsigned int pulse;
BITS FlagKey;
#define mSetFlagKey(a) {FlagKey.byte &= a;}
#define isKeyPushUp FlagKey.bits.b0
#define isKeyPushMinus FlagKey.bits.b1
#define isKeyPushPlus FlagKey.bits.b2
#define isKeyDown FlagKey.bits.b3
#define RESET_KEYS 0xF8

// Main function
void main(void)
{
    OSCCAL = __osccal_val();
	InitCPU();
    InitVAR();
    
    mInterruptEnable();
    
    while(1)
    {
        // ADC
        if(isReadADC)
        {
            isReadADC = false;
            mStartADC();
            __delay_us(10);
            mRunADC();
            while(mBusyADC) continue;
            Nop();
            mReadADC(adc);
            mStopADC();
            mSetFlagKey(RESET_KEYS);
            if(adc > PUSH_UP) { isKeyPushUp = true; isKeyDown = false; }
            else if(adc < PUSH_MINUS) { isKeyPushMinus = true; }
            else { isKeyPushPlus = true; }
        }
        
        // Service Keys
        if(!isStartWelding)
        {
            if(isKeyPushPlus && !isKeyDown)
            {
                isKeyDown = true;
                if(mode < MODE_MAX) mode++;
            }
            if(isKeyPushMinus && !isKeyDown)
            {
                isKeyDown = true;
                if(mode > MODE_OFF) mode--;
            }
            mInterruptDisable();
            pins = table_bin[mode];
            mInterruptEnable();
            mSetPINS(pins);
        }
    }
    
    return;
}

// Initialization CPU
void InitCPU(void)
{
    mInterruptDisable();
    // Ports
    mResetPorts();
    mInitPINS();
    // Timer0
    mInitTimer0();
    mLoadTimer0();
    // Timer1
    mInitTimer1();
    // ADC
    mInitADC();
}

// Initialization Variable
void InitVAR(void)
{
    isReadADC = true; isStartWelding = false; isDelayOn = false; isDelayOff = false;
    mSetFlagKey(RESET_ALL);
    mode = 0; // off pulse
    pulse = table_pulse[mode];
    pins = table_bin[mode];
    mSetPINS(pins);
    mStartTimer0();
}

// Interrupt
void __interrupt() myISR(void)
{
	// For TMR0 -> tick = 50 ms
	if(mInterruptTMR0)
	{
	    mLoadTimer0();
        mFlagTMR0 = false;
        if(!isStartWelding)
        {
            if(mStart() && (mode != MODE_OFF)) 
            { 
                isStartWelding = true; 
                isDelayOn = true; 
                delay = 0; 
            }
            else
            {
                isReadADC = true;
            }
        }
        // DelayOn befor welding
        if(isDelayOn)
        {
            delay++;
            if(delay > 10)
            {
                isDelayOn = false;
                delay = 0;
                if(mStart())
                {
                    pulse = table_pulse[mode];
                    mRunPulse();
                    mLoadTimer1(pulse);
                    mStartTimer1();
                }
                else
                {
                    isStartWelding = false;
                }
            }
            else
            {
                if(!mStart())
                {
                    isDelayOn = false;
                    delay = 0;
                    isStartWelding = false;
                }
            }
        }
        // DelayOff after welding
        if(isDelayOff)
        {
            delay++;
            if(delay > 10)
            {
                isDelayOff = false;
                delay = 0;
                if(mStart())
                {
                    isDelayOn = true;
                }
                else
                {
                    isStartWelding = false;
                }
            }
        }
	}
	
	// For TMR1 -> clr out Pulse
	if(mInterruptTMR1)
	{
	    mStopPulse();
	    mStopTimer1();
        isDelayOff = true;
        delay = 0;
	}
}
