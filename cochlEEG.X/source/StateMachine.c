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
UINT32 test = 93; // 0x5D

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
  // Current state = StateMcuInit
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if (pState == &StateMcuInit)
  {
    if (MCUINIT_2_ADSINIT)
    {
      pState = &StateAdsInit;  // Transition to StateAdsInit
    }
    else
    {
      pState = &StateMcuInit; // Stay on current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateAdsInit
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateAdsInit)
  {
    if (ADSINIT_2_ADSCONFIG)
    {
      pState = &StateAdsConfig;  // StateAdsConfig
    }
    else
    {
      pState = &StateAdsInit;  //Stay on current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateAdsConfig
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateAdsConfig)
  {
    if (ADSCONFIG_2_ADSSTANDBY)
    {
      pState = &StateAdsStandBy;  // Transition to StateAdsStandBy
    }
    else if (ADSCONFIG_2_DEVSTATE)
    {
      pState = &StateDevState;  // Transition to StateDevState
    }
    else if (ADSCONFIG_2_READDATACONT)
    {
      pState = &StateReadDataCont;  // Transition to StateReadDataCont
    }
    else
    {
      pState = &StateAdsConfig;  //Stay on current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateAdsStandBy
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateAdsStandBy)
  {
    if (ADSSTANDBY_2_ADSCONFIG)
    {
      pState = &StateAdsConfig;  // Transition to StateAdsConfig
    }
    else
    {
      pState = &StateAdsStandBy;  // Stay on current state
    }
  }
  
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateAdsStandBy
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateDevState)
  {
    if (DEVSTATE_2_ADSCONFIG)
    {
      pState = &StateAdsConfig;  // Transition to StateAdsConfig
    }
    else
    {
      pState = &StateDevState;  // Stay on current state
    }
  }
  
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateReadDataCont
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateReadDataCont)
  {
    if (READDATACONT_2_ADSCONFIG)
    {
      pState = &StateAdsConfig;  // Transition to StateAdsConfig
    }
    else
    {
      pState = &StateReadDataCont;  // Stay on current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = undetermined
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else
  {
    pState = &StateMcuInit;   // Go to Init state by default
  }
}

//===============================================================
// Name     : StateMcuInit
// Purpose  : Initialization routine of the microncontroller IOs.
//===============================================================
void StateMcuInit(void)
{
  oMcuInitFlag = 0; // MCU INIT STATE HAS NOT BEEN COMPLETED
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
    "\tSoftware version 1.0\r\n"
    "Developped by MIKAEL DUCHARME\r\n");   
  err = PrintToUart(UART4,
    "*** PIC32MX795F512H initialization routine ***\r\n"
    "PORTS Init - DONE\r\n"
    "UART Init - DONE\r\n");
  
  INIT_SPI;
  err = PrintToUart(UART4,"SPI Init - DONE\r\n");
  
//  INIT_WDT;
//  err = PrintToUart(UART4,"WDT Init - DONE\r\n");
  
  INIT_TIMER;
  err = PrintToUart(UART4,"TIMER Init - DONE\r\n");
  
//  INIT_INPUT_CAPTURE;
//  err = PrintToUart(UART4,"INPUTCAPTURE Init - DONE\r\n");
  
//  INIT_PWM;
//  err = PrintToUart(UART4,"PWM Init - DONE\r\n");
  
//  INIT_ADC;
//  err = PrintToUart(UART4,"ADC Init - DONE\r\n");
  
  INIT_SKADI;
  err = PrintToUart(UART4,"SKADI Init - DONE\r\n");
  
//  INIT_CAN;
//  err = PrintToUart(UART4,"CAN Init - DONE\r\n");
  
//  INIT_I2C;
//  err = PrintToUart(UART4,"I2C Init - DONE\r\n");
  
  START_INTERRUPTS;
  err = PrintToUart(UART4,"INTERRUPTS STARTED*\r\n");
  
  LED_CHARGE_ON;
  Timer.DelayMs(100);
  LED_CHARGE_OFF;
  err = PrintToUart(UART4,"*** PIC32MX795F512H initialization routine COMPLETED ***\r\n");
  
  oMcuInitFlag = 1; // MCU INIT STATE COMPLETED
}

//===============================================================
// Name     : StateAdsInit
// Purpose  : Initialization routine of the ADS1299
//===============================================================
void StateAdsInit(void)
{
  oAdsInitFlag = 0; // ADS1299 INIT ROUTINE HAS NOT BEEN COMPLETED
  
  initialize_ads();
  ADS_getDeviceID(BOARD_ADS);
  
//  INT32 err = 0;
//  err = PrintToUart(UART4,"\r\n*** ADS1299 initialization routine ***\r\n");
//  
//  ADS_NO_RESET;
//  Timer.DelayMs(1);
//  AdsSDATAC(); // Stop data conversion in order to read and write registers 
//  UINT32 adsIdValue = AdsRREG(ID_REG);  // Read Device ID register (returns 0x3E)
//  if(adsIdValue != ADS_ID)  // Compare ID read vs 0x3E in order to identify hardware issues
//  {
//    LED_ERROR_ON;
//    err = PrintToUart(UART4,"ERROR READING ADS1299 ID\r\n"
//            "\r\nCheck CochlEEG for possible hardware issue");
//   }
//  else 
//  {
//    err = PrintToUart(UART4, "ADS1299 DEVICE ID DETECTED (0X3E)\r\n"
//      "*** ADS1299 initialization routine COMPLETED ***\r\n");
    LED_EEGACQ_ON;
    Timer.DelayMs(200);
    LED_EEGACQ_OFF;
    Timer.DelayMs(200);
    LED_EEGACQ_ON;
    Timer.DelayMs(200);
    LED_EEGACQ_OFF;   // Flashing EEGAcq LED twice
    daisyPresent = FALSE;
    oAdsInitFlag = 1; // ADS1299 INIT ROUTINE HAS BEEN COMPLETED 
//  }
}

//===============================================================
// Name     : StateAdsConfig
// Purpose  : 
//===============================================================
void StateAdsConfig(void)
{
//  AdsWREG(CONFIG3, 0xE0);
//  AdsWREG(CONFIG1, 0x96);
//  AdsWREG(CONFIG2, 0xC0);
//  AdsWREG(CH1SET, 0x01);
//  AdsWREG(CH2SET, 0x01);
//  AdsWREG(CH3SET, 0x01);
//  AdsWREG(CH4SET, 0x01);
//  AdsWREG(CH5SET, 0x01);
//  AdsWREG(CH6SET, 0x01);
//  AdsWREG(CH7SET, 0x01);
//  AdsWREG(CH8SET, 0x01);
//  AdsSTART();
//  AdsRDATAC();
  
  configureInternalTestSignal(ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_FAST);
  WREG(CH1SET,ADSINPUT_TESTSIG, BOARD_ADS);
  WREG(CH2SET,ADSINPUT_TESTSIG, BOARD_ADS);
  WREG(CH3SET,ADSINPUT_TESTSIG, BOARD_ADS);
  WREG(CH4SET,ADSINPUT_TESTSIG, BOARD_ADS);
  WREG(CH5SET,ADSINPUT_TESTSIG, BOARD_ADS);
  WREG(CH6SET,ADSINPUT_TESTSIG, BOARD_ADS);
  WREG(CH7SET,ADSINPUT_TESTSIG, BOARD_ADS);
  startStreaming();
  oDevStateFlag = 1;
  INT32 err = 0;
//  err = PrintToUart(UART4, "\r\n*** Loading ADS1299 configuration ***\r\n");
}

//===============================================================
// Name     : StateAdsStandBy
// Purpose  : 
//===============================================================
void StateAdsStandBy(void)
{
 
  oWakeUpFlag = 1;
}

//===============================================================
// Name     : StateDevState
// Purpose  : 
//===============================================================
void StateDevState(void)
{
  //UINT32 datardy = Port.B.ReadBits(BIT_13);
  
  LED_EEGACQ_ON;
  if(isDataAvailable())
  {
    updateChannelData();
    sendChannelData();    
  }
}

//===============================================================
// Name     : StateReadDataCont
// Purpose  : 
//===============================================================
void StateReadDataCont(void)
{

  oWakeUpFlag = 1;
}

