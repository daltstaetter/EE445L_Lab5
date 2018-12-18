// Timer0A.c
// Runs on LM3S1968
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 14, 2011

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2011
  Program 7.5, example 7.6

 Copyright 2011 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "timer0A.h"
#include "Systick.h"
#include "DAC.h"
#include "lm3s1968.h"
#include "Music.h"

#define MAXVOLUME 20
#define DUMPSIZE 175

extern const unsigned short* instrumentPtr;
extern NotePtr songPtr; // this is no longer a pointer to a pointer
extern int wait0;
extern int volume;
extern int songNumber;
extern const unsigned short SinWave1[128];

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function
void Debugging_Profile(void);

static unsigned char index = 0;
static unsigned char index2 = 0;
static unsigned char index3 = 0;
unsigned long debugArray[DUMPSIZE];
unsigned long songNote;

void Timer2A_Handler(void)
{
#ifndef CHORD
	if(wait0 == -1)
	{
		wait0 = songPtr->duration;
	}
	
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer2A timeout

	if(wait0 == 0)
	{	
		songNote++; // move to the next note if duration = 0 
		wait0 = songPtr[songNote].duration;
		Debugging_Profile();
	}
	else
	{wait0--;}

	TIMER2_TAILR_R = 62500; // interrupt every 62.5 ms
	
	if(songNote > ENDSONG)
	{ 
		songNote = ENDSONG; 
	}
#else
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;
	TIMER2_TAILR_R = 62500;
#endif
}


void Timer0B_Handler(void)
{	
	TIMER0_ICR_R = TIMER_ICR_TBTOCINT;
	TIMER0_TBILR_R = G;
#ifdef CHORD
	DAC_Out(instrumentPtr[index]*volume/MAXVOLUME+instrumentPtr[index2]*volume/MAXVOLUME+instrumentPtr[index3]*volume/MAXVOLUME);
	index3 = ((index3+1)&0x7F); // [0,127]
#endif
}

void Timer1A_Handler(void)
{	
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;
	TIMER1_TAILR_R = C; // reload value
#ifdef CHORD
	DAC_Out(instrumentPtr[index]*volume/MAXVOLUME+instrumentPtr[index2]*volume/MAXVOLUME+instrumentPtr[index3]*volume/MAXVOLUME);
	index2 = ((index2+1)&0x7F); // [0,127]
#endif
}

void Timer0A_Handler(void)
{
	GPIO_PORTG_DATA_R ^= 0x04;
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout

#ifdef CHORD
	DAC_Out(instrumentPtr[index]*volume/MAXVOLUME+instrumentPtr[index2]*volume/MAXVOLUME+instrumentPtr[index3]*volume/MAXVOLUME);
	TIMER0_TAILR_R = E;
	index = ((index+1)&0x7F);
#else
	TIMER0_TAILR_R = songPtr[songNote].frequency;
	if(songPtr[songNote].frequency != REST) 
	{
		DAC_Out(instrumentPtr[index]*volume/MAXVOLUME);
		index = ((index+1)&0x7F); // index = [0,127]
	}
	else
	{
		return;
	}	
#endif
	
/*	
//if(!GPIO_PORTF_DATA_R)
//{
//	TIMER0_TAILR_R = 1000;
//	return;
//}

//DAC_Out(SinWave1[index]);
//	//DAC_Out((SinWave1[index]));/envelope[index])/16);
//	//DAC_Out(Bassoon[index]);
//	//DAC_Out(Horn[index]*3-1063);
//	
//	//DAC_Out(Flute[index]);
//	//DAC_Out(Trumpet[index]);
//	//DAC_Out(Oboe[index]*((index%8)/8));
//		if(GPIO_PORTF_DATA_R &0x01)
//		{
//			DAC_Out(SinWave1[index]*6/10);
//		  TIMER0_TAILR_R = Ab;//200;		
//		}
//		else if(GPIO_PORTF_DATA_R &0x02)
//		{
//			DAC_Out(SinWave1[index]*6/10);
//			TIMER0_TAILR_R = Bb;
//		 }
//		else if(GPIO_PORTF_DATA_R &0x04)
//		{
//			DAC_Out(SinWave1[index]*6/10);
//			TIMER0_TAILR_R = Db;
//		}
//		else if(GPIO_PORTF_DATA_R &0x08)
//		{
//			DAC_Out(SinWave1[index]*6/10);
//			TIMER0_TAILR_R = Eb;
//			}
//		else if(GPIO_PORTF_DATA_R &0x10)
//		{// this one just dont work
//			DAC_Out(SinWave1[index]*6/10);
//			TIMER0_TAILR_R = D;
//		}
//		else if(GPIO_PORTF_DATA_R &0x20)
//		{
//			DAC_Out(SinWave1[index]*6/10);
//			TIMER0_TAILR_R = E;
//		}
//		else if(GPIO_PORTF_DATA_R &0x40)
//		{
//			DAC_Out(SinWave1[index]*6/10);
//			TIMER0_TAILR_R = F;
//		}
//		else if(GPIO_PORTF_DATA_R &0x80)
//		{
//			DAC_Out(SinWave1[index]*6/10);
//			TIMER0_TAILR_R = Gb;
//		}

	
	//index = ((index+1)&0x3F); // 0-63
	//index = ((index+1)&0x1F); // 0-31
	*/
}


// ***************** Timer0A_Init ****************
// Activate Timer0A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in usec
// Outputs: none
// timer0A is for outputting the sine Wave and creating sound
void Timer0A_Init(void(*task)(void), unsigned short period)
{ 
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0; // 0) activate timer0
  PeriodicTask = task;             // user function 
  TIMER0_CTL_R &= ~0x00000001;     // 1) disable timer0A during setup
  TIMER0_CFG_R = 0x00000004;       // 2) configure for 16-bit timer mode
  TIMER0_TAMR_R = 0x00000002;      // 3) configure for periodic mode
  TIMER0_TAILR_R = period-1;       // 4) reload value
  TIMER0_TAPR_R = 0;              // 5) 20 ns timer0A //49=>1 us timer0A
  TIMER0_ICR_R = 0x00000001;       // 6) clear timer0A timeout flag
  TIMER0_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x01FFFFFF)|0x40000000; // 8) priority 2
  NVIC_EN0_R |= NVIC_EN0_INT19;    // 9) enable interrupt 19 in NVIC
  TIMER0_CTL_R |= 0x00000001;      // 10) enable timer0A
	
//	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0; // 0) activate timer0
//  PeriodicTask = task;             // user function 
//  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // 1) disable timer0A during setup
//  TIMER0_CFG_R = TIMER_CFG_16_BIT; // 2) configure for 16-bit timer mode
//  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;  // 3) configure for periodic mode
//  TIMER0_TAILR_R = period-1;       // 4) reload value
//  TIMER0_TAPR_R = 0;               // 5) 20 ns timer0A //49=>1 us timer0A
//  TIMER0_ICR_R = TIMER_ICR_TATOCINT;       // 6) clear timer0A timeout flag
//  TIMER0_IMR_R |= TIMER_IMR_TATOIM;      // 7) arm timeout interrupt
//  NVIC_PRI4_R = (NVIC_PRI4_R&0x01FFFFFF)|0x40000000; // 8) priority 2
//  NVIC_EN0_R |= NVIC_EN0_INT19;    // 9) enable interrupt 19 in NVIC
//  TIMER0_CTL_R |= TIMER_CTL_TAEN;      // 10) enable timer0A
//	
	
	
}

void Timer0B_Init(void(*task)(void), unsigned short period)
{ 
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0; // 0) activate timer0
  PeriodicTask = task;             // user function 
  TIMER0_CTL_R &= ~TIMER_CTL_TBEN;     // 1) disable timer0B during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT;       // 2) configure for 16-bit timer mode
  TIMER0_TBMR_R = TIMER_TBMR_TBMR_PERIOD;      // 3) configure for periodic mode
  TIMER0_TBILR_R = period-1;       // 4) reload value
  TIMER0_TBPR_R = 0;               // 5) 20 ns timer0A //49=>1 us timer0A
  TIMER0_ICR_R = TIMER_ICR_TBTOCINT;       // 6) clear timer0A timeout flag
  TIMER0_IMR_R |= TIMER_IMR_TBTOIM;      // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFFFF1F)|0x00000040; // 8) priority 2
  NVIC_EN0_R |= NVIC_EN0_INT20;    // 9) enable interrupt 20 in NVIC
  TIMER0_CTL_R |= TIMER_CTL_TBEN;      // 10) enable timer0B
}

// timer1A is for the chord to play
void Timer1A_Init(void(*task)(void), unsigned short period)
{ 
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER1; // 0) activate timer0
  PeriodicTask = task;             // user function 
  TIMER1_CTL_R &= ~0x00000001;     // 1) disable timer1A during setup
  TIMER1_CFG_R = 0x00000004;       // 2) configure for 16-bit timer mode
  TIMER1_TAMR_R = 0x00000002;      // 3) configure for periodic mode
  TIMER1_TAILR_R = period-1;       // 4) reload value
  TIMER1_TAPR_R = 0;               // 5) 20 ns timer1A //49=>1 us timer0A
  TIMER1_ICR_R = 0x00000001;       // 6) clear timer1A timeout flag
  TIMER1_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF1FFF)|0x00004000; // 8) priority 2
  NVIC_EN0_R |= NVIC_EN0_INT21;    // 9) enable interrupt 19 in NVIC
  TIMER1_CTL_R |= 0x00000001;      // 10) enable timer1A
}

// timer2A is for the the length of the note to play
void Timer2A_Init(void(*task)(void), unsigned short period)
{
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER2; // 0) activate timer2
  PeriodicTask = task;             // user function 
  TIMER2_CTL_R &= ~0x00000001;     // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000004;       // 2) configure for 16-bit timer mode
  TIMER2_TAMR_R = 0x00000002;      // 3) configure for periodic mode
  TIMER2_TAILR_R = period-1;       // 4) reload value
  TIMER2_TAPR_R = 49;              // 5) 1us timer2A
  TIMER2_ICR_R = 0x00000001;       // 6) clear timer2A timeout flag
  TIMER2_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x1FFFFFFF)|0x40000000; // 8) priority 2
  NVIC_EN0_R |= NVIC_EN0_INT23;    // 9) enable interrupt 23 in NVIC
  TIMER2_CTL_R |= 0x00000001;      // 10) enable timer2A
}

void Debugging_Profile(void)
{
	static unsigned long ind = 0;
	if(ind < DUMPSIZE)
	{
		debugArray[ind] = NVIC_ST_CURRENT_R;
		ind++;
	}
  return;	
}
