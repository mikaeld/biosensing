//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : SourceTemplate.c
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is a template header file that every developper should use as
//           a starter when developping code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "../headers/Ads1299.h"


//==============================================================================
// Private functions prototypes
//==============================================================================


//==============================================================================
// Variable definitions
//==============================================================================



//==============================================================================
// Functions
//==============================================================================

/**************************************************************
 * Function name  : TemplateFunction
 * Purpose        : Give a template for developpers to start from.
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void InitAds1299(void)
{
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, 'A');
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;
}

/**************************************************************
 * Function name  : AdsWAKEUP
 * Purpose        : Wake up from stand by Mode
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void AdsWAKEUP(void) 
{
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, _WAKEUP);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;
  Timer.DelayMs(1);
}

/**************************************************************
 * Function name  : AdsSTANDBY
 * Purpose        : Enter Standby Mode
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void AdsSTANDBY(void) 
{    // only allowed to send WAKEUP after sending STANDBY
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, _STANDBY);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;
}

/**************************************************************
 * Function name  : AdsRESET
 * Purpose        : Reset the device
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void AdsRESET(void) 
{      // reset all the registers to default settings
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, _RESET);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;
  Timer.DelayMs(1);
}

/**************************************************************
 * Function name  : AdsSTART
 * Purpose        : Start and Restart (synch) conversions
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void AdsSTART(void) 
{      //start data conversion 
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, _START);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;  
}

/**************************************************************
 * Function name  : AdsSTOP
 * Purpose        : Stop Conversion.
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void AdsSTOP(void) 
{     //stop data conversion
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, _STOP);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;  
}

/**************************************************************
 * Function name  : AdsRDATAC
 * Purpose        : Start Read Data Continuously Mode
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void AdsRDATAC(void) 
{
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, _RDATAC);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;  
  Timer.DelayMs(1);  
}

/**************************************************************
 * Function name  : AdsSDATAC
 * Purpose        : Stop Read Data Continuously Mode
 * Arguments      : None.
 * Returns        : None.
 *************************************************************/
void AdsSDATAC(void) 
{
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, _SDATAC);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;  
  Timer.DelayMs(1);  
}

/**************************************************************
 * Function name  : AdsWREG
 * Purpose        : Writes register at _address
 * Arguments      : BYTE address, BYTE value
 * Returns        : None.
 *************************************************************/
void AdsWREG(BYTE _address, BYTE _value) 
{ 
  BYTE opcode1 = _address + 0x40;   //  WREG expects 010rrrrr where rrrrr = _address
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  Spi.SendCharacter(SPI4, opcode1);
  Spi.SendCharacter(SPI4, 0x00);    // Number of registers to write -1
  Spi.SendCharacter(SPI4, _value);
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_HIGH;
//  regData[_address] = _value;     //  update the mirror array
}


