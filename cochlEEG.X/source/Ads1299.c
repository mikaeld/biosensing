//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : Ads1299.c
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This file declares all interrupt subroutines used.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Library is adapted from openBCI, as this device was supposed to be 
//           openBCI compatible. However openBCI compatibility was dropped due
//           to performance limitations of openBCI. Daisy chain support is 
//           not implemented.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "../headers/Ads1299.h"
#include "../headers/Setup.h"
#include "../headers/StateFunctions.h"
#include <plib.h>

#define SHORT_PACKET 2 // if 1 : | Header 4 bytes | Framecount 4 bytes | Payload 24 bytes | 
                       // if 0 : | Header 4 bytes | Timestamp 4 bytes | Framecount 4 bytes | Payload 24 bytes | CRC 4 bytes |


//==============================================================================
// Private functions prototypes
//==============================================================================


//==============================================================================
// Variable definitions
//==============================================================================

extern volatile UINT32 timeFromStart100Us;    // Unused
extern volatile UINT32 timeStampUs;           // Unused



extern BYTE adsDataConversion[27];
extern volatile UINT32 frameCount;
sUartLineBuffer_t AdsPacket = {0}; //Uart buffer 

//==============================================================================
// Board Functions
//==============================================================================

//print out the state of all the control registers

void printADSregisters(int targetSS)   
{
    BOOL prevverbosityState = verbosity;
    verbosity = TRUE;						// set up for verbosity output
    RREGS(0x00,0x0C,targetSS);     	// read out the first registers
    Timer.DelayMs(10);  						// stall to let all that data get read by the PC
    RREGS(0x0D,0x17-0x0D,targetSS);	// read out the rest
    verbosity = prevverbosityState;
}

void printAllRegisters(){
  if(!isRunning){
    PrintlnToUart(UART4,"\nBoard ADS Registers");
    printADSregisters(BOARD_ADS);  
    if(daisyPresent){ 
      PrintlnToUart(UART4,"\nDaisy ADS Registers");
      printADSregisters(DAISY_ADS); 
    }
  }
}

void startStreaming(){  
  startADS(); 
} 

void stopStreaming(){
  stopADS();  
}

//SPI communication method, used only for device configuration
//SPI communicaton for data conversions is done with DMA and triggered by 
//Change notice interrupt on DRDY pin
BYTE xfer(BYTE _data) 
{
    BYTE inByte;
//    while(Spi.IsSpiBusy(SPI4));
//    SPI4_CS_LOW;
    inByte = SpiTransfer(SPI4, _data);
    return inByte;
}

//SPI chip select method
void csLow(int SS)
{ // select an SPI slave to talk to
  switch(SS){
    case BOARD_ADS:
      SPI4_CS_LOW;
      break;
    case BOTH_ADS:
      SPI4_CS_LOW;
      break;
    default: break;
  }
}

void csHigh(int SS)
{ // deselect SPI slave
  switch(SS){
    case BOARD_ADS:
      SPI4_CS_HIGH; 
      break;
    case BOTH_ADS:
      SPI4_CS_HIGH;
      break;
    default:
      break;
  }
}

//==============================================================================
// ADS1299 Functions
//==============================================================================


void initialize_ads(){
  int i, j;
// recommended power up sequence requiers >Tpor (~32mS)	
    Timer.DelayMs(50);				
//    pinMode(ADS_RST,OUTPUT);  
    ADS_RESET;
    Timer.DelayMs(1);	// toggle reset pin
    ADS_NO_RESET; // this will reset the Daisy if it is present
    Timer.DelayMs(1);	// recommended to wait 18 Tclk before using device (~8uS);    
// initalize the  data ready chip select and reset pins:
//    pinMode(ADS_DRDY, INPUT); // we get DRDY asertion from the on-board ADS
    Timer.DelayMs(40);
    resetADS(BOARD_ADS); // reset the on-board ADS registers, and stop DataContinuousMode
    Timer.DelayMs(10);
    WREG(CONFIG1,0xB5,BOARD_ADS); // tell on-board ADS to output its clk, set the data rate to 500SPS (default)
    Timer.DelayMs(40);
    numChannels = 8;    // expect up to 8 ADS channels


    // DEFAULT CHANNEL SETTINGS FOR ADS
    defaultChannelSettings[POWER_DOWN] = NO;        // on = NO, off = YES
    defaultChannelSettings[GAIN_SET] = ADS_GAIN24;     // Gain setting
    defaultChannelSettings[INPUT_TYPE_SET] = ADSINPUT_NORMAL;// input muxer setting
    defaultChannelSettings[BIAS_SET] = YES;    // add this channel to bias generation
    defaultChannelSettings[SRB2_SET] = YES;       // connect this P side to SRB2
    defaultChannelSettings[SRB1_SET] = NO;        // don't use SRB1

    for(i=0; i<numChannels; i++){
      for(j=0; j<6; j++){
        channelSettings[i][j] = defaultChannelSettings[j];  // assign default settings
      }
      useInBias[i] = TRUE;    // keeping track of Bias Generation
      useSRB2[i] = TRUE;      // keeping track of SRB2 inclusion
    }
    boardUseSRB1 = daisyUseSRB1 = FALSE;

    writeChannelSettingsAll(); // write settings to the on-board and on-daisy ADS if present

    WREG(CONFIG3,0b11101100,BOTH_ADS); 
    Timer.DelayMs(1);  // enable internal reference drive and etc.
    for(i=0; i<numChannels; i++)
    {  // turn off the impedance measure signal
      leadOffSettings[i][PCHAN] = OFF;
      leadOffSettings[i][NCHAN] = OFF;
    }
    verbosity = FALSE;      // when verbosity is TRUE, there will be Serial feedback
    firstDataPacket = TRUE;
}

//reset all the ADS1299's settings. Stops all data acquisition
void resetADS(int targetSS) 
{
  int startChan, stopChan;
  if(targetSS == BOARD_ADS) {startChan = 1; stopChan = 8;}
  if(targetSS == DAISY_ADS) {startChan = 9; stopChan = 16;}
  RESET(targetSS);             // send RESET command to default all registers
  SDATAC(targetSS);            // exit Read Data Continuous mode to communicate with ADS
  Timer.DelayMs(100);
  // turn off all channels
  int chan;
  for (chan=startChan; chan <= stopChan; chan++) {
    deactivateChannel(chan);
  } 
}

void setChannelsToDefault(void){ 
  int i, j;
  for(i=0; i<numChannels; i++){
    for(j=0; j<6; j++){
      channelSettings[i][j] = defaultChannelSettings[j];
    }
    useInBias[i] = TRUE;    // keeping track of Bias Generation
    useSRB2[i] = TRUE;      // keeping track of SRB2 inclusion
  }
  boardUseSRB1 = daisyUseSRB1 = FALSE;

  writeChannelSettingsAll();       // write settings to on-board ADS

  for(i=0; i<numChannels; i++){   // turn off the impedance measure signal
    leadOffSettings[i][PCHAN] = OFF;
    leadOffSettings[i][NCHAN] = OFF;
  }
  changeChannelLeadOffDetectAll(); // write settings to all ADS

  
  WREG(MISC1,0x00,BOARD_ADS);  // open SRB1 switch on-board
  if(daisyPresent){ WREG(MISC1,0x00,DAISY_ADS); } // open SRB1 switch on-daisy
}


void reportDefaultChannelSettings(void){

    Uart.SendDataByte(UART4,defaultChannelSettings[POWER_DOWN] + '0');        // on = NO, off = YES
    Uart.SendDataByte(UART4,(defaultChannelSettings[GAIN_SET] >> 4) + '0');     // Gain setting
    Uart.SendDataByte(UART4,defaultChannelSettings[INPUT_TYPE_SET] +'0');// input muxer setting
    Uart.SendDataByte(UART4,defaultChannelSettings[BIAS_SET] + '0');    // add this channel to bias generation
    Uart.SendDataByte(UART4,defaultChannelSettings[SRB2_SET] + '0');       // connect this P side to SRB2
    Uart.SendDataByte(UART4,defaultChannelSettings[SRB1_SET] + '0');        // don't use SRB1
}

// write settings for ALL 8 channels for a given ADS board
// channel settings: powerDown, gain, inputType, SRB2, SRB1 
void writeChannelSettingsAll(){   
  BOOL use_SRB1 = FALSE;
  BYTE setting, startChan, endChan, targetSS;  
  int b;
  BYTE i;
  for(b=0; b<2; b++){
    if(b == 0){ targetSS = BOARD_ADS; startChan = 0; endChan = 8; }
    if(b == 1){ 
      if(!daisyPresent){ return; }
      targetSS = DAISY_ADS; startChan = 8; endChan = 16;
    }
  
    SDATAC(targetSS); Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS

    for(i=startChan; i<endChan; i++){ // write 8 channel settings
      setting = 0x00;
      if(channelSettings[i][POWER_DOWN] == YES){setting |= 0x80;}
      setting |= channelSettings[i][GAIN_SET]; // gain
      setting |= channelSettings[i][INPUT_TYPE_SET]; // input code
      if(channelSettings[i][SRB2_SET] == YES){
        setting |= 0x08;    // close this SRB2 switch
        useSRB2[i] = TRUE;  // remember SRB2 state for this channel 
      }else{
        useSRB2[i] = FALSE; // rememver SRB2 state for this channel
      }
      WREG(CH1SET+(i-startChan),setting,targetSS);  // write this channel's register settings
      
      // add or remove this channel from inclusion in BIAS generation
      setting = RREG(BIAS_SENSP,targetSS);                   //get the current P bias settings
      if(channelSettings[i][BIAS_SET] == YES){
        BITSET(setting,i-startChan); 
        useInBias[i] = TRUE;    //add this channel to the bias generation
      }else{
        BITCLEAR(setting,i-startChan); 
        useInBias[i] = FALSE; //remove this channel from bias generation
      }
      WREG(BIAS_SENSP,setting,targetSS); Timer.DelayMs(1);           //send the modified BYTE back to the ADS

      setting = RREG(BIAS_SENSN,targetSS);                   //get the current N bias settings
      if(channelSettings[i][BIAS_SET] == YES){
        BITSET((setting),(i-startChan));    //set this channel's bit to add it to the bias generation
      }else{
        BITCLEAR((setting),(i-startChan));  // clear this channel's bit to remove from bias generation
      }
      WREG(BIAS_SENSN,setting,targetSS); Timer.DelayMs(1);           //send the modified BYTE back to the ADS
       
      if(channelSettings[i][SRB1_SET] == YES){
        use_SRB1 = TRUE;  // if any of the channel setting closes SRB1, it is closed for all
      }
    } // end of CHnSET and BIAS settings
  } // end of board select loop
    if(use_SRB1){
      for(i=startChan; i<endChan; i++){
        channelSettings[i][SRB1_SET] = YES;
      }
      WREG(MISC1,0x20,targetSS);     // close SRB1 swtich
      if(targetSS == BOARD_ADS){ boardUseSRB1 = TRUE; }
      if(targetSS == DAISY_ADS){ daisyUseSRB1 = TRUE; }
    }else{
      for(i=startChan; i<endChan; i++){
        channelSettings[i][SRB1_SET] = NO;
      }
      WREG(MISC1,0x00,targetSS);    // open SRB1 switch
      if(targetSS == BOARD_ADS){ boardUseSRB1 = FALSE; }
      if(targetSS == DAISY_ADS){ daisyUseSRB1 = FALSE; }
    }
}

// write settings for a SPECIFIC channel on a given ADS board
void writeChannelSettings(BYTE N){  
  int i;
  BYTE setting, startChan, endChan, targetSS;  
  if(N < 9)
  {  // channels 1-8 on board
    targetSS = BOARD_ADS; startChan = 0; endChan = 8; 
  }
  else
  {      // channels 9-16 on daisy module
    if(!daisyPresent) { return; }
    targetSS = DAISY_ADS; startChan = 8; endChan = 16;
  }
  
// function accepts channel 1-16, must be 0 indexed to work with array
  N = constrain(N-1,startChan,endChan-1);  //subtracts 1 so that we're counting from 0, not 1
// first, disable any data collection
  SDATAC(targetSS); 
  Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS
  
  setting = 0x00;
  if(channelSettings[N][POWER_DOWN] == YES) setting |= 0x80;
  setting |= channelSettings[N][GAIN_SET]; // gain
  setting |= channelSettings[N][INPUT_TYPE_SET]; // input code
  if(channelSettings[N][SRB2_SET] == YES)
  {
    setting |= 0x08; // close this SRB2 switch
    useSRB2[N] = TRUE;  // keep track of SRB2 usage
  }
  else
  {
    useSRB2[N] = FALSE;
  }
  WREG(CH1SET+(N-startChan), setting, targetSS);  // write this channel's register settings

  // add or remove from inclusion in BIAS generation
  setting = RREG(BIAS_SENSP,targetSS);       //get the current P bias settings
  if(channelSettings[N][BIAS_SET] == YES)
  {
    useInBias[N] = TRUE;
    BITSET(setting,N-startChan);    //set this channel's bit to add it to the bias generation
  }
  else
  {
    useInBias[N] = FALSE;
    BITCLEAR(setting,N-startChan);  // clear this channel's bit to remove from bias generation
  }
  WREG(BIAS_SENSP,setting,targetSS); 
  Timer.DelayMs(1); //send the modified BYTE back to the ADS
  
  setting = RREG(BIAS_SENSN,targetSS);       //get the current N bias settings
  if(channelSettings[N][BIAS_SET] == YES)
  {
    BITSET(setting,N-startChan);    //set this channel's bit to add it to the bias generation
  }
  else
  {
    BITCLEAR(setting,N-startChan);  // clear this channel's bit to remove from bias generation
  }
  WREG(BIAS_SENSN,setting,targetSS); 
  Timer.DelayMs(1); //send the modified BYTE back to the ADS
    
// if SRB1 is closed or open for one channel, it will be the same for all channels    
  if(channelSettings[N][SRB1_SET] == YES)
  {
    for(i=startChan; i<endChan; i++)
    {
      channelSettings[i][SRB1_SET] = YES;
    }
    if(targetSS == BOARD_ADS) boardUseSRB1 = TRUE;
    if(targetSS == DAISY_ADS) daisyUseSRB1 = TRUE;
    setting = 0x20;     // close SRB1 swtich
  }
  if(channelSettings[N][SRB1_SET] == NO)
  {
    for(i=startChan; i<endChan; i++)
    {
      channelSettings[i][SRB1_SET] = NO;
    }
    if(targetSS == BOARD_ADS) boardUseSRB1 = FALSE;
    if(targetSS == DAISY_ADS) daisyUseSRB1 = FALSE;
    setting = 0x00;     // open SRB1 switch
  }
  WREG(MISC1,setting,targetSS);
}

//  deactivate the given channel.
void deactivateChannel(BYTE N) 
{
  BYTE setting, startChan, endChan, targetSS;  
  if(N < 9)
  {
    targetSS = BOARD_ADS; startChan = 0; endChan = 8; 
  }
  else
  {
    if(!daisyPresent) 
    { 
      return; 
    }
    targetSS = DAISY_ADS; startChan = 8; 
    endChan = 16;
  }
  SDATAC(targetSS); 
  Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS
  N = constrain(N-1,startChan,endChan-1);  //subtracts 1 so that we're counting from 0, not 1

  setting = RREG(CH1SET+(N-startChan),targetSS); 
  Timer.DelayMs(1); // get the current channel settings
  BITSET(setting,7);     // set bit7 to shut down channel
  BITCLEAR(setting,3);   // clear bit3 to disclude from SRB2 if used
  WREG(CH1SET+(N-startChan),setting,targetSS); 
  Timer.DelayMs(1);     // write the new value to disable the channel
  
  //remove the channel from the bias generation...
  setting = RREG(BIAS_SENSP,targetSS); 
  Timer.DelayMs(1); //get the current bias settings
  BITCLEAR(setting,N-startChan);                  //clear this channel's bit to remove from bias generation
  WREG(BIAS_SENSP,setting,targetSS); 
  Timer.DelayMs(1);   //send the modified BYTE back to the ADS

  setting = RREG(BIAS_SENSN,targetSS); 
  Timer.DelayMs(1); //get the current bias settings
  BITCLEAR(setting,N-startChan);                  //clear this channel's bit to remove from bias generation
  WREG(BIAS_SENSN,setting,targetSS); 
  Timer.DelayMs(1);   //send the modified BYTE back to the ADS
  
  leadOffSettings[N][0] = leadOffSettings[N][1] = NO;
  changeChannelLeadOffDetect(N+1);
} 

void activateChannel(BYTE N) 
{
	BYTE setting, startChan, endChan, targetSS;  
  if(N < 9){
    targetSS = BOARD_ADS; startChan = 0; endChan = 8; 
  }else{
    if(!daisyPresent) { return; }
    targetSS = DAISY_ADS; startChan = 8; endChan = 16;
  }

  N = constrain(N-1,startChan,endChan-1);  // 0-7 or 8-15

  SDATAC(targetSS);  // exit Read Data Continuous mode to communicate with ADS
  setting = 0x00;
//  channelSettings[N][POWER_DOWN] = NO; // keep track of channel on/off in this array  REMOVE?
  setting |= channelSettings[N][GAIN_SET]; // gain
  setting |= channelSettings[N][INPUT_TYPE_SET]; // input code
  if(useSRB2[N] == TRUE)
  {
    channelSettings[N][SRB2_SET] = YES;
  }
  else
  {
    channelSettings[N][SRB2_SET] = NO;
  }
  if(channelSettings[N][SRB2_SET] == YES) 
  {
    BITSET(setting,3); // close this SRB2 switch
  } 
  WREG(CH1SET+(N-startChan),setting,targetSS);
  
  // add or remove from inclusion in BIAS generation
  if(useInBias[N])
  {
    channelSettings[N][BIAS_SET] = YES;
  }
  else
  {
    channelSettings[N][BIAS_SET] = NO;
  }
  setting = RREG(BIAS_SENSP,targetSS);       //get the current P bias settings
  if(channelSettings[N][BIAS_SET] == YES)
  {
    BITSET(setting,N-startChan);    //set this channel's bit to add it to the bias generation
    useInBias[N] = TRUE;
  }
  else
  {
    BITCLEAR(setting,N-startChan);  // clear this channel's bit to remove from bias generation
    useInBias[N] = FALSE;
  }
  WREG(BIAS_SENSP,setting,targetSS); 
  Timer.DelayMs(1); //send the modified BYTE back to the ADS
  setting = RREG(BIAS_SENSN,targetSS);       //get the current N bias settings
  if(channelSettings[N][BIAS_SET] == YES)
  {
    BITSET(setting,N-startChan);    //set this channel's bit to add it to the bias generation
  }
  else
  {
    BITCLEAR(setting,N-startChan);  // clear this channel's bit to remove from bias generation
  }
  WREG(BIAS_SENSN,setting,targetSS); 
  Timer.DelayMs(1); //send the modified BYTE back to the ADS
    
  setting = 0x00;
  if(targetSS == BOARD_ADS && boardUseSRB1 == TRUE) setting = 0x20;
  if(targetSS == DAISY_ADS && daisyUseSRB1 == TRUE) setting = 0x20;
  WREG(MISC1,setting,targetSS);     // close all SRB1 swtiches
}

// change the lead off detect settings for all channels
void changeChannelLeadOffDetectAll() 
{
  BYTE setting, startChan, endChan, targetSS;  

  int b;
  for(b=0; b<2; b++){
    if(b == 0){ targetSS = BOARD_ADS; startChan = 0; endChan = 8; }
    if(b == 1){ 
      if(!daisyPresent){ return; }
      targetSS = DAISY_ADS; startChan = 8; endChan = 16;
    }
 
    SDATAC(targetSS); Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS
    BYTE P_setting = RREG(LOFF_SENSP,targetSS);
    BYTE N_setting = RREG(LOFF_SENSN,targetSS);
    
    int i;
    for(i=startChan; i<endChan; i++){
      if(leadOffSettings[i][PCHAN] == ON){
        BITSET(P_setting,i-startChan);
      }else{
        BITCLEAR(P_setting,i-startChan);
      }
      if(leadOffSettings[i][NCHAN] == ON){
        BITSET(N_setting,i-startChan);
      }else{
        BITCLEAR(N_setting,i-startChan);
      }
     WREG(LOFF_SENSP,P_setting,targetSS);
     WREG(LOFF_SENSN,N_setting,targetSS);
    }
  }
}

// change the lead off detect settings for specified channel
void changeChannelLeadOffDetect(BYTE N) // N arrives as zero indexed
{
  BYTE setting, targetSS, startChan; //, endChan;

  if(N < 0x08){
    targetSS = BOARD_ADS; startChan = 0x00; //endChan = 0x08; 
  }else{
    if(!daisyPresent) { PrintlnToUart(UART4,"no daisy attached!"); return; }
    targetSS = DAISY_ADS; startChan = 0x08; //endChan = 0x10;
  }


  // N = constrain(N-1,startChan,endChan-1);
  // SDATAC(targetSS); Timer.DelayMs(1);      // exit Read Data Continuous mode to communicate with ADS
  BYTE P_setting = RREG(LOFF_SENSP,targetSS);
  BYTE N_setting = RREG(LOFF_SENSN,targetSS);
  
    if(leadOffSettings[N][PCHAN] == ON){
      BITSET(P_setting,N-startChan);
    }else{
      BITCLEAR(P_setting,N-startChan);
    }
    if(leadOffSettings[N][NCHAN] == ON){
      BITSET(N_setting,N-startChan);
    }else{
      BITCLEAR(N_setting,N-startChan);
    }
   WREG(LOFF_SENSP,P_setting,targetSS);
   WREG(LOFF_SENSN,N_setting,targetSS);
   int test = 0;
} 

void configureLeadOffDetection(BYTE amplitudeCode, BYTE freqCode) 
{
	amplitudeCode &= 0b00001100;  //only these two bits should be used
	freqCode &= 0b00000011;  //only these two bits should be used
	
	BYTE setting, targetSS;
	int i;
  for(i=0; i<2; i++){
    if(i == 0){ targetSS = BOARD_ADS; }
    if(i == 1){ 
      if(!daisyPresent){ return; }
      targetSS = DAISY_ADS; 
    }
  	setting = RREG(LOFF,targetSS); //get the current bias settings
  	//reconfigure the BYTE to get what we want
  	setting &= 0b11110000;  //clear out the last four bits
  	setting |= amplitudeCode;  //set the amplitude
  	setting |= freqCode;    //set the frequency
  	//send the config BYTE back to the hardware
  	WREG(LOFF,setting,targetSS); Timer.DelayMs(1);  //send the modified BYTE back to the ADS
	}
}

//Configure the test signals that can be inernally generated by the ADS1299
void configureInternalTestSignal(BYTE amplitudeCode, BYTE freqCode) 
{
  BYTE setting, targetSS;
  int i;
  for(i=0; i<2; i++){
    if(i == 0){ targetSS = BOARD_ADS;}
    if(i == 1){ 
      if(daisyPresent == FALSE){ return; }
      targetSS = DAISY_ADS; 
    }
  	if (amplitudeCode == ADSTESTSIG_NOCHANGE) amplitudeCode = (RREG(CONFIG2,targetSS) & (0b00000100));
  	if (freqCode == ADSTESTSIG_NOCHANGE) freqCode = (RREG(CONFIG2,targetSS) & (0b00000011));
  	freqCode &= 0b00000011;  		//only the last two bits are used
  	amplitudeCode &= 0b00000100;  	//only this bit is used
  	BYTE setting = 0b11010000 | freqCode | amplitudeCode;  //compose the code
  	WREG(CONFIG2,setting,targetSS); Timer.DelayMs(1);
	}
}

void changeInputType(BYTE inputCode){
  int i;
  for(i=0; i<numChannels; i++){
    channelSettings[i][INPUT_TYPE_SET] = inputCode;
  }

  writeChannelSettingsAll();

}
 
// Start continuous data acquisition
void startADS(void) // NEEDS ADS ADDRESS, OR BOTH?
{
  frameCount = 0;
  firstDataPacket = TRUE;
  RDATAC(BOTH_ADS); // enter Read Data Continuous mode
	Timer.DelayMs(1);   
  START(BOTH_ADS);  // start the data acquisition
	Timer.DelayMs(1);
  isRunning = TRUE;
}
  
// Query to see if data is available from the ADS1299...return TRUE is data is available
BOOL isDataAvailable(void)
{
  return (!(Port.B.ReadBits(BIT_13)));
}
  
// CALLED WHEN DRDY PIN IS ASSERTED. NEW ADS DATA AVAILABLE!
void updateChannelData(){ 
  updateBoardData();
//  if(daisyPresent) {updateDaisyData();}
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
//      boardChannelDataInt[i] = (boardChannelDataInt[i]<<8) | inByte;  // int data goes here
    }
  }
  SPI4_CS_HIGH;        //  close SPI
  // need to convert 24bit to 32bit if using the filter
//  for(i=0; i<8; i++){     // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment 
//    if(BITREAD(boardChannelDataInt[i],23) == 1){ 
//      boardChannelDataInt[i] |= 0xFF000000;
//    }else{
//      boardChannelDataInt[i] &= 0x00FFFFFF;
//    }
//  }
  timeStampUs = timeFromStart100Us * 100;
  if(firstDataPacket == TRUE){firstDataPacket = FALSE;}
}

void updateDaisyData(){
    BYTE inByte;
    int ByteCounter = 0;
    int i;
    
    if(daisyPresent && !firstDataPacket){
      for(i=0; i<8; i++){  // shift and average the BYTE arrays
        lastDaisyChannelDataInt[i] = daisyChannelDataInt[i]; // remember the last samples
      }
    }

    csLow(DAISY_ADS);       //  open SPI
    for(i=0; i<3; i++){ 
      inByte = xfer(0x00);    //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
      daisyStat = (daisyStat << 8) | inByte;
    }
    for(i = 0; i<8; i++){
      int j;
      for(j=0; j<3; j++){   //  read 24 bits of channel data in 8 3 BYTE chunks
        inByte = xfer(0x00);
        daisyChannelDataRaw[ByteCounter] = inByte;  // raw data goes here
        ByteCounter++;
        daisyChannelDataInt[i] = (daisyChannelDataInt[i]<<8) | inByte; // int data goes here
      }
    }
    csHigh(DAISY_ADS);        //  close SPI
  // need to convert 24bit to 32bit
    for(i=0; i<8; i++){     // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment 
    if(BITREAD(daisyChannelDataInt[i],23) == 1){ 
      daisyChannelDataInt[i] |= 0xFF000000;
    }else{
      daisyChannelDataInt[i] &= 0x00FFFFFF;
    }
  }
  if(daisyPresent && !firstDataPacket){
    ByteCounter = 0;
    for(i=0; i<8; i++){   // average this sample with the last sample
      meanDaisyChannelDataInt[i] = (lastDaisyChannelDataInt[i] + daisyChannelDataInt[i])/2;
    }
    for(i=0; i<8; i++){  // place the average values in the meanRaw array
      int b;
      for(b=2; b>=0; b--){
        meanDaisyDataRaw[ByteCounter] = (meanDaisyChannelDataInt[i] >> (b*8)) & 0xFF;
        ByteCounter++;
      }
    }   
  }  
    
  if(firstDataPacket == TRUE){firstDataPacket = FALSE;}
}

// Stop the continuous data acquisition
void stopADS()  
{
  STOP(BOTH_ADS);     // stop the data acquisition
	Timer.DelayMs(1);   		
  SDATAC(BOTH_ADS);   // stop Read Data Continuous mode to communicate with ADS
	Timer.DelayMs(1);   
  isRunning = FALSE;
}


//write as binary each channel's data
void sendChannelData() 
{ 
  #if SHORT_PACKET == 0
    AdsPacket.length = 40; 
    uint2Bytes(PacketCounter,&count_byte[0]);       // Converting UINT32 PacketCounter to byte array
    uint2Bytes(timeStampUs,&timeStamp_byte[0]);    // Converting UINT32 timeStampUs to byte array

    memcpy(&AdsPacket.buffer[0], sync_byte, 4);            // Copy sync code to AdsPacket
    AdsPacket.buffer[4] = count_byte[3];         // Rearranging from little endian to big endian
    AdsPacket.buffer[5] = count_byte[2];
    AdsPacket.buffer[6] = count_byte[1];
    AdsPacket.buffer[7] = count_byte[0];

    AdsPacket.buffer[8] = timeStamp_byte[3];     // Rearranging from little endian to big endian
    AdsPacket.buffer[9] = timeStamp_byte[2];
    AdsPacket.buffer[10] = timeStamp_byte[1];
    AdsPacket.buffer[11] = timeStamp_byte[0];

    memcpy(&AdsPacket.buffer[12], &adsDataConversion[3], 24); // Copy RawData to AdsPacket
    memcpy(&AdsPacket.buffer[36], crc_byte, 4);            // Copy CRC to AdsPacket

    Uart.SendDataBuffer(UART4, &AdsPacket.buffer, AdsPacket.length);
    PacketCounter++;
    
  #elif SHORT_PACKET == 1
    AdsPacket.length = 30; 
    uint2Bytes(PacketCounter,&count_byte[0]);          // Converting UINT32 PacketCounter to byte array

    memcpy(&AdsPacket.buffer[0], sync_byte_short, 2);  // Copy sync code to AdsPacket
    AdsPacket.buffer[2] = count_byte[3];               // Rearranging from little endian to big endian
    AdsPacket.buffer[3] = count_byte[2];
    AdsPacket.buffer[4] = count_byte[1];
    AdsPacket.buffer[5] = count_byte[0];

    memcpy(&AdsPacket.buffer[6], &adsDataConversion[3], 24); // Copy RawData to AdsPacket

    Uart.SendDataBuffer(UART4, &AdsPacket.buffer, AdsPacket.length);
    PacketCounter++;
  #endif
}


BYTE ADS_getDeviceID(int targetSS) {      // simple hello world com check
  BYTE data = RREG(ID_REG,targetSS);
  if(verbosity)
  {            // verbosity otuput
    PrintToUart(UART4,"On Board ADS ID ");
    printHex(UART4, data);
    PrintlnToUart(UART4, " ");
  }
  return data;
}
  
//System Commands
void WAKEUP(int targetSS) {
    csLow(targetSS); 
    xfer(_WAKEUP);
    csHigh(targetSS); 
    Timer.DelayMs(1);     //must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}

void STANDBY(int targetSS) {    // only allowed to send WAKEUP after sending STANDBY
    csLow(targetSS);
    xfer(_STANDBY);
    csHigh(targetSS);
}

void RESET(int targetSS) {      // reset all the registers to default settings
    csLow(targetSS);
    xfer(_RESET);
    Timer.DelayMs(1);    //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
    csHigh(targetSS);
}

void START(int targetSS) {      //start data conversion 
    csLow(targetSS);
    xfer(_START);           // KEEP ON-BOARD AND ON-DAISY IN SYNC
    csHigh(targetSS);
}

void STOP(int targetSS) {     //stop data conversion
    csLow(targetSS);
    xfer(_STOP);            // KEEP ON-BOARD AND ON-DAISY IN SYNC
    csHigh(targetSS);
}

void RDATAC(int targetSS) {
    csLow(targetSS);
    xfer(_RDATAC);      // read data continuous
    csHigh(targetSS);
    Timer.DelayMs(1);
}
void SDATAC(int targetSS) {
    csLow(targetSS);
    xfer(_SDATAC);
    csHigh(targetSS);
    Timer.DelayMs(1);   //must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}


//  THIS NEEDS CLEANING AND UPDATING TO THE NEW FORMAT
void RDATA(int targetSS) {          //  use in Stop Read Continuous mode when DRDY goes low
  BYTE inByte;            //  to read in one sample of the channels
  int i;
    csLow(targetSS);        //  open SPI
    xfer(_RDATA);         //  send the RDATA command
    for(i=0; i<3; i++){   //  read in the status register and new channel data
      inByte = xfer(0x00);
      boardStat = (boardStat<<8) | inByte; //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
    }   
  if(targetSS == BOARD_ADS){    
    for(i = 0; i<8; i++){
      int j;
      for(j=0; j<3; j++){   //  read in the new channel data
        inByte = xfer(0x00);
        boardChannelDataInt[i] = (boardChannelDataInt[i]<<8) | inByte;
      }
    }
    for(i=0; i<8; i++){
      if(BITREAD(boardChannelDataInt[i],23) == 1){  // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment
        boardChannelDataInt[i] |= 0xFF000000;
      }else{
        boardChannelDataInt[i] &= 0x00FFFFFF;
      }
    }
  }else{
    for(i = 0; i<8; i++){
      int j;
      for(j=0; j<3; j++){   //  read in the new channel data
        inByte = xfer(0x00);
        daisyChannelDataInt[i] = (daisyChannelDataInt[i]<<8) | inByte;
      }
    }
    for(i=0; i<8; i++){
      if(BITREAD(daisyChannelDataInt[i],23) == 1){  // convert 3 BYTE 2's compliment to 4 BYTE 2's compliment
        daisyChannelDataInt[i] |= 0xFF000000;
      }else{
        daisyChannelDataInt[i] &= 0x00FFFFFF;
      }
    }
  }
   csHigh(targetSS);        //  close SPI
  
  
}

BYTE RREG(BYTE _address,int targetSS) 
{    //  reads ONE register at _address
  BYTE opcode1 = _address + 0x20;   //  RREG expects 001rrrrr where rrrrr = _address
  csLow(targetSS);        //  open SPI
  xfer(opcode1);          //  opcode1
  xfer(0x00);           //  opcode2
  regData[_address] = xfer(0x00);//  update mirror location with returned BYTE
  csHigh(targetSS);       //  close SPI 
  if (verbosity)
  {           //  verbosity output
    printRegisterName(_address);
    printHex(UART4, _address);
    PrintToUart(UART4,", ");
    printHex(UART4, regData[_address]);
    PrintToUart(UART4,", ");
    BYTE j;
    for(j = 0; j<8; j++)
    {
      PrintToUartDec(UART4,BITREAD(regData[_address], 7-j));
      if(j!=7) PrintToUart(UART4,", ");
    }
    
    PrintlnToUart(UART4, " ");
  }
  return regData[_address];     // return requested register value
}


// Read more than one register starting at _address
void RREGS(BYTE _address, BYTE _numRegistersMinusOne, int targetSS) {
  BYTE opcode1 = _address + 0x20;   //  RREG expects 001rrrrr where rrrrr = _address
  csLow(targetSS);        //  open SPI
  xfer(opcode1);          //  opcode1
  xfer(_numRegistersMinusOne);  //  opcode2
  int i;
  for(i = 0; i <= _numRegistersMinusOne; i++)
  {
    regData[_address + i] = xfer(0x00);   //  add register BYTE to mirror array
  }
  csHigh(targetSS);       //  close SPI
  if(verbosity)
  {            //  verbosity output
    int i;
    for(i = 0; i<= _numRegistersMinusOne; i++)
    {
      printRegisterName(_address + i);
      printHex(UART4, _address + i);
      PrintToUart(UART4,", ");
      printHex(UART4, regData[_address + i]);
      PrintToUart(UART4,", ");
      int j;
      for(j = 0; j<8; j++)
      {
        PrintToUartDec(UART4,BITREAD(regData[_address + i], 7-j));
        if(j!=7) PrintToUart(UART4,", ");
      }
      PrintlnToUart(UART4, " ");
      Timer.DelayMs(30);
    }
  }
}

void WREG(BYTE _address, BYTE _value, int target_SS) { //  Write ONE register at _address
  BYTE opcode1 = _address + 0x40;   //  WREG expects 010rrrrr where rrrrr = _address
  csLow(target_SS);        //  open SPI
  xfer(opcode1);          //  Send WREG command & address
  xfer(0x00);           //  Send number of registers to read -1
  xfer(_value);         //  Write the value to the register
  csHigh(target_SS);       //  close SPI
  regData[_address] = _value;     //  update the mirror array
  if(verbosity)
  {            //  verbosity output
    PrintToUart(UART4,"Register ");
    printHex(UART4, _address);
    PrintlnToUart(UART4," modified.");
  }
}

void WREGS(BYTE _address, BYTE _numRegistersMinusOne, int targetSS) {
  BYTE opcode1 = _address + 0x40;   //  WREG expects 010rrrrr where rrrrr = _address
  csLow(targetSS);        //  open SPI
  xfer(opcode1);          //  Send WREG command & address
  xfer(_numRegistersMinusOne);  //  Send number of registers to read -1 
  int i;
  for (i=_address; i <=(_address + _numRegistersMinusOne); i++)
  {
    xfer(regData[i]);     //  Write to the registers
  } 
  csHigh(targetSS);
  if(verbosity)
  {
    PrintToUart(UART4,"Registers ");
    printHex(UART4, _address); PrintToUart(UART4," to ");
    printHex(UART4, _address + _numRegistersMinusOne);
    PrintlnToUart(UART4," modified");
  }
}

void printRegisterName(BYTE _address) {
    switch(_address){ 
      case ID_REG: 
        PrintToUart(UART4,"ADS_ID, "); break;
      case CONFIG1:
        PrintToUart(UART4,"CONFIG1, "); break;
      case CONFIG2:
        PrintToUart(UART4,"CONFIG2, "); break;
      case CONFIG3:
        PrintToUart(UART4,"CONFIG3, "); break;
      case LOFF:
        PrintToUart(UART4,"LOFF, "); break;
      case CH1SET:
        PrintToUart(UART4,"CH1SET, "); break;
      case CH2SET:
        PrintToUart(UART4,"CH2SET, "); break;
      case CH3SET:
        PrintToUart(UART4,"CH3SET, "); break;
      case CH4SET:
        PrintToUart(UART4,"CH4SET, "); break;
      case CH5SET:
        PrintToUart(UART4,"CH5SET, "); break;
      case CH6SET:
        PrintToUart(UART4,"CH6SET, "); break;
      case CH7SET:
        PrintToUart(UART4,"CH7SET, "); break;
      case CH8SET:
        PrintToUart(UART4,"CH8SET, "); break;
      case BIAS_SENSP:
        PrintToUart(UART4,"BIAS_SENSP, "); break;
      case BIAS_SENSN:
        PrintToUart(UART4,"BIAS_SENSN, "); break;
      case LOFF_SENSP:
        PrintToUart(UART4,"LOFF_SENSP, "); break;
      case LOFF_SENSN:
        PrintToUart(UART4,"LOFF_SENSN, "); break;
      case LOFF_FLIP:
        PrintToUart(UART4,"LOFF_FLIP, "); break;
      case LOFF_STATP:
        PrintToUart(UART4,"LOFF_STATP, "); break;
      case LOFF_STATN:
        PrintToUart(UART4,"LOFF_STATN, "); break;
      case GPIO:
        PrintToUart(UART4,"GPIO, "); break;
      case MISC1:
        PrintToUart(UART4,"MISC1, "); break;
      case MISC2:
        PrintToUart(UART4,"MISC2, "); break;
      case CONFIG4:
        PrintToUart(UART4,"CONFIG4, "); break;
      default: 
        break;
    }
}



//
//void updateBoardData(){
//  BYTE inByte;
//  INT32 ByteCounter = 0;
//
//  if(daisyPresent && !firstDataPacket){
//    for(int i=0; i<8; i++){  // shift and average the byte arrays
//      lastBoardChannelDataInt[i] = boardChannelDataInt[i]; // remember the last samples
//    }
//  }
//  csLow(BOARD_ADS);       //  open SPI
//  for(int i=0; i<3; i++){ 
//    inByte = xfer(0x00);    //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
//    boardStat = (boardStat << 8) | inByte;
//  }
//  for(int i = 0; i<8; i++){
//    for(int j=0; j<3; j++){   //  read 24 bits of channel data in 8 3 byte chunks
//      inByte = xfer(0x00);
//      boardChannelDataRaw[byteCounter] = inByte;  // raw data goes here
//      byteCounter++;
//      boardChannelDataInt[i] = (boardChannelDataInt[i]<<8) | inByte;  // int data goes here
//    }
//  }
//  csHigh(BOARD_ADS);        //  close SPI
//  // need to convert 24bit to 32bit if using the filter
//  for(int i=0; i<8; i++){     // convert 3 byte 2's compliment to 4 byte 2's compliment 
//    if(bitRead(boardChannelDataInt[i],23) == 1){ 
//      boardChannelDataInt[i] |= 0xFF000000;
//    }else{
//      boardChannelDataInt[i] &= 0x00FFFFFF;
//    }
//  }
//  if(daisyPresent && !firstDataPacket){
//    byteCounter = 0;
//    for(int i=0; i<8; i++){   // take the average of this and the last sample
//      meanBoardChannelDataInt[i] = (lastBoardChannelDataInt[i] + boardChannelDataInt[i])/2;
//    }
//    for(int i=0; i<8; i++){  // place the average values in the meanRaw array
//      for(int b=2; b>=0; b--){
//        meanBoardDataRaw[byteCounter] = (meanBoardChannelDataInt[i] >> (b*8)) & 0xFF;
//        byteCounter++;
//      }
//    }    
//  }
//    
//  if(firstDataPacket == true){firstDataPacket = false;}
//}
//
//
//INT32 IsDataReady(void)
//{
//  return Port.B.ReadBits(BIT_13);
//}
//
///**************************************************************
// * Function name  : TemplateFunction
// * Purpose        : Give a template for developpers to start from.
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void InitAds1299(void)
//{
//// recommended power up sequence requiers >Tpor (~32mS)	
//    Timer.DelayMs(1);
//    ADS_NO_RESET;
//    Timer.DelayMs(1);
//    ADS_RESET;
//    Timer.DelayMs(1);
//    resetADS(BOARD_ADS); // reset the on-board ADS registers, and stop DataContinuousMode
//    Timer.DelayMs(1);
//    AdsWREG(CONFIG1,0xB6); // tell on-board ADS to output its clk, set the data rate to 250SPS
//    Timer.DelayMs(1);
//    
//    WREG(CONFIG1,0x96); // turn off clk output if no daisy present
//    numChannels = 8;    // expect up to 8 ADS channels 
//
//    // DEFAULT CHANNEL SETTINGS FOR ADS
//    defaultChannelSettings[POWER_DOWN] = NO;        // on = NO, off = YES
//    defaultChannelSettings[GAIN_SET] = ADS_GAIN24;     // Gain setting
//    defaultChannelSettings[INPUT_TYPE_SET] = ADSINPUT_NORMAL;// input muxer setting
//    defaultChannelSettings[BIAS_SET] = YES;    // add this channel to bias generation
//    defaultChannelSettings[SRB2_SET] = YES;       // connect this P side to SRB2
//    defaultChannelSettings[SRB1_SET] = NO;        // don't use SRB1
//
//    for(int i=0; i<numChannels; i++){
//      for(int j=0; j<6; j++){
//        channelSettings[i][j] = defaultChannelSettings[j];  // assign default settings
//      }
//      useInBias[i] = TRUE;    // keeping track of Bias Generation
//      useSRB2[i] = TRUE;      // keeping track of SRB2 inclusion
//    }
//    boardUseSRB1 = FALSE;
//
//    writeChannelSettings(); // write settings to the on-board and on-daisy ADS if present
//
//    WREG(CONFIG3,0b11101100); 
//    Timer.DelayMs(1);  // enable internal reference drive and etc.
//    for(int i=0; i<numChannels; i++){  // turn off the impedance measure signal
//      leadOffSettings[i][PCHAN] = OFF;
//      leadOffSettings[i][NCHAN] = OFF;
//    }
//    verbosity = FALSE;      // when verbosity is true, there will be Serial feedback
//    firstDataPacket = TRUE;
//}
//
///**************************************************************
// * Function name  : AdsWAKEUP
// * Purpose        : Wake up from stand by Mode
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void AdsWAKEUP(void) 
//{
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, _WAKEUP);
//  SPI4_CS_HIGH;
//  Timer.DelayMs(1);
//}
//
///**************************************************************
// * Function name  : AdsSTANDBY
// * Purpose        : Enter Standby Mode
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void AdsSTANDBY(void) 
//{    // only allowed to send WAKEUP after sending STANDBY
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, _STANDBY);
//  SPI4_CS_HIGH;
//}
//
///**************************************************************
// * Function name  : AdsRESET
// * Purpose        : Reset the device
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void AdsRESET(void) 
//{      // reset all the registers to default settings
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, _RESET);
//  SPI4_CS_HIGH;
//  Timer.DelayMs(1);
//}
//
///**************************************************************
// * Function name  : AdsSTART
// * Purpose        : Start and Restart (synch) conversions
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void AdsSTART(void) 
//{      //start data conversion 
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, _START);
//  SPI4_CS_HIGH;  
//}
//
///**************************************************************
// * Function name  : AdsSTOP
// * Purpose        : Stop Conversion.
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void AdsSTOP(void) 
//{     //stop data conversion
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, _STOP);
//  SPI4_CS_HIGH;  
//}
//
///**************************************************************
// * Function name  : AdsRDATAC
// * Purpose        : Start Read Data Continuously Mode
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void AdsRDATAC(void) 
//{
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, _RDATAC);
//  SPI4_CS_HIGH;  
//  Timer.DelayMs(1);  
//}
//
///**************************************************************
// * Function name  : AdsSDATAC
// * Purpose        : Stop Read Data Continuously Mode
// * Arguments      : None.
// * Returns        : None.
// *************************************************************/
//void AdsSDATAC(void) 
//{
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, _SDATAC);
//  SPI4_CS_HIGH;  
//  Timer.DelayMs(1);  
//}
//
///**************************************************************
// * Function name  : AdsRREG
// * Purpose        : Reads register at address, returns its value
// * Arguments      : BYTE address
// * Returns        : Value of register specified as argument
// *************************************************************/
//UINT32 AdsRREG(BYTE address) 
//  {    
//    UINT32 regValue = 0; 
//    BYTE opcode = address + 0x20;   // RREG expects 001rrrrr where rrrrr = _address
//    SPI4_CS_HIGH;
//    while(Spi.IsSpiBusy(SPI4));
//    SPI4_CS_LOW;                //  open SPI
//    SpiTransfer(SPI4, opcode);  // Send opcode
//    SpiTransfer(SPI4, 0x00);    // Number of registers to read -1 
//    regValue = SpiTransfer(SPI4, 0x00); // Send dummy data and return Rx buffer contents
//    SPI4_CS_HIGH;  
//    return regValue;     // return requested register value
//  }
//
///**************************************************************
// * Function name  : AdsWREG
// * Purpose        : Writes register at _address
// * Arguments      : BYTE address, BYTE value
// * Returns        : None.
// *************************************************************/
//void AdsWREG(BYTE address, BYTE regValue) 
//{ 
//  BYTE opcode = address + 0x40;   //  WREG expects 010rrrrr where rrrrr = _address
//  SPI4_CS_HIGH;
//  while(Spi.IsSpiBusy(SPI4));
//  SPI4_CS_LOW;
//  SpiTransfer(SPI4, opcode);
//  SpiTransfer(SPI4, 0x00);    // Number of registers to write -1
//  SpiTransfer(SPI4, regValue);
//  SPI4_CS_HIGH;
//}


