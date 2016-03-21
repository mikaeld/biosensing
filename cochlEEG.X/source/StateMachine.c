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

//==============================================================================
// VARIABLE DECLARATIONS
//==============================================================================

UINT16 countTo50Ms  = 0;
UINT16 countTo1S    = 0;
UINT16 countTo1Ms   = 0;
UINT16 countTo100Ms = 0;
UINT32 dataId    = 0;
UINT8  samplesCount = 0;
float temp = 0.0f;

extern volatile BOOL oCapture5;
extern volatile BOOL oCapture4;
extern volatile BOOL oCapture3;

extern volatile BOOL oNewCanGear;
extern volatile BOOL oNewCanPitch;
extern volatile BOOL oNewCanBatteryVoltage;
extern volatile BOOL oNewCanBatteryCurrent;

extern volatile BOOL oNewAdcAvailable;

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
    if (INIT_2_DATAACQ)
    {
      pState = &StateDataAcq;  // Transition to DataAcq
    }
    else
    {
      pState = &StateInit; // Stay on current state
    }
  }
  /*
   * DEVELOPPER CODE HERE
   */

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateDataAcq
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateDataAcq)
  {
    if (DATAACQ_2_DATAAVG)
    {
      pState = &StateDataAvg;  // Transition to DataOutput
    }
    else
    {
      pState = &StateDataAcq;  //Stay on current state
    }
  }
  /*
   * DEVELOPPER CODE HERE
   */

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateDataAvg
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateDataAvg)
  {
    if (DATAAVG_2_DATAOUTPUT)
    {
      pState = &StateDataOutput;  // Transition to DataOutput
    }
    else
    {
      pState = &StateDataAvg;  //Stay on current state
    }
  }
  /*
   * DEVELOPPER CODE HERE
   */

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateDataOutput
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pState == &StateDataOutput)
  {
    if (DATAOUTPUT_2_DATAACQ)
    {
      pState = &StateDataAcq;  // Transition to DataAcq
    }
    else
    {
      pState = &StateDataOutput;  // Stay on current state
    }
  }
  /*
   * DEVELOPPER CODE HERE
   */

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = undetermined
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else
  {
    pState = &StateInit;   // Go to Init state by default
  }
  /*
   * DEVELOPPER CODE HERE
   */

}

//===============================================================
// Name     : StateInit
// Purpose  : Initialization of the system.
//===============================================================
void StateInit(void)
{
  
  INTDisableInterrupts();   // Disable all interrupts of the system.

  INIT_PORTS;
  
  LED_CHARGE_OFF;
  LED_EEGACQ_OFF;
  LED_DEBUG1_OFF;
  LED_DEBUG2_OFF;
  LED_DEBUG3_OFF;
  LED_STATUS_OFF;
  LED_ERROR_OFF;
  LED_CAN_OFF;
  
  INIT_SPI;
//  INIT_WDT;
  INIT_TIMER;
//  INIT_INPUT_CAPTURE;
//  INIT_PWM;
//  INIT_ADC;
  INIT_UART;
  INIT_SKADI;
//  INIT_CAN;
//  INIT_I2C;
  START_INTERRUPTS;
  
  initFlag = 1; // INIT STATE COMPLETED
}

//===============================================================
// Name     : StateDataAcq
// Purpose  : Data acquisition state, puts all the current values
//            in the structure
//===============================================================
void StateDataAcq(void)
{
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // VARIABLE DECLARATIONS
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // FIRST PART OF STATE
  // Developper should add a small description of expected behavior
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if(oNewAdcAvailable)
  {
    oNewAdcAvailable = 0;
  }

  if(oNewCanGear)
  {
    oNewCanGear = 0;
  }

  if(oNewCanPitch)
  {
    oNewCanPitch = 0;
  }
  if(oNewCanBatteryVoltage)
  {
    oNewCanBatteryVoltage = 0;
  }
  if(oNewCanBatteryCurrent)
  {
    oNewCanBatteryCurrent = 0;
  }

  if(oCapture5)
  {
    oCapture5 = 0;
  }

  if(oCapture4)
  {
    oCapture4 = 0;
//    LED_DEBG0_TOGGLE;
//    LED_DEBG3_TOGGLE;
//    LED_CAN_TOGGLE;
//    LED_ERROR_TOGGLE;
//    LED_STATUS_TOGGLE;
  }

  if(oCapture3)
  {
    oCapture3 = 0;
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SECOND PART OF STATE
  // Developper should add a small description of expected behavior
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


}

//===============================================================
// Name     : StateDataAvg
// Purpose  : Data averaging state, generates a mean value for each
//            and puts it in the structure
//===============================================================
void StateDataAvg(void)
{
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // VARIABLE DECLARATIONS
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // VALIDITY CHECK
  // Flag is set to 0 and counters are incremented by 1
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  o100UsFlag = 0;
  countTo1Ms++;
  countTo50Ms++;
  countTo100Ms++;
  countTo1S++;
  
//    LED_DEBG0_TOGGLE;
//    LED_DEBG3_TOGGLE;
//    LED_CAN_TOGGLE;
//    LED_ERROR_TOGGLE;
//    LED_STATUS_TOGGLE;

    
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // AVERAGING OF DATA
  // Data is sampled every 1 ms and averaged every 100 ms
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if(countTo1Ms >= ONE_MS)
  {
    samplesCount++;
    
    if(samplesCount <= SAMPLES_PER_AVERAGE)
    {
      if(samplesCount == SAMPLES_PER_AVERAGE)
      {


        samplesCount = 0;
      }
    }
  }

  dataAvgFlag = 1;
}

//===============================================================
// Name     : StateDataOutput
// Purpose  : Outputs the value of the structure to the Xbee, SD
//            Card, CAN, UART
//===============================================================
void StateDataOutput(void)
{
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // VARIABLE DECLARATIONS
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  sUartLineBuffer_t buffer = {0};
  INT8 err = 0;

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // FIRST PART OF STATE
  // Developper should add a small description of expected behavior
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//  Skadi.GetCmdMsgFifo();

  if(countTo50Ms >= 499)
  {
//    temp++;
//    Can.SendFloat(CAN1, TURBINE_RPM_ID, temp);
    countTo50Ms = 0;
  }

  if(countTo100Ms >= 999)
  {
    LED_DEBUG1_TOGGLE;
    buffer.length = sprintf(buffer.buffer,"test");
    do
      {
        Uart.PutTxFifoBuffer(UART4, &buffer);
      } while (err < 0);
    countTo100Ms = 0;
  }

  if(countTo1S >= 9999 )
  {
    LED_DEBUG2_TOGGLE;
//    PrintWindSpeed((sSensor_t *) &sSensor);
//    PrintWindDirection((sSensor_t *) &sSensor);
//    PrintWheelRpm((sSensor_t *) &sSensor);
//    PrintTurbineRpm((sSensor_t *) &sSensor);
//    PrintTorque((sSensor_t *) &sSensor);
//    PrintThrust((sSensor_t *) &sSensor);

    countTo1S = 0;
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // SECOND PART OF STATE
  // Developper should add a small description of expected behavior
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  outputFlag = 1;

}