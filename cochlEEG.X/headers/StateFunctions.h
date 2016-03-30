//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateFunctions.h
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the header file for the functions of the state machine
//           of the system.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code. User must set the functions here and adjust
//           the structure skadiCommandTable located in Setup.c.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifndef __STATE_FUNCTIONS_H__
#define	__STATE_FUNCTIONS_H__

#include "Setup.h"


//==============================================================================
// State Machine private functions prototypes
//==============================================================================
UINT32 SpiTransfer(SpiNum_t numSpi, const UINT32 data);
INT32 PrintToUart(UartModule_t uartModuleId, char *string);
BYTE constrain(BYTE x, BYTE a, BYTE b);


//==============================================================================
// Macro definitions
//==============================================================================

#include <limits.h>     /* for CHAR_BIT */

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)
#define BITREAD(value, bit) (((value) >> (bit)) & 0x01)


//==============================================================================
// Variable declarations
//==============================================================================


#endif	/* __STATE_FUNCTIONS_H__ */

