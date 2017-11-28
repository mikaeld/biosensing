//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateMachine.c
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the C file for the state machine of the system.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : State names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\StateMachine.h"
#include "..\headers\Interrupts.h"
#include "..\headers\SkadiFunctions.h"
#include "..\headers\Ads1299.h"
#include "..\headers\StateFunctions.h"

//==============================================================================
// VARIABLE DECLARATIONS
//==============================================================================

float temp = 0.0f;
extern volatile BOOL oDataAvailableFlag;

//==============================================================================
//	OPENBCI INTEGRATION
//==============================================================================

BOOL is_running = FALSE;    // this flag is set in serialEvent on reciept of ascii prompt
// these are used to change individual channel settings from PC
char currentChannelToSet;    // keep track of what channel we're loading settings for
BOOL getChannelSettings = FALSE; // used to receive channel settings command
int channelSettingsCounter; // used to retrieve channel settings from serial port
int leadOffSettingsCounter;
BOOL getLeadOffSettings = FALSE;
// these are all subject to the radio requirements: 31byte max packet length (maxPacketLength - 1 for packet checkSum)
#define OUTPUT_NOTHING (0)  // quiet
#define OUTPUT_8_CHAN (1)  // not using Daisy module
#define OUTPUT_16_CHAN (2)  // using Daisy module
int outputType = OUTPUT_8_CHAN;  // default to 8 channels

//==============================================================================
//	STATES OF STATE MACHINE
//==============================================================================

//===============================================================
// Name     : StateScheduler
// Purpose  : Decides which state is next depending on current
//            state and flags. Used as a function
//===============================================================
void StateScheduler(void)
{
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateInit
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if (pState == &StateInit)
  {
    if (INIT_2_SERIALTRIGGER)
    {
      pState = &StateSerialTrigger;  // Transition to StateAdsInit
    }
    else
    {
      pState = &StateInit; // Stay on current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateAdsStandBy
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateAdsStandBy)
  {
    if (ADSSTANDBY_2_SERIALTRIGGER)
    {
      pState = &StateSerialTrigger;  // Transition to StateAdsConfig
    }
    else
    {
      pState = &StateAdsStandBy;  // Stay on current state
    }
  }
  
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateDevState
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateSerialTrigger)
  {
    if (SERIALTRIGGER_2_ADSSTANDBY)
    {
      pState = &StateAdsStandBy;  // Transition to StateAdsConfig
    }
    else
    {
      pState = &StateSerialTrigger;  // Stay on current state
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = undetermined
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else
  {
    pState = &StateInit;   // Go to Init state by default
  }
}

//===============================================================
// Name     : StateInit
// Purpose  : Initialization routine of the microncontroller IOs and ADS1299
//===============================================================
void StateInit(void)
{
  oInitFlag = 0; // INIT STATE HAS NOT BEEN COMPLETED
  INT32 err = 0;
  INTDisableInterrupts();   // Disable all interrupts of the system.
  INIT_PORTS;
  
  //Turn all LEDs OFF
  LED_CHARGE_OFF;
  LED_EEGACQ_OFF;
  LED_DEBUG1_OFF;
  LED_DEBUG2_OFF;
  LED_DEBUG3_OFF;
  LED_STATUS_OFF;
  LED_ERROR_OFF;
  LED_CAN_OFF;
  
  INIT_UART;
  err = PrintToUart(UART4,"\f"  // Clear terminal
    "CochlEEG V1.0 - CRITIAS ECOLE DE TECHNOLOGIE SUPERIEURE \r\n"
    "\tSoftware version 2.0\r\n"
    "Developped by MIKAEL DUCHARME\r\n");   
  err = PrintToUart(UART4,
    "*** PIC32MX795F512H initialization routine ***\r\n"
    "PORTS Init - DONE\r\n"
    "UART Init - DONE\r\n");
  
  INIT_SPI;
  err = PrintToUart(UART4,"SPI Init - DONE\r\n");
  
  INIT_TIMER;
  err = PrintToUart(UART4,"TIMER Init - DONE\r\n");
   
  INIT_DMA;
  err = PrintToUart(UART4,"DMA Init - DONE\r\n");
  
  START_INTERRUPTS;
  err = PrintToUart(UART4,"INTERRUPTS STARTED*\r\n");
  
  LED_CHARGE_ON;
  Timer.DelayMs(100);
  LED_CHARGE_OFF;
  err = PrintToUart(UART4,"*** PIC32MX795F512H initialization routine COMPLETED ***\r\n");
  
  err = PrintToUart(UART4,"\r\n*** ADS1299 initialization routine ***\r\n");
  
  startFromScratch(); // Init ADS first time
    
  LED_EEGACQ_ON;
  Timer.DelayMs(200);
  LED_EEGACQ_OFF;
  Timer.DelayMs(200);
  LED_EEGACQ_ON;
  Timer.DelayMs(200);
  LED_EEGACQ_OFF;   // Flashing EEGAcq LED twice
  daisyPresent = FALSE;
  
  oInitFlag = 1; // INIT STATE COMPLETED
}

//===============================================================
// Name     : StateAdsStandBy
// Purpose  : UNIMPLEMENTED
//===============================================================
void StateAdsStandBy(void)
{
 
  oWakeUpFlag = 1;
}

//===============================================================
// Name     : StateSerialTrigger
// Purpose  : 
//===============================================================
void StateSerialTrigger(void)
{
  oDataAcqCompletedFlag = 0;
  LED_DEBUG1_ON;
  LED_EEGACQ_OFF;
  eventSerial();
}
