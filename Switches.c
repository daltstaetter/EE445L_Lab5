//Switches.c

#include "Switches.h"
#include "lm3s1968.h"
#include "timer0A.h"
#include "music.h"

#define PLAYING 0
#define PAUSE   1

extern int volume;
extern unsigned long songNote;
extern const unsigned short* instrumentPtr; 
extern Instruments* instrumentStruct;
extern const unsigned short* instrArray[6];

extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value

unsigned long* MusicPlayOrPause(unsigned long* songIndex);
void DecreaseVolume(int* volumePtr);
void IncreaseVolume(int* volumePtr);
void Rewind(unsigned long* songIndex, unsigned long* enableSong);
void Change_Instruments(const unsigned short* instrumentsPtr);
static void Delay(unsigned long count);

unsigned long* MusicPlayOrPause(unsigned long* songIndex)
{
	static unsigned long enableDisable=0;
	Delay(100000);
	if(GPIO_PORTB_DATA_R & 0x04)
	{
		if(songNote < ENDSONG)
		{
			if(enableDisable == PLAYING)
			{
				// disable Timer intrpts
				NVIC_DIS0_R = NVIC_DIS0_INT19; // Timer0A
				NVIC_DIS0_R = NVIC_DIS0_INT20; // Timer0B
				NVIC_DIS0_R = NVIC_DIS0_INT21; // Timer1A
				NVIC_DIS0_R = NVIC_DIS0_INT23; // Timer2A
				enableDisable = PAUSE;
			}
			else
			{
				// enable Timer intrpts
				NVIC_EN0_R = NVIC_EN0_INT19; // Timer0A
				NVIC_EN0_R = NVIC_EN0_INT20; // Timer0B
				NVIC_EN0_R = NVIC_EN0_INT21; // Timer1A
				NVIC_EN0_R = NVIC_EN0_INT23; // Timer2A
				enableDisable = PLAYING;
			}
		}
		else
		{
			*songIndex = 0;
			// this is just to be safe but shouldn't be necessary
			if(enableDisable == PAUSE) // re-enable all interrupts
			{
				NVIC_EN0_R = NVIC_EN0_INT19;
				NVIC_EN0_R = NVIC_EN0_INT23;
				enableDisable = PLAYING;
			}
		}
	}
	Delay(5000000);
	return &enableDisable;
}

void DecreaseVolume(int* volumePtr)
{
	Delay(100000);
	if(GPIO_PORTB_DATA_R & 0x08)
	{
		(*volumePtr)--;
		if( *volumePtr < MIN_VOLUME)
		{
			*volumePtr = MIN_VOLUME;
		}
	}
	Delay(5000000);
}

void IncreaseVolume(int* volumePtr)
{
	Delay(100000);
	if(GPIO_PORTB_DATA_R & 0x10)
	{
#ifndef CHORD
		(*volumePtr)++;
		if((*volumePtr) > MAX_VOLUME)
		{
			(*volumePtr) = MAX_VOLUME;
		}
#else // too prevent clippng on the chords
		(*volumePtr)++;
		if((*volumePtr) > MAX_VOLUME/3)
		{
			(*volumePtr) = MAX_VOLUME/3-1;
		}
		
#endif
	}
	Delay(5000000);
}



void Rewind(unsigned long* songIndex,unsigned long* enableSong)
{
	Delay(100000);
	if(GPIO_PORTB_DATA_R & 0x20)
	{
		*songIndex = 0;
		// enable Timer intrpts
		NVIC_EN0_R = NVIC_EN0_INT19;
		NVIC_EN0_R = NVIC_EN0_INT23;
	}
	*enableSong = 0; // used in MusicPlayOrPause
	Delay(5000000);
}

void Change_Instruments(const unsigned short* instrumentsPtr)
{
	static unsigned long j = 0;
	
	Delay(10000);
	if(GPIO_PORTB_DATA_R & 0x40)
	{
		j = (j+1)%6;
		instrumentPtr = instrArray[j];
	}
	Delay(5000000);
}










static void Delay(unsigned long count)
{
	while(count)
	{
		count--;
	}
}

void PortC_Init(void)
{
	volatile unsigned long dummyInstr;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC; // activate PortC
	
	dummyInstr = SYSCTL_RCGC2_R;
	dummyInstr = SYSCTL_RCGC2_R;
	
	// used for the three buttons for play,stop,rewind
	GPIO_PORTC_DIR_R &= ~0xE0; // PC5,6,7 are inputs
	GPIO_PORTC_DEN_R |= 0xE0;  // enable digital inputs
	GPIO_PORTC_AFSEL_R &= ~0xE0; // disable alt function
}

void PortF_Init(void)
{
	volatile unsigned long dummyInstr;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate PortC
	
	dummyInstr = SYSCTL_RCGC2_R;
	dummyInstr = SYSCTL_RCGC2_R;
	dummyInstr = SYSCTL_RCGC2_R;
	dummyInstr = SYSCTL_RCGC2_R;
	
	// used for the three buttons for play,stop,rewind
	GPIO_PORTF_DIR_R &= ~0xFF; // PC5,6,7 are inputs
	GPIO_PORTF_DEN_R |= 0xFF;  // enable digital inputs
	GPIO_PORTF_AFSEL_R &= ~0xFF; // disable alt function
}



// used for the three buttons for play,stop,rewind
void PortB_Init(void)
{
	volatile unsigned long dummyInstr;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate PortB
	
	dummyInstr = SYSCTL_RCGC2_R;
	dummyInstr = SYSCTL_RCGC2_R;
	dummyInstr = SYSCTL_RCGC2_R;
	
	GPIO_PORTB_DIR_R &= ~0x7C; // PB2-6 inputs
	GPIO_PORTB_DEN_R |= 0x7C;  // enable digital inputs
	GPIO_PORTB_AFSEL_R &= ~0x7C; // disable alt function
	
	GPIO_PORTB_DATA_R = 0;
	
	GPIO_PORTB_IS_R &= ~0x7C;    // makes PB4-6 level-triggered interrupts 
	GPIO_PORTB_IBE_R &= ~0x7C;   //sets it so it looks at GPIO_IEV
	GPIO_PORTB_ICR_R = 0x7C;     // clear flag4-6, do this every ISR call
	GPIO_PORTB_IEV_R |= 0x7C;    // interrupt triggers on HIGH level
	GPIO_PORTB_IM_R |= 0x7C;     // arm interrupt
	NVIC_PRI0_R = NVIC_PRI0_R&0xFFFF1FFF; // PL bits 13-15  pri = 0; 
	NVIC_EN0_R = NVIC_EN0_INT1; // enables intrpts in PB, its a friendly operation	
// NVIC_DIS0_R disables interpts for that particular port letter	
	
}

void PortG_Init(void)
{
	volatile unsigned long dummyInstr;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOG; // activate PortG
	
	dummyInstr = SYSCTL_RCGC2_R;
	dummyInstr = SYSCTL_RCGC2_R;

	GPIO_PORTG_DIR_R &= ~0x04; // PG2 Output
	GPIO_PORTG_DEN_R |= 0x04;  // enable digital IO
	GPIO_PORTG_AFSEL_R &= ~0x04; // disable alt function
}

void GPIOPortB_Handler(void)
{
	//unsigned long iBits;
	//iBits = StartCritical();
	unsigned long* songEnableDisablePtr;
	DisableInterrupts();
	
	if(GPIO_PORTB_DATA_R & 0x04) // PB2
	{ // this pauses the song
		songEnableDisablePtr = MusicPlayOrPause(&songNote);
		GPIO_PORTB_ICR_R = 0x04; // acknowledge interrupt
	}
	else if(GPIO_PORTB_DATA_R & 0x08) // PB3
	{// turn down the volume
		DecreaseVolume(&volume);
		GPIO_PORTB_ICR_R = 0x08; // acknowledge interrupt
	}
	else if(GPIO_PORTB_DATA_R & 0x10) // PB4
	{ // turn up the volume
		IncreaseVolume(&volume);
		GPIO_PORTB_ICR_R = 0x10; // acknowledge interrupt
	}
	else if(GPIO_PORTB_DATA_R & 0x20) // PB5
	{ // start song from beginning
		Rewind(&songNote,songEnableDisablePtr);
		GPIO_PORTB_ICR_R = 0x20; // acknowledge interrupt
	}
	else if(GPIO_PORTB_DATA_R & 0x40) // PB6
	{ // change instrument
		Change_Instruments(instrumentPtr);
		GPIO_PORTB_ICR_R = 0x40; // acknowledge interrupt
	}
	
	//EndCritical(iBits);
	GPIO_PORTB_ICR_R = 0x7C; // clear all interrupt ack bits again
	EnableInterrupts();
}




