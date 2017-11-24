//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateFunctions.c
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the C file for the functions of the state machine of
//           the system.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\StateFunctions.h"
#include "..\headers\Ads1299.h"
#include "..\headers\Interrupts.h"

extern BOOL is_running;    // this flag is set in serialEvent on reciept of ascii prompt
// these are used to change individual channel settings from PC
extern char currentChannelToSet;    // keep track of what channel we're loading settings for
extern BOOL getChannelSettings; // used to receive channel settings command
extern int channelSettingsCounter; // used to retrieve channel settings from serial port
extern int leadOffSettingsCounter;
extern BOOL getLeadOffSettings;
extern int outputType;  // default to 8 channels

extern BOOL serialTrigger;
extern BOOL triggerTimer;
extern volatile UINT32 timeFromStartMs;

//==============================================================================
// State Machine private functions prototypes
//==============================================================================

/**************************************************************
 * Function name  : uint2Bytes
 * Purpose        : Converts UINT32 to byte array (little endian)
 * Arguments      : UINT32 val, BYTE* bytes_array
 * Returns        : void
 *************************************************************/
void uint2Bytes(UINT32 val, BYTE* bytes_array){
  // Create union of shared memory space
  union {
    UINT32 uint_variable;
    BYTE temp_array[4];
  } u;
  // Overite bytes of union with float variable
  u.uint_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.temp_array, 4);
}

/**************************************************************
 * Function name  : SpiTransfer
 * Purpose        : Sends data over SPI specified in argument,
 *                  and empties Rx buffer
 * Arguments      : SpiNum_t numSpi, const UINT32 data
 * Returns        : SPI Rx Buffer (UINT32)
 *************************************************************/
UINT32 SpiTransfer(SpiNum_t numSpi, const UINT32 data)
{
  Spi.SendCharacter(numSpi, data);
  while(Spi.IsSpiBusy(numSpi));
  return Spi.GetCharacter(numSpi);
}

/**************************************************************
 * Function name  : PrintToUart
 * Purpose        : Prints char argument to UART argument
 * Arguments      : UartModule_t uartModuleId, char *string
 * Returns        : error INT32
 *************************************************************/
INT32 PrintToUart(UartModule_t uartModuleId, char *string)
{
  INT32 err = 0;
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, string);
    do
    {
      err = Uart.PutTxFifoBuffer(uartModuleId, &buffer);
    } while (err < 0);
}

INT32 PrintToUartChar(UartModule_t uartModuleId, char c)
{
  INT32 err = 0;
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, "%c", c);
    do
    {
      err = Uart.PutTxFifoBuffer(uartModuleId, &buffer);
    } while (err < 0);
}

INT32 PrintToUartHex(UartModule_t uartModuleId, UINT32 val)
{
  INT32 err = 0;
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, "%X", val);
    do
    {
      err = Uart.PutTxFifoBuffer(uartModuleId, &buffer);
    } while (err < 0);
}

INT32 PrintToUartDec(UartModule_t uartModuleId, UINT32 val)
{
  INT32 err = 0;
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, "%d", val);
  do
  {
    err = Uart.PutTxFifoBuffer(uartModuleId, &buffer);
  } while (err < 0);  
}

INT32 PrintlnToUart(UartModule_t uartModuleId, char *string)
{
  INT32 err = 0;
  sUartLineBuffer_t buffer;
    buffer.length = sprintf(buffer.buffer, string);
    do
    {
      err = Uart.PutTxFifoBuffer(uartModuleId, &buffer);
    } while (err < 0);
  buffer.length = sprintf(buffer.buffer, "\r\n");
  do
    {
      err = Uart.PutTxFifoBuffer(uartModuleId, &buffer);
    } while (err < 0);
}

INT32 printHex(UartModule_t uartModuleId, BYTE _data){
  PrintToUart(uartModuleId, "0x");
  if(_data < 0x10) PrintToUart(uartModuleId, "0");
  PrintToUartHex(uartModuleId,_data);
}

BYTE constrain(BYTE x, BYTE a, BYTE b)
{
  if(x < a) 
  {
    return a;
  }
  else if(b < x) 
  {
    return b;
  }
   else return x;
}

// some variables to help find 'burger protocol' commands
int plusCounter = 0;
char testChar;
unsigned long commandTimer;
sUartLineBuffer_t buffer = { .buffer = {0} ,.length = 0 }; 
INT32 err;

void eventSerial()
{
  while(Uart.Var.oIsRxDataAvailable[UART4])
  { 
    err = Uart.GetRxFifoBuffer(UART4, &buffer, FALSE);
    char inChar = (char)buffer.buffer[0];
    
    if(getChannelSettings)
    { 
      loadChannelSettings(inChar); // 'x' expect channel setting parameters  
    }
    else if(getLeadOffSettings)
    {  
      loadLeadOffSettings(inChar); // 'z' expect lead-off setting parameters
    }
    else
    {
      getCommand(inChar);    // decode the command
    }  
  }
}

void getCommand(char token){
    switch (token){
      case 'q':
        WREG(CONFIG1,0xB6,BOARD_ADS); break; // Sampling rate is 250Hz
      case 'w':
        WREG(CONFIG1,0xB5,BOARD_ADS); break; // Sampling rate is 500Hz
      case 'e':
        WREG(CONFIG1,0xB4,BOARD_ADS); break; // Sampling rate is 1000Hz
      case 'r':
        WREG(CONFIG1,0xB3,BOARD_ADS); break; // Sampling rate is 2000Hz
      case 't':
        WREG(CONFIG1,0xB2,BOARD_ADS); break; // Sampling rate is 4000Hz
      case 'y':
        WREG(CONFIG1,0xB1,BOARD_ADS); break; // Sampling rate is 8000Hz
      case 'u':
        WREG(CONFIG1,0xB0,BOARD_ADS); break; // Sampling rate is 16000Hz
      

// TEST SIGNAL CONTROL COMMANDS
      case '0':
        activateAllChannelsToTestCondition(ADSINPUT_SHORTED,ADSTESTSIG_NOCHANGE,ADSTESTSIG_NOCHANGE); break;
      case '-':
        activateAllChannelsToTestCondition(ADSINPUT_TESTSIG,ADSTESTSIG_AMP_1X,ADSTESTSIG_PULSE_SLOW); break;
      case '=':
        activateAllChannelsToTestCondition(ADSINPUT_TESTSIG,ADSTESTSIG_AMP_1X,ADSTESTSIG_PULSE_FAST); break;
      case 'p':
        activateAllChannelsToTestCondition(ADSINPUT_TESTSIG,ADSTESTSIG_AMP_2X,ADSTESTSIG_DCSIG); break;
      case '[':
        activateAllChannelsToTestCondition(ADSINPUT_TESTSIG,ADSTESTSIG_AMP_2X,ADSTESTSIG_PULSE_SLOW); break;
      case ']':
        activateAllChannelsToTestCondition(ADSINPUT_TESTSIG,ADSTESTSIG_AMP_2X,ADSTESTSIG_PULSE_FAST); break;

// CHANNEL SETTING COMMANDS
      case 'x':  // expect 6 parameters
        if(!is_running) 
        {
//          PrintlnToUart(UART4, "ready to accept new channel settings");
        }
        channelSettingsCounter = 0;
        getChannelSettings = TRUE; break;
      case 'X':  // latch channel settings
        if(!is_running) 
        {
//          PrintlnToUart(UART4, "updating channel settings");
        }
        writeChannelSettings_maintainRunningState(currentChannelToSet); break;
      case 'd':  // reset all channel settings to default
        if(!is_running) 
        {
//          PrintlnToUart(UART4, "updating channel settings to default");
        }
        setChannelsToDefaultSetting(); break;
      case 'D':  // report the default settings
        sendDefaultChannelSettings(); break;

// LEAD OFF IMPEDANCE DETECTION COMMANDS
      case 'z':  // expect 2 parameters
        if(!is_running) {PrintlnToUart(UART4, "ready to accept new impedance detect settings");}
        leadOffSettingsCounter = 0;  // reset counter
        getLeadOffSettings = TRUE;
        break;
      case 'Z':  // latch impedance parameters
        if(!is_running) {PrintlnToUart(UART4, "updating impedance detect settings");}
        changeChannelLeadOffDetect_maintainRunningState(currentChannelToSet);
        break;

// STREAM DATA AND FILTER COMMANDS
      case 'b':  // stream data
        startRunning(outputType);       // turn on the fire hose
        break;
     case 's':  // stop streaming data
        stopRunning();
        break;

//  INITIALIZE AND VERIFY
      case 'v':
         startFromScratch();
         break;
      default:
        break;
     }
  }// end of getCommand

void loadChannelSettings(char c){
  if(channelSettingsCounter == 0)
  {  // if it's the first BYTE in this channel's array, this BYTE is the channel number to set
    currentChannelToSet = getChannelNumber(c); // we just got the channel to load settings into (shift number down for array usage)
    channelSettingsCounter++;
    return;
  }
//  setting BYTEs are in order: POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET
  c -= '0';
  if(channelSettingsCounter-1 == GAIN_SET)
  { 
    c <<= 4; 
  }
  channelSettings[currentChannelToSet][channelSettingsCounter-1] = c;
  channelSettingsCounter++;
  if(channelSettingsCounter == 7)
  {  // 1 currentChannelToSet, plus 6 channelSetting parameters
    getChannelSettings = FALSE;
  }
}

void writeChannelSettings_maintainRunningState(char chan){
  BOOL is_running_when_called = is_running;
  int cur_outputType = outputType;
  stopRunning();                   //must stop running to change channel settings
  writeChannelSettings(chan+1);    // change the channel settings on ADS
  if (is_running_when_called == TRUE) {
    startRunning(cur_outputType);  //restart, if it was running before
  }
}

void setChannelsToDefaultSetting(){
  BOOL is_running_when_called = is_running;
  int cur_outputType = outputType;
  stopRunning();  //must stop running to change channel settings

  setChannelsToDefault();   // default channel settings

  if (is_running_when_called == TRUE) {
    startRunning(cur_outputType);  //restart, if it was running before
  }
}

void loadLeadOffSettings(char c){
   if(leadOffSettingsCounter == 0){  // if it's the first BYTE in this channel's array, this BYTE is the channel number to set
    currentChannelToSet = getChannelNumber(c); // we just got the channel to load settings into (shift number down for array usage)
    if(!is_running) PrintToUart(UART4, "changing LeadOff settings for channel "); PrintToUartDec(UART4, currentChannelToSet+1); PrintlnToUart(UART4, " ");
    leadOffSettingsCounter++;
    return;
  }
//  setting BYTEs are in order: PCHAN, NCHAN
  if(!is_running) {
    PrintToUart(UART4, "load setting "); PrintToUartDec(UART4, leadOffSettingsCounter-1);
    PrintToUart(UART4, " with "); PrintToUartChar(UART4, c); PrintlnToUart(UART4, " ");
  }
  c -= '0';
  leadOffSettings[currentChannelToSet][leadOffSettingsCounter-1] = c;
  leadOffSettingsCounter++;
  if(leadOffSettingsCounter == 3){  // 1 currentChannelToSet, plus 2 leadOff setting parameters
    if(!is_running) PrintToUart(UART4, "done receiving leadOff settings for channel ");PrintToUartDec(UART4, currentChannelToSet+1); PrintlnToUart(UART4, " ");
    getLeadOffSettings = FALSE; // release the serial COM
  }
}

char getChannelNumber(char n){
  if(n > '0' && n < '9'){
    n -= '1';
  }
  switch(n){
    case 'Q':
      n = 0x08; break;
    case 'W':
      n = 0x09; break;
    case 'E':
      n = 0x0A; break;
    case 'R':
      n = 0x0B; break;
    case 'T':
      n = 0x0C; break;
    case 'Y':
      n = 0x0D; break;
    case 'U':
      n = 0x0E; break;
    case 'I':
      n = 0x0F; break;
    default: break;
  }
  return n;
}

void changeChannelState_maintainRunningState(BYTE chan, int start)
{
  BOOL is_running_when_called = is_running;
  int cur_outputType = outputType;

  //must stop running to change channel settings
  stopRunning();
  if (start == 1) {
    activateChannel(chan);
  } else if (start == 0){
    deactivateChannel(chan);
  }
  //restart, if it was running before
  if (is_running_when_called == TRUE) {
    startRunning(cur_outputType);
  }
}

void activateAllChannelsToTestCondition(BYTE testInputCode, BYTE amplitudeCode, BYTE freqCode)
{
  BOOL is_running_when_called = is_running;
  int cur_outputType = outputType;
  //must stop running to change channel settings
  stopRunning(); Timer.DelayMs(10);

  //set the test signal to the desired state
  configureInternalTestSignal(amplitudeCode,freqCode);
  //change input type settings for all channels
  changeInputType(testInputCode);

  //restart, if it was running before
  if (is_running_when_called == TRUE) {
    startRunning(cur_outputType);
  }
}

int changeChannelLeadOffDetect_maintainRunningState(char chan)
{
  BOOL is_running_when_called = is_running;
  int cur_outputType = outputType;

  //must stop running to change channel settings
  stopRunning();

  changeChannelLeadOffDetect(chan);

  //restart, if it was running before
  if (is_running_when_called == TRUE) {
    startRunning(cur_outputType);
  }
}

void sendDefaultChannelSettings(){
  BOOL is_running_when_called = is_running;
  int cur_outputType = outputType;

  reportDefaultChannelSettings();
  Timer.DelayMs(10);

  //restart, if it was running before
  if (is_running_when_called == TRUE) {
    startRunning(cur_outputType);
  }
}

BOOL stopRunning(void) {
  if(is_running){
    stopStreaming();  // stop the data acquisition, turn off accelerometer
    is_running = FALSE;
    }
    return is_running;
  }

BOOL startRunning(int OUT_TYPE) {
  if(!is_running){
    outputType = OUT_TYPE;
    startStreaming();  // start the data acquisition
    is_running = TRUE;
  }
    return is_running;
}

void printRegisters(){

  if(!is_running){
    // print the ADS
    printAllRegisters();
    Timer.DelayMs(20);
  }

}

void startFromScratch(){
  if(!is_running){
    initialize_ads();
    Timer.DelayMs(500);
    PrintlnToUart(UART4, "CochlEEG - CRITIAS ETS/r/n");
    configureLeadOffDetection(LOFF_MAG_6NA, LOFF_FREQ_31p2HZ);
    PrintToUart(UART4, "On Board ADS1299 Device ID: 0x"); PrintToUartHex(UART4, ADS_getDeviceID(ON_BOARD)); PrintlnToUart(UART4, " ");
  }
}


UINT32 millis(){
  return timeFromStartMs;
}