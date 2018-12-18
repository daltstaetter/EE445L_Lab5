// PeriodicTimer0AInts.c
// Runs on LM3S1968
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 14, 2011

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvanto, copyright (c) 2011
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

// oscilloscope connected to PC5 for period measurement
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "timer0A.h"
#include "lm3s1968.h"
#include "Music.h"
#include "DAC.h"
#include "Systick.h"
#include "Switches.h"

extern const unsigned short* instrArray[6];
extern Note* SongAlbum[5];
extern const unsigned short* instrumentPtr;
//^^^ the current output waveform sent to DAC

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void UserTask(void);
int wait0 = -1;
//Instruments instrument;
//Instruments* instrumentStruct = &instrument;

extern NotePtr songPtr; // this is no longer a pointer to a pointer

int main(void)
{ 
	volatile unsigned long delay;
  instrumentPtr = instrArray[SINE];
	songPtr = SongAlbum[0]; // this is a pointer to a pointer
	DisableInterrupts();
	
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_8MHZ); // 50 MHz Clock

  Timer0A_Init(&UserTask,200);  // timer0A->DAC output
#ifdef CHORD
	Timer0B_Init(&UserTask,800);  // timer0B->DAC output
	Timer1A_Init(&UserTask,1200); // timer1A->DAC output
#else
	Timer2A_Init(&UserTask,600);  // timer2A->changes note
#endif
	//PortF_Init();
	PortB_Init(); // used for switch interrupts
	PortG_Init();
	SysTick_Init(); // used for timing of debugging profile
  DAC_Init();
	
	EnableInterrupts();

	while(1)
	{
/*		
//		if(GPIO_PORTF_DATA_R &0x01)
//		{
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(1019/2);//noteArray[NOTE_Gb]/2);
			// outputs at 703 should be 740
//		}
//		else if(GPIO_PORTF_DATA_R &0x02)
//		{
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(1020/2);//noteArray[NOTE_F]/2);
			// outputs at 703 should be 698
//		}
//		else if(GPIO_PORTF_DATA_R &0x04)
//		{
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(1075/2);//noteArray[NOTE_E]/2);
//			// outputs at 624 should be 659
//			// it plays an Eb, Eb = 
//		}
//		else if(GPIO_PORTF_DATA_R &0x08)
//		{
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(1260/2);//noteArray[NOTE_D]/2);
//			// outputs at 546 should be 587
//			// it plays an Eb, Eb = 
//		}
//		else if(GPIO_PORTF_DATA_R &0x10)
//		{// this one just dont work
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(1412/2);//noteArray[NOTE_C]/2);
//			// outputs at 547 should be 523
//			// it plays an Eb, Eb =
//		}
//		else if(GPIO_PORTF_DATA_R &0x20)
//		{
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(1386/2);//noteArray[NOTE_B]/2);
//			// outputs at 468 should be 659
//			// it plays an Bb, Bb = 1582
//		}
//		else if(GPIO_PORTF_DATA_R &0x40)
//		{
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(noteArray[NOTE_A]/2);
//			// outputs at 468 should be 440
//			// it plays an Bb, Bb = 1776
//		}
//		else if(GPIO_PORTF_DATA_R &0x80)
//		{
//			DAC_Out(SinWave1[i]);
//		  SysTick_Wait(1730/2);//noteArray[NOTE_G]/2);
//			// outputs at 391 should be 392
//			// it plays an Eb, Eb =
//		}
		

		
		
		
		//GPIO_PORTG_DATA_R &= ~0x04;
    //GPIO_PORTC4 ^= 0x10;
//DAC_Out(SinWave[i]);
//SysTick_Wait(noteArray[NOTE_G]);
		//SysTick_Wait(2000);
		//DAC_Out(i*10);
		
		
//		i = (i+1)%64;
//		i = (i+1)&0x7F;
		
//		
//		if(i == 63)
//		{	
//			j++;
//			if (j%60 == 0)
//			{
//				GPIO_PORTG_DATA_R ^= 0x04;
//			}
//		}
*/
  }
}

// This is never used and came default in the starter code
void UserTask(void)
{
  //GPIO_PORTC5 ^= 0x20;
	GPIO_PORTG_DATA_R ^= 0x04;
}






