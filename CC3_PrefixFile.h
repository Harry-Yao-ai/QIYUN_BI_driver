/***************************************************************************
                                            COPYRIGHT (c) MOTOROLA 2002 
 FILE NAME: CC3_PrefixFile                                              

 UPDATE HISTORY                                                         
  REV      AUTHOR      DATE       DESCRIPTION OF CHANGE                  
 ---   -----------  ---------    ---------------------                  
 1.0   M. Noel    Sep 23, 2002    This generic prefix file caters for all usual CC3 build types.
 1.1   A. Dobbin  Dec 11, 2002    Reduced modes to DEBUG, SA and IBE.
 1.2   D. Laing	  Oct 16, 2007    Added conditionals to auto-select correct target based on CodeWarrior IDE drop-down settings.
 1.3   M. LaCroix Sep 13, 2013    Modified conditionals to auto-select when project contains multiple 'IBE' targets based on CodeWarrior IDE drop-down settings.

Instructions - This prefix file can be included with all targets in the project
It will define directives for the compiler based on the selection.  This is based on the makefile
originally written by Allan Dobbin.  The Makefile includes selection of the files to include in the build, however
with MW this is done though the GUI.  This prefix file should bne included in all project files with directives

Before revision 1.2, users had to manually change the first #define line to be either DEBUG, SA, or IBE. 
Revision 1.2 gets this information directly from the CodeWarrior IDE, so this file should not need to be changed as much.
If a new build target is added to the CodeWarrior drop-down, or a different compiler is used,
this file will need to be changed or else it will reach the #error case and halt the build.

Before revision 1.3, the CC3 project only supported 3 possible targets. The change to the prefix file allows the introduction of multiple targets
in a single project, all of which are defined as selecting the 'IBE' environment. Strategy introduced for Kinetis platform.

 Choose one of the following selections as the build type.

 DEBUG 			- Generate user code in SDRAM for debugger. First run FPGA code to program the FPGA.
 SA     		- Run user code in FLASH. No IBE conneceion. OS must also be present in FLASH.
 IBE 			- Generate DUT code for SDRAM which will be downloaded by IBE. OS must be present in FLASH.

***********************************************************************************/
//#define IBE

// This will not work if the user has defined multiple unique targets in the IDE - modify to employ desired strategy.

//#if __ide_target("DEBUG") 
//	#define DEBUG
//#elif __ide_target("STANDALONE")
//	#define SA
//#elif __ide_target("IBE")
//	#define IBE
//#else
//	#error Target Not Recognized in prefixfile.h
//#endif

#if __ide_target("DEBUG") 
	#define DEBUG
#elif __ide_target("STANDALONE")
	#define SA
#else
	#define IBE
//	#warning Target environment IBE automatically selected in cc3_prefixfile.h
#endif

//#ifndef IBE
//#ifndef DEBUG
//#ifndef STANDALONE
//#error "No target selected in cc3_prefixfile.h"
//#endif
//#endif
//#endif

#ifndef _PREFIX_H
#define _PREFIX_H

#ifdef OS
#define FLASH
#define CC3_SOURCE
#define IS_OS
#endif


#ifdef SA
#define FLASH
#define CC3_TABLE
#define NOT_OS
#endif

#ifdef IBE
#define SDRAM_IBE
#define SDRAM
#define CC3_TABLE
#define NOT_OS
#endif

#ifdef DEBUG
#define SDRAM_DEBUG
#define SDRAM
#define CC3_SOURCE
#define NOT_OS
#endif

#endif /* ifndef _PREFIX_H  */
