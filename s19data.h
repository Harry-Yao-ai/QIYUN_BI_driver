/***************************************************************************
                                            COPYRIGHT (c) FREESCALE 2014 
 FILE NAME: s19data.h                                              

 UPDATE HISTORY                                                         
  REV      AUTHOR      DATE       DESCRIPTION OF CHANGE                  
 ---   -----------  ---------    ---------------------                  
 1.0   M.LaCroix    28Mar2014    Combine build targets into single header file using __ide_target("<target>") syntax.

Instructions - This file can be included with all targets in the project.
It will define header file for associated procedure to be used in build.
At present RCS File Header is imbedded in the revision.h file located in the target directory. This is to provide a reminder to
always update your revision history when introducing a new version of the procedure.

To introduce new/additional targets in project, append to existing list.
 DEBUG 			- Generate user code in SDRAM for debugger. First run FPGA code to program the FPGA.
 SA     		- Run user code in FLASH. No IBE conneceion. OS must also be present in FLASH.
 IBE 			- Generate DUT code for SDRAM which will be downloaded by IBE. OS must be present in FLASH.

 Add your DUT specific targets using similar code structure, along with include for parameter header file.

***********************************************************************************/

#define 	NVM_10K_Passcode 0xE0002710		/* non-Flex */
//#define 	NVM_10K_Passcode 0xF0004E20		/* Flex?? sometimes */
#define 	NVM_1K_Passcode  0xE00003E8
//#define 	NVM_1K_Passcode  0xF00003E1
#define 	NVM_40K_Passcode 0xF0009C40		
#define		NVM_CLR_Passcode 0xFFFFFFFF		/* CLEAR_NVM_SECURITY switch erases array, including status words */
#define		HTOL_12hrs		 0x0000000C
#define		HTOL_168hrs		 0x000000A8
#define 	HTOL_Passcode	 HTOL_168hrs	/* Security word reads from FLASH Option Register
	 
/* Targets IBE / STANDALONE / DEBUG are not normally used when generating production diag files. */

/* DEBUG */
                              
#if __ide_target("DEBUG") 
//#	define TARGET_NVM				/* TARGET_NVM or TARGET_HTOL driver */
#define		TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
//#	define DCDC_BUCK				/* specify power supply resources and Vdd ramp */
//#	include "..\parameterfiles\kw35a_blinkfee_bk__07apr2017.h"

#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\QINYUN_burnin_168hrs_27Jul2021.h"

#define HTOL_Status (UINT32)HTOL_Passcode
#define NVM_Passcode (UINT32)NVM_1K_Passcode

/* STANDALONE */

#elif __ide_target("STANDALONE")
 #include "..\parameterfiles\kw40_htoldebug_28mar2014.h"

#define HTOL_Status (UINT32)HTOL_Passcode

/* IBE */

#elif __ide_target("IBE")
#	define TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\QINYUN_burnin_168hrs_27Jul2021.h"

#define HTOL_Status (UINT32)HTOL_Passcode

/* Development */

#elif __ide_target("Development_BK")
#	define TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BUCK				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw40_512_htol1008_4p5_bk_09may2018.h"

#define HTOL_Status (UINT32)HTOL_Passcode

#elif __ide_target("Development_BP")
#	define TARGET_NVM 				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_nvm100x1kc_13jun2017.h"

#define NVM_Passcode (UINT32)NVM_1K_Passcode

/* HTOL_1008 */

#elif __ide_target("HTOL_1008_BK")
#	define TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BUCK				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_htol1008_bk_23apr2018.h"

#define HTOL_Status (UINT32)HTOL_Passcode

#elif __ide_target("HTOL_1008_BP")
#	define TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_htol1008_bp_23apr2018.h"

#define HTOL_Status (UINT32)HTOL_Passcode

#elif __ide_target("KW37_HTOL_1008_BP")
#	define TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw37_htol1008_bp_22jun2018.h"

#define HTOL_Status (UINT32)HTOL_Passcode

/* ELFR_24 */

#elif __ide_target("ELFR_24_BK")
#	define TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BUCK				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_elfr24_bk_23apr2018.h"

#define HTOL_Status (UINT32)HTOL_Passcode

#elif __ide_target("ELFR_24_BP")
#	define TARGET_HTOL				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_elfr24_bp_24apr2018.h"

#define HTOL_Status (UINT32)HTOL_Passcode

/* NVM_1K_Cycle */

#elif __ide_target("NVM_1K_Cycle")
#	define TARGET_NVM 				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_nvm100x1kbp33_23apr2018.h"

#define NVM_Passcode (UINT32)NVM_1K_Passcode

/* NVM_10K_Cycle */

#elif __ide_target("NVM_10K_Cycle_3V3")
#	define TARGET_NVM 				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_nvm100x10kbp33_23apr2018.h"

#define NVM_Passcode (UINT32)NVM_10K_Passcode

#elif __ide_target("NVM_10K_Cycle_1V7")
#	define TARGET_NVM 				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_nvm100x10kbp17_23apr2018.h"

#define NVM_Passcode (UINT32)NVM_10K_Passcode

#elif __ide_target("NVM_10K_Cycle_1V71")
#	define TARGET_NVM 				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_nvm100x10kbp171_15feb2018.h"

#define NVM_Passcode (UINT32)NVM_10K_Passcode

/* NVM_40K_Cycle */

#elif __ide_target("NVM_40K_Cycle_1V7")
#	define TARGET_NVM 				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	include "..\parameterfiles\kw35a_nvm100x40kbp17_23apr2018.h"

#define NVM_Passcode (UINT32)NVM_40K_Passcode

/* NVM_Clear - define as NVM target to use correct Bin2 with Erase Verify switch */

#elif __ide_target("NVM_Clear")
#	define TARGET_NVM 				/* TARGET_NVM or TARGET_HTOL driver */
#	define DCDC_BYPASS				/* specify power supply resources and Vdd ramp */
#	define CLEAR_NVM_SECURITY
#	include "..\parameterfiles\kw35a_blinkfee_bp__11apr2017.h"

#define NVM_Passcode  (UINT32)NVM_CLR_Passcode

#else
	#error Target Not Recognized in prefixfile.h
#endif
