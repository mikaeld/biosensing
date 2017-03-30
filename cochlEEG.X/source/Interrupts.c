//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// cochlEEG - CRITIAS ETSMTL
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : Interrupts.c
// Author  : Mikael Ducharme
// Date    : 2016-01-28
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This file declares all interrupt subroutines used.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Interrupts can be defined as iplnauto, iplnsrs and iplnsoft,
//           where n can be from 0 to 7. 7 is the highest priority, while 1 is
//           the lowest. A priority of 0 will disable the interrupt.
//           Subpriorities can go from (lowest to highest) 0 to 3.
//           SRS mode uses the shadow registers, soft uses the software
//           registers and auto is an automatic assignment.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\Interrupts.h"

//==============================================================================
// VARIABLES
//==============================================================================
volatile UINT32 timeFromStartMs = 0;
volatile BOOL   oDataAvailableFlag = FALSE;
volatile UINT32 timeFromStart10Us = 0;
volatile BOOL		oDmaSpiTxIntFlag = 0;			// flag used in interrupts, signal that DMA transfer ended
volatile BOOL		oDmaSpiRxIntFlag = 0;			// flag used in interrupts, signal that DMA transfer ended
volatile BOOL   oDmaUartTxIntFlag = 0;  
volatile UINT32 timeStampUs = 0;


/*******************************************************************************
 ***********************                               *************************
 ********************            DMA INTERRUPTS           **********************
 ***********************                               *************************
 *******************************************************************************/
void __ISR(_DMA1_VECTOR, IPL5SOFT) Dma1InterruptHandler(void)
{
	int	evFlags;				// event flags when getting the interrupt

	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL1));	// acknowledge the INT controller, we're servicing int

	evFlags=DmaChnGetEvFlags(DMA_CHANNEL1);	// get the event flags

    if(evFlags&DMA_EV_BLOCK_DONE)
    { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
    	oDmaSpiTxIntFlag=1;
      DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
    }
  SPI4_CS_HIGH;
  oDataAvailableFlag = TRUE;
}

void __ISR(_DMA2_VECTOR, IPL5SOFT) Dma2InterruptHandler(void)
{
	int	evFlags;				// event flags when getting the interrupt

	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL2));	// acknowledge the INT controller, we're servicing int

	evFlags=DmaChnGetEvFlags(DMA_CHANNEL2);	// get the event flags

    if(evFlags&DMA_EV_BLOCK_DONE)
    { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
    	oDmaSpiRxIntFlag=1;
      DmaChnClrEvFlags(DMA_CHANNEL2, DMA_EV_BLOCK_DONE);
    }
//  SPI4_CS_HIGH;
//  oDataAvailableFlag = TRUE;
}

void __ISR(_DMA3_VECTOR, IPL5SOFT) Dma3InterruptHandler(void)
{
	int	evFlags;				// event flags when getting the interrupt

	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL3));	// release the interrupt in the INT controller, we're servicing int

	evFlags=DmaChnGetEvFlags(DMA_CHANNEL3);	// get the event flags

    if(evFlags&DMA_EV_BLOCK_DONE)
    { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
	oDmaUartTxIntFlag=1;
	DmaChnClrEvFlags(DMA_CHANNEL3, DMA_EV_BLOCK_DONE);
    }
}

/*******************************************************************************
 ***********************                               *************************
 ********************       CHANGENOTICE INTERRUPTS       **********************
 ***********************                               *************************
 *******************************************************************************/
void __ISR( _CHANGE_NOTICE_VECTOR, ipl7auto) ChangeNoticeInterruptHandler(void)
{
  if(!Port.C.ReadBits(BIT_14))
  {
    SPI4_CS_LOW;  
    DmaChnStartTxfer(DMA_CHANNEL1,DMA_WAIT_NOT,0);
    DmaChnEnable(DMA_CHANNEL2); 
    oDataAvailableFlag = TRUE;
    timeStampUs = timeFromStart10Us * 10;
    LED_EEGACQ_TOGGLE;
  }
  else
  {
    oDataAvailableFlag = FALSE;
  }
  mCNClearIntFlag(); // Clear CN interrupt flag
}

/*******************************************************************************
 ***********************                               *************************
 ********************           TIMER INTERRUPTS          **********************
 ***********************                               *************************
 *******************************************************************************/

//=============================================
// Configure the Timer 1 interrupt handler
//=============================================
void __ISR(_TIMER_1_VECTOR, T1_INTERRUPT_PRIORITY) Timer1InterruptHandler(void)
{
  
  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[0]++;

  mT1ClearIntFlag();
}

//=============================================
// Configure the Timer 2 interrupt handler
//=============================================
void __ISR(_TIMER_2_VECTOR, T2_INTERRUPT_PRIORITY) Timer2InterruptHandler(void)
{
  /*
   * DEVELOPPER CODE HERE
   */

  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[1]++;

  mT2ClearIntFlag();
}

//=============================================
// Configure the Timer 3 interrupt handler
//=============================================
void __ISR(_TIMER_3_VECTOR, T3_INTERRUPT_PRIORITY) Timer3InterruptHandler(void)
{
  /*
   * DEVELOPPER CODE HERE
   */

  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[2]++;

  mT3ClearIntFlag();
}

//=============================================
// Configure the Timer 4 interrupt handler
//=============================================
void __ISR(_TIMER_4_VECTOR, T4_INTERRUPT_PRIORITY) Timer4InterruptHandler(void)
{
  timeFromStart10Us++;
  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[3]++;

  mT4ClearIntFlag();
}

//=============================================
// Configure the Timer 5 interrupt handler
//=============================================
void __ISR(_TIMER_5_VECTOR, T5_INTERRUPT_PRIORITY) Timer5InterruptHandler(void)
{
  timeFromStartMs++;


  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[4]++;

  mT5ClearIntFlag();
}


/*******************************************************************************
 ***********************                               *************************
 ********************           UART INTERRUPTS           **********************
 ***********************                               *************************
 *******************************************************************************/

//=============================================
// Configure the UART 5 interrupt handler
//=============================================

void __ISR(_UART_4_VECTOR, U4_INTERRUPT_PRIORITY) Uart4InterruptHandler(void)
{
  UINT8  i
        ,iMax   // Read/write max 8 bytes/interrupt
        ,data   // used in UartFifoWrite/Read functions
        ;

	// TX interrupt handling
  //===========================================================

  if ( INTGetEnable ( INT_SOURCE_UART_TX(UART4) ) )               // If TX interrupts enabled
  {
    if ( INTGetFlag ( INT_SOURCE_UART_TX(UART4) ) )               // If TX interrupt occured
    {
      if ( UARTTransmitterIsReady(UART4) && !Uart.Var.uartTxFifo[UART4].bufEmpty )  // If TX buffer is ready to receive data and the user's TX buffer is not empty
      {
        if (Uart.Var.uartTxFifo[UART4].lineBuffer.length < 8)     // Write max 8 bytes/interrupt
        {
          iMax = Uart.Var.uartTxFifo[UART4].lineBuffer.length;
        }
        else
        {
          iMax = 8;
        }

        for (i = 0; i < iMax; i++)
        {
          UartFifoRead((void *) &Uart.Var.uartTxFifo[UART4], &data);  // Copy from user
          U4TXREG = data;                                         // Put data in PIC32's TX buffer
        }
      }

      if (Uart.Var.uartTxFifo[UART4].bufEmpty)                    // If User's TX buffer is empty
      {
        Uart.DisableTxInterrupts(UART4);                          // Disable TX interrupts
      }

      INTClearFlag(INT_SOURCE_UART_TX(UART4));                    // Clear the TX interrupt Flag
    }
  }
  //===========================================================
  

	// RX interrupt handling
  //===========================================================
  if ( INTGetEnable ( INT_SOURCE_UART_RX(UART4) ) )               // If RX interrupts enabled
  {
    if ( INTGetFlag ( INT_SOURCE_UART_RX(UART4) ) )               // If RX interrupt occured
    {
      i = 0;
      iMax = 8;                                                   // Read max 8 bytes/interrupt
      while (   UARTReceivedDataIsAvailable(UART4)                // While RX data available
            && !Uart.Var.uartRxFifo[UART4].bufFull                // and user's RX buffer not full
            && (i < iMax)                                         // and under 8 bytes read
            )
      { // while ^
        data = UARTGetDataByte(UART4);                            // Get data for PIC32's RX FIFO buffer and copy it to user (next line)
        if ( UartFifoWrite((void *) &Uart.Var.uartRxFifo[UART4], &data) < 0 ) // If copy to user did not work
        {
          break;                                                  // Exit while loop
        }
        i++;
      } // end while

      if (!Uart.Var.uartRxFifo[UART4].bufEmpty)                   // If there is data in the user's RX buffer
      {
        Uart.Var.oIsRxDataAvailable[UART4] = 1;                   // Set according flag
      }

      INTClearFlag (INT_SOURCE_UART_RX(UART4) );                  // Clear the RX interrupt Flag

    }
	}
  //===========================================================
}
//=============================================


/*******************************************************************************
 ***********************                               *************************
 ********************           ADC INTERRUPT             **********************
 ***********************                               *************************
 *******************************************************************************/

//=============================================
// Configure the ADC interrupt handler
//=============================================
void __ISR(_ADC_VECTOR, ADC_INTERRUPT_PRIO) AdcInterruptHandler(void)
{
//  Port.B.ToggleBits(BIT_13);
  Adc.Read();               // Read the enabled channels and puts them in Adc.Var.adcReadValues[]
  //oNewAdcAvailable = 1;
  INTClearFlag(INT_AD1);    // Clear the ADC conversion done interrupt Flag
}
//=============================================


/*******************************************************************************
 ***********************                               *************************
 ********************      INPUT CAPTURE INTERRUPTS       **********************
 ***********************                               *************************
 ******************************************************************************/

//================================================
// Configure the InputCapture 1 interrupt handler
//================================================
void __ISR(_INPUT_CAPTURE_1_VECTOR, IC1_INT_PRIORITY) InputCapture1InterruptHandler(void)
{
  /*
   * DEVELOPPER CODE HERE
   */

  // Get the timer used by this Input Capture
  TimerNum_t numTimer = InputCapture.Var.timerUsed[IC1];

  // Wait for capture data to be ready
  while(!(InputCapture.IsCaptureReady(IC1)));

  // Update values of timer overflows
  InputCapture.Var.previousTimerOverflows[IC1] = InputCapture.Var.currentTimerOverflows[IC1];
  InputCapture.Var.currentTimerOverflows[IC1]  = Timer.ReadOverflows(numTimer);

  // Store the timer value of the capture event
  InputCapture.Var.previousCaptureCountValue[IC1] = InputCapture.Var.currentCaptureCountValue[IC1];
  InputCapture.Var.currentCaptureCountValue [IC1] = InputCapture.ReadCapture(IC1);

//  oCapture = 1;   // Flag that tells that a new Capture event occured

  // Clear the interrupt flag
  INTClearFlag(INT_IC1);
}
//================================================

//================================================
// Configure the InputCapture 2 interrupt handler
//================================================
void __ISR(_INPUT_CAPTURE_2_VECTOR, IC2_INT_PRIORITY) InputCapture2InterruptHandler(void)
{
  /*
   * DEVELOPPER CODE HERE
   */
  Port.B.ToggleBits(BIT_13);

  // Get the timer used by this Input Capture
  TimerNum_t numTimer = InputCapture.Var.timerUsed[IC2];

  // Wait for capture data to be ready
  while(!(InputCapture.IsCaptureReady(IC2)));

  // Update values of timer overflows
  InputCapture.Var.previousTimerOverflows[IC2] = InputCapture.Var.currentTimerOverflows[IC2];
  InputCapture.Var.currentTimerOverflows[IC2]  = Timer.ReadOverflows(numTimer);

  // Store the timer value of the capture event
  InputCapture.Var.previousCaptureCountValue[IC2] = InputCapture.Var.currentCaptureCountValue[IC2];
  InputCapture.Var.currentCaptureCountValue [IC2] = InputCapture.ReadCapture(IC2);

//  oCapture = 1;   // Flag that tells that a new Capture event occured

  // Clear the interrupt flag
  INTClearFlag(INT_IC2);
}
//================================================

//================================================
// Configure the InputCapture 3 interrupt handler
//================================================
void __ISR(_INPUT_CAPTURE_3_VECTOR, IC3_INT_PRIORITY) InputCapture3InterruptHandler(void)
{

  // Get the timer used by this Input Capture
  TimerNum_t numTimer = InputCapture.Var.timerUsed[IC3];

  // Wait for capture data to be ready
  while(!(InputCapture.IsCaptureReady(IC3)));

  // Update values of timer overflows
  InputCapture.Var.previousTimerOverflows[IC3] = InputCapture.Var.currentTimerOverflows[IC3];
  InputCapture.Var.currentTimerOverflows[IC3]  = Timer.ReadOverflows(numTimer);

  // Store the timer value of the capture event
  InputCapture.Var.previousCaptureCountValue[IC3] = InputCapture.Var.currentCaptureCountValue[IC3];
  InputCapture.Var.currentCaptureCountValue [IC3] = InputCapture.ReadCapture(IC3);

  // Clear the interrupt flag
  INTClearFlag(INT_IC3);
}
//================================================

//================================================
// Configure the InputCapture 4 interrupt handler
//================================================
void __ISR(_INPUT_CAPTURE_4_VECTOR, IC4_INT_PRIORITY) InputCapture4InterruptHandler(void)
{
  // Get the timer used by this Input Capture
  TimerNum_t numTimer = InputCapture.Var.timerUsed[IC4];

  // Wait for capture data to be ready
  while(!(InputCapture.IsCaptureReady(IC4)));

  // Update values of timer overflows
  InputCapture.Var.previousTimerOverflows[IC4] = InputCapture.Var.currentTimerOverflows[IC4];
  InputCapture.Var.currentTimerOverflows[IC4]  = Timer.ReadOverflows(numTimer);

  // Store the timer value of the capture event
  InputCapture.Var.previousCaptureCountValue[IC4] = InputCapture.Var.currentCaptureCountValue[IC4];
  InputCapture.Var.currentCaptureCountValue [IC4] = InputCapture.ReadCapture(IC4);

  // Clear the interrupt flag
  INTClearFlag(INT_IC4);
}
//================================================

//================================================
// Configure the InputCapture 5 interrupt handler
//================================================
void __ISR(_INPUT_CAPTURE_5_VECTOR, IC5_INT_PRIORITY) InputCapture5InterruptHandler(void)
{
  /*
   * DEVELOPPER CODE HERE
   */
//  Port.B.ToggleBits(BIT_13);

  // Get the timer used by this Input Capture
  TimerNum_t numTimer = InputCapture.Var.timerUsed[IC5];

  // Wait for capture data to be ready
  while(!(InputCapture.IsCaptureReady(IC5)));

  // Update values of timer overflows
  InputCapture.Var.previousTimerOverflows[IC5] = InputCapture.Var.currentTimerOverflows[IC5];
  InputCapture.Var.currentTimerOverflows[IC5]  = Timer.ReadOverflows(numTimer);

  // Store the timer value of the capture event
  InputCapture.Var.previousCaptureCountValue[IC5] = InputCapture.Var.currentCaptureCountValue[IC5];
  InputCapture.Var.currentCaptureCountValue [IC5] = InputCapture.ReadCapture(IC5);

  // Clear the interrupt flag
  INTClearFlag(INT_IC5);
}
//================================================


void __ISR(_I2C_5_VECTOR, I2C5_INT_PRIORITY) I2c5InterruptHandler(void)
{
//  if (INTGetFlag(INT_I2C5S))  // Slave interrupt
//  {
//    INTClearFlag(INT_I2C5S);
//  }

//  if (INTGetFlag(INT_I2C5B))  //Bus Collision interrupt
//  {
//    INTClearFlag(INT_I2C5B);
//  }

  sI2cCmdBuffer_t masterData;   // Used for keeping track of the current state

  if (INTGetFlag(INT_I2C5M))  // Master interrupt
  {
    INTClearFlag(INT_I2C5M);

    if (I2c.Var.oReadDataInNextInterrupt[I2C5])     // If a read was started last interrupt
    {
      masterData.data  = I2C5RCV;                   // Read from I2C buffer
      masterData.state = I2C_MASTER_RECEIVE_DATA;   // Dummy write
      I2cFifoWrite((void *) &I2c.Var.i2cUserFifo[I2C5], &masterData);   // Copy to user
      I2c.Var.oRxDataAvailable[I2C5] = 1;
      I2c.Var.oReadDataInNextInterrupt[I2C5] = 0;
    }

    if (I2c.Var.oI2cWriteIsRunning[I2C5])   // Writing procedure
    {
      I2cFifoRead((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);  // Get current state
      switch (masterData.state)
      {
      //======================================================
        case I2C_MASTER_RECEIVE_DATA :
          I2C5CONbits.RCEN = 1;                         // Receive byte sequence
          I2c.Var.oReadDataInNextInterrupt[I2C5] = 1;   // Flag for the next interrupt to read the RX buffer
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_START_CONDITION :
          I2C5CONbits.SEN = 1;                          // Start condition sequence
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_STOP_CONDITION :
          I2C5CONbits.PEN = 1;                          // Stop condition sequence

          if (I2c.Var.oPoolSlaveAcknowledge[I2C5])      // If the user wanted to poll
          {
            if (!I2c.Var.oSecondStopAfterPooling[I2C5]) // If this is the first stop after writing all the data
            {                                           // Add the next three states required for polling
              masterData.state = I2C_MASTER_START_CONDITION;
              I2cFifoWrite((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);

              masterData.state = I2C_MASTER_TRANSMIT_DATA;
              I2cFifoWrite((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);

              masterData.state = I2C_MASTER_STOP_CONDITION;
              I2cFifoWrite((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);

              I2c.Var.oSecondStopAfterPooling[I2C5] = 1;
            }
            else                                        // If the first stop has passed
            {
              if (!I2CByteWasAcknowledged(I2C5))        // If slave sent NACK
              {                                         // Redo the 3 states for the polling
                masterData.state = I2C_MASTER_START_CONDITION;
                I2cFifoWrite((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);

                masterData.state = I2C_MASTER_TRANSMIT_DATA;
                I2cFifoWrite((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);

                masterData.state = I2C_MASTER_STOP_CONDITION;
                I2cFifoWrite((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);
              }
              else                                      // If slave sent ACK
              {                                         // End the communication with the slave
                masterData.state = I2C_MASTER_DONE;
                I2cFifoWrite((void *) &I2c.Var.i2cWriteQueue[I2C5], &masterData);
                I2c.Var.oSecondStopAfterPooling[I2C5] = 0;
              }
            }
          }
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_TRANSMIT_DATA :
          I2C5TRN = masterData.data;      // Put data in TX buffer
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_DONE :
          if (I2c.Var.i2cWriteQueue[I2C5].bufEmpty)     // If there is nothing more to send
          {
            I2c.Var.oI2cWriteIsRunning[I2C5] = 0;       // Turn off writing process
          }
          else                                          // If there is still data in the WriteQueue
          {
            INTSetFlag(INT_I2C5M);                      // Start another writing process
          }
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_REPEAT_START :
          I2C5CONbits.RSEN = 1;   //repeated start condition sequence
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_SLAVE_SENT_STOP :   // Not used
//          LED_ERROR_ON;
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_SEND_ACK :
          I2C5CONbits.ACKDT = 0;  //ACK
          I2C5CONbits.ACKEN = 1;  //Send ACK sequence
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_SEND_NACK :
          I2C5CONbits.ACKDT = 1;  //NACK
          I2C5CONbits.ACKEN = 1;  //Send NACK sequence
          break;
      //======================================================

      //======================================================
        case I2C_CMD_ERROR :      // Not used
//          LED_ERROR_ON;
          break;
      //======================================================

      //======================================================
        default :
          break;
      //======================================================
      } // end switch (master.state)
    } // end if (writing sequence)



    if (I2c.Var.oI2cReadIsRunning[I2C5])    // Reading procedure
    {
      I2cFifoRead((void *) &I2c.Var.i2cReadQueue[I2C5], &masterData);   // Get current state
      switch (masterData.state)
      {
      //======================================================
        case I2C_MASTER_RECEIVE_DATA :
          I2C5CONbits.RCEN = 1;             // Receive byte sequence
          I2c.Var.oReadDataInNextInterrupt[I2C5] = 1;
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_START_CONDITION :
          I2C5CONbits.SEN = 1;              // Start condition sequence
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_REPEAT_START :
          I2C5CONbits.RSEN = 1;             // Repeated start condition sequence
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_STOP_CONDITION :
          I2C5CONbits.PEN = 1;              // Stop condition
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_TRANSMIT_DATA :
          I2C5TRN = masterData.data;        // Put data in TX buffer
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_DONE :
          if (I2c.Var.i2cReadQueue[I2C5].bufEmpty)    // If there is nothing more to send
          {
            I2c.Var.oI2cReadIsRunning[I2C5] = 0;      // Turn off reading process
          }
          else                                        // If there is still data in the ReadQueue
          {
            INTSetFlag(INT_I2C5M);                    // Start another reading process
          }
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_SLAVE_SENT_STOP :   // Not used
//          LED_ERROR_ON;
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_SEND_ACK :
          I2C5CONbits.ACKDT = 0;  // ACK
          I2C5CONbits.ACKEN = 1;  // Send ACK sequence
          break;
      //======================================================

      //======================================================
        case I2C_MASTER_SEND_NACK :
          I2C5CONbits.ACKDT = 1;  // NACK
          I2C5CONbits.ACKEN = 1;  // Send NACK sequence
          break;
      //======================================================

      //======================================================
        case I2C_CMD_ERROR :      // Not used
//          LED_ERROR_ON;
          break;
      //======================================================

      //======================================================
        default :
          break;
      //======================================================
      } // end switch (master.state)
    } // end if (read sequence)
  }  // end if (master interrupt)
}   // end interrupt
//=============================================



//================================================
// Configure the InputCapture 5 interrupt handler
//================================================
void __ISR(_CAN_1_VECTOR, CAN1_INT_PRIORITY) Can1InterruptHandler(void)
{
    // Check if the source of the interrupt is RX_EVENT. This is redundant since
    // only this event is enabled in this example but this shows one scheme for
    // handling events
    if ((CANGetModuleEvent(CAN1) & CAN_RX_EVENT) != 0) {

        
    }

    // The CAN1 Interrupt flag is  cleared at the end of the interrupt routine.
    // This is because the event source that could have caused this interrupt to
    // occur (CAN_RX_CHANNEL_NOT_EMPTY) is disabled. Attempting to clear the
    // CAN1 interrupt flag when the the CAN_RX_CHANNEL_NOT_EMPTY interrupt is
    // enabled will not have any effect because the base event is still present.
    INTClearFlag(INT_CAN1);
}

