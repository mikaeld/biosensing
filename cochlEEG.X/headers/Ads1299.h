//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : HeaderTemplate.h
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


#ifndef __ADS1299_H__
#define	__ADS1299_H__

#include "Setup.h"


//==============================================================================
// Public functions prototypes
//==============================================================================
UINT32 SpiTransfer(SpiNum_t numSpi, const UINT32 data);
INT32 IsDataReady(void);
//void AdsWAKEUP(void);
//void AdsSTANDBY(void);
//void AdsRESET(void);
//void AdsSTART(void);
//void AdsSTOP(void);
//void AdsRDATAC(void);
//void AdsSDATAC(void);
//void WREG(BYTE _address, BYTE _value);

// BOARD 
void initialize(void);  // ADD DAISY USE outputType
void printAllRegisters(void);   // ADD DAISY USE outputType
void sendChannelData(void); // send the current data with sample number
void startStreaming(void);  // ADD DAISY USE outputType
void stopStreaming(void);   // ADD DAISY USE outputType

// ADS1299  
void initialize_ads(void);
void updateChannelSettings(void);
void writeChannelSettingsAll(void);
void writeChannelSettings(BYTE);
void setChannelsToDefault(void);
void setChannelsToEMG(void);
void setChannelsToECG(void);
void reportDefaultChannelSettings(void);
void printADSregisters(int);
void WAKEUP(int);  // get out of low power mode
void STANDBY(int); // go into low power mode
void RESET(int);   // set all register values to default
void START(int);   // start data acquisition
void STOP(int);    // stop data acquisition
void RDATAC(int);  // go into read data continuous mode 
void SDATAC(int);  // get out of read data continuous mode
void RDATA(int);   // read data one-shot   
BYTE RREG(BYTE,int);            // read one ADS register
void RREGS(BYTE,BYTE,int);      // read multiple ADS registers
void WREG(BYTE,BYTE,int);       // write one ADS register
void WREGS(BYTE,BYTE,int);      // write multiple ADS registers
BYTE ADS_getDeviceID(int);         
void printRegisterName(BYTE);   // used for verbosity
void updateChannelData(void);   // retrieve data from ADS
void updateBoardData(void);
void updateDaisyData(void);
BYTE xfer(BYTE);        // SPI Transfer function 
void resetADS(int);     // reset all the ADS1299's settings
void startADS(void);
void stopADS(void);
void activateChannel(BYTE);                  // enable the selected channel
void deactivateChannel(BYTE);                // disable given channel 1-8(16)
void configureLeadOffDetection(BYTE,BYTE);
void changeChannelLeadOffDetectAll(void);
void changeChannelLeadOffDetect(BYTE);
void configureInternalTestSignal(BYTE,BYTE); 
void changeInputType(BYTE); 
BOOL isDataAvailable(void);
void ADS_writeChannelData(void);
// void ADS_printDeviceID(int);   // 
BOOL smellDaisy(void);
void removeDaisy(void);
void attachDaisy(void);
void writeAuxData(void);




//==============================================================================
// Macro definitions
//==============================================================================

//
//  Definitions_32.h
//  
//	
//  Created by Conor Russomanno, Luke Travis, and Joel Murphy. Summer 2013.
//	Modified by Joel Murphy, Summer 2014
//	Joel Added Daisy Functionality Fall 2014
//

#define BOARD_ADS	8 	// ADS chip select
#define DAISY_ADS	3   // ADS Daisy chip select
#define BOTH_ADS	5   // Slave Select Both ADS chips

//ADS1299 SPI Command Definition Byte Assignments
#define _WAKEUP   0x02 // Wake-up from standby mode
#define _STANDBY  0x04 // Enter Standby mode
#define _RESET    0x06 // Reset the device registers to default
#define _START    0x08 // Start and restart (synchronize) conversions
#define _STOP     0x0A // Stop conversion
#define _RDATAC   0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC   0x11 // Stop Read Data Continuous mode
#define _RDATA    0x12 // Read data by command; supports multiple read back

//ASD1299 Register Addresses
#define ADS_ID      0x3E	// product ID for ADS1299
#define ID_REG      0x00	// this register contains ADS_ID
#define CONFIG1     0x01
#define CONFIG2     0x02
#define CONFIG3     0x03
#define LOFF        0x04
#define CH1SET      0x05
#define CH2SET      0x06
#define CH3SET      0x07
#define CH4SET      0x08
#define CH5SET      0x09
#define CH6SET      0x0A
#define CH7SET      0x0B
#define CH8SET      0x0C
#define BIAS_SENSP  0x0D
#define BIAS_SENSN  0x0E
#define LOFF_SENSP  0x0F
#define LOFF_SENSN  0x10
#define LOFF_FLIP   0x11
#define LOFF_STATP  0x12
#define LOFF_STATN  0x13
#define GPIO        0x14
#define MISC1       0x15
#define MISC2       0x16
#define CONFIG4     0x17

#define OUTPUT_NOTHING  (0) // quiet
#define OUTPUT_8_CHAN   (1) // not using Daisy module
#define OUTPUT_16_CHAN  (2) // using Daisy module
#define ON_BOARD        (8) // slave address for on board ADS
#define ON_DAISY        (3) // slave address for daisy ADS

// CHANNEL SETTINGS 
#define POWER_DOWN      (0)
#define GAIN_SET        (1)
#define INPUT_TYPE_SET  (2)
#define BIAS_SET        (3)
#define SRB2_SET        (4)
#define SRB1_SET        (5)
#define YES      	(0x01)
#define NO      	(0x00)

//gainCode choices
#define ADS_GAIN01 (0b00000000)	// 0x00
#define ADS_GAIN02 (0b00010000)	// 0x10
#define ADS_GAIN04 (0b00100000)	// 0x20
#define ADS_GAIN06 (0b00110000)	// 0x30
#define ADS_GAIN08 (0b01000000)	// 0x40
#define ADS_GAIN12 (0b01010000)	// 0x50
#define ADS_GAIN24 (0b01100000)	// 0x60

//inputType choices
#define ADSINPUT_NORMAL     (0b00000000)
#define ADSINPUT_SHORTED    (0b00000001)
#define ADSINPUT_BIAS_MEAS  (0b00000010)
#define ADSINPUT_MVDD       (0b00000011)
#define ADSINPUT_TEMP       (0b00000100)
#define ADSINPUT_TESTSIG    (0b00000101)
#define ADSINPUT_BIAS_DRP   (0b00000110)
#define ADSINPUT_BIAL_DRN   (0b00000111)

//test signal choices...ADS1299 datasheet page 41
#define ADSTESTSIG_AMP_1X     (0b00000000)
#define ADSTESTSIG_AMP_2X     (0b00000100)
#define ADSTESTSIG_PULSE_SLOW (0b00000000)
#define ADSTESTSIG_PULSE_FAST (0b00000001)
#define ADSTESTSIG_DCSIG      (0b00000011)
#define ADSTESTSIG_NOCHANGE   (0b11111111)

//Lead-off signal choices
#define LOFF_MAG_6NA      (0b00000000)
#define LOFF_MAG_24NA     (0b00000100)
#define LOFF_MAG_6UA      (0b00001000)
#define LOFF_MAG_24UA     (0b00001100)
#define LOFF_FREQ_DC      (0b00000000)
#define LOFF_FREQ_7p8HZ   (0b00000001)
#define LOFF_FREQ_31p2HZ  (0b00000010)
#define LOFF_FREQ_FS_4    (0b00000011)
#define PCHAN (0)
#define NCHAN (1)
#define OFF   (0)
#define ON    (1)

// used for channel settings
#define ACTIVATE_SHORTED  (2)
#define ACTIVATE          (1)
#define DEACTIVATE        (0)

#define PCKT_START  0xA0	// prefix for data packet error checking
#define PCKT_END    0xC0	// postfix for data packet error checking

// daisy module 
#define CLK_EN	5

//LIS3DH
#define READ_REG      0x80
#define READ_MULTI		0x40

#define STATUS_REG_AUX  0x07	// axis over-run and data available flags (see 0x27)
#define OUT_ADC1_L      0x08	// 
#define OUT_ADC1_H      0x09	//
#define OUT_ADC2_L      0x0A	//	ADC input values (check DS)
#define OUT_ADC2_H      0x0B	//
#define OUT_ADC3_L      0x0C	//
#define OUT_ADC3_H      0x0D	//
#define INT_COUNTER_REG	0x0E	// ??
#define WHO_AM_I        0x0F	// DEVICE ID = 0x33
#define TMP_CFG_REG     0x1F	// ADC enable (0x80); Temperature sensor enable (0x40)
#define CTRL_REG1       0x20	// Data Rate; Power Mode; X enable; Y enable; Z enable (on >= 0x10)
#define CTRL_REG2       0x21	// High Pass Filter Stuph
#define CTRL_REG3       0x22	// INT1 select register
#define CTRL_REG4       0x23	// Block update timing; endian; G-force; resolution; self test; SPI pins
#define CTRL_REG5       0x24	// reboot; FIFO enable; latch; 4D detection;
#define CTRL_REG6       0x25	// ??
#define REFERENCE       0x26	// interrupt reference
#define STATUS_REG2     0x27	// axis overrun and availale flags (see 0x07)
#define OUT_X_L         0x28	//
#define OUT_X_H         0x29	//
#define OUT_Y_L         0x2A	//	tripple axis values (see 0x0A)
#define OUT_Y_H         0x2B	//
#define OUT_Z_L         0x2C	//
#define OUT_Z_H         0x2D	//
#define FIFO_CTRL_REG	  0x2E	// FIFO mode; trigger output pin select (?); 
#define FIFO_SRC_REG	  0x2F	// ??
#define INT1_CFG        0x30	// 6 degree control register
#define INT1_SOURCE     0x31	// axis threshold interrupt control
#define INT1_THS        0x32	// INT1 threshold
#define INT1_DURATION	  0x33	// INT1 duration
#define CLICK_CFG       0x38	// click on axis
#define CLICK_SRC       0x39	// other click
#define CLICK_THS       0x3A	// more click
#define TIME_LIMIT      0x3B	// click related
#define TIME_LATENCY	  0x3C	// and so on
#define TIME_WINDOW     0x3D	// contined click

#define SCALE_2G        0x00  //(b00000000)	// +/- 2G sensitivity
#define SCALE_4G        0x10  //(b00010000)	// +/- 4G sensitivity
#define SCALE_8G        0x20  //(b00100000)	// +/- 8G sensitivity
#define SCALE_16G       0x30  //(b00110000)	// +/- 16G sensitivity
#define RATE_1HZ        0x10  //(b00010000)	// 1Hz sample rate in normal or low-power mode
#define RATE_10HZ       0x20  //(b00100000)	// 10Hz sample rate in normal or low-power mode
#define RATE_25HZ       0x30  //(b00110000)	// 25Hz sample rate in normal or low-power mode
#define RATE_50HZ       0x40  //(b01000000)	// 50Hz sample rate in normal or low-power mode
#define RATE_100HZ      0x50  //(b01010000)	// 100Hz sample rate in normal or low-power mode
#define RATE_200HZ      0x60  //(b01100000)	// 200Hz sample rate in normal or low-power mode
#define RATE_400HZ      0x70  //(b01110000)	// 400Hz sample rate in normal or low-power mode
#define RATE_1600HZ_LP	0x80  //(b10000000)	// 1600Hz sample rate in low-power mode
#define RATE_1250HZ_N		0x90  //(b10010000)	// 1250Hz sample rate in normal mode
#define RATE_5000HZ_LP	0x90  //(b10010000)	// 5000Hz sample rate in low-power mode

#define FOOTER 0xC0 
#define HEADER 0xA0

//==============================================================================
// Variable declarations
//==============================================================================

    INT16 auxData[3];
    BYTE regData[24];                 // array is used to mirror register data
    BYTE lastBoardDataRaw[24];
    BYTE boardChannelDataRaw[24];     // array to hold raw channel data
    BYTE meanBoardDataRaw[24];
    BYTE lastDaisyDataRaw[24];
    BYTE daisyChannelDataRaw[24];
    BYTE meanDaisyDataRaw[24];
    INT32 boardStat;                  // used to hold the status register
    INT32 daisyStat;
    INT32 boardChannelDataInt[8];     // array used when reading channel data as INT32s
    INT32 lastBoardChannelDataInt[8];
    INT32 meanBoardChannelDataInt[8];
    INT32 daisyChannelDataInt[8];     // array used when reading channel data as INT32s
    INT32 lastDaisyChannelDataInt[8];
    INT32 meanDaisyChannelDataInt[8];
    INT32 numChannels;
    BYTE channelSettings[16][6];      // array to hold current channel settings
    BYTE defaultChannelSettings[6];   // default channel settings
    BYTE leadOffSettings[16][2];      // used to control on/off of impedance measure for P and N side of each channel
    
    BOOL useInBias[16];               // used to remember if we were included in Bias before channel power down
    BOOL useSRB2[16];
    BOOL boardUseSRB1;                // used to keep track of if we are using SRB1
    BOOL daisyUseSRB1;
    BOOL verbosity;                   // turn on/off Serial verbosity
    BOOL daisyPresent;
    BOOL firstDataPacket;
    BOOL isRunning;
    BOOL useAux;
    BOOL useAccel;
    
    BYTE sampleCounter;

/***********************************
 * Template structure
 **********************************/

/***********************************
 * Template global variable
 **********************************/

/***********************************
 * Global struct
 **********************************/

#endif	/* __ADS1299_H__ */

