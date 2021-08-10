#include "cc3.h" 
// #define SWD_DIO_HIGH		OUT0_31 |= 0x00000000		/* SET THE BITS for output high  OP n/n  */ 
 
 
void SWD_Qcheck(void)
{
	SWD_DIO_HIGH;
	Toggle_SWD_CLK(50);
}
