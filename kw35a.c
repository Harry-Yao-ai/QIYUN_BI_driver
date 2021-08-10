/******************************************************************************/
/*                                            COPYRIGHT (c) NXP 2017          */
/* FILE NAME kw35a.c                                         		          */
/* VERSION:  1.x                                   All Rights Reserved     	  */
/*                                                                        	  */
/* DESCRIPTION:                                                               */
/*                                                                            */
/*													                          */
/* PART INITIALIZATION SEQUENCE								                  */
/*                                                                            */
/* Omissions:                                                                 */
/*                                                                            */
/*============================================================================*/ 
/* AUTHOR: DOUG GROVER                                                   	  */
/* COMPILER: Code Warrior        VERSION: MPC55x                           	  */
/*                                                                        	  */
/*============================================================================*/ 
/* UPDATE HISTORY - KW35A                                                     */
/*                                                                            */
/* 1.00 M.LaCroix 20Mar2017                                                   */
/*	Generated from KW40_512 burn-in procedure. Reference KW35A includes.      */
/******************************************************************************/
/******************************************************************************/
 
/**************************************************/
/*               INCLUDE FILES                    */
/**************************************************/ 
#include "cc3.h"
#include "functions.h"

#include "s19data.h"
#include "revision.h"

#include "kw35a.h"

#include "jtag.h"

#include "uart.h"
#include "string.h"
#include "stdio.h"

/******************************************************************************/
/*                    Global and External Variables                           */
/******************************************************************************/

static UINT8 *badDut;

//UINT8 ModLoopCount[MAX_DUTS+3];	/*track number of cycles executed before qcheck failed, belongs in qcheck.c*/

UINT8 DutNumbers[] = SOCKET_LIST; 	/* defines which DUT return lines are used on the BIB */
UINT8 SCANNumbers[] = SCAN_LIST; 	/* defines which DUT return lines are used on the BIB */

UINT16 qfail =0;

/* set up the RCS text using format "$filename$rev$author" - IBE reads from s-record */
const UINT8  RCS[] = RCS_TEXT;

int power(int value,int power);
int strToHex(char *convert);

static VUINT16 *pStartButton;				/* indicates when START button is pressed for BI flow control 

/*******************************************************************************
FUNCTION        : main
PURPOSE         : Main function
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/
void main (void)
{	
//	static VUINT16 *pStartButton;			/* indicates when START button is pressed for BI flow control */

/******************************************************************************/
/* DECLARE VARIABLES HERE                                                     */	
/******************************************************************************/

//	UINT16 status;
	UINT32 i = RCS[1]; 		/* not used by executable code, but includes RCS header string in s19 */

// Define arrays to capture mode and data from each execution component during each loop.
// Define within main() so arrays reside in stack. No conflict when providing TK EDT stimulus using HSVG.

	UINT32 MISCData0[SOCKET_COUNT];			/* Use to capture JTAG ID / RAM BIST loop count / SCAN output word */
	UINT32 MISCData5[SOCKET_COUNT];			/* Use to capture mode by logging JTAG Test Register for each component */

/******************************************************************************/
/* INITIALIZE THE DRIVER HERE                                                 */
/******************************************************************************/
	SetupBI ();								/* This must be the first function called from the BI code */
	badDut = GetBadDutAddress ();			/* Get the address of global badDut */
	pStartButton = GetStartButtonAddress ();

	SetDutDisplay (DUT_TYPE, DUT_SW_REV);	/* set up DUT specific text for start button */
 
	DisplayScroll (FAST, SCROLL_NAME);		/* Display the device type */

	StartButton = BURN_IN;  				/* for stand alone mode, display the device type and await start button */
                 
	if (MODEID != MODE_IBE)                  
	{                                        
		while (StartButton != QCHECK);     	/* Wait here until "START" button is pushed */
	}                                        
	StartButton = QCHECK;	  				/* force qcheck to run first time */

	VdutsPowerDownOrder(4,3,2,1,5,100000);	/* orderly shut-down on driver supply event w/100ms delay */
	
/**********************************************************************************/
/* Initialize the o/p pins */
/**********************************************************************************/

// Perform this in VddRamp(); specify CC3/CC3E to determine control syntax
//	SET_VIO_VDUT1;							/* VIO  follows VDUT1 (or VDUT4) */
//	SET_VCLK_VDUT1;							/* VCLK follows VDUT1 (or VDUT2) */
// Initialize resources, not used until after VddRamp() selects actual operating conditions

	OUT0_31 = (0x00000000);					/* CLEAR ALL OP prior to initial power up */

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1((UINT16)1500,DUTCODE);	/* init resources, not used until after VddRamp() */
		SetPsBuf2((UINT16)1500,DUTCODE);	/* init resources, not used until after VddRamp() */
		SetPsBuf3((UINT16)1500,DUTCODE);	/* init resources, not used until after VddRamp() */
	}
	else if(DRIVER_TYPE == CC3)
	{

#ifdef DCDC_BUCK							/* VDUT1 supplies DCDC voltage */

		SetVdut(4,4000);					/*	VIO/SCK tracks with VDUT4 */
		WaitSeconds(1);
	
 		AnOut(10000); 						/* Set Anout to > 10V to power up eFuse circuit */

		SET_VIO_VDUT4;						/* VIO  follows VDUT1 (or VDUT4) */
		SET_VCLK_VDUT2;						/* VCLK follows VDUT1 (or VDUT2) */		

		WaitSeconds(1);		
#endif

#ifdef DCDC_BYPASS

		SetVdut(4,4000);					/*	VIO/SCK tracks with VDUT4 ... ONLY for ARMING eFUSE  */
		WaitSeconds(1);
	
 		AnOut(10000); 						/* Set Anout to > 10V to power up eFuse circuit */
		SET_VIO_VDUT4;						/* VIO  follows VDUT1 (or VDUT4) */
		SET_VCLK_VDUT2;						/* VCLK follows VDUT1 (or VDUT2) */
#endif

	}

// note: after trip idd test, driver level source is set for Buck vs Bypass in the VddRamp function

	OE1_DISABLE;  							/* DIS = DUT81-88, EN = OP8-15 */

	OE2_ENABLE;  							/* DIS = DUT89-92, EN = OP16-19 */
	OE3_ENABLE;  							/* DIS = DUT93-96, EN = OP20-23 */

	SEL_DISABLE;							/* EN= PCS0-3, DIS = OP0-3 */

	DDR_DRIVER_OUTPUT;						/* IN = DUT97-104, OUT = IO24-31 */

//	TempRx(3, UPDATE_DATA);					/* Sample the temperature IC */
//	WaitSeconds(2);
//	status = TempRx(3, GET_DATA);			/* Collect the previously sampled temperature */

/**********************************************************************************/
/* Initialize Part */
/**********************************************************************************/

/* DUT_CLK1 at default Fbus - other clocks follow DUT_CLK1 */
/* NOTE: SetClock() function will enable clocks (defeats SetClockOptions() OE_OFF_LOW ) */

	SetClocksInit();

/* After initial setup, terminate inputs low until needed in run functions */

	SetClocksOff(); 						/* clock inputs held low for initialization */		
	
/**********************************************************************************/
/* Initialize VDUT - avoid ECB trip due to current surge at power up, relax limit */
/**********************************************************************************/

	if(DRIVER_TYPE == CC3E)
	{
		SetVdut(5,9000);						/* Set VDUT5 to > 10V to power up eFuse circuit */
		SetEcb5Limit(3000);						/* some reliability ovens set Vin1 to 10V - cannot exceed */
		WaitSeconds(1);
	}
	else if(DRIVER_TYPE == CC3)
	{
 		AnOut(9000);                          	/* Set Anout to > 10V to power up eFuse circuit */
		WaitSeconds(1);							/* some reliability ovens set Vin1 to 10V - cannot exceed */
	}

	eFuse_Ctrl_INIT;							/* QSMCM.DDRQS.B.QDDSCK configured as output */
	eFuse_Ctrl_DISABLE; 						/* SCKAB = 1 to disable/reset eFuse circuit */
	WaitSeconds(1);								/* Delay while signals stabilize */ 
	eFuse_Ctrl_ENABLE;							/* SCKAB = 0 to enable over-current monitoring on eFuse  */
	WaitSeconds(1);								/* Delay while signals stabilize */ 

/* avoid false trip at inital power up by cranking current limit wide open */

//	SetEcbLimit (10000, 4000, 4000, 4000);
//
//	voltage[0]=ECB_REARM1;
//	voltage[1]=ECB_REARM2;
//	voltage[2]=ECB_REARM3;
//	voltage[3]=ECB_REARM4;
//	
//	status = SetVdutsAll(voltage[0],voltage[1],voltage[2],voltage[3]);	
//	WaitSeconds(1);
//
//	voltage[0]=ECB_VLTG1;
//	voltage[1]=ECB_VLTG2;
//	voltage[2]=ECB_VLTG3;
//	voltage[3]=ECB_VLTG4;
//	
//	status = SetVdutsAll(voltage[0],voltage[1],voltage[2],voltage[3]);	
//	WaitSeconds(1);

/* read/display current draw on board (display is in mA) */

//	DisplayScroll (FAST, "VDUT1(mA)    ");
//	DisplayDec (GetCurrent(1)*100);
//	WaitSeconds(1);
//
//	DisplayScroll (FAST, "VDUT2(mA)    ");
//	DisplayDec (GetCurrent(2)*100);
//	WaitSeconds(1);

/* confirm absence of high current conditions using actual ECB circuit */

//	SetEcbLimit (ECB1_IPOR,ECB2_IPOR,ECB3_IPOR,ECB4_IPOR);
//	WaitSeconds(1); 						/* time allowed to read */

// VDUT2/3/4 should always be disabled. VddRamp() function enables, controlled by header voltage[].

//	voltage[1]=0;							/* initial ramp from POR always disables supply */
//	Vdut2Off();					
//
//	voltage[2]=0;							/* initial ramp from POR always disables supply */
//	Vdut3Off();					
//
//	voltage[3]=0;							/* initial ramp from POR always disables supply */
//	Vdut4Off();					

// set ECB to allow surge during VDUT switching, run mode changes, could maybe be lower. Q-Check will reduce for partially populated BIBs
	SetEcbLimit (ECB1_RUN,ECB2_RUN,ECB3_RUN,ECB4_RUN);

/********************************************************************************************/
/* All downloads including polls are performed in BKGD Test Mode - enter w/ Q-Check levels  */
/********************************************************************************************/

/********************************************************************************************/
/* Do Qcheck first through after a Driver reset.  Make results available to IBE and display */
/********************************************************************************************/
	StartButton =QCHECK;
/***************************************************************************************/
/* BEGIN THE BURN-IN CODE */
/***************************************************************************************/
/* IBE BURN-IN LOOP */
	while(1)
	{
		switch (StartButton)									/* Either perform QCHECK, BURN-IN, OR BIN2_CHECK */
		{
			case QCHECK :			
			{
				WaitSeconds (1);
				if (StartButton != QCHECK)
					break;										/* required in case button pressed twice quickly for BIN2 check since loop is fast */
								
				DisplayChar("QCHK");

				SWD_Qcheck();									/* enable eFuse after power-up, poll for populated sockets */

				PreviousButton = QCHECK;						/* set after polling - used in Bin2 flow control */

				while(StartButton == QCHECK)
				{	
				DisplayBadDutsOnce();    
				}

				break;
			}

			case BURN_IN :										/* BI CODE */
			{
				DisplayScroll (FAST, "Log ECID");				/* Update display as we begin stresses */

// Capture JTAGID Register
				DisableDutData(0);								/* insures array available to post data to log file */
				SWDReadReg(DPACC,JTAGID,MISCData0);
				EnableDutData(0,(UINT32)MISCData0,DatarrySize);

// Capture ECID

// On KW35A, there is no SIM_UIDH Register . . . 
//				DisableDutData(1);								/* prevent host from polling prior to data initialization */
//				SWDInterimRead(MISCData1,1,DatarrySize,ECIDLoc1);

				DisableDutData(2);
				SWDInterimRead(MISCData2,2,DatarrySize,ECIDLoc2);
				DisableDutData(3);
				SWDInterimRead(MISCData3,3,DatarrySize,ECIDLoc3); 
				DisableDutData(4);
				SWDInterimRead(MISCData4,4,DatarrySize,ECIDLoc4);

// Read Time Zero NVM Status - log if interested (enable in data collection - only place DutData 6-9 are captured)

#ifdef TARGET_NVM
				DisableDutData(6);
				SWDInterimRead(MISCData6,6,DatarrySize,StatusAddr1);
				DisableDutData(7);
				SWDInterimRead(MISCData7,7,DatarrySize,StatusAddr2);
				DisableDutData(8);
				SWDInterimRead(MISCData8,8,DatarrySize,StatusAddr3); 
				DisableDutData(9);
				SWDInterimRead(MISCData9,9,DatarrySize,StatusAddr4);

				WaitSeconds(90);						/* Delay while logging data */ 
#endif

				DisplayScroll (FAST, "Start -BI-");				/* Update display as we begin stresses */

				SetStartTime();									/* Records beginning of burn-in (in seconds) */
				SWDRunModules(MISCData0, MISCData5);          	/* all components share arrays stored in stack */
				PreviousButton = BURN_IN;						/* set after run modules - used in Bin2 flow control */
		
				VddRamp(HSVGData[BIN2_PTR].downloadVoltage); 	/* ready for operator to request polling operation */

				SWD_AHB_CFG(BIN2_CHECK,0);		/* setup for polling */

				while(StartButton == BURN_IN)
				{	
				DisplayScroll (FAST, "Stress Complete ... Waiting for Bin2   "); /* Display while waiting for operator */
				}
				break;	            			/* exit case, proceed with Bin2 check next */ 

			} /* end case BURN_IN */

			case BIN2_CHECK :
			{
				DisplayScroll (FAST, "Run BIN2"); /* Display upon entry */

				SWDBin2();						/* put the Bin2 table 2nd in the structure */			

				PreviousButton = BIN2_CHECK;	/* set after polling - used in Bin2 flow control */

				ShutDown();						/* orderly shut-down of all DUT supplies and inputs */

				StartButton = BURNIN_DONE;  	/* added to allow operator to use 'RUN BIN2' GUI */
 				while(StartButton == BURNIN_DONE)
				{	
					DisplayScroll (FAST, "Bin2 Test Complete ... Burn-In DONE"); /* Display upon exit */
					WaitSeconds(1);				
				}
				break;							/* Leave bin2 check */
			}
		}	/* end of switch */
		
	}	/* end of while */

}	/* end of MAIN */

/* =============================================================================== */

void ShutDown(void)								/* power off all resources in an orderly fashion */
{
	RESET_LOW; 									/* Rst = low */

	OUT0_31 = 0x00000000;						/* all driver outputs terminated low */
	SetClocksOff();								/* clocks driven low before removing power */

	voltage[0]=POR_REARM1;						/* values are tested in VddRamp function, allows restart, repeated poll */
	voltage[1]=POR_REARM2;
	voltage[2]=POR_REARM3;
	voltage[3]=POR_REARM4;

	SetVdutsAll((UINT16)voltage[0],(UINT16)voltage[1],(UINT16)voltage[2],(UINT16)voltage[3]);	
	WaitSeconds(2);								/* drives to voltage that will de-assert POR */
	VdutsOff();									/* power supplies off */
}

/* =============================================================================== */
/* =============================================================================== */

/*******************************************************************************
FUNCTION        : SWD_QcheckDut
PURPOSE         : performs a generic socket check by accessing target memory and checking contents.
INPUTS NOTES    : Enhanced to check that DUT is unsecured. Fail is security or protection enabled.
			sockets - pointer to array where a list of the dut return lines
			that are used.  sockets[0] = total number of sockets and the rest
			contain the return lines.
RETURNS NOTES   : void
GENERAL NOTES   : QCHKResult will be used to filter ghost sockets (NRDs) on interim data reads
                  
*******************************************************************************/

void SWD_Qcheck(void)
{
	UINT8  countFail, i;

	UINT32 PartialECB1 = 0;							/* ECB limits are reduced for partially populated BIBs */
	UINT32 PartialECB2 = 0;

 	for (i = 0; i < SOCKET_COUNT; i++)				/* Initialize dataDut to all zeros */
		QCHKResult[i] = 0;	

// Set up for polling

	QSMCM.PQSPAR.B.QPAMISO=0;						/* Use MISO as general purpose input from DUT_MUX */
	ResetBadDut ();									/* clear BadDut array to 0 */

	VddRamp(HSVGData[QCHECK_PTR].downloadVoltage); 	/* note: starting from POR_VLTG1 will ramp up only */

	DisplayChar("QCHK");

// eFuse arming sequence is performed one time only during procedure, immediately prior to Q-Check

//	eFuse_Ctrl_DISABLE; 							/* SCKAB = 1 to disable/reset eFuse circuit */
//	WaitSeconds(1);									/* Delay while signals stabilize */ 
//	eFuse_Ctrl_ENABLE;								/* SCKAB = 0 to enable over-current monitoring on eFuse  */
//	WaitSeconds(1);									/* Delay while signals stabilize */ 

// Q-Check will read protection/security word at 0x0000040C, should read 0xFFFFFFFE
	SWD_AHB_CFG(QCHECK,0);							/* set up for polling */
	
// Now set the AHB_TAR to the address of the 1st word in status, read using AHB_DRW
	SWDWriteReg(APACC,AHB_TAR,SecurityLoc);			/* Set target location in TAR for APACC memory access */

#ifdef DEBUG
	SWDReadReg(APACC,AHB_TAR, QCHKResult);			/* first read posts data to shift chain */
	SWDReadReg(APACC,AHB_TAR, QCHKResult);			/* second read shifts data out for capture */
#endif

// Actual AHB_DRW data is shifted out on command following read request - read again to capture true results
	SWDReadReg(APACC,AHB_DRW, QCHKResult);			/* first read posts data to shift chain */
	SWDReadReg(APACC,AHB_DRW, QCHKResult);			/* second read shifts data out for capture */
													/* WRITE THE FAILING SOCKETS TO GLOBAL VARIABLE badDut */
	for (i=0,countFail=0; i<DutNumbers[0]; i++)		/* identifies the failing sockets for baddut array */
	{
		if ((QCHKResult[i] !=  SecurityVal))		/* Protection and security should be disabled */
		{
			badDut[countFail+Q_DUT_OFFSET] = (i+1);	/* adds bad socket to list of bad sockets */
			countFail++;							/* increments bad socket count */
		}
	}

	badDut[Q_FAIL_COUNT] = countFail;				/* Write total dut failures to array */
	badDut[Q_STATUS] = QCHECK_READY; 				/* table completed */

// Adjust ECB limits based on number of populated sockets. Fraction of current limit coincides with fraction of populated sockets
	PartialECB1 = (ECB1_RUN * (DutNumbers[0]-countFail) / DutNumbers[0]) + 400; 
	PartialECB2 = (ECB2_RUN * (DutNumbers[0]-countFail) / DutNumbers[0]) + 400; 

	SetEcbLimit ((UINT16)PartialECB1, (UINT16)PartialECB2, (UINT16)ECB3_RUN, (UINT16)ECB4_RUN);

}													/* end SWD_Qcheck() */

/**********************************************************************************/
/**********************************************************************************/

/***************************************************************************************/
// use QChkResult[] to filter reads, eliminate ghosting from unpopulated sockets
// defined as global UINT32 QCHKResult[SOCKET_COUNT]; /* used to filter ghosts during DUT data reads */
/***************************************************************************************/

void SWDBin2(void)
{
	UINT32 StatusWord[SOCKET_COUNT];			/* for DP_ACC/AP_ACC, shift read results into array */
	UINT32 dataDut[SOCKET_COUNT];

	UINT8 i,DUTStatus;

//	QSMCM.PQSPAR.B.QPAMISO=0;					/* Use MISO as general purpose input from DUT_MUX */
	ResetBadDut();								/* clear badDut array to 0 */

 	for (i = 0; i < SOCKET_COUNT; i++)				/* Initialize dataDut to all zeros */
		dataDut[i] = 0;	

	DisplayChar("BIN2");


// Since you don't really know when or where you are calling from, may need to exit SCAN/BIST mode
//	if (PreviousButton == BIN2_CHECK)		 	/* if repeating Bin2, need to restore power and initialize JTAG */
//	{
		VddRamp(HSVGData[BIN2_PTR].downloadVoltage); 	/* */
//	}

//	RESET_LOW;									/* included in function SWD_AHB_CFG() */
//	WaitMicroSeconds(100000);					/* Delay 100mS */
	
	SWD_AHB_CFG(BIN2_CHECK,0);							/* setup for polling */
	
// Select the AHB-AP to access RAM data transfer registers (debug access port 0, bank 0)

	SWDWriteReg(DPACC,DP_SELECT,AHBAP_SEL);			/* Select the AHB-AP for memory access */

// Set the AHB_TAR to the address of the 1st word in status, read using AHB_DRW

#ifdef TARGET_NVM
	SWDWriteReg(APACC,AHB_TAR,StatusAddr1);		/* Selects AHB-AP TAR for APACC access */			
#endif
#ifdef TARGET_HTOL
	SWDWriteReg(APACC,AHB_TAR,SecurityLoc);		/* Selects AHB-AP TAR for APACC access */		
#endif

	SWDReadReg(APACC, AHB_DRW, StatusWord);		/* first read posts data to shift chain */
	SWDReadReg(APACC, AHB_DRW, StatusWord);		/* second read shifts data out for capture */

// Extract status and pass into badDut array 

	for (i = 0; i < DutNumbers[0]; i++)			
	{

#ifdef TARGET_NVM
			DUTStatus = StatusWord[i] >> 24;	/* shift returned status value so status is lower byte */
#	ifdef CLEAR_NVM_SECURITY
 			badDut[Q_DUT_OFFSET+i] = (QCHKResult[i] != Populated) ? BIN2_NRD : (StatusWord[i] == NVM_Passcode) ? BIN2_PASS : BIN3_FAIL;
#	else
 			badDut[Q_DUT_OFFSET+i] = (QCHKResult[i] != Populated) ? BIN2_NRD : (StatusWord[i] == NVM_Passcode) ? BIN2_PASS : ((DUTStatus == 0xFF) || (DUTStatus == 0x10)) ? BIN2_FAIL : BIN3_FAIL;
#	endif
#endif

#ifdef TARGET_HTOL
			DUTStatus = StatusWord[i] << 24;	/* not used for HTOL, included here to avoid compile warning when TARGET_HTOL defined */
			badDut[Q_DUT_OFFSET+i] = (QCHKResult[i] != Populated) ? BIN2_NRD : (StatusWord[i] == HTOL_Passcode) ? BIN2_PASS : BIN3_FAIL;
#endif
	}

	badDut[Q_FAIL_COUNT] = DutNumbers[0]; 		/* numSockets - Bin2 needs to know number of sockets */
	badDut[Q_STATUS] 	 = QCHECK_FINAL; 		/* Bin2 complete, ready for IBE to get data */


// On NVM cycling resubmission, avoid excessive execution of redundant downloads by checking DUT status. Final Bin2 from GUI when all BIBs done.
// Broken out for ease of introduction / removal. Supports Flex (10K + 10K) as well as non-Flex cycling procedures.

#ifdef TARGET_NVM

	if (FirstPassFlag)
	{
		RAMLOAD_ENABLED = RAMLOAD_COMPLETE;					/* if no DUTs identified as Bin2, cycling will be complete */

		for (i = 0; i < DutNumbers[0]; i++)			
		{
			if (badDut[Q_DUT_OFFSET+i] != BIN2_NRD)			/* only look at DUTs not previously identified as NRD */
			{
				if (badDut[Q_DUT_OFFSET+i] == BIN2_FAIL)
					RAMLOAD_ENABLED = RAMLOAD_CONTINUE;		/* if any DUT identified as Bin2/NRD, cycling is NOT complete */			
			}
		}		
	}
	else
	{
		FirstPassFlag = FLAG_SET;
		
		for (i = 0; i < DutNumbers[0]; i++)			
		{
			if (badDut[Q_DUT_OFFSET+i] != BIN2_NRD)			/* only look at DUTs not previously identified as NRD */
			{
				if (badDut[Q_DUT_OFFSET+i] == BIN2_FAIL)
					FirstPassFlag = FLAG_CLR;				/* if any DUT identified as Bin2/NRD, cycling is NOT complete */			
			}
		}		
	}

#endif

}

/***************************************************************************************/
/*   RunModules -  Runs through a list of modules.                                     */
/***************************************************************************************/

void SWDRunModules(UINT32 *Datarry0, UINT32 *Datarry5)
{
	UINT32 EnterTime, DisplayTime, TimeToRun;
	UINT16 status;
	int k, project_loop, ModuleLoop;

	/* Number of times to run through the entire module list*/
	for(project_loop=0;project_loop<LOOPPROJECT;project_loop++)
	{

		BREAK_FLAG = TRUE;									/* will indicate loop is exited with polling request */

/***************************************************************************************/
#ifdef TARGET_HTOL

		if(StartButton != BIN2_CHECK)
		{
		if((HSVGData[BIST_PTR].NumberOfRuns) != 0)			/* If flag set, interpret as desire to run */
			SWD_RAMBIST(&HSVGData[BIST_PTR],Datarry0,Datarry5);	
		}
		else break;											/* allow flow to exit project_loop */
	
		if(StartButton != BIN2_CHECK)
		{
	if((HSVGData[SCAN_PTR].NumberOfRuns) != 0)				/* If flag set, interpret as desire to run */
			SWD_BI_SCAN(&HSVGData[SCAN_PTR],Datarry0,Datarry5);
		}
		else break;											/* allow flow to exit project_loop */		

#endif

/***************************************************************************************/

		if(StartButton != BIN2_CHECK)						/* allow flow to exit project_loop */
		{
		 for(k=FUNC_PTR;k<NUMMODULE;k++)					/* Run Through All Functional Download/Execute */
		 {
		  if(StartButton != BIN2_CHECK)						/* allow flow to exit k loop */
		  {
		   for(ModuleLoop=0;(ModuleLoop<data[k].NumberOfRuns)&&(RAMLOAD_ENABLED);ModuleLoop++)
			{	
				EnterTime = ReadSeconds();					/* deduct ramp, RAM Load and message scroll from execution time */

				DisableDutData(5);							/* avoid mode capture during array manipulation. host syns DutData0 */

				VddRamp(data[k].downloadVoltage);			/* ramp down to POR, then up; clocks, all I/P low */

				SetClocksInit();							/* Set up EXTAL / XTAL / EXTAL32  default configurations */
				SetClock(DUT_CLK1,PLL1,data[k].Clock1Frequency,0);	/* EXTAL bank1 */
				SetClock(DUT_CLK2,CK1FB,0,0);						/* EXTAL bank2 */
				SetClocksOn();								/* sets up normal operating config - some off, some inverted */
				XTAL_LOW;									/* hold low ... ipp_ind_test_sync_dividers if in test mode */
		
		
		
// Any memory access into FLASH/RAM from JTAG should be performed while core is regulated.

				DisplayChar("-DL-");
				SWDDownloadS19(&data[k]);					/* exit reset w/ core hold, transfer data for download to RAM */
				SWDPatchReset();							/* access M0+:VTOR to re-map reset vector */
					
				if(data[k].downloadDelay!=0)
				 WaitSeconds(data[k].downloadDelay);
			
	voltage[0]=data[k].runVoltage[0];
	voltage[1]=data[k].runVoltage[1];
	voltage[2]=data[k].runVoltage[2];
	voltage[3]=data[k].runVoltage[3];

	status = SetVdutsAll((UINT16)voltage[0],(UINT16)voltage[1],(UINT16)voltage[2],(UINT16)voltage[3]);	
	if ((status != PASS)&&(status != VDUT_WARNING))
	{
		VdutsOff ();	
		DisplayToggle ("VDT1", "Fail");
	}

	WaitMicroSeconds(100000);						/* Delay 100mS */

#ifdef DCDC_BUCK

// Buck uses VDUT1 for DCDC_in, VDUT2 for VIO/VIOZ terminations, VDUT4 for GPIO.
// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf2((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf3((UINT16)voltage[3],DUTCODE);		/* Clocks tracks with VDUT4 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT4;						/*  VIO follows VDUT4 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT2;						/* VCLK follows VDUT2 - if powered-off, redeclare to restore voltage */
	}

#else
 #ifdef DCDC_BYPASS

// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf2(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf3(voltage[0],DUTCODE);		/* Clocks tracks with VDUT1 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT1;						/*  VIO follows VDUT1 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT1;						/* VCLK follows VDUT1 - if powered-off, redeclare to restore voltage */
	}

 #else
  #error "Select only one DCDC Converter Mode DCDC_BUCK or DCDC_BYPASS in procedure parameter s19data.h file"
 #endif
#endif

// Select APACC MDM-AP for register capture for logging

				SWDWriteReg(DPACC,DP_SELECT,MDMAP_SEL);	/* Selects MDM-AP for APACC access (Access Port 1) */

// TestModeFlag determines/selects Regulator Bypass. Always be in Functional Test Mode (set in SWDDownloadS19)

				if(data[k].TestModeFlag == 1)				/* If flag set, select engage voltage override */
				{
					SWDWriteReg(APACC,MDM_TR,FuncTestDAC);	/* power up DAC */
					WaitMicroSeconds(50000);				/* Delay 50mS after completing operation */
					SWDWriteReg(APACC,MDM_TR,FuncTestPMC);	/* engage DAC for core voltage override */
					WaitMicroSeconds(50000);				/* Delay 50mS after completing operation */
				}

// If log has not captured ECID in first loop interation, do not overwrite with MDM-AP Status Register

// 				if(GetDutDataLength((UINT32)Datarry0) == 0)	/* if previous data has been logged, otherwise do not overwrite */
//				{
//					SWDReadReg(APACC, MDM_SR,Datarry0);		/* two reads required for APACC bank registers */
//					SWDReadReg(APACC, MDM_SR,Datarry0);		/* need to read once to post, once for capture */
//					EnableDutData(0,(UINT32)Datarry0,DatarrySize);
//				}

// Capture contents of MDM-AP Test Register to confirm operating conditions (mode / DAC)
 				SWDReadReg(APACC, MDM_TR,Datarry5);		/* engage DAC for core voltage override */
 				SWDReadReg(APACC, MDM_TR,Datarry5);		/* engage DAC for core voltage override */
// Log Test Register contents for all DUTs
				EnableDutData(5,(UINT32)Datarry5,DatarrySize);

// EXECUTE CODE DOWNLOADED TO RAM

				SWDGo();									/* release core hold, select AHB-AP for memory reads */

				DisplayScroll(FAST,data[k].s19FileName);	
//				sprintf(dispArray,"ok%2d",k);			/* display 'ok' in left 2 digits */
//				DisplayChar(dispArray);					/* Display running Module #*/
				DisplayChar("RL&E");						/* RAM Load and Execute */

// 				if(GetDutDataLength((UINT32)Datarry0) == 0)	/* if previous data has been logged, otherwise do not overwrite */
//				{
//	 				Capture MCG Status Register (MCG_S)
//					Confirm correct clock configuration following execution start-up
// 					Select 8-bit register access, no auto-increment to enable register write
//					SWDWriteReg(APACC,AHB_CSW,0x23000000);			/* Selects AHB-AP CSW for APACC access, DO NOT DISTURB PROT BITS */
//
// 					Specify Target Address Register for MCG Status, capture and log
//					SWDWriteReg(APACC,AHB_TAR,MCG_S);			/* Selects AHB-AP TAR for APACC access - Low Voltage Detect Status And Control 1 register */
//					SWDReadReg(APACC, AHB_DRW,Datarry0);		/* two reads required for APACC bank registers */
 //					SWDReadReg(APACC, AHB_DRW,Datarry0);		/* need to read once to post, once for capture */
//					EnableDutData(0,(UINT32)Datarry0,DatarrySize);
//
// 					Set up data transfer - AddrInc[5:4]=01 for autoincrement, Size[2:0]=010 for 32-bit writes
//					SWDWriteReg(APACC,AHB_CSW,0x23000012);			/* Selects AHB-AP CSW for APACC access, DO NOT DISTURB PROT BITS */
//				}



				DisplayTime = (ReadSeconds()) - EnterTime; /* adjust execution time to avoid drift in duration */
//				WaitSeconds(data[k].seconds - DisplayTime);

				TimeToRun = data[k].seconds - DisplayTime;
				PolledDelay(TimeToRun);

//				WaitSeconds(data[k].seconds - DisplayTime);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Begin Polling Insertion - read status after each module download for characterization           */
//				DisplayScroll(FAST,"Updating Log Results    ");	
				DisplayChar("LOGS");						/* avoid drift in execution for unaccounted time */

#ifdef TARGET_HTOL
// To read data out of RAM, we cannot perform POR. Issues seen accessing memory with DAC override.
// To facilitate reads through ARM debug port, reset, clear PMC bit in Test Reg, DAC control, assert core hold.

				SWD_AHB_CFG(BURN_IN,data[k].TestModeFlag);				/* setup reads with regulated core */

				SWDInterimRead(Datarry0, 0,DatarrySize,SecurityLoc);	/* reads broken out of polling procedure for persistent data */
				SWDInterimRead(MISCData1,1,DatarrySize,TempSenseLoc);	/* reads broken out of polling procedure for persistent data */
				SWDInterimRead(MISCData2,2,DatarrySize,BandgapLoc); 	/* reads broken out of polling procedure for persistent data */
				SWDInterimRead(MISCData3,3,DatarrySize,NVMChkSumLoc);
				SWDInterimRead(MISCData4,4,DatarrySize,NVMStatusLoc);

//				SWDBin2();												/* update GUI,set up JTAG for InterimRead call */
#endif

#ifdef TARGET_NVM

				SWD_AHB_CFG(BURN_IN,data[k].TestModeFlag);				/* setup reads with regulated core */

				SWDInterimRead(MISCData1,1,DatarrySize,StatusAddr1); 	/* reads broken out of polling procedure for persistent data */
				SWDInterimRead(MISCData2,2,DatarrySize,StatusAddr2); 	/* reads broken out of polling procedure for persistent data */
				SWDInterimRead(MISCData3,3,DatarrySize,StatusAddr3); 	/* reads broken out of polling procedure for persistent data */
				SWDInterimRead(MISCData4,4,DatarrySize,StatusAddr4); 	/* reads broken out of polling procedure for persistent data */
//				SWDBin2();												/* update GUI, set up JTAG for InterimRead call */
#endif

				BREAK_FLAG = FALSE;										/* tells SWDBin2() loop is not exited with polling request */
				PreviousButton = BURN_IN;								/* tells SWDBin2() call is after gathering valid results */
				SWDBin2();												/* update GUI, set up JTAG for InterimRead call */

/* End   Polling Insertion - read status after each module download for characterization           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		   } 												/* end of ModuleLoop */
	   	  }													/* end of StartButton != BIN2_CHECK run ModuleLoop */
	   	  else break;										/* exit NUMMODULES k++ loop */
	  	 }													/* end NUMMODULES k++ loop */

	  	 RESET_LOW;											/* Put device in Reset */
	  	 OUT0_31=0x00000000;								/* all output drivers low prior to exit, including RESET */
	  	 SetClocksOff();									/* all external inputs terminated low */

	 	}													/* end StartButton != BIN2_CHECK run NUMMODULES k++ loop */ 
	 	else break;											/* allows flow to exit project_loop */
	} 														/* end of project loop       */
} 															/* end of subroutine RunModules */

/***************************************************************************************/
/***************************************************************************************/
void SWD_AHB_CFG(int Button, UINT8 TestMode)
{
	extern VUINT16 *pStartButton;						/* indicates when START button is pressed for BI flow control */

	int	i = Button;										/* avoid compiler warning when CLEAR_NVM_SECURITY not defined */
	int ndx=0;

	UINT8  dispArray[5]="----";							/* used in count-down */	
	UINT32 ReadResult[SOCKET_COUNT];					/* for DP_ACC/AP_ACC, shift read results into array */

	RESET_LOW;											/* required for interim entry points, already low otherwise */
	WaitMicroSeconds(100000);							/* Delay 100mS */

	SWDResetToIdle();									/* reset JTAG port, select Run-Test/Idle state */		
	SWDReadReg(DPACC,DP_IDR, ReadResult);				/* required operation following reset */

	SWDWriteReg(DPACC,DP_CTRLSTAT,0x50000000);			/* sets CSYSPWRUPREQ, CDBGPWRUPREQ to enable access to debug  */
	WaitMicroSeconds(100000);							/* Delay 100mS */

#ifdef DEBUG
	SWDReadReg(DPACC,DP_CTRLSTAT, ReadResult);			/* check status from previous write, capture results */
#endif

	SWDWriteReg(DPACC,DP_ABORT,0x0000001F);				/* STICKYERR and others on SWD - clear with write  */

#ifdef DEBUG
	SWDReadReg(DPACC,DP_CTRLSTAT, ReadResult);			/* check status from previous write, capture results */
#endif


// Select the MDM-AP to perform system/debug reset, enable CPU hold, access test register
	SWDWriteReg(DPACC,DP_SELECT,MDMAP_SEL);				/* Selects MDM-AP for APACC access (Access Port 1) */

#ifdef DEBUG
	SWDReadReg(DPACC,DP_CTRLSTAT, ReadResult);			/* check status from previous write, capture results */
#endif


/***************************************************************************************/
#ifdef CLEAR_NVM_SECURITY

// While holding the external resetb pin low, request a mass erase by setting MDM-AP CSR bit 0.

	if (Button == QCHECK)								/* perform ONLY immediately prior to QCHECK */
	{
		DisplayScroll (FAST, "Mass Erase Enabled - ABORT BY PRESSING <<<<");
		WaitSeconds(1);								/* one second at a time */

		for(ndx=0;ndx<5;ndx++)
		{
			if (StartButton  == QCHECK)
			{
				sprintf(dispArray," %1ds ",5-ndx);		/* display shows count-down */
				DisplayChar(dispArray);					/* Display seconds before erase */
			
				WaitSeconds(1);							/* one second at a time */
			}
		}
		if (StartButton == QCHECK)						/* perform ONLY if operation not aborted */
		{
			DisplayChar("ERAS");
			SWDWriteReg(APACC,MDM_CR,0x00000001);		/* mass erase request */
			WaitSeconds(2);								/* time allowed to perform mass erase */

// Insert a POR to clear the MDM-AP Status Register MAss Erase Acknowledge Bit following the operation			
			VddRamp(HSVGData[QCHECK_PTR].downloadVoltage);

			SWDResetToIdle();							/* reset JTAG port, select Run-Test/Idle state */		
			SWDReadReg(DPACC,DP_IDR, ReadResult);		/* required operation following reset */
			SWDWriteReg(DPACC,DP_CTRLSTAT,0x50000000);	
			SWDWriteReg(DPACC,DP_ABORT,0x0000001F);		/* STICKYERR and others on SWD - clear sticky bit with write 1 to bit  */
			SWDWriteReg(DPACC,DP_SELECT,MDMAP_SEL);		/* Select the MDM-AP to perform system/debug reset, enable CPU hold */
		}
		else
		{
			DisplayScroll (FAST, "Mass Erase ABORTED !!!!");
			StartButton = QCHECK;						/* pushed to abort - restore original value for flow */
		}
	}
	
#endif
/***************************************************************************************/

// BURN_IN RL&E always in Functional Test Mode. Latch on rising edge of Reset.

	if (Button == BURN_IN)
	{
		if (TestMode == 1)
			SWDWriteReg(APACC,MDM_TR,FuncTestDAC);		/* 1: configure DAC, disengaged from core */
		else
			SWDWriteReg(APACC,MDM_TR,FuncTest);			/* 0: do not require DAC */
	WaitMicroSeconds(50000);							/* Delay 50mS after Test Register write */	
	}
	
// Execute system reset while maintaining CM4 Core Hold

	SWDWriteReg(APACC,MDM_CR,0x00000010);				/* clear system reset request (bit3), maintain CM4 Core Hold reset */

#ifdef DEBUG
	SWDReadReg(APACC,MDM_CR, ReadResult);			/* check status from previous write, capture results */
	SWDReadReg(APACC,MDM_CR, ReadResult);			/* check status from previous write, capture results */

	SWDReadReg(APACC,MDM_SR,ReadResult);				/* first read posts data to shift chain */
	SWDReadReg(APACC,MDM_SR,ReadResult);				/* first read posts data to shift chain */
#endif

	RESET_HIGH;											/* now resetb pin alone controls system reset */
	WaitMicroSeconds(100000);							/* Delay 2mS */


#ifdef DEBUG
// Debug Begin
// At this point, it is convenient to read the MDM-AP Status Register to confirm Reset/Security state
// Security should be disabled (bit 2 cleared), Reset should be released (bit 3 set). Normal read value 0x0000003A

	SWDReadReg(APACC,MDM_SR,ReadResult);				/* first read posts data to shift chain */
	SWDReadReg(APACC,MDM_SR,ReadResult);				/* second read shifts data out for capture */

	SWDReadReg(DPACC, DP_CTRLSTAT, ReadResult);			/* first read posts data to shift chain  */
//	SWDReadReg(DPACC, DP_CTRLSTAT, ReadResult);			/* second read shifts data out for capture */
// Debug End
#endif

// Select the AHB-AP to access RAM data transfer registers (debug access port 0, bank 0)

	SWDWriteReg(DPACC,DP_SELECT,AHBAP_SEL);				/* Selects AHB-AP for APACC access */
	
#ifdef DEBUG
	SWDReadReg(DPACC,DP_CTRLSTAT, ReadResult);			/* check status from previous write, capture results */
#endif

// Set up data transfer - AddrInc[5:4]=00 for NO autoincrement, Size[2:0]=010 for 32-bit writes

	SWDWriteReg(APACC,AHB_CSW,0x23000002);				/* Selects AHB-AP CSW for APACC access */

#ifdef DEBUG
	SWDReadReg(APACC,AHB_CSW, ReadResult);			/* check status from previous write, capture results */
	SWDReadReg(APACC,AHB_CSW, ReadResult);			/* check status from previous write, capture results */
#endif

// Now set the AHB_TAR to the address of the word targeted for read/write

}

/***************************************************************************************/
/***************************************************************************************/
void SWDInterimRead(UINT32 *InterimArray, int InterimDUTData, UINT32 InterimSize, UINT32 InterimLocation)
{

// Reorganized to facilitate debug. Always read target data into ReadResult.
// Do not copy into target InterimArray if previously stored data will be over-written.

	UINT32 ReadResult[SOCKET_COUNT];					/* for DP_ACC/AP_ACC, shift read results into array */
	int i;
	
// Clear any potential sticky errors from previous operations

	SWDWriteReg(DPACC,DP_ABORT,0x0000001F);			/* STICKYERR and others on SWD - clear sticky bit with write 1 to bit  */

// Set the AHB_TAR to the target address, read using AHB_DRW

	SWDWriteReg(APACC,AHB_TAR,InterimLocation);		/* Selects AHB-AP TAR for APACC access */

	SWDReadReg(APACC, AHB_DRW, ReadResult);			/* first read posts data to shift chain */
	SWDReadReg(APACC, AHB_DRW, ReadResult);			/* second read shifts data out for capture */

	if(GetDutDataLength(InterimDUTData) == 0)			/* if previous data has been logged, otherwise do not overwrite */
	{
		for (i = 0; i < SOCKET_COUNT; i++)
			InterimArray[i] = (QCHKResult[i] == Populated) ? ReadResult[i] : Unpopulated;	/* fill unpopulated sockets with blank data */
			
		EnableDutData(InterimDUTData,(UINT32)InterimArray,(UINT32)InterimSize);
	}
	
	WaitMicroSeconds(10000);				/* Default delay 10mS after completing operation */

}

/***************************************************************************************/
/***************************************************************************************/

void 	PolledDelay(UINT32 DelayTime)
{

#ifdef TARGET_HTOL

// Run stress in increments to allow test for polling request - will terminate procedure from operator interface
// Algorithm insures a non-zero value is always specified for the delay

	UINT32	TimeUsed,TimeRemaining;

	TimeUsed = 0;										/* Initialized for use in loop */
	while ( (StartButton != BIN2_CHECK) && (DelayTime > TimeUsed) )
	{
		TimeRemaining = DelayTime - TimeUsed;			/* deduct expended time from total */
		if (TimeRemaining > TimeSegment)				/* time remaining is less than the increment */	
		{
			WaitSeconds(TimeSegment);					/* check for Bin2 button every nn seconds */
			TimeUsed = TimeUsed + TimeSegment;
		}
		else
		{
			WaitSeconds(TimeRemaining);					/* check for Bin2 button every nn seconds */
			TimeUsed = TimeUsed + TimeRemaining;
		}					
	}
#endif

#ifdef TARGET_NVM
	WaitSeconds(DelayTime);								/* never interrupt a cycling download */
#endif
		
}

/***************************************************************************************/
/***************************************************************************************/
void VddRamp(int *NextVoltage)
{
	UINT16 status;
	UINT16 PreviousVdd;

	PreviousVdd = voltage[0]; 			/* VDUT1 current setting */

	DisplayChar("RAMP");

// prepare resources for ramp - terminate DUT inputs low and stop clocks ... ramp or no

	RESET_LOW; 							/* Rst = low */
	SetClocksOff();						/* clocks were driven low during ECB trip test */
	OUT0_31 = 0x00000000;				/* terminate all driver inputs low during POR */

	Vdut2Off();							/* turn off during ramp down */
	Vdut3Off();							/* turn off during ramp down */
	Vdut4Off();							/* turn off during ramp down */
	WaitSeconds(1);						/* Delay 1s while signals decay */ 

#ifdef DCDC_BUCK

// Buck uses VDUT1 for DCDC_in, VDUT3 for GPIO terminations. Use VDUT2/VDUT4 (not hooked up) to pass VIO/VIOZ levels

// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range
// Do not use CC3 . . .
//	SET_VIO_VDUT4;	/*  VIO follows VDUT4 - after any power-off event, must redeclare any time you intend to restore voltage */
//	SET_VCLK_VDUT2;	/* VCLK follows VDUT2 - after any power-off event, must redeclare any time you intend to restore voltage */


// If 1st time we are powerng up, do not set VDUT1 on then off again . . .
	if (PreviousVdd != 0000)
	{
	SetVdut(1,800);					/*	try to give clean POR deassertion */
	WaitSeconds(2);

	Vdut1Off();							/* turn off supply to DCDC converter / Vdd in Bypass */
	WaitSeconds(2);						/* Delay 1s while signals decay */ 
	}

	voltage[0] = (UINT16)NextVoltage[0];
	voltage[1] = (UINT16)NextVoltage[1];
	voltage[2] = (UINT16)NextVoltage[2];		/* if not used, set to 0 */
	voltage[3] = (UINT16)NextVoltage[3];

	status = SetVdutsAll(voltage[0],voltage[1],voltage[2],voltage[3]);	
	if ((status != PASS)&&(status != VDUT_WARNING))
	{
		VdutsOff ();	
		DisplayToggle ("VDT1", "Fail");
	}		

	WaitMicroSeconds(StepDelay);		/* Delay n00ms  */

// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf2((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf3((UINT16)voltage[3],DUTCODE);		/* Clocks tracks with VDUT4 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT4;						/*  VIO follows VDUT4 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT2;						/* VCLK follows VDUT2 - if powered-off, redeclare to restore voltage */
	}

	WaitSeconds(1);						/* Delay 1s while signals stabilize */ 	
	
#else
 #ifdef DCDC_BYPASS

// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range
// Do not use CC3 . . .
//	SET_VIO_VDUT1;	/*  VIO follows VDUT1 - if powered-off, redeclare to restore voltage */
//	SET_VCLK_VDUT1;	/* VCLK follows VDUT1 - if powered-off, redeclare to restore voltage */

//	DisplayChar("RAMP");					/* voltages don't match, ramp to next level */
// ramp down to POR from previous voltage

// If 1st time we are powerng up, do not set VDUT1 on then off again . . .
	if (PreviousVdd != 0000)
	{
	SetVdut(1,800);					/*	try to give clean POR deassertion */
	WaitSeconds(2);

	Vdut1Off();							/* turn off supply to DCDC converter / Vdd in Bypass */
	WaitSeconds(2);						/* Delay 1s while signals decay */ 
	}

	voltage[0] = (UINT16)NextVoltage[0];
	voltage[1] = (UINT16)NextVoltage[1];
	voltage[2] = (UINT16)NextVoltage[2];		/* if not used, set to 0 */
	voltage[3] = (UINT16)NextVoltage[3];

	status = SetVdutsAll(voltage[0],voltage[1],voltage[2],voltage[3]);	
	if ((status != PASS)&&(status != VDUT_WARNING))
	{
		VdutsOff ();	
		DisplayToggle ("VDT1", "Fail");
	}		

	WaitMicroSeconds(StepDelay);		/* Delay n00ms  */

// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf2(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf3(voltage[0],DUTCODE);		/* Clocks tracks with VDUT1 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT1;						/*  VIO follows VDUT1 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT1;						/* VCLK follows VDUT1 - if powered-off, redeclare to restore voltage */
	}

 #else
  #error "Select only one DCDC Converter Mode DCDC_BUCK or DCDC_BYPASS in procedure parameter s19data.h file"
 #endif
#endif

	SWD_Drive_ON;						/* may have been disabled due to mode muxing */
	WaitSeconds(1);						/* Delay 1s while signals stabilize */ 

} 										/* end of ramp-down /POR / ramp-up */

/***************************************************************************************/
/***************************************************************************************/

/***************************************************************************************/
/*   SetClocksInit - default clock configuration for Functional Mode                   */
/***************************************************************************************/

void SetClocksInit(void)
{
	SetClock(DUT_CLK1,PLL1,Fext,0);						/* EXTAL bank1 */
	SetClock(DUT_CLK2,CK1FB,0,0);						/* EXTAL bank2 */

	SetClock(DUT_CLK3,PWM1,32000,50);					/* not used    */
	SetClock(DUT_CLK4,PWM1,32000,50);					/* not used    */
}

/***************************************************************************************/
/*   SetClocksOff- temporarily disable clocks terminating pin input levels low         */
/***************************************************************************************/

void SetClocksOff(void)
{
	SetClockOptions(DUT_CLK1,1,DLY0,INV_OFF,OE_OFF_LOW); /* clock inputs held Lo for power-down */
	SetClockOptions(DUT_CLK2,1,DLY0,INV_OFF,OE_OFF_LOW);

	SetClockOptions(DUT_CLK3,1,DLY0,INV_OFF,OE_OFF_LOW);	
	SetClockOptions(DUT_CLK4,1,DLY0,INV_OFF,OE_OFF_LOW);	
}

/***************************************************************************************/
/*   SetClocksOn- enable clock inputs to pins (terminated low during power cyclng)     */
/***************************************************************************************/

void SetClocksOn(void)
{
	SetClockOptions(DUT_CLK1,1,DLY0,INV_OFF,OE_ON);		/* EXTAL bank1 */
	SetClockOptions(DUT_CLK2,1,DLY0,INV_OFF,OE_ON);		/* EXTAL bank2 */


//	SetClockOptions(DUT_CLK3,1,DLY0,INV_OFF,OE_ON);		/* not used */
	SetClockOptions(DUT_CLK3,1,DLY0,INV_OFF,OE_OFF_LOW);	

//	SetClockOptions(DUT_CLK4,1,DLY0,INV_OFF,OE_ON);		/* not used */
	SetClockOptions(DUT_CLK4,1,DLY0,INV_OFF,OE_OFF_LOW);	

}

/***************************************************************************************/

int SWDDownloadS19(struct burnInModule *module)
{
	int 	i,address,DataCharSize;
	char 	temp[20];								/* used to parse data of s19 for processing */
	UINT8	EndianMap[8]={6,7,4,5,2,3,0,1};			/* used to reverse s19 byte order for AP write */

// to perform the download, the DUT must first be configured for writes to RAM
// user 1st address in record line to set transfer address register
// use incremental word writes to transfer data to RAM

// resetb is currently held low. If required, Functional Test Mode will have been selected.
// Debug Port must be powered up in order to use access ports:

	SWD_AHB_CFG(BURN_IN,module->TestModeFlag);		/* always sets Regulated PMC Functional Test Mode for RAM Load */

// Prior to RAM download and execute, disable the Low Voltage Reset for running at lower operating limits
// Select 8-bit register access, no auto-increment to enable register write
	SWDWriteReg(APACC,AHB_CSW,0x23000000);			/* Selects AHB-AP CSW for APACC access, DO NOT DISTURB PROT BITS */
// Specify Target Address Register for LVR control
	SWDWriteReg(APACC,AHB_TAR,PMC_LVDSC1);			/* Selects AHB-AP TAR for APACC access - Low Voltage Detect Status And Control 1 register */
// Clear LVDRE bit in control register to disable low voltage resets
	SWDWriteReg(APACC,AHB_DRW,LVR_DISABLE);			/* Selects AHB-AP DRW for APACC access - clear LVDRE bit in control register */

//	DisableDutData(1);								/* prevent host from polling prior to data initialization */
//	SWDInterimRead(MISCData1,1,DatarrySize,PMC_LVDSC1);
//	MISCData1[0] = MISCData1[0] | 0xDEFE0000;

// Set up data transfer - AddrInc[5:4]=01 for autoincrement, Size[2:0]=010 for 32-bit writes

	SWDWriteReg(APACC,AHB_CSW,0x23000012);			/* Selects AHB-AP CSW for APACC access, DO NOT DISTURB PROT BITS */

// Now set the AHB_TAR to the address of the 1st word in each line in s-record, write the data on the line to AHB_DRW

// 32-bit S-Record Format: LL=length; AA=Address; DD=Data; CS=Checksum.
// S3LLAAAAAAAADDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDCS
// 0123456789012345678901234567890123456789012345

// download until the string is END, end of S-Record found. 

	for (i=0; strcmp(module->TheFile.s19Record[i],"END")!=0; i++)
	   { 
		int m,charndx,numBytes,dataWord;

// Get the start address of the line of data

            char *theRecord=module->TheFile.s19Record[i];
            for(m=4;m<12;m++)						/* 4 byte address, 8 characters in string */
                temp[m-4]=theRecord[m];
            temp[m-4]='\0';
		address=StrToHex(temp);
 
// write address to AHB-AP Transfer Address Register at beginning of each line of S19
		
		SWDWriteReg(APACC,AHB_TAR,address);			/* Selects AHB-AP TAR for APACC access */

// Because ARM is strongly 16-bit aligned (not 32-bit), partial words may be contained on lines and must be accommodated.
// Determine the number of BYTES on the current line of the S-Record

            for(m=2;m<4;m++)						/* one byte character count, two characters in string */
                   temp[m-2]=theRecord[m];
            temp[m-2]='\0';							/* s19 contains byte count, not word count */

            numBytes=StrToHex(temp)-5;				/* convert character representation to number, adjust for address field and checksum */
            
// Convert one line of bytes into words / partial words - 1st data word character is always 12th string character

		m=12;										/* pointer to character in string during operation */
		while (numBytes)							/* combine bytes into longer values until all used */
		{

		DataCharSize = (numBytes >= 4) ? WordCharSize : numBytes*2; /* word or partial word */
		numBytes -= DataCharSize/2;					/* reduce remaining byte count */

// initialize temp array in case data is missing
			for (charndx = 0; charndx < WordCharSize; charndx++)
				temp[charndx]='0';					/* in case you don't overwrite a complete word, need to init string to zeroes */
// tricky ...
			for(charndx = WordCharSize-DataCharSize ; charndx < WordCharSize; charndx++)
				temp[charndx]=theRecord[m+EndianMap[charndx]]; /* words in s-record are Little-Endian, need to reverse */
			temp[WordCharSize]='\0';
	
			dataWord = StrToHex(temp);

// Transfer the current data word to target address in RAM

			SWDWriteReg(APACC,AHB_DRW,dataWord);	/* Selects AHB-AP DRW for APACC access */
			
			m += DataCharSize;						/* shift to next word / partial word */
      	}
	}
	return 1;
}

/***************************************************************************************/
/***************************************************************************************/

int StrToHex(char *convert)
{
	int i,multiply,word,temp;

   	for(i=0,word=0,multiply=strlen(convert)-1;convert[i]!='\0';i++,multiply--)
	{
		if(convert[i]=='A')							/* hex characters, convert to decimal equivalent */
		 temp = 10;
      	 else if(convert[i]=='B')
		  temp =11;
		  else if(convert[i]=='C')
		   temp=12;
		   else if(convert[i]=='D')
		    temp=13;
      	    else if(convert[i]=='E')
		     temp=14;
 		     else if(convert[i]=='F')
			temp=15;
 		      else
 			 temp=convert[i]-48;					/* numeric character, convert ANSI code to decimal equivalent ('0' = 0x30, 48 base 10) */
    
 		word = word + temp*power(16,multiply) ;
    	}
	return word;
}

/***************************************************************************************/
int power(int value,int power)
{
 	int i=0;
 	int result=1;
 	if(power==0)
      	return 1;

 	for(i=0;i<power;i++)
		result = result*value;
 
      return result;
}


/**********************************************************************************/

/*******************************************************************************/
/* FUNCTION                                                                    */ 
/*******************************************************************************/

/*******************************************************************************
FUNCTION        : Toggle_SWD_CLK
PURPOSE         : Toggles SWD_CLK the number of times specified.
INPUTS NOTES    : numClocks - number of SWD_CLK clock pulses
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void Toggle_SWD_CLK (UINT32 numClocks)
{
	UINT32 i;
		
	WaitMicroSeconds(20);						/* Delay before continuing */

	for (i = 0; i < numClocks; i++)
	{
		SWD_CLK_HIGH;							/* capture occurs on rising edge, insure low to high transistion */
		WaitMicroSeconds(20);					/* Delay before continuing */
		SWD_CLK_LOW;
		WaitMicroSeconds(20);					/* Delay before continuing */
	}	

	WaitMicroSeconds(20);						/* Delay before continuing */
}

 // End of Toggle_SWD_CLK

/*******************************************************************************
FUNCTION        : ToggleTCLK
PURPOSE         : Toggles TCLK the number of times specified.
INPUTS NOTES    : numClocks - number of TCLK clock pulses
RETURNS NOTES   : void
GENERAL NOTES   : Based on CC2 TOG_TCLK.
                  **ad may want to delay this as the CC2 file delays
*******************************************************************************/
void ToggleTCLK (UINT32 numClocks)
{
	UINT32 i;
	
	WaitMicroSeconds(20);						/* Delay before continuing */

	for (i = 0; i < numClocks; i++)
	{
		TCLK_HIGH;								/* capture occurs on rising edge, insure low to high transistion */
		WaitMicroSeconds(20);					/* Delay before continuing */
		TCLK_LOW;
		WaitMicroSeconds(20);					/* Delay before continuing */
	}	

	WaitMicroSeconds(20);						/* Delay before continuing */
}
 // End of ToggleTCLK

/*******************************************************************************/

/*******************************************************************************
FUNCTION        : SWDResetToIdle()
PURPOSE         : performs a JTAG Reset and initialization of state machine
INPUTS NOTES    : 
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void SWDResetToIdle(void)
{
	SWD_DIO_HIGH;
	Toggle_SWD_CLK(100);
	SWD_DIO_LOW;
	Toggle_SWD_CLK(100);
	
	WaitMicroSeconds(100000);					/* Delay before continuing */
}

/*******************************************************************************
FUNCTION        : JTAGResetToIdle()
PURPOSE         : performs a JTAG Reset and selects the Run - Test/Idle TAP State
INPUTS NOTES    : 
			sockets - pointer to array where a list of the dut return lines
			that are used.  sockets[0] = total number of sockets and the rest
			contain the return lines.
RETURNS NOTES   : void
GENERAL NOTES   : There is a one cycle delay when shifting, so toggle TCLK twice to shift TDI->TDO
*******************************************************************************/

void JTAGResetToIdle(void)
{
	TMS_LOW;									/* enables TAP Test-Logic-Reset state exit on next clock */
	JRESET_LOW;									/* insure JRESET is held high - leave high always */
	ToggleTCLK(10);								/* 5 TCLK rising edges guarantees TAP in Test-Logic-Reset state */

	TMS_HIGH;									/* To ensure deterministic operation of test logic, TMS high  while JRESET goes high. */
	JRESET_HIGH;								/* insure JRESET is held high - leave high always */
	ToggleTCLK(10);								/* 5 TCLK rising edges guarantees TAP in Test-Logic-Reset state */

	TMS_LOW;									/* enables TAP Test-Logic-Reset state exit on next clock */
	ToggleTCLK(10);								/* shift to TAP Run-Test/Idle state */

	WaitMicroSeconds(10000);					/* Delay before continuing */

} // 	END JTAGResetToIdle()

/*******************************************************************************
FUNCTION        : SWDWriteReg()
PURPOSE         : performs a JTAG DR Write/Update.
INPUTS NOTES    : Check defines for DR command names corresponding to various register selects.
                  
RETURNS NOTES   : void
GENERAL NOTES   : 
                : 
*******************************************************************************/
void SWDWriteReg(UINT8 DPAP, UINT8 DPAPAddr, UINT32 DPAPCode)
{
	int 	i, parity;	
	UINT8 	dut;
	UINT8 	ack;

	int		RegisterSize=32;
	UINT32 	DRmask;

	UINT8	DAP_ACK[SOCKET_COUNT];				/* for DP_ACC/AP_ACC, 1st 3 bits [2:0] returned are ACKnowledge */

//	QSMCM.PQSPAR.B.QPAMISO=0;					/* Use MISO as general purpose input from DUT_MUX */

// Begin write sequence

	parity = 0;									/* initialize, need to calculate parity bit for preamble */

// Start Bit - A single start bit, with value 1

	SWD_DIO_HIGH;								/* set SWD_DIO to '1' for start bit */

	Toggle_SWD_CLK(1);							/* next state */
	
// APnDP A single bit, indicating whether the Debug Port or the Access Port Access Register is to be accessed. This bit is 0 for an DPACC access, or 1 for a APACC access.

	switch (DPAP)								/* select register bit in command string based on port accessed */
	{
		case DPACC :							/* */
		{
			SWD_DIO_LOW;						/* set to '0' for DPACC */
			break; 								/* exit case, proceed  */ 
		} 										/* end case DPACC */
		case APACC :							/*  */
		{
			SWD_DIO_HIGH;						/* set to '1' for APACC */
			parity++;							/* increment count of bits set to one for parity calcuation */				
			break;						
		}										/* end case APACC */
	}											/* end of switch */

	Toggle_SWD_CLK(1);							/* next state */
	
// RnW A single bit, indicating whether the access is a read or a write. This bit is 0 for an write access, or 1 for a read access.

	SWD_DIO_LOW;								/* set to '0' for write */
					
	Toggle_SWD_CLK(1);							/* next state */

// A[2:3] Two bits, giving the A[3:2] address field for the DP or AP Register Address:
// For a DPACC access, the A[3:2] value is the address of the register in the SW-DP
// register map, see SW-DP Register Map on page 6-5.
// For an APACC access, the register being addressed depends on the A[3:2] value and
// the value held in the SELECT register. 
// The A[3:2] value is transmitted Least Significant Bit (LSB) first on the wire.

	DPAPAddr >>= 2;								/* use DRmask to determine SWD_DIO for address field */
	for (i=0,DRmask=0x01; i<2; i++,DRmask<<=1)	/* address field is two bits wide */
	{
		if ((DPAPAddr & DRmask) != 0)
		{
			
			SWD_DIO_HIGH;						/* non-zero value indicates bit is high */
			parity++;							/* increment count of bits set to one for parity calcuation */				
		}
		else
			SWD_DIO_LOW;

		Toggle_SWD_CLK(1);						/* next state */
	}

// Parity A single parity bit for the preceding packet.
// The parity check is made over the APnDP, RnW and A[2:3] bits. If, of these four bits:
// the number of bits set to 1 is odd, then the parity bit is set to 1
// the number of bits set to 1 is even, then the parity bit is set to 0.

	if ((parity & 1) != 0)
		SWD_DIO_HIGH;							/* non-zero value indicates odd bit count, parity bit is high */
	else
		SWD_DIO_LOW;

	Toggle_SWD_CLK(1);							/* next state */

// Stop A single stop bit. In the synchronous SWD protocol this is always 0.

	SWD_DIO_LOW;
	Toggle_SWD_CLK(1);							/* next state */

// Park A single bit. The host must drive the Park bit HIGH to park the line before tristating it for the
// turnaround period. This ensures the line is read as HIGH by the target. This is required as the pull-up
// on the Serial Wire Debug interface is weak. The target reads this bit as 1.

	SWD_DIO_HIGH;								/* non-zero value indicates odd bit count, parity bit is high */
	Toggle_SWD_CLK(1);							/* next state */

// Trn Turnaround. This is a period during which the line is not driven and the state of the line is
// undefined. The length of the turnaround period is controlled by the TURNROUND field in
// the Wire Control Register, see The Wire Control Register, WCR (SW-DP only) on page 6-18.
// The default setting is a turnaround period of one clock cycle.

// To avoid shadow results on monitor lines of unpopulated sockets, do not tri-state
	SWD_Drive_OFF;								/*  drivers for SWD_DIO off */

// ACK[0:2] A three-bit target-to-host response. 001 = 
// Note
// The ACK value is transmitted LSB-first on the wire ( ACK[0:2] ).

	for (ack = 0; ack<3; ack++)					/* ACKnowledge field is three bits wide */
	{
		Toggle_SWD_CLK(1);						/* shift out acknowledge bit */
		for (dut=0,DRmask=0x01; dut < DutNumbers[0]; dut++,DRmask<<=1 )		
		{
			DUT_MUX = DutNumbers[dut+1]-1;  	/* ignore non-existing return lines */
			DAP_ACK[dut] = (DUT_INPUT == 1) ? (DAP_ACK[dut] |= DRmask) : (DAP_ACK[dut] &= ~DRmask);
		}
	}

// Behavior on scope looks like DUT parks line, tri-state SWD_DIO
// Park A single bit. The host must drive the Park bit HIGH to park the line before tristating it for the
// turnaround period. This ensures the line is read as HIGH by the target. This is required as the pull-up
// on the Serial Wire Debug interface is weak. The target reads this bit as 1.

	Toggle_SWD_CLK(1);							/* clocked/park/tri-state at end of ACK loop */

// Trn Turnaround. This is a period during which the line is not driven and the state of the line is
// undefined. The length of the turnaround period is controlled by the TURNROUND field in
// the Wire Control Register, see The Wire Control Register, WCR (SW-DP only) on page 6-18.
// The default setting is a turnaround period of one clock cycle.
// after tri-state period, engage driver

// To avoid shadow results on monitor lines of unpopulated sockets, do not tri-state
	SWD_Drive_ON;								/*  drivers for SWD_DIO on */
	Toggle_SWD_CLK(1);							/* clocked/tri-state at end of ACK loop */

// WDATA[0:31] 32 bits of write data, from host to target.
// Note - The WDATA value is transmitted LSB-first on the wire hence representation WDATA[0:31]

	for (i=0,parity=0,DRmask=0x00000001; i<RegisterSize; i++, DRmask <<= 1)
	{
		if ((DPAPCode & DRmask) != 0)
		{	
			SWD_DIO_HIGH;						/* non-zero value indicates bit set */
			parity++;							/* calculate parity */
		}
		else
			SWD_DIO_LOW;

		Toggle_SWD_CLK(1);						/* shift bit into DR */
	}

// Parity A single parity bit for the preceding packet.
// The parity check is made over the APnDP, RnW and A[2:3] bits. If, of these four bits:
// the number of bits set to 1 is odd, then the parity bit is set to 1
// the number of bits set to 1 is even, then the parity bit is set to 0.

	if ((parity & 1) != 0)
		SWD_DIO_HIGH;							/* non-zero value indicates odd bit count, parity bit is high */
	else
		SWD_DIO_LOW;

	Toggle_SWD_CLK(1);							/* next state */

// Stop A single stop bit. In the synchronous SWD protocol this is always 0.

	SWD_DIO_LOW;
	Toggle_SWD_CLK(1);							/* next state */

// Trn By default, there are single-cycle turnaround periods between the first and second of these phases, and after the third phase
// During a Write sequence, there is NO Turnaround after the third phase.

//	Toggle_SWD_CLK(1);							/* next state, turn around */
		
// If a SW-DP is implemented with a synchronous serial interface, the host must continue to clock the interface
// for a number of cycles after the data phase of any data transfer. This ensures that the transfer can be clocked
// through the SW-DP. This means that after the data phase of any transfer the host must do one of the
// following:
//  - immediately start a new SW-DP operation;
//  - continue to clock the SW-DP serial interface until the host starts a new SW-DP operation;
//  - after clocking out the data parity bit, continue to clock the SW-DP serial interface until it has clocked
//    out at least 8 more clock rising edges, before stopping the clock.

	SWD_DIO_LOW;								/* next start bit is high */
	Toggle_SWD_CLK(10);							/* next state */

	WaitMicroSeconds(10000);					/* Default delay 10mS after completing operation */

}
/*******************************************************************************
FUNCTION        : SWDReadDR()
PURPOSE         : performs a JTAG DR Read, returns to idle state.
INPUTS NOTES    : Check defines for DR command names corresponding to various register selects.
RETURNS NOTES   : All read calls must conform to storage array parameters. Pass in pointer to results array.
GENERAL NOTES   : Separate local array records 3-bit acknowledge for DP/AP-ACC.
*******************************************************************************/

void SWDReadReg(UINT8 DPAP, UINT8 DPAPAddr, UINT32 *RdRslt)
{
	int 	i, parity;	
	UINT8 	dut;
	UINT8 	ack;

	int		RegisterSize=32;
	UINT32 	DRmask;

	UINT8	DAP_ACK[SOCKET_COUNT];				/* for DP_ACC/AP_ACC, 1st 3 bits [2:0] returned are ACKnowledge */

//	QSMCM.PQSPAR.B.QPAMISO=0;					/* Use MISO as general purpose input from DUT_MUX */

// Begin read sequence

	parity = 0;									/* need to calculate parity bit for preamble */

// Start Bit - A single start bit, with value 1

	SWD_DIO_HIGH;								/* set SWD_DIO to '1' for start bit */

	Toggle_SWD_CLK(1);							/* next state */
	
// APnDP A single bit, indicating whether the Debug Port or the Access Port Access Register is to be accessed. This bit is 0 for an DPACC access, or 1 for a APACC access.

	switch (DPAP)								/* select register bit in command string based on port accessed */
	{
		case DPACC :							/* */
		{
			SWD_DIO_LOW;						/* set to '0' for DPACC */
			break; 								/* exit case, proceed  */ 
		} 										/* end case DPACC */
		case APACC :							/*  */
		{
			SWD_DIO_HIGH;						/* set to '1' for APACC */
			parity++;							/* increment count of bits set to one for parity calcuation */				
			break;						
		}										/* end case APACC */
	}											/* end of switch */

	Toggle_SWD_CLK(1);							/* next state */
	
// RnW A single bit, indicating whether the access is a read or a write. This bit is 0 for an write access, or 1 for a read access.

	SWD_DIO_HIGH;								/* set to '1' for read */
	parity++;									/* increment count of bits set to one for parity calcuation */				
					
	Toggle_SWD_CLK(1);							/* next state */

// A[2:3] Two bits, giving the A[3:2] address field for the DP or AP Register Address:
// For a DPACC access, the A[3:2] value is the address of the register in the SW-DP
// register map, see SW-DP Register Map on page 6-5.
// For an APACC access, the register being addressed depends on the A[3:2] value and
// the value held in the SELECT register. 
// The A[3:2] value is transmitted Least Significant Bit (LSB) first on the wire.

	DPAPAddr >>= 2;								/* use DRmask to determine SWD_DIO for address field */
	for (i=0,DRmask=0x01; i<2; i++,DRmask<<=1)	/* address field is two bits wide */
	{
		if ((DPAPAddr & DRmask) != 0)
		{
			
			SWD_DIO_HIGH;						/* non-zero value indicates bit is high */
			parity++;							/* increment count of bits set to one for parity calcuation */				
		}
		else
			SWD_DIO_LOW;

		Toggle_SWD_CLK(1);						//* next state */
	}

// Parity A single parity bit for the preceding packet.
// The parity check is made over the APnDP, RnW and A[2:3] bits. If, of these four bits:
// the number of bits set to 1 is odd, then the parity bit is set to 1
// the number of bits set to 1 is even, then the parity bit is set to 0.

	if ((parity & 1) != 0)
		SWD_DIO_HIGH;							/* non-zero value indicates odd bit count, parity bit is high */
	else
		SWD_DIO_LOW;

	Toggle_SWD_CLK(1);							/* next state */

// Stop A single stop bit. In the synchronous SWD protocol this is always 0.

	SWD_DIO_LOW;								/* stop bit drives SWD_DIO low */
	Toggle_SWD_CLK(1);							/* next state */

// Park A single bit. The host must drive the Park bit HIGH to park the line before tristating it for the
// turnaround period. This ensures the line is read as HIGH by the target. This is required as the pull-up
// on the Serial Wire Debug interface is weak. The target reads this bit as 1.

	SWD_DIO_HIGH;								/* park line high for turn-around */
	Toggle_SWD_CLK(1);							/* next state */


// Trn Turnaround. This is a period during which the line is not driven and the state of the line is
// undefined. The length of the turnaround period is controlled by the TURNROUND field in
// the Wire Control Register, see The Wire Control Register, WCR (SW-DP only) on page 6-18.
// The default setting is a turnaround period of one clock cycle.

// !!!!
// To avoid reflections on unpopulated sockets, driver must be held high - causing contention and limiting bank size.
// Knowing which sockets are unpopulated enables them to be excluded during reads - identified in Q-Check.
// This allows polling to be performed with driver tri-stated, and reflections to be removed after reading all sockets.
// The only time we will NOT ! do this is during Q-Check when capturing the QCHKResult where driver termination eliminates ghosting.

	if (RdRslt != QCHKResult)
		SWD_Drive_OFF;							/*  drivers for SWD_DIO off except during Q-Check */

// ACK[0:2] A three-bit target-to-host response.
// Note
// The ACK value is transmitted LSB-first on the wire ( ACK[0:2] ).

	for (ack = 0; ack<3; ack++)					/* ACKnowledge field is three bits wide */
	{
		Toggle_SWD_CLK(1);						/* next bit */
		for (dut=0,DRmask=0x01; dut < DutNumbers[0]; dut++,DRmask<<=1 )		
		{
			DUT_MUX = DutNumbers[dut+1]-1;  	/* ignore non-existing return lines */
			DAP_ACK[dut] = (DUT_INPUT == 1) ? (DAP_ACK[dut] |= DRmask) : (DAP_ACK[dut] &= ~DRmask);
		}
	}

// A successful read operation consists of three phases:
// an eight-bit read packet request, from the host to the target
// a three-bit OK acknowledge response, from the target to the host
// a 33-bit data read phase, where data is transferred from the target to the host.
// By default, there are single-cycle turnaround periods between the first and second of these phases, and after
// the third phase. See the description of Trn in Key to illustrations of operations on page 5-5 for more
// information. However, there is NO turnaround period between the second and third phases, because the line
// is driven by the target in both of these phases.

	for (i=0,DRmask=0x01; i<RegisterSize; i++,DRmask<<=1)
	{
		Toggle_SWD_CLK(1);						/* next bit */
		WaitMicroSeconds(1000);					/* Delay 2mS */
		for (dut = 0; dut < DutNumbers[0]; dut++)		
		{
			DUT_MUX = DutNumbers[dut+1]-1;  	/* ignore non-existing return lines */
			RdRslt[dut] = (DUT_INPUT == 1) ? (RdRslt[dut] |= DRmask) : (RdRslt[dut] &= ~DRmask); /* see note */
		}
	}

// Note:
// Rather than using a relatively complex statement to filter out NRDs (and ghosts), kill ghost post population of read array
// The statement in the loop above would look something like this if they were combined:
//			RdRslt[dut] = (QCHKResult[dut]!=SecurityVal) ? Unpopulated : (DUT_INPUT == 1) ? (RdRslt[dut] |= DRmask) : (RdRslt[dut] &= ~DRmask);
// In this way, the actual polling loop gathers data in a manner consistent with M4/M7 full JTAGc implementations

#ifndef DEBUG

	for (dut = 0; dut < DutNumbers[0]; dut++)		
	{
		if (QCHKResult[dut] != Populated)		/* data captured on 1st read during Q-check is overwritten on 2nd read ... regardless */
			RdRslt[dut] = Unpopulated;			/* eliminates reflected data image due to adjacent sockets */
	}

#endif

// Parity Data transfers (WDATA and RDATA)
// The parity check is made over the 32 data bits, WDATA[0:31] or RDATA[0:31]. If, of these 32 bits:
// the number of bits set to 1 is odd, then the parity bit is set to 1
// the number of bits set to 1 is even, then the parity bit is set to 0.

// For CC-3 data reads, discard the parity bit. 32-bit register value has been captured into array

	Toggle_SWD_CLK(1);							/* next state, turn around */

// Trn By default, there are single-cycle turnaround periods between the first and second of these phases, and after the third phase

	Toggle_SWD_CLK(1);							/* next state, turn around */
		
// If a SW-DP is implemented with a synchronous serial interface, the host must continue to clock the interface
// for a number of cycles after the data phase of any data transfer. This ensures that the transfer can be clocked
// through the SW-DP. This means that after the data phase of any transfer the host must do one of the
// following:
//  - immediately start a new SW-DP operation;
//  - continue to clock the SW-DP serial interface until the host starts a new SW-DP operation;
//  - after clocking out the data parity bit, continue to clock the SW-DP serial interface until it has clocked
//    out at least 8 more clock rising edges, before stopping the clock.

	SWD_DIO_LOW;								/* drive pin low at end of read operation */

// !!!!
// To avoid reflections on unpopulated sockets, driver must be held high - causing contention and limiting bank size.
// Knowing which sockets are unpopulated enables them to be excluded during reads - identified in Q-Check.
// This allows polling to be performed with driver tri-stated, and reflections to be removed after reading all sockets.
// The only time we will NOT ! do this is during Q-Check when capturing the QCHKResult where driver termination eliminates ghosting.

	if (RdRslt != QCHKResult)
		SWD_Drive_ON;							/* drivers for SWD_DIO on - already enabled if running Q-Check */

	Toggle_SWD_CLK(10);							/* next state */

	WaitMicroSeconds(10000);					/* Default delay 10mS after completing operation */

}


/*******************************************************************************
FUNCTION        : SWDGo()
PURPOSE         : Release Core Hold in MDM-AP Control Register to start execution of RAM loaded code.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void SWDGo(void)
{
// Select the MDM-AP to perform system/debug reset, enable CPU hold
	SWDWriteReg(DPACC,DP_SELECT,MDMAP_SEL);		/* Select the MDM-AP to perform system/debug reset, enable CPU hold */

// Clear the Code Hold bit in the MDM-AP COntrol Register to start execution
	SWDWriteReg(APACC,MDM_CR,0x00000000);		/* clear CM4 Core Hold reset */

// CODE IS NOW EXECUTING

// Select the AHB-AP to access RAM data transfer registers (debug access port 0, bank 0) during execution
	SWDWriteReg(DPACC,DP_SELECT,AHBAP_SEL);		/* Select the AHB-AP  */

// Set up data transfer - AddrInc[5:4]=00 for NO autoincrement, Size[2:0]=010 for 32-bit writes
	SWDWriteReg(APACC,AHB_CSW,0x23000002);		/* Selects AHB-AP CSW for APACC access */
}


/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************
FUNCTION        : SWDPatchReset()
PURPOSE         : Enable FLASH Patch and Breakpoint Unit to overwrite reset vectors to start execution of RAM loaded code.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : Call with AHB-AP selected
*******************************************************************************/

/* 
Reset Patch takes advantage of the current flow in that:
 - 1. AHB-AP Bank 0 is currently selected
 - 2. Automatic address increment is enabled
 - 3. 32-bit data transfer is selected
 - 4. APACC is current JTAG instruction
Otherwise setup for RAM Load would need to be duplicated.
*/
/******************************************************************************/
void SWDPatchReset(void)
{

// patch reset vectors to access RAM using VTOR - Target Address defined as Remap_Base in header

// Now enable the FLASH Patch Breakpoint for the two vector fetches - auto-increment still enabled for addressing
// Set Transfer Address Register to point at first register in FPB

	SWDWriteReg(DPACC,DP_SELECT,AHBAP_SEL);		/* Selects AHB-AP TAR for APACC access */

	SWDWriteReg(APACC,AHB_TAR,VTOR);			/* Set target location in TAR for APACC memory access */
	SWDWriteReg(APACC,AHB_DRW,Remap_Base);		/* check status from previous write, capture results */
}

/*******************************************************************************/

/*******************************************************************************
FUNCTION        : BI_SCAN
PURPOSE         : exercise logic using Burn-in Dynamic SCAN Mode
INPUTS NOTES    : module structure passes in control values
RETURNS NOTES   : populates array of captured SCAN output for logging
GENERAL NOTES   : 
*******************************************************************************/
void SWD_BI_SCAN(struct HSVGModule *module, UINT32 *SCANData,UINT32 *SCANMode)
{
	UINT32	EnterTime, DisplayTime, TimeToRun,TimeUsed,TimeRemaining;
	UINT32 *data_ptr;
	UINT32 CaptureEnable=0x0000001C;				/* POINTER 'WORD' OFFSET- clears WRAP bit from 1st block stimulus to enable 2nd */
	UINT32 DataMask;
	UINT16 status;
	int	   dut;

	UINT32 ReadResult[SOCKET_COUNT];				/* used in data capture  */

	EnterTime = ReadSeconds();								/* deduct ramp, RAM Load and message scroll from execution time */

	DisableDutData(0);								/* SCAN data logs - avoid capture during array manipulation */
	DisableDutData(5);								/* Operating Mode - avoid capture during array manipulation */

// Stimulus during exercise is provided by HSVG (staggered clock sequences).
// Set up vector memory prior to calling procedure

	SCAN_HSVG_INIT(module);							/* initialize HSVG, load up vectors for SCAN stimulus */

// BURN-IN SCAN Re-uses the TestKompress EDT decompressor to generate pseudo-random sequences.
// These are clocked throughout the scanned logic to toggle input and output nodes.
// A single "observe" output from one of the compactor outputs allows monitoring of toggle activity.
// Patterns are generated functionally and toggle coverage can be measured using simulation.

/*
	Dynamic Burnin Setup Sequence:
	1. Apply power to all supplies.
	2. Enter Dynamic Burnin mode thru BDM 
 	3. Set scan_se (pta6) to 1
	4. Set edt_update(pta7) to 1
	5. Apply a clock pulse to edt_clk (pta5)
	6. Set edt_update (pta7) to 0
*/
	VddRamp(module->downloadVoltage);				/* ramp down to POR, then up; clocks, all I/P low */

	SWDResetToIdle();								/* reset JTAG port, select Run-Test/Idle state */		
	SWDReadReg(DPACC,DP_IDR, ReadResult);			/* required operation following reset */
	SWDWriteReg(DPACC,DP_CTRLSTAT,0x50000000);		/* sets CSYSPWRUPREQ, CDBGPWRUPREQ */

#ifdef DEBUG
	SWDReadReg(DPACC,DP_CTRLSTAT, ReadResult);		/* check status from previous write, capture results */
#endif

	SWDWriteReg(DPACC,DP_ABORT,0x0000001F);			/* STICKYERR and others on SWD - clear sticky bit with write 1 to bit  */

#ifdef DEBUG
	SWDReadReg(DPACC,DP_CTRLSTAT, ReadResult);		/* check status from previous write, capture results */
#endif

	SWDWriteReg(DPACC,DP_SELECT,MDMAP_SEL);			/* Select the MDM-AP to perform system/debug reset, enable CPU hold */

	if(module->TestModeFlag == 1)					/* If flag set, select functional test mode with voltage override */
	{
		SWDWriteReg(APACC, MDM_TR, BISCAN_DAC);
		Toggle_SWD_CLK(50);							/* wait while voltage stabilizes before applying to bus */
		SWDWriteReg(APACC, MDM_TR, BISCAN_PMC);
		Toggle_SWD_CLK(10);							/* wait while voltage stabilizes before applying to bus */
	}
	else
	{
		SWDWriteReg(APACC, MDM_TR, BISCAN);
		Toggle_SWD_CLK(10);							/* wait while voltage stabilizes before applying to bus */
	}


	SWDReadReg(APACC, MDM_TR, SCANMode);			/* check status from previous write, capture results */
	SWDReadReg(APACC, MDM_TR, SCANMode);			/* data is posted on first read, captured on second  */
	EnableDutData(5,(UINT32)SCANMode, DatarrySize);

	Toggle_SWD_CLK(10);								/* wait while voltage stabilizes before applying to bus */

	if(module->downloadDelay!=0)
		WaitSeconds(module->downloadDelay);
			
	voltage[0]=module->runVoltage[0];
	voltage[1]=module->runVoltage[1];
	voltage[2]=module->runVoltage[2];
	voltage[3]=module->runVoltage[3];

	status = SetVdutsAll((UINT16)voltage[0],(UINT16)voltage[1],(UINT16)voltage[2],(UINT16)voltage[3]);	
	if ((status != PASS)&&(status != VDUT_WARNING))
	{
		VdutsOff ();	
		DisplayToggle ("VDT1", "Fail");
	}

	WaitMicroSeconds(100000);						/* Delay 50mS */

#ifdef DCDC_BUCK

// Buck uses VDUT1 for DCDC_in, VDUT2 for VIO/VIOZ terminations, VDUT4 for GPIO.
// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf2((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf3((UINT16)voltage[3],DUTCODE);		/* Clocks tracks with VDUT4 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT4;						/*  VIO follows VDUT4 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT2;						/* VCLK follows VDUT2 - if powered-off, redeclare to restore voltage */
	}

#else
 #ifdef DCDC_BYPASS

// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf2(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf3(voltage[0],DUTCODE);		/* Clocks tracks with VDUT1 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT1;						/*  VIO follows VDUT1 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT1;						/* VCLK follows VDUT1 - if powered-off, redeclare to restore voltage */
	}

 #else
  #error "Select only one DCDC Converter Mode DCDC_BUCK or DCDC_BYPASS in procedure parameter s19data.h file"
 #endif
#endif

	DisplayChar("SCAN");							/* driver LED display */

	SCAN_CLK_LOW;									/* Dynamic Burnin Setup Sequence */
	EDT_CLK_LOW;										

	EDT_UPDATE_LOW;
	SCAN_SE_LOW;

	SWD_Drive_OFF;									/* at mode entry, disable drivers on SWD_DIO */
	RESET_HIGH;

/*
	Initializing TK Logic and scan chains (to known state for simulation)

	Repeat the following clocking sequence (maximum chain length) times to initialize 
	the decompressor and scan chains. (minimum
	of (maximum chain length) is needed in simulation
	to clear last X's from chains)
	1. Apply clock pulse to all internal clocks on extal, xtal and other
	2. Apply clock pulse to edt_clk ()
	Next, apply the following sequence to initialize the TK Compactor
	to a deterministic starting state:
	1. Set scan_se () to 1
	2. Set edt_update () to 1
	3. Apply a clock pulse to edt_clk ()
	4. Set edt_update () to 0
*/

	Toggle_SWD_CLK(10);								/* insure mode latched across clock boundaries */
	
	SCAN_SE_HIGH;									/* configure deterministic state */
	EDT_UPDATE_HIGH;
	TOGGLE_EDT_CLOCK(1);
	EDT_UPDATE_LOW;
	
	TOGGLE_SCAN_CLOCKS(200);						/* toggles EXTAL/SCAN_CLK and EDT_CLK with non-overlapping transitions */

	SCAN_SE_HIGH;									/* configure deterministic state */
	EDT_UPDATE_HIGH;
	TOGGLE_EDT_CLOCK(1);
	EDT_UPDATE_LOW;
	
	TOGGLE_SCAN_CLOCKS(200);						/* toggles EXTAL/SCAN_CLK and EDT_CLK with non-overlapping transitions */

/*
	Shift Path and PRPG (Pseudo-random pattern gen.) Clocking Sequence:
	1. Apply a clock pulse to all internal clocks on extal, xtal and ptb4 [shift chains once]
	2. Apply a clock pulse to edt_clk (pta5) {updates new ps-random data]
	3. (optional) Observe output pin (ptc2) for chain output activity.
*/

	SetIO ( 4, HSVG, 1, INV_OFF);					/* OUT4/PCLK drives EXTAL/SCAN_CLK  */

	SetIO ( 2, HSVG, 1, INV_OFF);					/* OP.2 drives XTAL/EDT_CLK bank 1  */
	SetIO ( 3, HSVG, 1, INV_OFF);					/* OP.3 drives XTAL/EDT_CLK bank 2  */

	SetIO (16, HSVG, 1, INV_OFF);					/* OP.16 drives SCAN_SE bank 1 */
	SetIO (17, HSVG, 1, INV_OFF);					/* OP.17 drives SCAN_SE bank 2 */

	SetClock(DUT_CLK1,PCLK,0,0);					/* EXTAL (CLK1) uses PCLK/OUT4 as source */
	SetClockOptions(DUT_CLK1,1,DLY0,INV_OFF,OE_ON);	/* SCAN Clk / EXTAL bank 1 */

	SetClock(DUT_CLK2,PCLK,0,0);					/* EXTAL (CLK1) uses PCLK/OUT4 as source */
	SetClockOptions(DUT_CLK2,1,DLY0,INV_OFF,OE_ON);	/* SCAN Clk / EXTAL bank 2 */

	DisplayTime = (ReadSeconds()) - EnterTime; 		/* adjust execution time to avoid drift in duration */
	TimeToRun = module->seconds - DisplayTime;			

//	RunHsvg(module->ExecuteAddress, module->seconds-CapturePortion , REPEAT);	/* ENABLE THE HSVG - SCAN */

// Run stress in segments to allow test for polling request - will terminate procedure from operator interface 
// It is critical that the algorithm insures a non-zero value is always specified for the delay

	TimeUsed = 0;									/* Initialized for use in loop */
	while ( (StartButton != BIN2_CHECK) && (TimeToRun-CapturePortion > TimeUsed) )
	{
		TimeRemaining = module->seconds - TimeUsed;	/* deduct expended time from total */
		if (TimeRemaining > TimeSegment)			/* time remaining is less than the increment */	
		{
			RunHsvg(module->ExecuteAddress, TimeSegment, REPEAT);	/* ENABLE THE HSVG - SCAN / BIST */			
			TimeUsed = TimeUsed + TimeSegment;
		}
		else
		{
			RunHsvg(module->ExecuteAddress, TimeRemaining, REPEAT);	/* ENABLE THE HSVG - SCAN / BIST */			
			TimeUsed = TimeUsed + TimeRemaining;
		}					
	}

/*
	Capture Path Sequence: (extal, xtal and ptb4 clocks d-inputs to Flops, disrupts PRPG)
	1. Set scan_se (pta6) to 0   [change flop inputs from shift to functional cap.]
	2. Apply a clock pulse to all internal clocks on extal, xtal and ptb4
	3. Set scan_se (pta6) to 1 [change from functional cap. back to shift mode]
*/

// Vectors are already initialized in HSVG memory.
// Clear WRAP bit in first block of vectors to allow/include execution of Captures in SCAN stimulus.

	data_ptr = (UINT32 *)(module->ExecuteAddress+CaptureEnable);
	*(data_ptr) = 0x00003000;								

	if(StartButton != BIN2_CHECK)
	RunHsvg(module->ExecuteAddress, CapturePortion, REPEAT); /* ENABLE THE HSVG - SCAN */

/*
	Reset Activity Sequence:
	1. Set RESET to 0  [asynchronous resets (or sets) may be exercised]
	2. Apply clock pulse to all internal clocks on extal, xtal and ptb4
		[activates synch. resets/sets]
	3. (optional) Apply a clock pulse to edt_clk (pta5)

	Example Dynamic Burn-in sequences to get maximal toggle coverage and also
	exercise functional captures/resets:
	1. Apply the Setup Sequence once, followed by the Initialization Sequence.
	2. Repeat the Shift-path sequence for 95% of dynamic burnin time without
	   captures.  This assures the maximal amount of non-repeating
	   pseudo-random data, needed to reach hard-to-toggle logic such as large
	   decoders.
	3. Follow this with the last 5% of dynamic burnin time being a mixed set of 
	   Capture & Shift-path sequences to exercise the d/si input multiplexer of
	  flip-flops.  The Reset sequence can also be mixed into this last segment
	  for exercising asynchronous and synchronous set/reset paths.
*/

// Restore control of driver resources to output commands

	SCAN_CLK_LOW;									/* avoid pulse when restoring clocks to driver control from HSVG */
	EDT_CLK_LOW;

	SetIO ( 4, IO, 1, INV_OFF);						/* OUT4/PCLK drives EXTAL/SCAN_CLK  */

	SetIO ( 2, IO, 1, INV_OFF);						/* OP.2 drives XTAL/EDT_CLK bank 1  */
	SetIO ( 3, IO, 1, INV_OFF);						/* OP.3 drives XTAL/EDT_CLK bank 2  */

	SetIO (16, IO, 1, INV_OFF);						/* OP.16 drives SCAN_SE bank 1 */
	SetIO (17, IO, 1, INV_OFF);						/* OP.17 drives SCAN_SE bank 2 */

	SCAN_SE_HIGH;									/* flushes chains, loads deterministic data */
	EDT_UPDATE_HIGH;
	TOGGLE_EDT_CLOCK(1);
	EDT_UPDATE_LOW;
	TOGGLE_SCAN_CLOCKS(200);

// capture scan output into array, pass to host
	
	for (DataMask = 0x00000001; DataMask != 0; 	DataMask<<=1)
	{
		TOGGLE_SCAN_CLOCKS(1);

		for (dut = 0; dut < SCANNumbers[0]; dut++)		
		{
			DUT_MUX = SCANNumbers[dut+1]-1;  		/* select implemented return lines in DUT socket order */
			SCANData[dut] = (DUT_INPUT == 1) ? (SCANData[dut] | DataMask) : (SCANData[dut] & ~DataMask);
		}
	}

// 	Ghost Buster
	for (dut = 0; dut < SCANNumbers[0]; dut++)
		SCANData[dut] = (QCHKResult[dut] == Populated) ? SCANData[dut] : Unpopulated;	/* fill unpopulated sockets with blank data */

	EnableDutData(0,(UINT32)SCANData,DatarrySize);	/* log SCAN data capture */

	RESET_LOW;										/* end of SCAN exercise */

	OUT0_31=0x00000000;								/* all output drivers low prior to exit */
	SetClocksOff();									/* as well as all clocks terminated low */	

}

/*******************************************************************************/
/*******************************************************************************/
void TOGGLE_EDT_CLOCK(UINT32 numClocks)
{
	int i;
	
	WaitMicroSeconds(5);							/* Delay before continuing */

	for (i = 0; i < numClocks; i++)
	{
		EDT_CLK_HIGH;
		WaitMicroSeconds(5);						/* Delay before continuing */

		EDT_CLK_LOW;
		WaitMicroSeconds(5);						/* Delay, removed to get closer to 50% duty cycle */
	}	
}													/* End of TOGGLE_EDT_CLOCK */

/*******************************************************************************/
/*******************************************************************************/
void TOGGLE_SCAN_CLOCKS(UINT32 numClocks)
{
	int i;
	
	WaitMicroSeconds(5);							/* Delay before continuing */

	for (i = 0; i < numClocks; i++)
	{
		SCAN_CLK_HIGH;
		WaitMicroSeconds(5);						/* Delay before continuing */

		SCAN_CLK_LOW;
		WaitMicroSeconds(5);						/* Delay, removed to get closer to 50% duty cycle */

		EDT_CLK_HIGH;
		WaitMicroSeconds(5);						/* Delay before continuing */

		EDT_CLK_LOW;
		WaitMicroSeconds(5);						/* Delay, removed to get closer to 50% duty cycle */
	}	
}													/* End of TOGGLE_SCAN_CLOCKS */

/*******************************************************************************/
/*******************************************************************************
FUNCTION        : SCAN_HSVG_INIT
PURPOSE         : Loads the HSVG SCAN stimulus to SDRAM
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/
void SCAN_HSVG_INIT(struct HSVGModule *module)
{
//	UINT16 status;
	UINT32 *data_ptr;

// High Speed Comparator setup - function is currently not used

	SA_REGS.B.HSC_RST = 1;							/* Reset the HSC results */
	SA_REGS.B.HSC_RST = 0;							/* Enable the HSC */
	SA_HSC_DISABLE;									/* Turn off the HSC and SA enables */

// HSVG Init - must be initialized by executing a dummy burst before use (due to a CC-3 bug)

	SetClock(VG_CLK,PLL1,module->Clock1Frequency,50); /* pass frequency from header, 50% duty cycle */
	SetClockOptions(VG_CLK,1,DLY0,INV_OFF,OE_ON );

	data_ptr = (UINT32 *)0xFFC00000;
	*data_ptr = 0x00000000;	
	data_ptr = (UINT32 *)0xFFC00004;
	*data_ptr = 0x00000000;	
	data_ptr = (UINT32 *)0xFFC00008;
	*data_ptr = 0x80000000;							/* note MSB (wrap bit) sets termination vector */
	
	RunHsvg (0xFFC00000, 1, ONE_TIME); 				/* 1s timeout */

// HSVG Init Done - now initialize pattern data for SCAN stimulus

// The data for the multiphase clocks is written into the vector generator buffer at the desired location

	data_ptr = (UINT32 *)module->ExecuteAddress;

// STD Stimulus uses HSVG. Driver assignment vector designation
//
// MSB                .0011122
//                    .2604826
//                    ////////
//                    .0011122
//                    .3715937
//                    ////////
//                    00011222
//                    04826048
//                    ////////
//                    00011222
// LSB                15937159

	*(data_ptr++) = 0x02003000;						/* SCAN clock pulse on EXTAL uses OP.4 */
	*(data_ptr++) = 0x00003000;						/* SCAN SE 16/17 */
	*(data_ptr++) = 0x0C003000;						/* EDT_CLK clock pulse on XTAL uses OP.2/3 */
	*(data_ptr++) = 0x00003000;	
	*(data_ptr++) = 0x02003000;	
	*(data_ptr++) = 0x00003000;	
	*(data_ptr++) = 0x0C003000;	
	*(data_ptr++) = 0x80003000;						/* initial termination will be updated in main flow */

	*(data_ptr++) = 0x40802710;						/* clock DUT 20,000 cycles then capture. repeat. */

	*(data_ptr++) = 0x02003000;	
	*(data_ptr++) = 0x00003000;	
	*(data_ptr++) = 0x0C003000;	
	*(data_ptr++) = 0x00003000;	
	*(data_ptr++) = 0x02003000;	
	*(data_ptr++) = 0x00003000;	
	*(data_ptr++) = 0x0C003000;	
	*(data_ptr++) = 0x00003000;

	*(data_ptr++) = 0x00000000;	
	*(data_ptr++) = 0x02000000;						/* SE low to execute Capture, uses OP.16/17 */
	*(data_ptr++) = 0x00000000;	
	*(data_ptr++) = 0x80003000;						/* SE high to execute Shift, uses OP.16/17 */


// HSVG SCAN Init Done: module.ExecuteAddress

}

/*******************************************************************************
FUNCTION        : SWDRAMBIST - Invoke BI RAM BIST and apply stimulus
PURPOSE         : Execute stress in DFT mode using HSVG for Invoke and stimulus
INPUTS NOTES    : Structure passes in VDUT, VGCLK frequency, PCLK frequency.
                : Invoke vectors are always mapped to memory buffer at 0xFFC0_0000
                : 
                : Structure passes in duration for exercise. Redundancy is ignored.                  
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void SWD_RAMBIST(struct HSVGModule *module, UINT32 *BISTData, UINT32 *BISTMode)
{
	UINT32 EnterTime, DisplayTime, TimeToRun;
	int i = 0;										/* index into s-record string array */
	UINT8 status;

	UINT32 ReadResult[SOCKET_COUNT];				/*  */

	DisableDutData(5);								/* avoid data capture during array manipulation. host syns DutData0 */

	EnterTime = ReadSeconds();						/* deduct ramp, RAM Load and message scroll from execution time */

	VddRamp(module->downloadVoltage);				/* ramp down to POR, then up; clocks, all I/P low */

	SetClock(DUT_CLK1,PLL1,module->Clock1Frequency,0); /* EXTAL provides clock for BIST Mode */	
	SetClock(DUT_CLK2,CK1FB,0,0);					/* EXTAL for DUT Bank 2  */	

	SWDResetToIdle();								/* reset JTAG port, select Run-Test/Idle state */		
	SWDReadReg(DPACC,DP_IDR, ReadResult);			/* required operation following reset */
	SWDWriteReg(DPACC,DP_CTRLSTAT,0x50000000);		/* sets CSYSPWRUPREQ, CDBGPWRUPREQ */
	SWDReadReg(DPACC,DP_CTRLSTAT, ReadResult);		/* check status from previous write, capture results */
	SWDWriteReg(DPACC,DP_ABORT,0x0000001F);			/* STICKYERR and others on SWD - clear sticky bit with write 1 to bit  */
	SWDWriteReg(DPACC,DP_SELECT,MDMAP_SEL);			/* Select the MDM-AP to perform system/debug reset, enable CPU hold */

	if(module->TestModeFlag == 1)					/* If flag set, select functional test mode with voltage override */
	{
		Toggle_SWD_CLK(50);							/* wait while voltage stabilizes before applying to bus */
		SWDWriteReg(APACC,MDM_TR,BIBIST_DAC);		
		Toggle_SWD_CLK(50);							/* wait while voltage stabilizes before applying to bus */
		SWDWriteReg(APACC,MDM_TR,BIBIST_PMC);		
	}
	else
	{
		SWDWriteReg(APACC,MDM_TR,BIBIST);		
	}
	Toggle_SWD_CLK(50);								/* propagate final mode selection through logic */
	
	SWDReadReg(APACC,MDM_TR,BISTMode);				/* check status from previous write, capture results */
	SWDReadReg(APACC,MDM_TR,BISTMode);				/* data is posted on first read, captured on second  */

	EnableDutData(5,(UINT32)BISTMode,DatarrySize); 	/* pass mode confirmation to log file */
	EnableDutData(9,(UINT32)BISTData,DatarrySize); 	/* implemented as dummy data, not used, not logged (provided for CTI solution) */

	voltage[0]=module->runVoltage[0];
	voltage[1]=module->runVoltage[1];
	voltage[2]=module->runVoltage[2];
	voltage[3]=module->runVoltage[3];

	status = SetVdutsAll((UINT16)voltage[0],(UINT16)voltage[1],(UINT16)voltage[2],(UINT16)voltage[3]);	
	if ((status != PASS)&&(status != VDUT_WARNING))
	{
		VdutsOff ();	
		DisplayToggle ("VDT1", "Fail");
	}

	WaitMicroSeconds(100000);						/* Delay */

#ifdef DCDC_BUCK

// Buck uses VDUT1 for DCDC_in, VDUT2 for VIO/VIOZ terminations, VDUT4 for GPIO.
// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf2((UINT16)voltage[3],DUTCODE);		/* Driver tracks with VDUT4 */
		SetPsBuf3((UINT16)voltage[3],DUTCODE);		/* Clocks tracks with VDUT4 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT4;						/*  VIO follows VDUT4 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT2;						/* VCLK follows VDUT2 - if powered-off, redeclare to restore voltage */
	}

#else
 #ifdef DCDC_BYPASS

// For CC3/E compatibility, don't declare buffer output levels until power supplies in normal operating range

	if(DRIVER_TYPE == CC3E)
	{
		SetPsBuf1(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf2(voltage[0],DUTCODE);		/* Driver tracks with VDUT1 */
		SetPsBuf3(voltage[0],DUTCODE);		/* Clocks tracks with VDUT1 */
	}
	else if(DRIVER_TYPE == CC3)
	{
		SET_VIO_VDUT1;						/*  VIO follows VDUT1 - if powered-off, redeclare to restore voltage */
		SET_VCLK_VDUT1;						/* VCLK follows VDUT1 - if powered-off, redeclare to restore voltage */
	}

 #else
  #error "Select only one DCDC Converter Mode DCDC_BUCK or DCDC_BYPASS in procedure parameter s19data.h file"
 #endif
#endif

	RESET_HIGH;										/* enter BIST Mode on rising edge */

	BIST_INVOKE_HIGH;								/* begin execution */
	BIST_RESUME_HIGH;								/* continue following DR delay, loop continuously */

	DisplayScroll(FAST,module->s19FileName);		/* message on LED display */
	DisplayTime = (ReadSeconds()) - EnterTime;		/* adjust execution time to avoid drift in duration */

//	WaitSeconds(module->seconds - DisplayTime);

	TimeToRun = module->seconds - DisplayTime;			
	PolledDelay(TimeToRun);							/* fragment delay to check for Bin2 request */

	RESET_LOW;										/* after stress, assert reset */

	OUT0_31=0x00000000;								/* all output drivers low prior to exit */
	SetClocksOff();									/* as well as all clocks terminated low */
}

/**********************************************************************************/
/**********************************************************************************/
short sqrt(short num) 
{
	short op = num;					/* operand, the number whose square root is being calculated         */
	short res = 0;					/* cumulative results storage, pieced together a bit at a time       */
	short one = 1 << 14; 			/* the second-to-top bit is set: 1L<<30 for long                     */
       								/* "one" starts at the highest power of four <= the argument.        */
	while (one > op)			
		one >>= 2;					/* shift(divide) by 2^2 until results is a number less than 'op'     */
 
	while (one != 0)
	{								/* user 'one' as test value for successive [2^2]^n places            */
		if (op >= res + one)
		{							/* if sum of remaining lower bits + test bit does not overflow       */
			op -= (res + one);		/*  at a [2^2]^n boundary, remove result bit from op(2^(n-1), add    */
			res = (res >> 1) + one;	/*  op(2^n). Then divide cumulative result by 2; add new MSB.        */
		}
		else
			res >>= 1;				/* shift existing bits set by test bit to divide by 2 (sqrt(4))      */

		one >>= 2;					/* and always shift test value to right 2^2 after each MSB bit found */
	}
	return res;						/* pass result to calling argument                                   */
}

/**********************************************************************************/
/**********************************************************************************/


