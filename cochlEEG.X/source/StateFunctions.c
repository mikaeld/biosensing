//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateFunctions.c
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the C file for the functions of the state machine of
//           the system.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\StateFunctions.h"


//==============================================================================
// State Machine private functions prototypes
//==============================================================================

/**************************************************************
 * Function name  : SpiTransfer
 * Purpose        : Sends data over SPI specified in argument,
 *                  and empties Rx buffer
 * Arguments      : SpiNum_t numSpi, const UINT32 data
 * Returns        : SPI Rx Buffer (UINT32)
 *************************************************************/
UINT32 SpiTransfer(SpiNum_t numSpi, const UINT32 data)
{
  Spi.SendCharacter(numSpi, data);
  while(Spi.IsSpiBusy(numSpi));
  return Spi.GetCharacter(numSpi);
}

/**************************************************************
 * Function name  : PrintToUart
 * Purpose        : Prints char argument to UART argument
 * Arguments      : UartModule_t uartModuleId, char *string
 * Returns        : error INT32
 *************************************************************/
INT32 PrintToUart(UartModule_t uartModuleId, char *string)
{
  INT32 err = 0;
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, string);
    do
    {
      err = Uart.PutTxFifoBuffer(uartModuleId, &buffer);
    } while (err < 0);
}

BYTE constrain(BYTE x, BYTE a, BYTE b)
{
  if(x < a) 
  {
    return a;
  }
  else if(b < x) 
  {
    return b;
  }
   else return x;
}

