// DAC.c

#include "DAC.h"
#include "lm3s1968.h"

// from the book, Sec. 7-5 pg 371
void DAC_Init(void)
{
	volatile unsigned long delay;
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI0;  // activate SSI0
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate PortA
	
	delay = SYSCTL_RCGC2_R;  // allow time to finish activating
	delay = SYSCTL_RCGC2_R;  // allow time to finish activating
	
	GPIO_PORTA_AFSEL_R |= 0x2C; // enable alt func on PA2,3,5
	GPIO_PORTA_DEN_R |= 0x2C; // enable digital IO on PA2,3,5
	
	SSI0_CR1_R &= ~SSI_CR1_SSE; // disable SSI
	SSI0_CR1_R &= ~SSI_CR1_MS;  // master mode
	
	SSI0_CPSR_R = (SSI0_CPSR_R & ~SSI_CPSR_CPSDVSR_M) + 2; // 3MHz
	
	SSI0_CR0_R &= ~(SSI_CR0_SCR_M |   // SCR = 0;
									SSI_CR0_SPH   |   // SPH = 0;
									SSI_CR0_SPO);     // SPO = 0;
	
	SSI0_CR0_R |= SSI_CR0_SPO;        // SPO = 1;
	
	SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO; // Freescale
	SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_16; // 16-bit data
	SSI0_CR1_R |= SSI_CR1_SSE; // enable SSI
	
	return;
}

// from the book, Sec. 7-5 pg 372
// send the 16-bit data to the SSI, return a reply
unsigned short DAC_Out(unsigned short data)
{	
	while((SSI0_SR_R&SSI_SR_TNF) == 0) {}; // wait until room in FIFO
	SSI0_DR_R = (0xC000)+(data&0x0FFF);  // data out
	// ^^^ sets the speed and register select bits, look at pg 11 on tlv5618 datasheet for more info why 
	return 0;
}




