/* 
 * File:   header.h
 * Author: nva1773@gmail.com
 *
 * Created on 06/02/2023
 * "Created in MPLAB X IDE v5.30"
 */

#ifndef HEADER_H
#define	HEADER_H

/** A S M ***********************************************************/
#define Nop() {__asm("nop");}
#define ClrWdt() {__asm("clrwdt");}
#define Sleep() {__asm("sleep");}
#define Reset() {__asm("reset");}
#define Retfie() {__asm("retfie");}
#define Swapf(reg) {reg = (reg >> 4) | (reg << 4);}

/** O S C ***********************************************************/
// OSC = 4 MHz
#define OSCILLATOR	4000000
#define INSTRUCTION_CYCLE (OSCILLATOR)/4

/** P O R T S ******************************************************/
// Comp. - disable, ADC - disable, IO - inputs, IOCB - Off, WPU -Off
#define mResetPorts() {CMCON = 0x07;		\
					   ANSEL = 0x00;		\
					   GPIO = 0xFF;			\
					   TRISIO = 0xFF;		\
					   IOCB = 0x00;			\
					   WPU = 0x00;}

/** P I N S *********************************************************/
// GP0 - input ADC
// GP1 - output Pulse
// GP2 - output Bin1
// GP3 - input Start
// GP4 - output Bin2
// GP5 - output Bin2
#define mInitPINS() {TRISIO = 0xC9; GPIO = 0xC9; OPTION_REG = 0xC0;}
#define mSetPINS(a) {GPIO = a;}

/** P U L S E  *****************************************************/
#define PULSE GP1
#define mRunPulse() PULSE = 1
#define mStopPulse() PULSE = 0
#define mTogglePulse() PULSE = !PULSE

/** S T A R T  *****************************************************/
#define START GP3
#define mStart() !GP3

/** A D C ***********************************************************/
// ADC -> AN0 (Pin7), ADMF = Left, Vref = Vdd, Fosc/16
// ADC: (5V = PushUp; 2,5V = Push+; 0V = Push-; 255...0) -> ADRESH >> 2
// if (adc > 200) = PushUp (200...255)
// else if (adc < 20) = Push- (0...20)
// else = Push+
#define mInitADC() ADCON0 = 0x00; ANSEL = 0x51
#define mStartADC() ADON = 1
#define mRunADC() GO = 1
#define mBusyADC GO
#define mStopADC() ADON = 0
#define mReadADC(a) {a = ADRESH;}
#define PUSH_UP 200
#define PUSH_MINUS 20

/** T I M E R 0 *****************************************************/
// Timer0 = 20 Hz -> 50 ms (8 bits mode, Fosc/4, prescale 1:256)
#define TIMER0_PRESCALE 256
#define TIMER0_FREQUENCY 20
#define TIMER0_CORECTION 0
#define TMR0_VALUE 256-(INSTRUCTION_CYCLE/(TIMER0_FREQUENCY*TIMER0_PRESCALE))+TIMER0_CORECTION
#define mInitTimer0() OPTION_REG &= 0xC0; OPTION_REG |= 0x07
#define mStartTimer0() T0IF = 0; T0IE = 1
#define mStopTimer0() T0IF = 0; T0IE = 0
#define mLoadTimer0() TMR0 = TMR0_VALUE

/** T I M E R 1 *****************************************************/
// Fosc/4, prescale 1:1 T1CON = 0x00, (1:2 => 0x10) (1:4 => 0x20)
#define mInitTimer1() T1CON = 0x00; TMR1L = 0; TMR1H = 0
#define mStartTimer1() TMR1ON = 1; TMR1IF = 0; TMR1IE = 1
#define mStopTimer1() TMR1ON = 0; TMR1IF = 0; TMR1IE = 0
#define mLoadTimer1(a) {TMR1 = a;}

/** I S R  *********************************************************/
// Interrupt for: 
// - INT (GP2 rising pulse);
// - TMR0 - delay for angle ignition
// - TMR1 - over -> time out for measure RPM -> morot is stop
#define mInterruptEnable()  PEIE = 1; GIE = 1
#define mInterruptDisable() PEIE = 0; GIE = 0
// Timer0
#define mFlagTMR0 T0IF
#define mInterruptTMR0 T0IE&&T0IF
// Timer1
#define mFlagTMR1 TMR1IF
#define mInterruptTMR1 TMR1IE&&TMR1IF

#endif	/* HEADER_H */
