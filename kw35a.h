/**************************************************************************/
/*                                           COPYRIGHT (c) Freescale 2015 */
/* FILE NAME: kw41.h                                                      */
/* VERSION:  1.xx                                 All Rights Reserved     */
/*                                                                        */
/* DESCRIPTION:                                                           */
/* Common definitions for the DUT - KW41                                  */
/*========================================================================*/
/* AUTHOR: Michael LaCroix - rxen80                                       */
/* COMPILER: Code Warrior   VERSION: MPC55x		                          */
/*                                                                        */
/* UPDATE HISTORY                                                         */
/* REV      AUTHOR      DATE       DESCRIPTION OF CHANGE                  */
/* ---   -----------  ---------    ---------------------                  */
/* 1.00  M.LaCroix    15Sep2015    header .h of JTAG Serial Wire Debug    */
/**************************************************************************/

#ifndef _DUT_H
#define _DUT_H

/*******************************************************************************/
/* Constants */ 
/*******************************************************************************/

#define DUT_TYPE	"KW35"								/* 4-character DUT type to display */
#define DUT_SW_REV	"1.00"								/* 4-character DUT s/w rev to display */

#define SCROLL_NAME	"Kinetis KW35A Burn-in    "			/* Scrolls after power up */

#define SOCKET_LIST      {44,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44}
#define SOCKET_COUNT      44	  /* used in some array declarations rather than MAX_DUTS */

// On KW41, a 2nd monitor line is tied to the SCAN out of each DUT so output can be monitored/capture during stress

#define SCAN_LIST        {44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88}
#define SCAN_COUNT        44	  /* used in some array declarations rather than MAX_DUTS */

#define CC3		1										/* Use with DRIVER_TYPE macro to distinguish driver types */
#define CC3E	0

// Either NVM or HTOL target must be selected, otherwise force a compile error.
// Now passed to build from within structure header file ...
//#define TARGET_NVM
//#define TARGET_HTOL

// Either NVM or HTOL target must be selected, otherwise force a compile error.
#ifndef TARGET_NVM
#ifndef TARGET_HTOL
#error "Define either TARGET_NVM or TARGET_HTOL in driver source header file"
#endif
#endif

// Only one or the other but not both are to be selected.
#ifdef TARGET_NVM
#ifdef TARGET_HTOL
#error "Define only one TARGET_NVM or TARGET_HTOL in driver source header file"
#endif
#endif


/*******************************************************************************/
/* General Purpose Signals                                                     */ 
/*******************************************************************************/

#define eFuse_Ctrl_INIT  	QSMCM.DDRQS.B.QDDSCK = 1/*  makes SCK an output for eFuse control */
#define eFuse_Ctrl_DISABLE	SCKAB = 1				/* SCKAB = 1 to disable/reset eFuse circuit */
#define eFuse_Ctrl_ENABLE	SCKAB = 0				/* SCKAB = 0 to enable over-current monitoring on eFuse  */

#define DUT_MSK_LOW		OP.OP1 = 0
#define DUT_MSK_HIGH	OP.OP1 = 1

#define SA_HSC_DISABLE	OP.OP1 = 1

#define RESET_LOW 		OP.OP5 = 0						/* RESET Pin */
#define RESET_HIGH		OP.OP5 = 1

// XTAL not implemented in solution
#define XTAL_LOW 		OUT0_31 &= 0xFFFFFFFF		/* CLR THE BITS for output low  OP.N/A */
#define XTAL_HIGH		OUT0_31 |= 0x00000000		/* SET THE BITS for output high OP.N/A */

/*******************************************************************************/
/* JTAG Signals                                                                */ 
/*******************************************************************************/

#define SWD_DIO_LOW 		OUT0_31 &= 0xFFFFF0FF		/* CLEAR THE BITS for output low OP 20/21/22/23  */
#define SWD_DIO_HIGH		OUT0_31 |= 0x00000F00		/* SET THE BITS for output high  OP 20/21/22/23  */ 

#define SWD_CLK_LOW 		OUT0_31 &= 0xFFFFFF3F		/* CLEAR THE BITS for output low OP 24/25 */
#define SWD_CLK_HIGH		OUT0_31 |= 0x000000C0		/* SET THE BITS for output high  OP 24/25 */

#define SWD_Drive_ON		OE3_ENABLE					/* drivers are tri-stated during DUT outputs */
#define SWD_Drive_OFF		OE3_DISABLE	
/*******************************************************************************/

// Standard JTAG signals not used

#define TCLK_LOW 			OUT0_31 &= 0xFFFFFFFF		/* CLR THE BITS for output  low OP  */
#define TCLK_HIGH			OUT0_31 |= 0x00000000		/* SET THE BITS for output high OP  */

#define TDI_LOW 			OUT0_31 &= 0xFFFFFFFF		/* CLR THE BITS for output  low OP  */
#define TDI_HIGH			OUT0_31 |= 0x00000000		/* SET THE BITS for output high OP   */
#define TDI_INV 			OUT0_31 ^= 0x00000000		/* EOR invert BIT, O/P 1/0->0/1 OP     same bit field as HIGH */

#define TMS_LOW 			OUT0_31 &= 0xFFFFFFFF		/* CLR THE BITS for output  low OP  */
#define TMS_HIGH			OUT0_31 |= 0x00000000		/* SET THE BITS for output high OP  */

// The JRESET pin IS NOT the resetb for the microcontroller! Do not confuse the two.
// Some instantiations of the JTAGC port include a pin that defauls to JRESET, some do not.
// The pin/signal is referenced in the function JTAGResetToIdle().
// In order to maintain portability in the absence of the pin,
// provide a declaration that does not alter the state of the OUT0_31 parallel output port.
// In doing so, no modification of the function call or the function itself is required to maintain the portability..

#define JRESET_LOW			OUT0_31 &= 0xFFFFFFFF		/* CLR THE BITS for output  low OP not used  */
#define JRESET_HIGH			OUT0_31 |= 0x00000000		/* SET THE BITS for output high OP not used  */ 

/*******************************************************************************/
/* SCAN Signals - EDT Test Kompress SCAN                                       */ 
/*******************************************************************************/

#define SCAN_CLK_LOW		SetClockOptions(DUT_CLK1,1,DLY0,INV_OFF,OE_OFF_LOW);SetClockOptions(DUT_CLK2,1,DLY0,INV_OFF,OE_OFF_LOW)				
#define SCAN_CLK_HIGH		SetClockOptions(DUT_CLK1,1,DLY0,INV_OFF,OE_OFF_HIGH);SetClockOptions(DUT_CLK2,1,DLY0,INV_OFF,OE_OFF_HIGH)

#define SCAN_SE_LOW			OUT0_31 &= 0xFFFF3FFF		/* CLR THE BITS OP.16/17 */
#define SCAN_SE_HIGH		OUT0_31 |= 0x0000C000		/* SET THE BITS OP.16/17 */

#define EDT_CLK_LOW			OUT0_31 &= 0xCFFFFFFF		/* CLR THE BITS for output low  OP 2/3  */
#define EDT_CLK_HIGH		OUT0_31 |= 0x30000000		/* SET THE BITS for output high OP 2/3  */

#define EDT_UPDATE_LOW		OUT0_31 &= 0xFFFFFFF3		/* CLR THE BIT  OP.28/29 */
#define EDT_UPDATE_HIGH		OUT0_31 |= 0x0000000C		/* SET THE BIT  OP.28/29 */

/*******************************************************************************/
/* BIST Signals                                                                */ 
/*******************************************************************************/

#define BIST_INVOKE_LOW 	OUT0_31 &= 0xFFFFFFF3		/* CLR THE BIT  OP.28/29 */
#define BIST_INVOKE_HIGH	OUT0_31 |= 0x0000000C		/* SET THE BIT  OP.28/29 */

#define BIST_RESUME_LOW 	OUT0_31 &= 0xFFFF3FFF		/* CLR THE BITS OP.16/17 */
#define BIST_RESUME_HIGH	OUT0_31 |= 0x0000C000		/* SET THE BITS OP.16/17 */

/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************/
/*******************************************************************************/
/* MODE Defines - Unique Test Register Specifications for Target Device        */ 
/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************/
/*******************************************************************************/
//31 SAFETY Resets to 0, Enable 1
//30:29 TCLKS Used for clock generation
//          00=normal
//          01=bypass, Dividers forced to 1/2
//          10=bypass_bb, Dividers forced to 1/4
//          11=MCG Bypass; Dividers forced to 1/1
//28 	TPLLS Unused
//27 	TPRDIV Unused
//26:22 unused
//21 	FORCE2PIN Enable internal force bus to pad ATX
//20 	SENSE2PIN Enable internal sense bus to pad ATX
//19 	DAC2FORCE Enable DAC to force bus control
//18 	FORCE2PMC Force bus to PMC
//17:12 DAC control world DAC
//11 	RAMBOOT Boot from RAM
//10 	CORERESET Force Core Hold
//9 	BACKBIAS Back Bias - Decoded in the TCU
//8 	EDTBYPASS EDT Bypass - Decoded in the TCU
//7 	LPEDT Low Power EDT - Decoded in the TCU
//6:0 	Test Modes Test Mode Select - Decoded in the TCU

/*******************************************************************************/

#define TestCtrlSize 	32								/* !!! if register exceeds 32-bit, special read/write required */

#define FuncTest		0x80000001						/* (32LSB) Functional Test no DAC override*/
#define BISCAN	 		0x80000047						/* (32LSB) EDT Test Kompress B/I SCAN no DAC override */
//#define BIBIST			0x80000822						/* (32LSB) No PLL, no DAC override*/
#define BIBIST			0x80000022						/* (32LSB) No PLL, no DAC override*/

#ifdef DCDC_BYPASS

int SCAN_DAC_Word 		= 20;							/* Vdd=4.0V:20 for 1.65V Vcore : Vdd=4.0V:19 for 1.58V Vcore */
int BIST_DAC_Word 		= 20;							/* Vdd=4.0V:18 for 1.50V Vcore : Vdd=4.0V:17 for 1.42V Vcore */
int FUNC_DAC_Word 		= 20;							

#else
# ifdef DCDC_BUCK

//int SCAN_DAC_Word 		= 20;						/* Vdd=1.8V:45 for 1.63V Vcore */
//int BIST_DAC_Word 		= 20;						/* Vdd=1.8V:40 for 1.45V Vcore */
//int FUNC_DAC_Word 		= 20;						/* Vdd=1.8V:39 for 1.42V Vcore */
int SCAN_DAC_Word 		= 37;							/* Vdd=1.8V:38 for 1.38V Vcore */
int BIST_DAC_Word 		= 37;							/* Vdd=1.8V:37 for 1.35V Vcore */
int FUNC_DAC_Word 		= 37;							/* Vdd=1.8V:36 for 1.31V Vcore */

# else
#  error "Select only one DCDC Converter Mode DCDC_BUCK or DCDC_BYPASS in procedure parameter .h file"
# endif
#endif

/*******************************************************************************/

// For convenient adjustment of DAC Control Word selection, allow header to insert control bits into Test Register values to select 1.65V core

//19 DAC to force bus/enable control word override – ATX/DAC
//18 Force bus to PMC - PMC
//17:12 DAC control word - DAC

#define DAC			0x00080000										/* sets bit19 in Voltage Override Control, DAC to force bus / enable control word override */ 
#define PMC			0x000C0000										/* also sets bit18 in in Voltage Override Control, force bus to PMC ... after enabling */
	
#define FuncTestDAC	(FuncTest | DAC | (UINT32)FUNC_DAC_Word<<12)	/* DAC force bus/enable CW override, Functional Test  */
#define FuncTestPMC	(FuncTest | PMC | (UINT32)FUNC_DAC_Word<<12)	/* force bus to PMC, Functional Test  */

#define BISCAN_DAC	(BISCAN | DAC | ((UINT32)SCAN_DAC_Word<<12))	/* DAC force bus/enable CW override, BURN-IN SCAN  */
#define BISCAN_PMC	(BISCAN | PMC | ((UINT32)SCAN_DAC_Word<<12))	/* force bus to PMC, BURN-IN SCAN  */

// Added DAC setup using Functional Test Mode writes prior to SCAN selection - at SCAN override voltage

#define SCANInitDAC	(FuncTest | DAC | (UINT32)SCAN_DAC_Word<<12)	/* DAC force bus/enable CW override, Functional Test  */
#define SCANInitPMC	(FuncTest | PMC | (UINT32)SCAN_DAC_Word<<12)	/* force bus to PMC, Functional Test  */


#define BIBIST_DAC	(BIBIST | DAC | ((UINT32)BIST_DAC_Word<<12))	/* DAC force bus/enable CW override, BURN-IN SCAN  */
#define BIBIST_PMC	(BIBIST | PMC | ((UINT32)BIST_DAC_Word<<12))	/* force bus to PMC, BURN-IN SCAN  */

// Added DAC setup using Functional Test Mode writes prior to BIST selection - at BIST override voltage

#define BISTInitDAC	(FuncTest | DAC | (UINT32)BIST_DAC_Word<<12)	/* DAC force bus/enable CW override, Functional Test  */
#define BISTInitPMC	(FuncTest | PMC | (UINT32)BIST_DAC_Word<<12)	/* force bus to PMC, Functional Test  */

/*******************************************************************************/
/* MISC Defines                                                                */ 
/*******************************************************************************/

#define	WordCharSize 8		/* used in transfer of s19 records into RAM or HSVG */


#define PMC_LVDSC1  0x4007D000		/* Low Voltage Detect Status And Control 1 register */
#define LVR_DISABLE 0x00000000		/* SWD write 32-bit, set AHB-AP CSW 8-bit access (use 8 LSB for data) */

#define MCG_S		0x40064006		/* MCG Status Register (MCG_S) */

// Q-Check logged data
// No need to perform RAM Load & Execute for polling if ECID accessed directly

//#define ECIDLoc1 	 0x40048054		/* Read ECID data directly out of SIM UID  H Registers */
#define ECIDLoc2 	 0x40048058		/* Read ECID data directly out of SIM UID MH Registers */
#define ECIDLoc3 	 0x4004805C		/* Read ECID data directly out of SIM UID ML Registers */
#define ECIDLoc4 	 0x40048060		/* Read ECID data directly out of SIM UID  L Registers */

// Consolidated updates for NVM & HTOL Status

#define StatusAddr1  0x00000810		/* This is where NVM cycling status is stored  */
#define StatusAddr2  0x00000820
#define StatusAddr3  0x00000830
#define StatusAddr4  0x00000840

#define SecurityLoc  0x0000040C		/* NVOPT Register contains security bits */

#define SecurityVal  0xFFFFFFFE

#define Populated	 0xFFFFFFFE		/* used to eliminate reflected data when reading unpopulated sockets */
#define Unpopulated	 0xFFFFFFFF		

#define TempSenseLoc 0x1FFFE000		/* stored in RAM */
#define BandgapLoc	 0x1FFFE004		/* stored in RAM */
#define NVMChkSumLoc 0x1FFFE008		/* checksum calculated using word-wise XOR NVM  */
#define NVMStatusLoc 0x1FFFE00C

// Set up pointers into data structure

#define FUNC_PTR	 0

// Set up pointers into HSVGData structure

#define QCHECK_PTR 	 0  			/* by coincidence driver button for q-check execution (QCHECK) is also 0 */
#define BIN2_PTR	 0				/* Q-Check and Bin2 can use the same conditions, executable */
#define SCAN_PTR	 1
#define BIST_PTR	 2

#define TimeSegment (UINT32)30		/* execute stresses in discrete segments to allow real time termination */

//#define READ	 	 1				/* switch in WriteTestRegister function to enable capture of register read */
//#define NO_READ		 0				/* switch in WriteTestRegister function to enable capture of register read */

#define CapturePortion 30

//#define READ		1				/* switch in WriteTestRegister function to enable capture of register read */
//#define NO_READ		0				/* switch in WriteTestRegister function to enable capture of register read */

#define RAMLOAD_CONTINUE 1			/* while NVM w/e cycling, set status flag */
#define RAMLOAD_COMPLETE 0			/* when NVM w/e cycling is complete, clr status flag */
int		RAMLOAD_ENABLED =1;			/* flow control to bypass redundant cycling downloads */

#define FLAG_SET         1
#define FLAG_CLR         0
int 	FirstPassFlag   =FLAG_CLR;	/* for flex cycling, status pass flag appears TWICE */

int		BREAK_FLAG = FALSE;			/* used in polling */

#define BIN3_FAIL	0x33			/* indicates a failure during the BIN3 checker */

#define BURNIN_DONE 3				/* BIN2_CHECK uses to allow 'RUN BIN2' following integrated Bin2 */

const Fext = 5000000;				/* default ext frequency at powerup */

/******************************************************************************/

// Define array to hold Q-Check polling results.
// Used to identify NRD sockets and eliminate ghosting of unpopulated sockets during interim reads.
// Permits tri-state of drivers tied to SWD_DIO when DUTs are driving out data in SWDReadReg().

	UINT32 QCHKResult[SOCKET_COUNT];/* used to filter ghosts during DUT data reads */

// Define arrays to capture ECID at start-up, HTOL/NVM data during loop execution.
// Arrays reside in external memory and cannot be accessed during HSVG execution.
// Dictates order of execution for components: BIST, SCAN, RL&E.

	UINT32 MISCData1[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */
	UINT32 MISCData2[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */
	UINT32 MISCData3[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */
	UINT32 MISCData4[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */

	UINT32 MISCData6[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */
	UINT32 MISCData7[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */
	UINT32 MISCData8[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */
	UINT32 MISCData9[SOCKET_COUNT];	/* read status words into array, pass to log using EnableDutData */

	UINT32 DatarrySize = sizeof(MISCData1); /* all same size */

/*******************************************************************************/
/*******************************************************************************/
/* FUNCTION PROTOTYPES and associated definitions                              */ 
/*******************************************************************************/
/*******************************************************************************/
void VddRamp(int *NextVoltage);
UINT16 voltage[4]={0,0,0,0};		/* shared by all functions, may need to know previous execution levels */
#define StepSize 200				/* mV, incremental voltage step in VDUT ramp */
#define StepDelay 250000			/* us, delay between steps in ramp */

// void JTAGSCANStartup(struct HSVGModule *module, UINT32 *TestRegArray);
// void JTAGSCANRandom(struct HSVGModule *module, UINT32 *SCANData);
// void JTAGSCANToggle(struct HSVGModule *module, UINT32 *SCANData);

void SCAN_HSVG_INIT(struct HSVGModule *module);				/* load up vectors for SCAN stimulus */

void TOGGLE_EDT_CLOCK(UINT32 numClocks);
void TOGGLE_SCAN_CLOCKS(UINT32 numClocks);

void SetClocksInit(void);
void SetClocksOff(void);
void SetClocksOn(void);

UINT8 PreviousButton;				/* use to tell Bin2 whether called immediately after Q-Check or other */ 
void ShutDown(void);				/* power off all resources in an orderly fashion */

short sqrt(short num);

/*******************************************************************************/
/* ECB Settings	                     								   */ 
/*******************************************************************************/
// used in  ECB Trip Idd

//#define ECB1_IPOR 500				/* mA, static current limit for board in POR ECB1*/
//#define ECB2_IPOR 800				/* mA, static current limit for board in POR ECB2*/
//#define ECB3_IPOR 500				/* mA, static current limit for board in POR ECB3*/
//#define ECB4_IPOR 500				/* mA, static current limit for board in POR ECB4*/

// Limits adjusted so Trip can can be performed at hot (really no trip at all)
#define ECB1_IPOR (UINT16)1000				/* mA, static current limit for board in POR ECB1*/
#define ECB2_IPOR (UINT16)1000				/* mA, static current limit for board in POR ECB2*/
#define ECB3_IPOR (UINT16)500				/* mA, static current limit for board in POR ECB3*/
#define ECB4_IPOR (UINT16)500				/* mA, static current limit for board in POR ECB4*/

// Normal run current limits

#define ECB1_RUN	 (UINT16)6000			/* ECB limits set for normal operation and execution */
#define ECB2_RUN	 (UINT16)2000			/* ECB limits set for normal operation and execution */
#define ECB3_RUN	 (UINT16)2000			/* ECB limits set for normal operation and execution */
#define ECB4_RUN	 (UINT16)2500			/* ECB limits set for normal operation and execution */

#define ECB_VLTG1 (UINT16)1500				/* mV, must be above maximun POR assertion, below LVD trip */
#define ECB_VLTG2 (UINT16)1500				/* mV, must be above maximun POR assertion, below LVD trip */
#define ECB_VLTG3 (UINT16)0					/* mV, must be above maximun POR assertion, below LVD trip */
#define ECB_VLTG4 (UINT16)0					/* mV, must be above maximun POR assertion, below LVD trip */

#define ECB_REARM1 (UINT16)800				/* mV, must be below minimum POR de-assertion */
#define ECB_REARM2 (UINT16)0				/* mV, must be below minimum POR de-assertion */
#define ECB_REARM3 (UINT16)0				/* mV, must be below minimum POR de-assertion */
#define ECB_REARM4 (UINT16)0				/* mV, must be below minimum POR de-assertion */

// used in voltage ramps

#define POR_VLTG1 (UINT16)2000				/* mV, must be above maximun POR assertion, below LVD trip */
#define POR_VLTG2 (UINT16)0					/* mV, must be above maximun POR assertion, below LVD trip */
#define POR_VLTG3 (UINT16)0					/* mV, must be above maximun POR assertion, below LVD trip */
#define POR_VLTG4 (UINT16)0					/* mV, must be above maximun POR assertion, below LVD trip */

#define POR_REARM1 (UINT16)800				/* mV, must be below minimum POR de-assertion */
#define POR_REARM2 (UINT16)0				/* mV, must be below minimum POR de-assertion */
#define POR_REARM3 (UINT16)0				/* mV, must be below minimum POR de-assertion */
#define POR_REARM4 (UINT16)0				/* mV, must be below minimum POR de-assertion */


/*******************************************************************************/
/*******************************************************************************/

#endif 								/* ifndef _DUT_H  */



