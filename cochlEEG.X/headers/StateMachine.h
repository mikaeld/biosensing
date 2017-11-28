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
void StateInit          (void); // Initialization state for microcontroller and ADS1299
void StateSerialTrigger (void); // Data Acq, live data
void StateAdsStandBy    (void); // ADS1299 is in standby (unimplemented)
void StateError         (void); // Error state, goes back to StateInit()
void StateScheduler     (void); // State Scheduler. Decides which state is next
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

//%%INIT TRANSITION%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define INIT_2_SERIALTRIGGER         oInitFlag    // StateInit to SerialTrigger

//%%%%%STANDBY MODE%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define SERIALTRIGGER_2_ADSSTANDBY    oStandByFlag    // StateAdsConfig to StateAdsStandBy
#define ADSSTANDBY_2_SERIALTRIGGER    oWakeUpFlag     // StateAdsStandBy to StateAdsConfig

//%%DATA ACQ TRANSITIONS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define DATAACQ_2_DEVSTATE  oDataAcqCompletedFlag     // StateDataAcq to StateDevState
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//==============================================================================
// Variable declarations
//==============================================================================
void (*pState)(void);       // State pointer, used to navigate between states

volatile  INT8  oInitFlag          // Flag indication Init routine is done
               ,oStandByFlag          // Flag indicating StandBy cmd was issued to ADS1299
               ,oWakeUpFlag           // Flag indicating WakeUp cmd was issued to ADS1299 (resume from StandBy)
               ,oDataAcqCompletedFlag // Flag indicating Data Acquisition sequence is done
               ;

#endif	/* __STATE_MACHINE_H__ */

