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
#include "../headers/StateFunctions.h"

//==============================================================================
// Global Variable Declaration for ADS1299
//==============================================================================
UINT32 PacketCounter = 0;


Config1_t config1Register = 
{
  .byte = CONFIG1_DEFAULT_SETTINGS // default 0x96
};

Config2_t config2Register = 
{
  .byte = CONFIG2_DEFAULT_SETTINGS // default 0xC0
};

Config3_t config3Register = 
{
  .byte = CONFIG3_DEFAULT_SETTINGS // default 0x60
};

Loff_t    lOffRegister = 
{
  .byte = LOFF_DEFAULT_SETTINGS   // default 0x00
};

ChNSet_t  chSetRegister[8] = 
{ 
  {.byte = CH_SET_DEFAULT_SETTINGS} // default 0x61
};

BiasSensP_t biasSensPRegister = 
{
  .byte = BIAS_SENSP_DEFAULT_SETTINGS // default 0x00
};

BiasSensN_t biasSensNRegister = 
{
  .byte = BIAS_SENSN_DEFAULT_SETTINGS // default 0x00
};

LOffSensP_t lOffSensPRegister = 
{
  .byte = LOFF_SENSP_DEFAULT_SETTINGS // default 0x00
};

LOffSensN_t lOffSensNRegister = 
{
  .byte = LOFF_SENSN_DEFAULT_SETTINGS // default 0x00
};

LOffFlip_t lOffFlipRegister = 
{
  .byte = LOFF_FLIP_DEFAULT_SETTINGS // default 0x00
};



//==============================================================================
// Private functions prototypes for the ADS
//==============================================================================
static inline void AdsWakeUp(void);
static inline void AdsStandBy(void);
static inline void AdsReset(void);
static inline void AdsStart(void);
static inline void AdsStop(void);
static inline void AdsRdatac(void);
static inline void AdsSdatac(void);

static inline BYTE AdsRreg(AdsRegisters_t adsRegister);
static inline void AdsWreg(AdsRegisters_t adsRegister, BYTE _value);

static inline void AdsInitialize(void);
static inline void AdsResetSequence(void);
static inline void AdsDeactivateChannel(sAdsChannel_t channel);

//==============================================================================
// Public structure of functions for the ADS
//==============================================================================

struct sCochleegAds Ads =
{

  //System Commands  
  .SystemCmd.WakeUp         = &AdsWakeUp
  ,.SystemCmd.StandBy       = &AdsStandBy
  ,.SystemCmd.Reset         = &AdsReset
  ,.SystemCmd.Start         = &AdsStart
  ,.SystemCmd.Stop          = &AdsStop
          
  //DataRead Commands
  ,.DataReadCmd.Rdatac      = &AdsRdatac
  ,.DataReadCmd.Sdatac      = &AdsSdatac
          
  //Register Commands
  ,.RegisterCmd.Rreg        = &AdsRreg
  ,.RegisterCmd.Wreg        = &AdsWreg  
          
  // ADS cochlEEG Commands
  ,.Initialize              = &AdsInitialize
  ,.ResetSequence           = &AdsResetSequence
  ,.DeactivateChannel       = &AdsDeactivateChannel


};

//==============================================================================
// Declarations of private functions
//==============================================================================

/******************************************************************************/
/***********************SYSTEM COMMANDS****************************************/
/******************************************************************************/
static inline void AdsWakeUp(void) 
{
  SPI4_CS_LOW; 
  xfer(_WAKEUP);
  SPI4_CS_HIGH; 
  Timer.DelayMs(1);     //must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}

static inline void AdsStandBy(void) 
{    
  SPI4_CS_LOW; 
  xfer(_STANDBY);
  SPI4_CS_HIGH; 
}

static inline void AdsReset(void) 
{    
  SPI4_CS_LOW; 
  xfer(_RESET);
  Timer.DelayMs(1);    //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
  SPI4_CS_HIGH; 
}

static inline void AdsStart(void) 
{       
  SPI4_CS_LOW;
  xfer(_START);           
  SPI4_CS_HIGH;
}

static inline void AdsStop(void) 
{     
  SPI4_CS_LOW;
  xfer(_STOP);           
  SPI4_CS_HIGH;
}
/******************************************************************************/
/***********************END OF SYSTEM COMMANDS*********************************/
/******************************************************************************/


/******************************************************************************/
/***********************DATAREAD COMMANDS**************************************/
/******************************************************************************/
static inline void AdsRdatac(void) 
{
  SPI4_CS_LOW;
  xfer(_RDATAC);      // read data continuous
  SPI4_CS_HIGH;
  Timer.DelayMs(1);
}
static inline void AdsSdatac(void) 
{
  SPI4_CS_LOW;
  xfer(_SDATAC);
  SPI4_CS_HIGH;
  Timer.DelayMs(1);   //must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}
/******************************************************************************/
/***********************END OF DATAREAD COMMANDS*******************************/
/******************************************************************************/


/******************************************************************************/
/***********************REGISTER COMMANDS**************************************/
/******************************************************************************/
static inline BYTE AdsRreg(AdsRegisters_t adsRegister) 
{    
  BYTE opcode1 = adsRegister + 0x20;   //  RREG expects 001rrrrr where rrrrr = _address
  SPI4_CS_LOW;        //  open SPI
  xfer(opcode1);          //  opcode1
  xfer(0x00);           //  opcode2
  Ads.registerMap[adsRegister] = xfer(0x00);//  update mirror location with returned BYTE
  SPI4_CS_HIGH;       //  close SPI 
  
  return Ads.registerMap[adsRegister];     // return requested register value
}

static inline void AdsWreg(AdsRegisters_t adsRegister, BYTE registerValue) 
{ 
  BYTE opcode1 = adsRegister + 0x40;   //  Ads.RegisterCmd.Wreg expects 010rrrrr where rrrrr = _address
  SPI4_CS_LOW;        //  open SPI
  xfer(opcode1);          //  Send Ads.RegisterCmd.Wreg command & address
  xfer(0x00);           //  Send number of registers to read -1
  xfer(registerValue);         //  Write the value to the register
  SPI4_CS_HIGH;       //  close SPI
  Ads.registerMap[adsRegister] = registerValue;     //  update the mirror array
}
/******************************************************************************/
/***********************END OF REGISTER COMMANDS*******************************/
/******************************************************************************/


/******************************************************************************/
/***********************ADS COCHLEEG SPECIFIC FUNCTIONS************************/
/******************************************************************************/
static inline void AdsInitialize(void)
{
  Timer.DelayMs(50);				
  ADS_RESET;
  Timer.DelayMs(1);	// toggle reset pin
  ADS_NO_RESET; 
  Timer.DelayMs(40);
  Ads.ResetSequence(); // reset the ADS registers, and stop DataContinuousMode
  Timer.DelayMs(10);
  config1Register.bits.CLK_EN = 0;    
  config1Register.bits.DAISY_ENn = 1;     //Multiple readback (to check if ok)
  config1Register.bits.SAMPLE_RATE = 0b110; //250SPS
  Ads.RegisterCmd.Wreg(CONFIG1,config1Register.byte);

  config2Register.byte = CONFIG2_DEFAULT_SETTINGS; // default test signal settings
  Ads.RegisterCmd.Wreg(CONFIG2,config2Register.byte);

  config3Register.bits.PD_REFBUFn = 1;
  config3Register.bits.BIAS_MEAS = 0;
  config3Register.bits.BIASREF_INT = 1;
  config3Register.bits.PD_BIASn = 1;
  config3Register.bits.BIAS_LOFF_SENS = 0;
  config3Register.bits.BIAS_STAT = 0;
  Ads.RegisterCmd.Wreg(CONFIG3, config3Register.byte);
  Timer.DelayMs(1);  // enable internal reference drive and etc.

  lOffSensPRegister.byte = 0;
  Ads.RegisterCmd.Wreg(LOFF_SENSP, lOffSensPRegister.byte);

  lOffSensNRegister.byte = 0;
  Ads.RegisterCmd.Wreg(LOFF_SENSN, lOffSensNRegister.byte);

  firstDataPacket = TRUE;
}

static inline void AdsResetSequence(void) 
{
  Ads.SystemCmd.Reset();    // send RESET command to default all registers
  Ads.DataReadCmd.Sdatac(); // exit Read Data Continuous mode to communicate with ADS
  Timer.DelayMs(100);
  // turn off all channels
  UINT8 i;
  for (i=CH1; i <= CH8 ; i++) 
  {
    Ads.DeactivateChannel(i);
  } 
}

static inline void AdsDeactivateChannel(sAdsChannel_t channel) 
{
  Ads.DataReadCmd.Sdatac();
  Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS

  chSetRegister[channel].byte = Ads.RegisterCmd.Rreg(channel+CH_TO_REG); 
  Timer.DelayMs(1); // get the current channel settings
  chSetRegister[channel].bits.PD = 1; // shut down channel
  chSetRegister[channel].bits.SRB2 = 0;  //Disconnect from SRB2

  Ads.RegisterCmd.Wreg(channel+CH_TO_REG,chSetRegister[channel].byte); 
  Timer.DelayMs(1);     // channel disabled, SRB2 disconnected
  
  //remove the channel from the bias generation...
  biasSensPRegister.byte = Ads.RegisterCmd.Rreg(BIAS_SENSP); 
  Timer.DelayMs(1); //get the current bias settings
  
  BITCLEAR(biasSensPRegister.byte,channel);  //clear this channel's bit to remove from bias generation
  Ads.RegisterCmd.Wreg(BIAS_SENSP,biasSensPRegister.byte); 
  Timer.DelayMs(1);   //send the modified BYTE back to the ADS

  biasSensNRegister.byte = Ads.RegisterCmd.Rreg(BIAS_SENSN); 
  Timer.DelayMs(1); //get the current bias settings
  
  BITCLEAR(biasSensNRegister.byte,channel);  //clear this channel's bit to remove from bias generation
  Ads.RegisterCmd.Wreg(BIAS_SENSN,biasSensNRegister.byte); 
  Timer.DelayMs(1);   //send the modified BYTE back to the ADS
  
//  leadOffSettings[channel][0] = leadOffSettings[channel][1] = NO;
//  changeChannelLeadOffDetect(channel+1);  
}

void startADS(void) 
{
  PacketCounter = 0;
  firstDataPacket = TRUE;
  Ads.DataReadCmd.Rdatac(); // enter Read Data Continuous mode
	Timer.DelayMs(1);   
  Ads.SystemCmd.Start();  // start the data acquisition
	Timer.DelayMs(1);
}

void updateBoardData(){
  BYTE inByte;
  int ByteCounter = 0;
  int i;

  SPI4_CS_LOW;       //  open SPI
  for(i=0; i<3; i++){ 
    inByte = xfer(0x00);    //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
    boardStat = (boardStat << 8) | inByte;
  }
  for(i = 0; i<8; i++){
    int j;
    for(j=0; j<3; j++){   //  read 24 bits of channel data in 8 3 BYTE chunks
      inByte = xfer(0x00);
      boardChannelDataRaw[ByteCounter] = inByte;  // raw data goes here
      ByteCounter++;
      boardChannelDataInt[i] = (boardChannelDataInt[i]<<8) | inByte;  // int data goes here
    }
  }
  SPI4_CS_HIGH;        //  close SPI
  // need to convert 24bit to 32bit if using the filter
  for(i=0; i<8; i++){     // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment 
    if(BITREAD(boardChannelDataInt[i],23) == 1){ 
      boardChannelDataInt[i] |= 0xFF000000;
    }else{
      boardChannelDataInt[i] &= 0x00FFFFFF;
    }
  }
 
  if(firstDataPacket == TRUE){firstDataPacket = FALSE;}
}


// Stop the continuous data acquisition
void stopADS()  
{
  Ads.SystemCmd.Stop();     // stop the data acquisition
	Timer.DelayMs(1);   		
  Ads.DataReadCmd.Sdatac();   // stop Read Data Continuous mode to communicate with ADS
	Timer.DelayMs(1);   
  isRunning = FALSE;
}

///******************************************************************************/
///***********************END OF ADS COCHLEEG SPECIFIC FUNCTIONS*****************/
///******************************************************************************/
//
//
////==============================================================================
//// Private functions prototypes
////==============================================================================
//
////==============================================================================
//// Variable definitions
////==============================================================================
//BYTE footer = FOOTER;
//BYTE header = HEADER;
//float AdsPacket[9] = {0};
//float PacketCounter = 0;
//
//
////==============================================================================
//// Functions
////==============================================================================
//
////print out the state of all the control registers
//
//SPI communication method
//
//
//


//
//
//
//
//  deactivate the given channel.

//
//void activateChannel(BYTE N) 
//{
//	BYTE setting, startChan, endChan, targetSS;  
//  if(N < 9){
//    targetSS = BOARD_ADS; startChan = 0; endChan = 8; 
//  }else{
//    if(!daisyPresent) { return; }
//    targetSS = DAISY_ADS; startChan = 8; endChan = 16;
//  }
//
//  N = constrain(N-1,startChan,endChan-1);  // 0-7 or 8-15
//
//  SDATAC(targetSS);  // exit Read Data Continuous mode to communicate with ADS
//  setting = 0x00;
////  channelSettings[N][POWER_DOWN] = NO; // keep track of channel on/off in this array  REMOVE?
//  setting |= channelSettings[N][GAIN_SET]; // gain
//  setting |= channelSettings[N][INPUT_TYPE_SET]; // input code
//  if(useSRB2[N] == TRUE)
//  {
//    channelSettings[N][SRB2_SET] = YES;
//  }
//  else
//  {
//    channelSettings[N][SRB2_SET] = NO;
//  }
//  if(channelSettings[N][SRB2_SET] == YES) 
//  {
//    BITSET(setting,3); // close this SRB2 switch
//  } 
//  Ads.RegisterCmd.Wreg(CH1SET+(N-startChan),setting,targetSS);
//  
//  // add or remove from inclusion in BIAS generation
//  if(useInBias[N])
//  {
//    channelSettings[N][BIAS_SET] = YES;
//  }
//  else
//  {
//    channelSettings[N][BIAS_SET] = NO;
//  }
//  setting = RREG(BIAS_SENSP,targetSS);       //get the current P bias settings
//  if(channelSettings[N][BIAS_SET] == YES)
//  {
//    BITSET(setting,N-startChan);    //set this channel's bit to add it to the bias generation
//    useInBias[N] = TRUE;
//  }
//  else
//  {
//    BITCLEAR(setting,N-startChan);  // clear this channel's bit to remove from bias generation
//    useInBias[N] = FALSE;
//  }
//  Ads.RegisterCmd.Wreg(BIAS_SENSP,setting,targetSS); 
//  Timer.DelayMs(1); //send the modified BYTE back to the ADS
//  setting = RREG(BIAS_SENSN,targetSS);       //get the current N bias settings
//  if(channelSettings[N][BIAS_SET] == YES)
//  {
//    BITSET(setting,N-startChan);    //set this channel's bit to add it to the bias generation
//  }
//  else
//  {
//    BITCLEAR(setting,N-startChan);  // clear this channel's bit to remove from bias generation
//  }
//  Ads.RegisterCmd.Wreg(BIAS_SENSN,setting,targetSS); 
//  Timer.DelayMs(1); //send the modified BYTE back to the ADS
//    
//  setting = 0x00;
//  if(targetSS == BOARD_ADS && boardUseSRB1 == TRUE) setting = 0x20;
//  if(targetSS == DAISY_ADS && daisyUseSRB1 == TRUE) setting = 0x20;
//  Ads.RegisterCmd.Wreg(MISC1,setting,targetSS);     // close all SRB1 swtiches
//}
//
//// change the lead off detect settings for all channels
//void changeChannelLeadOffDetectAll() 
//{
//  BYTE setting, startChan, endChan, targetSS;  
//
//  int b;
//  for(b=0; b<2; b++){
//    if(b == 0){ targetSS = BOARD_ADS; startChan = 0; endChan = 8; }
//    if(b == 1){ 
//      if(!daisyPresent){ return; }
//      targetSS = DAISY_ADS; startChan = 8; endChan = 16;
//    }
// 
//    SDATAC(targetSS); Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS
//    BYTE P_setting = RREG(LOFF_SENSP,targetSS);
//    BYTE N_setting = RREG(LOFF_SENSN,targetSS);
//    
//    int i;
//    for(i=startChan; i<endChan; i++){
//      if(leadOffSettings[i][PCHAN] == ON){
//        BITSET(P_setting,i-startChan);
//      }else{
//        BITCLEAR(P_setting,i-startChan);
//      }
//      if(leadOffSettings[i][NCHAN] == ON){
//        BITSET(N_setting,i-startChan);
//      }else{
//        BITCLEAR(N_setting,i-startChan);
//      }
//     Ads.RegisterCmd.Wreg(LOFF_SENSP,P_setting,targetSS);
//     Ads.RegisterCmd.Wreg(LOFF_SENSN,N_setting,targetSS);
//    }
//  }
//}
//
//// change the lead off detect settings for specified channel
//void changeChannelLeadOffDetect(BYTE N) // N arrives as zero indexed
//{
//  BYTE setting, targetSS, startChan; //, endChan;
//
//  if(N < 0x08){
//    targetSS = BOARD_ADS; startChan = 0x00; //endChan = 0x08; 
//  }else{
//    if(!daisyPresent) { PrintlnToUart(UART4,"no daisy attached!"); return; }
//    targetSS = DAISY_ADS; startChan = 0x08; //endChan = 0x10;
//  }
//
//
//  // N = constrain(N-1,startChan,endChan-1);
//  // SDATAC(targetSS); Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS
//  BYTE P_setting = RREG(LOFF_SENSP,targetSS);
//  BYTE N_setting = RREG(LOFF_SENSN,targetSS);
//  
//    if(leadOffSettings[N][PCHAN] == ON){
//      BITSET(P_setting,N-startChan);
//    }else{
//      BITCLEAR(P_setting,N-startChan);
//    }
//    if(leadOffSettings[N][NCHAN] == ON){
//      BITSET(N_setting,N-startChan);
//    }else{
//      BITCLEAR(N_setting,N-startChan);
//    }
//   Ads.RegisterCmd.Wreg(LOFF_SENSP,P_setting,targetSS);
//   Ads.RegisterCmd.Wreg(LOFF_SENSN,N_setting,targetSS);
//   int test = 0;
//} 
//
//void configureLeadOffDetection(BYTE amplitudeCode, BYTE freqCode) 
//{
//	amplitudeCode &= 0b00001100;  //only these two bits should be used
//	freqCode &= 0b00000011;  //only these two bits should be used
//	
//	BYTE setting, targetSS;
//	int i;
//  for(i=0; i<2; i++){
//    if(i == 0){ targetSS = BOARD_ADS; }
//    if(i == 1){ 
//      if(!daisyPresent){ return; }
//      targetSS = DAISY_ADS; 
//    }
//  	setting = RREG(LOFF,targetSS); //get the current bias settings
//  	//reconfigure the BYTE to get what we want
//  	setting &= 0b11110000;  //clear out the last four bits
//  	setting |= amplitudeCode;  //set the amplitude
//  	setting |= freqCode;    //set the frequency
//  	//send the config BYTE back to the hardware
//  	Ads.RegisterCmd.Wreg(LOFF,setting,targetSS); Timer.DelayMs(1);  //send the modified BYTE back to the ADS
//	}
//}
//
////Configure the test signals that can be inernally generated by the ADS1299
//void configureInternalTestSignal(BYTE amplitudeCode, BYTE freqCode) 
//{
//  BYTE setting, targetSS;
//  int i;
//  for(i=0; i<2; i++){
//    if(i == 0){ targetSS = BOARD_ADS;}
//    if(i == 1){ 
//      if(daisyPresent == FALSE){ return; }
//      targetSS = DAISY_ADS; 
//    }
//  	if (amplitudeCode == ADSTESTSIG_NOCHANGE) amplitudeCode = (RREG(CONFIG2,targetSS) & (0b00000100));
//  	if (freqCode == ADSTESTSIG_NOCHANGE) freqCode = (RREG(CONFIG2,targetSS) & (0b00000011));
//  	freqCode &= 0b00000011;  		//only the last two bits are used
//  	amplitudeCode &= 0b00000100;  	//only this bit is used
//  	BYTE setting = 0b11010000 | freqCode | amplitudeCode;  //compose the code
//  	Ads.RegisterCmd.Wreg(CONFIG2,setting,targetSS); Timer.DelayMs(1);
//	}
//}
//
//void changeInputType(BYTE inputCode){
//  int i;
//  for(i=0; i<numChannels; i++){
//    channelSettings[i][INPUT_TYPE_SET] = inputCode;
//  }
//
//  writeChannelSettingsAll();
//
//}
// 
//// Start continuous data acquisition
//void startADS(void) // NEEDS ADS ADDRESS, OR BOTH?
//{
//  PacketCounter = 0;
//  firstDataPacket = TRUE;
//  RDATAC(BOTH_ADS); // enter Read Data Continuous mode
//	Timer.DelayMs(1);   
//  START(BOTH_ADS);  // start the data acquisition
//	Timer.DelayMs(1);
//  isRunning = TRUE;
//}
//  
//// Query to see if data is available from the ADS1299...return TRUE is data is available
//BOOL isDataAvailable(void)
//{
//  return (!(Port.B.ReadBits(BIT_13)));
//}
//  
//// CALLED WHEN DRDY PIN IS ASSERTED. NEW ADS DATA AVAILABLE!
//void updateChannelData(){ 
//  updateBoardData();
////  if(daisyPresent) {updateDaisyData();}
//}
//
//void updateBoardData(){
//  BYTE inByte;
//  int ByteCounter = 0;
//  int i;
//
//  SPI4_CS_LOW       //  open SPI
//  for(i=0; i<3; i++){ 
//    inByte = xfer(0x00);    //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
//    boardStat = (boardStat << 8) | inByte;
//  }
//  for(i = 0; i<8; i++){
//    int j;
//    for(j=0; j<3; j++){   //  read 24 bits of channel data in 8 3 BYTE chunks
//      inByte = xfer(0x00);
//      boardChannelDataRaw[ByteCounter] = inByte;  // raw data goes here
//      ByteCounter++;
//      boardChannelDataInt[i] = (boardChannelDataInt[i]<<8) | inByte;  // int data goes here
//    }
//  }
//  csHigh(BOARD_ADS);        //  close SPI
//  // need to convert 24bit to 32bit if using the filter
//  for(i=0; i<8; i++){     // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment 
//    if(BITREAD(boardChannelDataInt[i],23) == 1){ 
//      boardChannelDataInt[i] |= 0xFF000000;
//    }else{
//      boardChannelDataInt[i] &= 0x00FFFFFF;
//    }
//  }
// 
//  if(firstDataPacket == TRUE){firstDataPacket = FALSE;}
//}
//
//
//// Stop the continuous data acquisition
//void stopADS()  
//{
//  STOP(BOTH_ADS);     // stop the data acquisition
//	Timer.DelayMs(1);   		
//  SDATAC(BOTH_ADS);   // stop Read Data Continuous mode to communicate with ADS
//	Timer.DelayMs(1);   
//  isRunning = FALSE;
//}
//
//
////write as binary each channel's data
//void ADS_writeChannelData() 
//{ 
//  AdsPacket[0] = PacketCounter;
//  sUartLineBuffer_t buffer = {0};
//  buffer.length = 36;
//
//  int i;
//  for(i=0; i<8; i++)
//  {
////    AdsPacket[i+1] = (float)boardChannelDataInt[i];  // ADS1299 Datasheet page 25
//    AdsPacket[i+1] = ((float)boardChannelDataInt[i])*4.5f/8388607.0f;  // ADS1299 Datasheet page 25
//  }
//
//  int j;
//  for(j=0; j < 9 ; j++)
//  {
//    float f = AdsPacket[j];
//    memcpy((void *)&buffer.buffer[j*4], (void *)&f, 4);
//  }
//  
//  INT32 err = 0;
//  do
//  {
//    err = Uart.PutTxFifoBuffer(UART4, &buffer);
//  }
//  while ( err < 0);  
//  
//  
////  int j=0;
////  for(j=0; j < 9 ; j++)
////  {
////    BYTE data[sizeof(float)];
////    float f = AdsPacket[j];
////    memcpy(data, &f, sizeof f);
////    Uart.SendDataByte(UART4,*(data + 0));
////    Uart.SendDataByte(UART4,*(data + 1));
////    Uart.SendDataByte(UART4,*(data + 2));
////    Uart.SendDataByte(UART4,*(data + 3));
////  }
//  
//  PacketCounter++;
//}
//
//
//BYTE ADS_getDeviceID() {      // simple hello world com check
//  BYTE data = RREG(ID_REG,targetSS);
//  if(verbosity)
//  {            // verbosity otuput
//    PrintToUart(UART4,"On Board ADS ID ");
//    printHex(UART4, data);
//    PrintlnToUart(UART4, " ");
//  }
//  return data;
//}
//  
////System Commands
//
//
//
//
//
//
//
//
//
////  THIS NEEDS CLEANING AND UPDATING TO THE NEW FORMAT
//void RDATA() {          //  use in Stop Read Continuous mode when DRDY goes low
//  BYTE inByte;            //  to read in one sample of the channels
//  int i;
//    csLow(targetSS);        //  open SPI
//    xfer(_RDATA);         //  send the RDATA command
//    for(i=0; i<3; i++){   //  read in the status register and new channel data
//      inByte = xfer(0x00);
//      boardStat = (boardStat<<8) | inByte; //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
//    }   
//  if(targetSS == BOARD_ADS){    
//    for(i = 0; i<8; i++){
//      int j;
//      for(j=0; j<3; j++){   //  read in the new channel data
//        inByte = xfer(0x00);
//        boardChannelDataInt[i] = (boardChannelDataInt[i]<<8) | inByte;
//      }
//    }
//    for(i=0; i<8; i++){
//      if(BITREAD(boardChannelDataInt[i],23) == 1){  // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment
//        boardChannelDataInt[i] |= 0xFF000000;
//      }else{
//        boardChannelDataInt[i] &= 0x00FFFFFF;
//      }
//    }
//  }else{
//    for(i = 0; i<8; i++){
//      int j;
//      for(j=0; j<3; j++){   //  read in the new channel data
//        inByte = xfer(0x00);
//        daisyChannelDataInt[i] = (daisyChannelDataInt[i]<<8) | inByte;
//      }
//    }
//    for(i=0; i<8; i++){
//      if(BITREAD(daisyChannelDataInt[i],23) == 1){  // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment
//        daisyChannelDataInt[i] |= 0xFF000000;
//      }else{
//        daisyChannelDataInt[i] &= 0x00FFFFFF;
//      }
//    }
//  }
//   csHigh(targetSS);        //  close SPI
//  
//  
//}
//
//BYTE RREG(BYTE _address) 
//{    //  reads ONE register at _address
//  BYTE opcode1 = _address + 0x20;   //  RREG expects 001rrrrr where rrrrr = _address
//  csLow(targetSS);        //  open SPI
//  xfer(opcode1);          //  opcode1
//  xfer(0x00);           //  opcode2
//  Ads.registerMap[_address] = xfer(0x00);//  update mirror location with returned BYTE
//  csHigh(targetSS);       //  close SPI 
//  if (verbosity)
//  {           //  verbosity output
//    printRegisterName(_address);
//    printHex(UART4, _address);
//    PrintToUart(UART4,", ");
//    printHex(UART4, Ads.registerMap[_address]);
//    PrintToUart(UART4,", ");
//    BYTE j;
//    for(j = 0; j<8; j++)
//    {
//      PrintToUartDec(UART4,BITREAD(Ads.registerMap[_address], 7-j));
//      if(j!=7) PrintToUart(UART4,", ");
//    }
//    
//    PrintlnToUart(UART4, " ");
//  }
//  return Ads.registerMap[_address];     // return requested register value
//}
//
//
//// Read more than one register starting at _address
//void RREGS(BYTE _address, BYTE _numRegistersMinusOne, ) {
//  BYTE opcode1 = _address + 0x20;   //  RREG expects 001rrrrr where rrrrr = _address
//  csLow();        //  open SPI
//  xfer(opcode1);          //  opcode1
//  xfer(_numRegistersMinusOne);  //  opcode2
//  int i;
//  for(i = 0; i <= _numRegistersMinusOne; i++)
//  {
//    Ads.registerMap[_address + i] = xfer(0x00);   //  add register BYTE to mirror array
//  }
//  csHigh(targetSS);       //  close SPI
//  if(verbosity)
//  {            //  verbosity output
//    int i;
//    for(i = 0; i<= _numRegistersMinusOne; i++)
//    {
//      printRegisterName(_address + i);
//      printHex(UART4, _address + i);
//      PrintToUart(UART4,", ");
//      printHex(UART4, Ads.registerMap[_address + i]);
//      PrintToUart(UART4,", ");
//      int j;
//      for(j = 0; j<8; j++)
//      {
//        PrintToUartDec(UART4,BITREAD(Ads.registerMap[_address + i], 7-j));
//        if(j!=7) PrintToUart(UART4,", ");
//      }
//      PrintlnToUart(UART4, " ");
//      Timer.DelayMs(30);
//    }
//  }
//}
//
//
//void Ads.RegisterCmd.WregS(BYTE _address, BYTE _numRegistersMinusOne, ) {
//  BYTE opcode1 = _address + 0x40;   //  Ads.RegisterCmd.Wreg expects 010rrrrr where rrrrr = _address
//  csLow(targetSS);        //  open SPI
//  xfer(opcode1);          //  Send Ads.RegisterCmd.Wreg command & address
//  xfer(_numRegistersMinusOne);  //  Send number of registers to read -1 
//  int i;
//  for (i=_address; i <=(_address + _numRegistersMinusOne); i++)
//  {
//    xfer(Ads.registerMap[i]);     //  Write to the registers
//  } 
//  csHigh(targetSS);
//  if(verbosity)
//  {
//    PrintToUart(UART4,"Registers ");
//    printHex(UART4, _address); PrintToUart(UART4," to ");
//    printHex(UART4, _address + _numRegistersMinusOne);
//    PrintlnToUart(UART4," modified");
//  }
//}
//
//void printRegisterName(BYTE _address) {
//    switch(_address){ 
//      case ID_REG: 
//        PrintToUart(UART4,"ADS_ID, "); break;
//      case CONFIG1:
//        PrintToUart(UART4,"CONFIG1, "); break;
//      case CONFIG2:
//        PrintToUart(UART4,"CONFIG2, "); break;
//      case CONFIG3:
//        PrintToUart(UART4,"CONFIG3, "); break;
//      case LOFF:
//        PrintToUart(UART4,"LOFF, "); break;
//      case CH1SET:
//        PrintToUart(UART4,"CH1SET, "); break;
//      case CH2SET:
//        PrintToUart(UART4,"CH2SET, "); break;
//      case CH3SET:
//        PrintToUart(UART4,"CH3SET, "); break;
//      case CH4SET:
//        PrintToUart(UART4,"CH4SET, "); break;
//      case CH5SET:
//        PrintToUart(UART4,"CH5SET, "); break;
//      case CH6SET:
//        PrintToUart(UART4,"CH6SET, "); break;
//      case CH7SET:
//        PrintToUart(UART4,"CH7SET, "); break;
//      case CH8SET:
//        PrintToUart(UART4,"CH8SET, "); break;
//      case BIAS_SENSP:
//        PrintToUart(UART4,"BIAS_SENSP, "); break;
//      case BIAS_SENSN:
//        PrintToUart(UART4,"BIAS_SENSN, "); break;
//      case LOFF_SENSP:
//        PrintToUart(UART4,"LOFF_SENSP, "); break;
//      case LOFF_SENSN:
//        PrintToUart(UART4,"LOFF_SENSN, "); break;
//      case LOFF_FLIP:
//        PrintToUart(UART4,"LOFF_FLIP, "); break;
//      case LOFF_STATP:
//        PrintToUart(UART4,"LOFF_STATP, "); break;
//      case LOFF_STATN:
//        PrintToUart(UART4,"LOFF_STATN, "); break;
//      case GPIO:
//        PrintToUart(UART4,"GPIO, "); break;
//      case MISC1:
//        PrintToUart(UART4,"MISC1, "); break;
//      case MISC2:
//        PrintToUart(UART4,"MISC2, "); break;
//      case CONFIG4:
//        PrintToUart(UART4,"CONFIG4, "); break;
//      default: 
//        break;
//    }
//}
