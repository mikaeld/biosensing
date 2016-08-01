//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateMachine.h
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the header file for the state machine of the system.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : State names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifndef __STATE_MACHINE_H__
#define	__STATE_MACHINE_H__

#include "Setup.h"
#include "StateFunctions.h"


//==============================================================================
// State Machine public function prototypes
//==============================================================================
void StateMcuInit     (void); // Initialization state for microcontroller
void StateAdsInit     (void); // Initialization state for ADS1299
void StateAdsConfig   (void); // Configuration state for ADS1299 (parameters
                              // set from EEPROM, Serial Port, Buttons, etc.)
void StateDataAcq     (void); // Data Acq, live data
void StateAdsStandBy  (void); // ADS1299 is in standby
void StateDevState    (void); // Development state (for coding purposes)
void StateError       (void); // Error state, goes back to StateMcuInit()
void StateScheduler   (void); // State Scheduler. Decides which state is next
                              // depending on current state and flags. Used as a function


//==============================================================================
// Macro definitions
//==============================================================================

#define ONE_MS 10   // 10*100us = 1ms (Timer5 is 100us)

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// State scheduler flags
// The state scheduler is at the end of each state and
// decides which state is next. Following flags
// are used in this decision. The names used can and
// should be renamed to improve readability. Also, the
// conditions tested in the defines should be changed
// to proper tests
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//%%MCU INIT TRANSITIONS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define MCUINIT_2_ADSINIT         oMcuInitFlag    // StateMcuInit to StateAdsInit

//%%ADS INIT TRANSITIONS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define ADSINIT_2_ADSCONFIG       oAdsInitFlag    // StateAdsInit to StateAdsConfig

//%%CONFIG TRANSITIONS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%STANDBY MODE%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define ADSCONFIG_2_ADSSTANDBY    oStandByFlag    // StateAdsConfig to StateAdsStandBy
#define ADSSTANDBY_2_ADSCONFIG    oWakeUpFlag     // StateAdsStandBy to StateAdsConfig

//%%DEV STATE TRANSITIONS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define ADSCONFIG_2_DEVSTATE      oDevStateFlag   // StateAdsConfig to StateDevState
#define DEVSTATE_2_DATAACQ         /*isRunning &*/ oDataAvailableFlag // StateDevState to StateDataAcq

//%%DATA ACQ TRANSITIONS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define DATAACQ_2_DEVSTATE  oDataAcqCompletedFlag     // StateDataAcq to StateDevState
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//==============================================================================
// Variable declarations
//==============================================================================
void (*pState)(void);       // State pointer, used to navigate between states

volatile  INT8  oMcuInitFlag          // Flag indication MCU init routine is done
               ,oAdsInitFlag          // Flag indicating ADS1299 init routine is done
               ,oStandByFlag          // Flag indicating StandBy cmd was issued to ADS1299
               ,oWakeUpFlag           // Flag indicating WakeUp cmd was issued to ADS1299 (resume from StandBy)
               ,oDevStateFlag         // Flag indicating state used for development was called
               ,oDataAcqCompletedFlag // Flag indicating Data Acquisition sequence is done
               ;

#endif	/* __STATE_MACHINE_H__ */

