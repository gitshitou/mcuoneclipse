/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : RTOSSWI1.h
**     Project     : TWR_S12G240_FreeRTOS
**     Processor   : MC9S12G240CLL
**     Component   : InterruptVector
**     Version     : Component 02.023, Driver 01.06, CPU db: 3.00.010
**     Compiler    : CodeWarrior HC12 C Compiler
**     Date/Time   : 14.04.2020, 09:46
**     Abstract    :
**         This component "InterruptVector" gives an access to interrupt vector.
**         The purpose of this component is to allocate the interrupt vector
**         in the vector table. Additionally it can provide settings of
**         the interrupt priority register.
**         The interrupt handling routines must be implemented by the user.
**     Settings    :
**         Interrupt vector            : Vswi
**         Handle name                 : vPortYieldISR
**     Contents    :
**         No public methods
**
**Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**
**http      : www.freescale.com
**mail      : support@freescale.com
** ###################################################################*/
/*!
** @file RTOSSWI1.h
** @version 01.06
** @brief
**         This component "InterruptVector" gives an access to interrupt vector.
**         The purpose of this component is to allocate the interrupt vector
**         in the vector table. Additionally it can provide settings of
**         the interrupt priority register.
**         The interrupt handling routines must be implemented by the user.
*/         
/*!
**  @addtogroup RTOSSWI1_module RTOSSWI1 module documentation
**  @{
*/         

#ifndef __RTOSSWI1
#define __RTOSSWI1

/* MODULE RTOSSWI1. */

#include "PE_types.h"

/*
** ===================================================================
** The interrupt service routine must be implemented by user in one
** of the user modules (see RTOSSWI1.c file for more information).
** ===================================================================
*/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void vPortYieldISR(void);

#pragma CODE_SEG DEFAULT

/* END RTOSSWI1. */

#endif /* ifndef __RTOSSWI1 */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.03 [04.46]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
