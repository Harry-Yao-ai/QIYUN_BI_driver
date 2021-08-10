

// JTAGWRiteIR(IRCode) values

#define IRSize			4

#define TestCtrl		0x06

// JTAG ID Register (JTAG_ID) definitions

#define JTAGID			0x00
#define JTAGIDSize		32

// JTAG Debug Port (DPACC) definitions

#define DPACC			0x0A
#define DPACCSize		32

#define DP_ABORT		0x00
#define DP_IDR			0x00

#define DP_CTRLSTAT 	0x04

#define DP_SELECT 		0x08
#define MDMAP_SEL 		0x01000000
#define AHBAP_SEL 		0x00000000

#define DP_RDBUFF 		0x0C

#define IDCODE			0x0E

#define BYPASS			0x0F
#define BYPASSSize 		1

// JTAG Access Port (APACC) definitions

#define APACC			0x0B
#define APACCSize		32

// Access Port 0 - AHB-AP
#define AHB_CSW			0x00					/* AHB-AP Control/Status Register */
#define AHB_TAR			0x04					/* AHB-AP Target Address Register */
#define AHB_DRW			0x0C					/* AHB-AP Data Read/Write Register */

// Access Port 1 - MDM-AP
#define MDM_SR			0x00					/* MDM-AP Status         Register */
#define MDM_CR			0x04					/* MDM-AP Control        Register */
	
#define MDM_TR			0x08					/* MDM-AP Test           Register */
#define MDM_IDR			0x0C					/* MDM-AP Identification Register */

// FLASH Patch and Breakpoint Unit (FPB)
// Pioneer 2 supports 6 instruction address comparators (NUM_CODE) and 2 literal address comparators (NUM_LIT).
// Remapping must be aligned to (NUM_CODE + NUM_LIT) words: 6 + 2 = 8 words, 32 bytes (0x20).
// IAR places vectors at start of RAM_H, SP initialized to 0x2000FFF8 - use same in burn-in to leverage development.
// Execution start address PC is passed in from data structure.

#define FP_CTRL     	0xE0002000		// FlashPatch Control Register
#define FP_Enable   	0x00000003		// Write this value to enable remapping

#define FP_REMAP    	0xE0002004		// FlashPatch Remap register
//#define Remap_Base  	0x20000000		/* when performing ramload/execute, re-map vectors to this address */
#define Remap_Base  	0x1FFFCBF2		/* when performing ramload/execute, re-map vectors to this address */
//#define PC_loc			0x1fffd61c		/* the location you want to run the code from */
//#define PC_loc			0x1fffd1b4		/* the location you want to run the code from */ /* toggle PTB2 */
//#define PC_loc			0x1fffd1ec		/* the location you want to run the code from */	/* disable ECC */ //final
#define PC_loc				0x1fffde65
//#define PC_loc			0x1fffd1f0		/* the location you want to run the code from */	/* add ers firstly */
#define Stack_loc		0x20000800		/* the location to store stack */

#define FP_COMP0    	0xE0002008		// FlashPatch Comparator register
#define FP_COMP0_En 	0x00000001		// Stack Pointer
#define StackPatch  	0x2000FFF8 		/* initial location of stack point for RAM loads */

#define FP_COMP1    	0xE000200C		// FlashPatch Comparator register
#define FP_COMP1_En 	0x00000005		// Reset Vector

#define	VTOR			0xE000ED08 		/* Vector Table Offset Register */
/*******************************************************************************/
/* FUNCTION PROTOTYPES                                                         */ 
/*******************************************************************************/

/*******************************************************************************/
/*******************************************************************************/


/*******************************************************************************
FUNCTION        : DownloadS19
PURPOSE         : Copy s19 stored in data structure to DUT RAM for RL&E.
INPUTS NOTES    : structure, module
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

int JTAGDownloadS19(struct burnInModule *module);
int SWDDownloadS19(struct burnInModule *module);


/*******************************************************************************
FUNCTION        : Toggle_SWD_CLK
PURPOSE         : Toggles SWD_CLK the number of times specified.
INPUTS NOTES    : numClocks - number of SWD_CLK clock pulses
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void Toggle_SWD_CLK (UINT32 numClocks);

/*******************************************************************************
FUNCTION        : SWDResetToIdle()
PURPOSE         : performs a JTAG Reset and initialization of state machine
INPUTS NOTES    : 
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void SWDResetToIdle(void);

/*******************************************************************************
FUNCTION        : JTAG_AHB_Init
PURPOSE         : performs Core Hold out of Reset, initialize AHB-AP for memory read/write.
INPUTS NOTES    : Default setup selects no auto-increment in CSW register.
			
RETURNS NOTES   : void
GENERAL NOTES   : argument allows special handling during events (ex: nvm clr during Q-Check)
*******************************************************************************/

void SWD_AHB_Init(int Button);

/*******************************************************************************
FUNCTION        : SWD_Qcheck
PURPOSE         : performs a generic socket check on a toggling pin using AHB-AP memory access.
INPUTS NOTES    : 
			sockets - pointer to array where a list of the dut return lines
			that are used.  sockets[0] = total number of sockets and the rest
			contain the return lines.
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void SWD_Qcheck(void);
void SWD_Qcheck_DBG(void);

/*******************************************************************************
FUNCTION        : SWDReadReg()
PURPOSE         : performs a JTAG DR Read, returns to idle state.
INPUTS NOTES    : Check defines for DR command names corresponding to various register selects.
RETURNS NOTES   : All read calls must conform to storage array parameters. Pass in pointer to results array.
GENERAL NOTES   : Separate local array records 3-bit acknowledge for DP/AP-ACC.
*******************************************************************************/

void SWDReadReg(UINT8 DPAP, UINT8 DPAPAddr, UINT32 *RdRslt);

/*******************************************************************************
FUNCTION        : SWDReadReg()
PURPOSE         : performs a JTAG DR Read, returns to idle state.
INPUTS NOTES    : Check defines for DR command names corresponding to various register selects.
RETURNS NOTES   : All read calls must conform to storage array parameters. Pass in pointer to results array.
GENERAL NOTES   : Separate local array records 3-bit acknowledge for DP/AP-ACC.
*******************************************************************************/

void SWDWriteReg(UINT8 DPAP, UINT8 DPAPAddr, UINT32 DPAPCode);

/*******************************************************************************
FUNCTION        : ToggleTCLK
PURPOSE         : Toggles TCLK the number of times specified.
INPUTS NOTES    : numClocks - number of TCLK clock pulses
RETURNS NOTES   : void
GENERAL NOTES   : Based on CC2 TOG_TCLK.
*******************************************************************************/

void ToggleTCLK (UINT32 numClocks);

/*******************************************************************************
FUNCTION        : ToggleSCANClk
PURPOSE         : Toggles all scan clock drivers the number of times specified.
INPUTS NOTES    : numClocks - number of SCAN clock pulses
RETURNS NOTES   : void
GENERAL NOTES   : Based on CC2 TOG_TCLK.
*******************************************************************************/

void ToggleSCANClk(UINT32 numClocks);

/*******************************************************************************
FUNCTION        : JTAGQcheck
PURPOSE         : performs a generic socket check on a toggling pin using JTAG Bypass Register.
INPUTS NOTES    : 
			sockets - pointer to array where a list of the dut return lines
			that are used.  sockets[0] = total number of sockets and the rest
			contain the return lines.
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void JTAGQcheck(void);
void JTAGQcheck_DBG(void);

/*******************************************************************************
FUNCTION        : JTAGNVMBin2
PURPOSE         : performs Q-Check using JTAG Bypass followed by read of NVM status, formats polling results for host.
INPUTS NOTES    : 
			sockets - pointer to array where a list of the dut return lines
			that are used.  sockets[0] = total number of sockets and the rest
			contain the return lines.
RETURNS NOTES   : prepares data for driver which passes final results to host - Bin1/2/3
GENERAL NOTES   : 
*******************************************************************************/

//void JTAGBin2(struct burnInModule module, UINT8 *sockets);
void JTAGNVMBin2(void);
//void SWDNVMBin2(void);

/*******************************************************************************
FUNCTION        : JTAGHTOLBin2
PURPOSE         : performs simple Q-Check using JTAG bypass followed by read of protection/security, formats polling results for host.
INPUTS NOTES    : 
			sockets - pointer to array where a list of the dut return lines
			that are used.  sockets[0] = total number of sockets and the rest
			contain the return lines.
RETURNS NOTES   : prepares data for driver which passes final results to host - Bin1 / Bin3 indicates protected or secure
GENERAL NOTES   : 
*******************************************************************************/

//void JTAGBin2(struct burnInModule module, UINT8 *sockets);
void JTAGHTOLBin2(void);
//void SWDHTOLBin2(void);

/*******************************************************************************
FUNCTION        : SWDBin2
PURPOSE         : performs simple Q-Check using JTAG bypass followed by read of protection/security, formats polling results for host.
INPUTS NOTES    : 
			sockets - pointer to array where a list of the dut return lines
			that are used.  sockets[0] = total number of sockets and the rest
			contain the return lines.
RETURNS NOTES   : prepares data for driver which passes final results to host - Bin1 /Bin2 / Bin3 indicates protected or secure
GENERAL NOTES   : 
*******************************************************************************/

//void JTAGBin2(struct burnInModule module, UINT8 *sockets);
//void JTAGHTOLBin2(void);
void SWDHTOLBin2(void);
void SWDNVMBin2(void);
void SWDBin2(void);

/*******************************************************************************
FUNCTION        : JTAGResetToIdle()
PURPOSE         : performs a JTAG Reset and selects the Run - Test/Idle TAP State
INPUTS NOTES    : 
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void JTAGResetToIdle(void);

/*******************************************************************************
FUNCTION        : JTAGWriteIR()
PURPOSE         : performs a JTAG IR Write/Update, returns to the Run - Test/Idle TAP State.
INPUTS NOTES    : Check defines for IR command names corresponding to various register selects.
                  All commands are 4 binary bits, passed to functions as 4 LSB in argument.
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void JTAGWriteIR(UINT8 IRCode);

/*******************************************************************************
FUNCTION        : JTAGWriteDR()
PURPOSE         : performs a JTAG DR Write/Update, returns to the Run - Test/Idle TAP State.
INPUTS NOTES    : Check defines for DR command names corresponding to various register selects.
                  Register lengths vary, upper bits ignored where register size exceeded.
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void JTAGWriteDR(UINT8 TAPRegister, UINT8 DAPAddr, UINT32 DRCode);

/*******************************************************************************
FUNCTION        : JTAGReadDR()
PURPOSE         : performs a JTAG DR Read, returns to the Run - Test/Idle TAP State.
INPUTS NOTES    : Check defines for DR command names corresponding to various register selects.
                  Register lengths vary, upper bits ignored where register size exceeded.
RETURNS NOTES   : All read calls must conform to storage array parameters. Pass in pointer to results array.
GENERAL NOTES   : Separate local array records 3-bit acknowledge for DP/AP-ACC (not much you can do about it ...).
*******************************************************************************/

void JTAGReadDR(UINT8 TAPRegister, UINT8 DAPAddr, UINT32 *RdRslt);

/*******************************************************************************
FUNCTION        : JTAGWriteTestReg()
PURPOSE         : performs a JTAG Test Register Write/Update, returns to the Run - Test/Idle TAP State.
INPUTS NOTES    : Check defines for Test Register size and implementation, differs from part to part.
                : Function is separated from other JTAG register writes for portability (ease of editing).
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void JTAGWriteTestReg(UINT32 RegisterValue, UINT32 ReadSwitch,  UINT32 *RdRslt);

/*******************************************************************************
FUNCTION        : JTAGPatchReset()
PURPOSE         : Enable FLASH Patch and Breakpoint Unit to overwrite reset vectors to start execution of RAM loaded code.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void JTAGPatchReset(void);

/*******************************************************************************
FUNCTION        : SWDPatchReset()
PURPOSE         : Enable FLASH Patch and Breakpoint Unit to overwrite reset vectors to start execution of RAM loaded code.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void SWDPatchReset(void);

/*******************************************************************************
FUNCTION        : InterimCoreHold()
PURPOSE         : Assert Core Hold in MDM-AP Control Register and release Resetb for memory access.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : Call only while executing in functional Test or User Modes - no POR
*******************************************************************************/

void InterimCoreHold(UINT8 TestMode);
void SWDInterimCoreHold(UINT8 TestMode);

void SWD_AHB_CFG(int Button, UINT8 TestMode);
/*******************************************************************************
FUNCTION        : InterimRead()
PURPOSE         : Assert Read memory into array, pass to log file using DUT data.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : Call only while executing in functional Test or User Modes - no POR
*******************************************************************************/

void InterimRead(UINT32 *InterimArray, int InterimDUTData, UINT32 InterimSize, UINT32 InterimLocation);
void SWDInterimRead(UINT32 *InterimArray, int InterimDUTData, UINT32 InterimSize, UINT32 InterimLocation);

/*******************************************************************************
FUNCTION        : JTAGGo()
PURPOSE         : Release Core Hold in MDM-AP Control Register to start execution of RAM loaded code.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void JTAGGo(void);

/*******************************************************************************
FUNCTION        : SWDGo()
PURPOSE         : Release Core Hold in MDM-AP Control Register to start execution of RAM loaded code.
INPUTS NOTES    : void
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void SWDGo(void);

/*******************************************************************************
FUNCTION        : Scan_Random_Stimulus()
PURPOSE         : Use PRNG to clock random data into SCAN chain. 
INPUTS NOTES    : Duration passed as argument from DUT header
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/

void Scan_Random_Stimulus(UINT32 time_in_second);

void seed_random(UINT32 seed);
UINT32 get_and_iterate(void);

/*******************************************************************************/
/*******************************************************************************/


/*******************************************************************************
FUNCTION        : JTAGRAMBIST - Invoke BI RAM BIST and apply stimulus
PURPOSE         : Execute stress in DFT mode using HSVG for Invoke and stimulus
INPUTS NOTES    : Structure passes in VDUT, VGCLK frequency, PCLK frequency.
                : Invoke vectors are always mapped to memory buffer at 0xFFC0_0000
                : 
                : Structure passes in duration for exercise. Redundancy is ignored.                  
RETURNS NOTES   : void
GENERAL NOTES   : 
*******************************************************************************/
void JTAGRAMBIST(struct HSVGModule *module, UINT32 *BISTData, UINT32 *BISTMode);

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
void SWD_RAMBIST(struct HSVGModule *module, UINT32 *BISTData, UINT32 *BISTMode);


/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************
FUNCTION        : Debug_CPU_Hold()
PURPOSE         : Confirm sequence for code hold/release by watching LEDs.
INPUTS NOTES    : 
RETURNS NOTES   : void
GENERAL NOTES   : Unit FLASH must be programmed with LED toggle routine that
			will execute automatically upon device reset. 
			If hold is successful, LED will not toggle until hold cleared.
			Driver LED display will expain expectations.
*******************************************************************************/
void	Debug_CPU_Hold(void);

/*******************************************************************************
FUNCTION        : Debug_AHBAP()
PURPOSE         : Confirm sequence for writing/reading memory.
INPUTS NOTES    : 
RETURNS NOTES   : void
GENERAL NOTES   : Capture data into array using register read function
*******************************************************************************/
void	Debug_AHBAP(struct HSVGModule module);

/*******************************************************************************
FUNCTION        : Debug_FPBU()
PURPOSE         : Confirm sequence for patching reset vectors using FLASH Patch & Breakpoint Unit.
INPUTS NOTES    : 
RETURNS NOTES   : void
GENERAL NOTES   : Default SP defined, reset vectors extracted from data header.
*******************************************************************************/
void	Debug_FPBU(UINT32 ResetVector);

/*******************************************************************************
FUNCTION        : PolledDelay()
PURPOSE         : Fragment stress delays into increments to check for Bin2polling request.
INPUTS NOTES    : 
RETURNS NOTES   : void
GENERAL NOTES   : Bin2 request from host will result in termination of stress.
*******************************************************************************/
void 	PolledDelay(UINT32 DelayTime);

void BI_SCAN(struct HSVGModule *module, UINT32 *SCANData,UINT32 *SCANMode);
void SWD_BI_SCAN(struct HSVGModule *module, UINT32 *SCANData,UINT32 *SCANMode);

void RunModules(UINT32 *Datarry0, UINT32 *Datarry5);
void SWDRunModules(UINT32 *Datarry0, UINT32 *Datarry5);

void StartModule(struct burnInModule *module);
void SWDStartModule(struct burnInModule *module);

