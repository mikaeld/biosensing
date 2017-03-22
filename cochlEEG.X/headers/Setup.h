//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : Setup.h
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the setup header for the system. It contains function
//           prototypes as well as macros and the public structure of
//           ChinookLib functions.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : For ChinookLib to be useable, it must be cloned on your hard
//           drive so the path
//               "..\..\..\ChinookLib\ChinookLib.X\headers\ChinookLib.h"
//           references an existing file.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifndef __SETUP_H__
#define	__SETUP_H__

//%%%%%%%%%%%%%%%%%%%%%%%%%
// ChinookLib files
//%%%%%%%%%%%%%%%%%%%%%%%%%
#include "..\..\..\ChinookLib\ChinookLib.X\headers\ChinookLib.h"

//==============================================================================
// ChinookLib functions
//==============================================================================
struct sChinook Chinook;            // Contains all ChinookLib functions


//==============================================================================
// Skadi functions
//==============================================================================
//extern const size_t lengthSkadiCommandTable;
//extern sSkadiCommand_t skadiCommandTable[];

//==============================================================================
// State Machine public functions prototypes
//==============================================================================
void InitTimer        (void);
void InitPwm          (void);
void InitPorts        (void);
void InitCan          (void);
void InitSkadi        (void);
void InitSpi          (void);
void InitUart         (void);
void InitI2c          (void);
void InitWdt          (void);
void InitAdc          (void);
void InitInputCapture (void);
void InitDma          (void);
void StartInterrupts  (void);


//==============================================================================
// Macro definitions
//==============================================================================

// INIT DEFINES
#define INIT_DMA            InitDma()
#define INIT_TIMER          InitTimer()
#define INIT_PWM            InitPwm()
#define INIT_PORTS          InitPorts()
#define INIT_CAN            InitCan()
#define INIT_SPI            InitSpi()
#define INIT_UART           InitUart()
#define INIT_SKADI          InitSkadi()
#define INIT_I2C            InitI2c()
#define INIT_WDT            InitWdt()
#define INIT_ADC            InitAdc()
#define INIT_INPUT_CAPTURE  InitInputCapture()
#define START_INTERRUPTS    StartInterrupts()

//LED DEFINES
#define LED_CHARGE_ON       Port.E.ClearBits(BIT_2) 
#define LED_EEGACQ_ON       Port.E.ClearBits(BIT_3)
#define LED_DEBUG1_ON       Port.E.ClearBits(BIT_4)
#define LED_DEBUG2_ON       Port.E.ClearBits(BIT_5)
#define LED_DEBUG3_ON       Port.E.ClearBits(BIT_6)
#define LED_STATUS_ON       Port.E.ClearBits(BIT_7)
#define LED_ERROR_ON        Port.B.ClearBits(BIT_5)
#define LED_CAN_ON          Port.B.ClearBits(BIT_4)

#define LED_CHARGE_OFF      Port.E.SetBits(BIT_2)
#define LED_EEGACQ_OFF      Port.E.SetBits(BIT_3)
#define LED_DEBUG1_OFF      Port.E.SetBits(BIT_4)
#define LED_DEBUG2_OFF      Port.E.SetBits(BIT_5)
#define LED_DEBUG3_OFF      Port.E.SetBits(BIT_6)
#define LED_STATUS_OFF      Port.E.SetBits(BIT_7)
#define LED_ERROR_OFF       Port.B.SetBits(BIT_5)
#define LED_CAN_OFF         Port.B.SetBits(BIT_4)

#define LED_CHARGE_TOGGLE   Port.E.ToggleBits(BIT_2)     
#define LED_EEGACQ_TOGGLE   Port.E.ToggleBits(BIT_3)
#define LED_DEBUG1_TOGGLE   Port.E.ToggleBits(BIT_4)
#define LED_DEBUG2_TOGGLE   Port.E.ToggleBits(BIT_5)
#define LED_DEBUG3_TOGGLE   Port.E.ToggleBits(BIT_6)
#define LED_STATUS_TOGGLE   Port.E.ToggleBits(BIT_7)
#define LED_ERROR_TOGGLE    Port.B.ToggleBits(BIT_5)
#define LED_CAN_TOGGLE      Port.B.ToggleBits(BIT_4)

#define SPI4_CS_HIGH        Port.F.SetBits(BIT_3)
#define SPI4_CS_LOW         Port.F.ClearBits(BIT_3)

#define ADS_DRDY            Port.B.ReadBits(BIT_13)
#define ADS_RESET           Port.B.ClearBits(BIT_15)
#define ADS_NO_RESET        Port.B.SetBits(BIT_15)  


//==============================================================================
// Variable definitions
//==============================================================================
#define CAN_NB_CHANNELS     2 // 2 CAN channels will be used
#define CAN_BUFFER_SIZE     8

BYTE Can1MessageFifoArea [ CAN_NB_CHANNELS     // Space used by CAN
                         * CAN_BUFFER_SIZE 
                         * CAN_TX_RX_MESSAGE_SIZE_BYTES 
                         ];

BYTE Can2MessageFifoArea [ CAN_NB_CHANNELS     // Space used by CAN
                         * CAN_BUFFER_SIZE 
                         * CAN_TX_RX_MESSAGE_SIZE_BYTES 
                         ];

#endif	/* __SETUP_H__ */