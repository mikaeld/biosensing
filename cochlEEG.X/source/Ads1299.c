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
#include "../headers/Setup.h"


//==============================================================================
// Private functions prototypes
//==============================================================================


//==============================================================================
// Variable definitions
//==============================================================================



//==============================================================================
// Functions
//==============================================================================

//  deactivate the given channel.
//void deactivateChannel(int N) 
//{
//  BYTE setting, startChan, endChan;  
//  
//  AdsSDATAC(); 
//  Timer.DelayMs(1); // exit Read Data Continuous mode to communicate with ADS
//  N = constrain(N-1,startChan,endChan-1);  //subtracts 1 so that we're counting from 0, not 1
//
//  setting = RREG(CH1SET+(N-startChan),targetSS); delay(1); // get the current channel settings
//  bitSet(setting,7);     // set bit7 to shut down channel
//  bitClear(setting,3);   // clear bit3 to disclude from SRB2 if used
//  WREG(CH1SET+(N-startChan),setting,targetSS); delay(1);     // write the new value to disable the channel
//  
//  //remove the channel from the bias generation...
//  setting = RREG(BIAS_SENSP,targetSS); delay(1); //get the current bias settings
//  bitClear(setting,N-startChan);                  //clear this channel's bit to remove from bias generation
//  WREG(BIAS_SENSP,setting,targetSS); delay(1);   //send the modified byte back to the ADS
//
//  setting = RREG(BIAS_SENSN,targetSS); delay(1); //get the current bias settings
//  bitClear(setting,N-startChan);                  //clear this channel's bit to remove from bias generation
//  WREG(BIAS_SENSN,setting,targetSS); delay(1);   //send the modified byte back to the ADS
//  
//  leadOffSettings[N][0] = leadOffSettings[N][1] = NO;
//  changeChannelLeadOffDetect(N+1);
//} 


////reset all the ADS1299's settings. Stops all data acquisition
//void rebootAds(void) 
//{
//  AdsRESET();             // send RESET command to default all registers
//  AdsSDATAC();            // exit Read Data Continuous mode to communicate with ADS
//  Time.DelayMs(1);
//  // turn off all channels
//  deactivateChannel(chan);
//}

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
  SpiTransfer(SPI4, _WAKEUP);
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
  SpiTransfer(SPI4, _STANDBY);
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
  SpiTransfer(SPI4, _RESET);
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
  SpiTransfer(SPI4, _START);
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
  SpiTransfer(SPI4, _STOP);
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
  SpiTransfer(SPI4, _RDATAC);
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
  SpiTransfer(SPI4, _SDATAC);
  SPI4_CS_HIGH;  
  Timer.DelayMs(1);  
}

/**************************************************************
 * Function name  : AdsRREG
 * Purpose        : Reads register at address, returns its value
 * Arguments      : BYTE address
 * Returns        : Value of register specified as argument
 *************************************************************/
UINT32 AdsRREG(BYTE address) 
  {    
    UINT32 regValue = 0; 
    BYTE opcode = address + 0x20;   // RREG expects 001rrrrr where rrrrr = _address
    SPI4_CS_HIGH;
    while(Spi.IsSpiBusy(SPI4));
    SPI4_CS_LOW;                //  open SPI
    SpiTransfer(SPI4, opcode);  // Send opcode
    SpiTransfer(SPI4, 0x00);    // Number of registers to read -1 
    regValue = SpiTransfer(SPI4, 0x00); // Send dummy data and return Rx buffer contents
    SPI4_CS_HIGH;  
    return regValue;     // return requested register value
  }

/**************************************************************
 * Function name  : AdsWREG
 * Purpose        : Writes register at _address
 * Arguments      : BYTE address, BYTE value
 * Returns        : None.
 *************************************************************/
void AdsWREG(BYTE address, BYTE regValue) 
{ 
  BYTE opcode = address + 0x40;   //  WREG expects 010rrrrr where rrrrr = _address
  SPI4_CS_HIGH;
  while(Spi.IsSpiBusy(SPI4));
  SPI4_CS_LOW;
  SpiTransfer(SPI4, opcode);
  SpiTransfer(SPI4, 0x00);    // Number of registers to write -1
  SpiTransfer(SPI4, regValue);
  SPI4_CS_HIGH;
}


