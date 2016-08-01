/*
 * File   :   Ads1299.h
 * Author :   Mikael Ducharme
 *
 * Created on July 25 2016, 19:34
 */

#ifndef __ADS1299_H__
#define	__ADS1299_H__

#include "Setup.h"



//==============================================================================
// General definitions
//==============================================================================

  //===========================================================
  // CONFIG1 Configuration Register 1
  //===========================================================
typedef union
{
  struct
  {
    UINT8               : 1   // Must always be set to 1
  /**
   * @Description   DAISY_EN: Daisy-chain and multiple readback mode \n
   * This bit determines which mode is enabled.\n
   *    0 = Daisy-chain mode (default)\n
   *    1 = Multiple readback mode\n
   */ 
           ,DAISY_ENn   : 1
  /**
   * @Description   CLK_EN: CLK connection(1)\n
   * This bit determines if the internal oscillator signal is connected to the 
   * CLK pin when the CLKSEL pin = 1.\n
   * 0 = Oscillator clock output disabled (default)\n
   * 1 = Oscillator clock output enabled   
   */ 
           ,CLK_EN      : 1
           ,            : 1   // Must always be set to 1
           ,            : 1   // Must always be set to 0
  /**
   * @Description   DR[2:0]: Output data rate\n
   * fMOD = fCLK / 2.\n
   * These bits determine the output data rate of the device.\n
   * (1) Additional power is consumed when driving external devices.\n
   * BIT  |  DATARATE     |  SAMPLERATE(1)\n
   * 000  |  fMOD / 64    |   16 kSPS\n
   * 001  |  fMOD / 128   |   8 kSPS\n
   * 010  |  fMOD / 256   |   4 kSPS\n
   * 011  |  fMOD / 512   |   2 kSPS\n
   * 100  |  fMOD / 1024  |   1 kSPS\n
   * 101  |  fMOD / 2048  |   500 SPS\n
   * 110  |  fMOD / 4096  |   250 SPS (default)\n
   * 111  |  Do not use n/a\n
   * (1) fCLK = 2.048 MHz.   
   */ 
           ,SAMPLE_RATE : 3
           ;
  } 
   /**
   * @Description   Used to set individual bits for Config1 Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for Config1 Register. 
   * 
   * @Example       config1Register.byte = 0x96;
   */
  UINT8 byte;
} Config1_t;

  //===========================================================
  // CONFIG2 Configuration Register 2
  //===========================================================
typedef union
{
  struct
  {
    UINT8               : 1   // Must always be set to 1
           ,            : 1   // Must always be set to 1
           ,            : 1   // Must always be set to 0
  /**
   * @Description   INT_CAL: TEST source\n
   * This bit determines the source for the Test signal.\n
   * 0 = Test signals are driven externally (default)\n
   * 1 = Test signals are generated internally
   */ 
           ,INT_CAL     : 1
           ,            : 1   // Must always be set to 0
  /**
   * @Description   CAL_AMP0: Test signal amplitude\n
   * This bit determines the calibration signal amplitude.\n
   * 0 = 1 × (VREFP ? VREFN) / 2.4 mV (default)\n
   * 1 = 2 × (VREFP ? VREFN) / 2.4 mV
   */ 
           ,CAL_AMP0    : 1
  /**
   * @Description   CAL_FREQ[1:0]: Test signal frequency\n
   * These bits determine the calibration signal frequency.\n
   * 00 = Pulsed at fCLK / 221 (default)\n
   * 01 = Pulsed at fCLK / 220\n
   * 10 = Not used\n
   * 11 = At dc
   */ 
           ,CAL_FREQ    : 2
           ;
  }
  /**
   * @Description   Used to set individual bits for Config2 Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
  
  /**
   * @Description   Used to set entire byte for Config2 Register. 
   * 
   * @Example       config2Register.byte = 0xC0;
   */
  UINT8 byte;
} Config2_t;

  //===========================================================
  // CONFIG3 Configuration Register 3
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   PD_REFBUFn: Power-down reference buffer\n
   * This bit determines the power-down reference buffer state.\n
   * 0 = Power-down internal reference buffer (default)\n
   * 1 = Internal reference buffer enabled\n
   */     
            PD_REFBUFn      : 1
           ,                : 1   // Must always be set to 1
           ,                : 1   // Must always be set to 1
  /**
   * @Description   This bit enables BIAS measurement. The BIAS signal may be 
   * measured with any channel.\n
   * 0 = Open (default)\n
   * 1 = BIASIN signal is routed to the channel that has the MUX_Setting 010 (VREF)
   */ 
           ,BIAS_MEAS       : 1
  /**
   * @Description   BIASREF_INT: BIASREF signal\n
   * This bit determines the BIASREF signal source.\n
   * 0 = BIASREF signal fed externally (default)\n
   * 1 = BIASREF signal (AVDD ? AVSS) / 2 generated internally
   */     
           ,BIASREF_INT     : 1
  /**
   * @Description   PD_BIASn: BIAS buffer power\n
   * This bit determines the BIAS buffer power state.\n
   * 0 = BIAS buffer is powered down (default)\n
   * 1 = BIAS buffer is enabled
   */     
           ,PD_BIASn        : 1
  /**
   * @Description   BIAS_LOFF_SENS: BIAS sense function\n
   * This bit enables the BIAS sense function.\n
   * 0 = BIAS sense is disabled (default)\n
   * 1 = BIAS sense is enabled\n
   */     
           ,BIAS_LOFF_SENS  : 1
  /**
   * @Description   BIAS_STAT: BIAS lead-off status\n
   * This bit determines the BIAS status.\n
   * 0 = BIAS is connected (default)\n
   * 1 = BIAS is not connected
   */     
           ,BIAS_STAT       : 1
           ;
  } 
   /**
   * @Description   Used to set individual bits for Config3 Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for Config3 Register. 
   * 
   * @Example       config3Register.byte = 0x60;
   */
  UINT8 byte;
} Config3_t;

  //===========================================================
  // LOFF: Lead-Off Control Register
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   COMP_TH[2:0]: Lead-off comparator threshold\n
   * These bits determine the lead-off comparator threshold level setting. 
   * See the Lead-Off Detection subsection of the EEGSpecific Functions section 
   * for a detailed description (ADS1299 Datasheet).\n
   * Comparator positive side\n
   * 000 = 95% (default)\n
   * 001 = 92.5%
   * 010 = 90%\n
   * 011 = 87.5%\n
   * 100 = 85%\n
   * 101 = 80%\n
   * 110 = 75%\n
   * 111 = 70%\n
   * Comparator negative side\n
   * 000 = 5% (default)\n
   * 001 = 7.5%\n
   * 010 = 10%\n
   * 011 = 12.5%\n
   * 100 = 15%\n
   * 101 = 20%\n
   * 110 = 25%\n
   * 111 = 30%\n
   */     
            COMP_TH   : 3
           ,          : 1   // Must always be set to 0
  /**
   * @Description   ILEAD_OFF[1:0]: Lead-off current magnitude\n
   * These bits determine the magnitude of current for the current lead-off 
   * mode.\n
   * 00 = 6 nA (default)\n
   * 01 = 24 nA\n
   * 10 = 6 ?A\n
   * 11 = 24 ?A
   */     
           ,ILEAD_OFF : 2
  /**
   * @Description   FLEAD_OFF[1:0]: Lead-off frequency\n
   * These bits determine the frequency of lead-off detect for each channel.\n
   * 00 = DC lead-off detection (default)\n
   * 01 = AC lead-off detection at 7.8 Hz (SYS_CLK / 218)\n
   * 10 = AC lead-off detection at 31.2 Hz (SYS_CLK / 216)\n
   * 11 = AC lead-off detection at fDR / 4\n
   */     
           ,FLEAD_OFF : 2
           ;
  } 
   /**
   * @Description   Used to set individual bits for LOFF Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for LOFF Register. 
   * 
   * @Example       lOffRegister.byte = 0x00;
   */
  UINT8 byte;
} Loff_t;


  //===========================================================
  // CHnSET: Individual Channel Settings (n = 1:8), Address = 05h to 0Ch
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   PD: Power-down\n
   * This bit determines the channel power mode for the corresponding channel.\n
   * 0 = Normal operation (default)\n
   * 1 = Channel power-down
   */     
            PD        : 1
   /**
   * @Description   GAIN[2:0]: PGA gain\n
    * These bits determine the PGA gain setting.\n
    * 000 = 1\n
    * 001 = 2\n
    * 010 = 4\n
    * 011 = 6\n
    * 100 = 8\n
    * 101 = 12\n
    * 110 = 24 (default)\n
    * 111 = n/a
   */  
           ,GAIN      : 3   
  /**
   * @Description   SRB2: Source, reference bias channel\n
   * This bit determines the SRB2 connection for the corresponding channel.\n
   * 0 = Open (off) (default)\n
   * 1 = Closed (on)
   */     
           ,SRB2 : 1
  /**
   * @Description   MUXn[2:0]: Channel input\n
   * These bits determine the channel input selection.\n
   * 000 = Normal electrode input (default)\n
   * 001 = Input shorted (for offset or noise measurements)\n
   * 010 = Used in conjunction with BIAS_MEAS bit for BIAS measurements. See the 
   * Bias Drive (DC Bias Circuit) subsection of the EEG-Specific Functions 
   * section for more details (ADS1299 Datasheet)\n
   * 011 = MVDD for supply measurement\n
   * 100 = Temperature sensor\n
   * 101 = Test signal\n
   * 110 = BIAS_DRP (positive electrode is the driver)\n
   * 111 = BIAS_DRN (negative electrode is the driver)
   */     
           ,MUX : 3
           ;
  } 
   /**
   * @Description   Used to set individual bits for CHnSET Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for CHnSET Register. 
   * 
   * @Example       ch1SetRegister.byte = 0x61;
   */
  UINT8 byte;
} ChNSet_t;


  //===========================================================
  // BIAS_SENSP: Bias Drive Positive Sense Selection
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            BIASP8        : 1
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASP7        : 1
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */    
            ,BIASP6        : 1
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */      
            ,BIASP5        : 1
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASP4        : 1
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASP3        : 1
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASP2        : 1
  /**
   * @Description   This register controls the selection of positive signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */      
            ,BIASP1        : 1
    ;
  }
   /**
   * @Description   Used to set individual bits for BIAS_SENSP Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for BIAS_SENSP Register. 
   * 
   * @Example       biasSensPRegister.byte = 0x00;
   */
  UINT8 byte;
} BiasSensP_t;


  //===========================================================
  // BIAS_SENSN: Bias Drive negative Sense Selection
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            BIASN8        : 1
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASN7        : 1
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */    
            ,BIASN6        : 1
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */      
            ,BIASN5        : 1
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASN4        : 1
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASN3        : 1
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */     
            ,BIASN2        : 1
  /**
   * @Description   This register controls the selection of negative signals 
   * from each channel for bias drive derivation. See the Bias Drive 
   * (DC Bias Circuit) subsection of the EEG-Specific Functions section for 
   * details. (ADS1299 Datasheet)
   */      
            ,BIASN1        : 1
    ;
  }
   /**
   * @Description   Used to set individual bits for BIAS_SENSN Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for BIAS_SENSN Register. 
   * 
   * @Example       biasSensNRegister.byte = 0x00;
   */
  UINT8 byte;
} BiasSensN_t;


  //===========================================================
  // LOFF_SENSP: Lead Off Positive Sense Selection
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */     
            LOFFP8        : 1
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */    
            ,LOFFP7        : 1
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */   
            ,LOFFP6        : 1
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */      
            ,LOFFP5        : 1
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */   
            ,LOFFP4        : 1
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */    
            ,LOFFP3        : 1
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */    
            ,LOFFP2        : 1
  /**
   * @Description   This register selects the positive side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATP 
   * register bits are only valid if the corresponding LOFF_SENSP bits are set 
   * to '1'.(ADS1299 Datasheet)
   */      
            ,LOFFP1        : 1
    ;
  }
   /**
   * @Description   Used to set individual bits for LOFF_SENSP Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for LOFF_SENSP Register. 
   * 
   * @Example       lOffSensPRegister.byte = 0x00;
   */
  UINT8 byte;
} LOffSensP_t;


  //===========================================================
  // LOFF_SENSN: Lead Off Positive Sense Selection
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */     
            LOFFN8        : 1
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */     
            ,LOFFN7        : 1
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */   
            ,LOFFN6        : 1
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */      
            ,LOFFN5        : 1
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */  
            ,LOFFN4        : 1
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */     
            ,LOFFN3        : 1
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */      
            ,LOFFN2        : 1
  /**
   * @Description   This register selects the negative side from each channel 
   * for lead-off detection. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details. Note that the LOFF_STATN 
   * register bits are only valid if the corresponding LOFF_SENSN bits are set 
   * to '1'.(ADS1299 Datasheet)
   */      
            ,LOFFN1        : 1
    ;
  }
   /**
   * @Description   Used to set individual bits for LOFF_SENSN Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for LOFF_SENSN Register. 
   * 
   * @Example       lOffSensNRegister.byte = 0x00;
   */
  UINT8 byte;
} LOffSensN_t;


  //===========================================================
  // LOFF_FLIP: Lead Off Current Direction Control
  //===========================================================
typedef union
{
  struct
  {
    UINT8   
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */     
            LOFF_FLIP8        : 1
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */    
            ,LOFF_FLIP7        : 1
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */  
            ,LOFF_FLIP6        : 1
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */      
            ,LOFF_FLIP5        : 1
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */ 
            ,LOFF_FLIP4        : 1
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */    
            ,LOFF_FLIP3        : 1
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */      
            ,LOFF_FLIP2        : 1
  /**
   * @Description   This register controls the current direction used for 
   * lead-off derivation. See the Lead-Off Detection subsection of the 
   * EEG-Specific Functions section for details.(ADS1299 Datasheet)
   */       
            ,LOFF_FLIP1        : 1
    ;
  }
   /**
   * @Description   Used to set individual bits for LOFF_FLIP Register. Write a dot
   *                "." after writing "bits" to see a description of the 
   *                available register options controlled by each bit.
   */
  bits;
  
   /**
   * @Description   Used to set entire byte for LOFF_FLIP Register. 
   * 
   * @Example       lOffFlipRegister.byte = 0x00;
   */
  UINT8 byte;
} LOffFlip_t;


typedef enum
{
  //===========================================================
  // CONFIG1 Configuration Register 1
  //===========================================================
  /* !DAISY_EN: Daisy-chain and multiple readback mode */
  CONFIG1_MULTIPLE_READBACK_MODE  =   0<<6    // Daisy-chain mode (default) 
  ,CONFIG1_DAISYCHAIN_MODE        =   1<<6    // Multiple readback mode
          
  /* CLK_EN: CLK connection (default fCLK = 2.048MHz) */
  ,CONFIG1_OCS_CLK_DIS      = 0<<5  // Oscillator clock output disabled (default)
  ,CONFIG1_OCS_CLK_EN       = 1<<5  // Oscillator clock output enabled
          
  /* DR[2:0]: Output data rate, fMOD = fCLK/2 */
  ,CONFIG1_SAMPLING_RATE_16000  = 0<<0  // fMOD/64
  ,CONFIG1_SAMPLING_RATE_8000   = 1<<0  // fMOD/128
  ,CONFIG1_SAMPLING_RATE_4000   = 2<<0  // fMOD/256          
  ,CONFIG1_SAMPLING_RATE_2000   = 3<<0  // fMOD/512        
  ,CONFIG1_SAMPLING_RATE_1000   = 4<<0  // fMOD/1024         
  ,CONFIG1_SAMPLING_RATE_500    = 5<<0  // fMOD/2048 
  ,CONFIG1_SAMPLING_RATE_250    = 6<<0  // fMOD/4096

  ,CONFIG1_DEFAULT_SETTINGS = 0b10010110 // default 0x96
  ,CONFIG1_MASK             = 0b10010000 // Bits[7]=1, Bits[4:3]=10
                                             // => 0b1xx10xxx
          
  //===========================================================
  // CONFIG2 Configuration Register 2
  //===========================================================
  /* INT_CAL: TEST source */
  ,CONFIG2_TEST_SIGNALS_EXT = 0<<4    // Test signals are driven externally (default)
  ,CONFIG2_TEST_SIGNALS_INT = 1<<4    // Test signals are generated internally
          
  /* CAL_AMP0: Test signal amplitude */
  ,CONFIG2_TEST_SIGNALS_AMP_1X  = 0<<2  // 1 × (VREFP ? VREFN) / 2.4 mV (default)
  ,CONFIG2_TEST_SIGNALS_AMP_2X  = 1<<2  // 2 × (VREFP ? VREFN) / 2.4 mV 
          
  /* CAL_FREQ[1:0]: Test signal frequency */
  ,CONFIG2_TEST_SIGNALS_FREQ_SLOW   = 0<<0  // Pulsed at fCLK / 2^21 (default)
  ,CONFIG2_TEST_SIGNALS_FREQ_FAST   = 1<<0  // Pulsed at fCLK / 2^20          
  ,CONFIG2_TEST_SIGNALS_FREQ_DC     = 3<<0  // At DC 
          
  ,CONFIG2_DEFAULT_SETTINGS = 0b11000000 // default 0xC0
  ,CONFIG2_MASK             = 0b11000000 // Bits[7:5]=110, Bits[3]=0 
                                             // => 0b110x0xxx

  //===========================================================
  // CONFIG3 Configuration Register 3
  //===========================================================
  /* !PD_REFBUF: Power-down reference buffer */
  ,CONFIG3_INT_REF_BUFFER_DIS =   0<<7    // Power-down internal reference buffer (default) 
  ,CONFIG3_INT_REF_BUFFER_EN  =   1<<7    // Internal reference buffer enabled
          
  /* BIAS_MEAS: BIAS measurement */
  ,CONFIG3_BIAS_MEAS_OPEN  = 0<<4  // Open (default)
  ,CONFIG3_BIAS_MEAS_VREF  = 1<<4  // BIASIN signal is routed to the channel that has the MUX_Setting 010 (VREF)
          
  /* BIASREF_INT: BIASREF signal */
  ,CONFIG3_BIAS_REF_EXT  = 0<<3  // BIASREF signal fed externally (default)
  ,CONFIG3_BIAS_REF_INT  = 1<<3  // BIASREF signal (AVDD ? AVSS) / 2 generated internally
          
  /* !PD_BIAS: BIAS buffer power */
  ,CONFIG3_BIAS_BUF_DIS  = 0<<2  // BIAS buffer is powered down (default)
  ,CONFIG3_BIAS_BUF_EN   = 1<<2  // BIAS buffer is enabled
          
  /* BIAS_LOFF_SENS: BIAS sense function */
  ,CONFIG3_BIAS_SENS_DIS  = 0<<1  // BIAS sense is disabled (default)
  ,CONFIG3_BIAS_SENS_EN   = 1<<1  // BIAS sense is enabled

  /* BIAS_STAT: BIAS lead-off status */
  ,CONFIG3_BIAS_STAT_ON  = 0<<0  // BIAS is connected (default)
  ,CONFIG3_BIAS_STAT_OFF = 1<<0  // BIAS is not connected          
          
  ,CONFIG3_DEFAULT_SETTINGS = 0b01100000 // default 0x60
  ,CONFIG3_MASK             = 0b01100000 // Bits[6:5]=11 
                                         // => 0bx11xxxxx
          
  //===========================================================
  // LOFF: Lead-Off Control Register
  //===========================================================
  /* COMP_TH[2:0]: Lead-off comparator threshold */
  ,LOFF_COMP_TH_P_950_N_050    =   000<<5   // Comparator positive side 95%,  (default) 
                                            // Comparator negative side 5%    (default)
          
  ,LOFF_COMP_TH_P_925_N_075    =   001<<5   // Comparator positive side 92.5%, 
                                            // Comparator negative side 7.5%
          
  ,LOFF_COMP_TH_P_900_N_100    =   010<<5   // Comparator positive side 90%,  
                                            // Comparator negative side 10%
          
  ,LOFF_COMP_TH_P_875_N_125    =   011<<5   // Comparator positive side 87.5%,
                                            // Comparator negative side 12.5%
          
  ,LOFF_COMP_TH_P_850_N_150    =   100<<5   // Comparator positive side 85%,  
                                            // Comparator negative side 15% 
          
  ,LOFF_COMP_TH_P_800_N_200    =   101<<5   // Comparator positive side 80%,  
                                            // Comparator negative side 20%
          
  ,LOFF_COMP_TH_P_750_N_250    =   110<<5   // Comparator positive side 75%,  
                                            // Comparator negative side 25%
          
  ,LOFF_COMP_TH_P_700_N_300    =   111<<5   // Comparator positive side 70%,  
                                            // Comparator negative side 30%        
    
          
  /* ILEAD_OFF[1:0]: Lead-off current magnitude */
  ,LOFF_ILEAD_OFF_6NA  = 00<<2  // 6 nA (default)
  ,LOFF_ILEAD_OFF_24NA = 01<<2  // 24 nA
  ,LOFF_ILEAD_OFF_6UA  = 10<<2  // 6 uA 
  ,LOFF_ILEAD_OFF_24UA = 11<<2  // 24 uA
          
  /* FLEAD_OFF[1:0]: Lead-off frequency */
  ,LOFF_FLEAD_OFF_DC      = 00<<0  // DC lead-off detection (default)
  ,LOFF_FLEAD_OFF_7_8_HZ  = 01<<0  // AC lead-off detection at 7.8 Hz (SYS_CLK / 2^18)
  ,LOFF_FLEAD_OFF_31_2_HZ = 10<<0  // AC lead-off detection at 31.2 Hz (SYS_CLK / 2^16)
  ,LOFF_FLEAD_OFF_FDR_HZ  = 11<<0  // AC lead-off detection at fDR / 4     
          
  ,LOFF_DEFAULT_SETTINGS = 0b00000000 // default 0x00
  ,LOFF_MASK             = 0b00000000 // Bits[4]=0 
                                      // => 0bxxx0xxxx
          
  //===========================================================
  // CHnSET: Individual Channel Settings (n = 1:8), Address = 05h to 0Ch
  //===========================================================
  /* PD: Power-down */
  ,CH_SET_POWER_ON    =   0<<7   // Normal operation (default) 
  ,CH_SET_POWER_OFF   =   1<<7   // Channel power-down
          
  /* GAIN[2:0]: PGA gain */   
  ,CH_SET_GAIN_1  =   000<<4   // PGA Gain 1
  ,CH_SET_GAIN_2  =   001<<4   // PGA Gain 2
  ,CH_SET_GAIN_4  =   010<<4   // PGA Gain 4 
  ,CH_SET_GAIN_6  =   011<<4   // PGA Gain 6
  ,CH_SET_GAIN_8  =   100<<4   // PGA Gain 8 
  ,CH_SET_GAIN_12 =   101<<4   // PGA Gain 12
  ,CH_SET_GAIN_24 =   110<<4   // PGA Gain 24 (default) 
  
  /* SRB2: Source, reference bias channel */
  ,CH_SET_SRB2_OFF =  0<<3   // Open (off) (default)
  ,CH_SET_SRB2_ON  =  1<<3   // Closed (on)
          
  /* MUXn[2:0]: Channel input */
  ,CH_SET_INPUT_NORMAL    = 000<<0   // Normal electrode input (default)
  ,CH_SET_INPUT_SHORT     = 001<<0   // Input shorted (for offset or noise measurements)
  ,CH_SET_INPUT_BIAS_MEAS = 010<<0   // Used in conjunction with BIAS_MEAS bit for BIAS measurements 
  ,CH_SET_INPUT_MVDD      = 011<<0   // MVDD for supply measurement
  ,CH_SET_INPUT_TEMP_SENS = 100<<0   // Temperature sensor
  ,CH_SET_INPUT_TEST_SIG  = 101<<0   // Test signal
  ,CH_SET_INPUT_BIAS_DRP  = 110<<0   // BIAS_DRP (positive electrode is the driver) 
  ,CH_SET_INPUT_BIAS_DRN  = 111<<0   // BIAS_DRN (negative electrode is the driver)
          
  ,CH_SET_DEFAULT_SETTINGS = 0b01100000 // default 0x61
  ,CH_SET_MASK             = 0b00000000 // => 0bxxxxxxxx        
          
  //===========================================================
  // Default register values
  //===========================================================          
  ,BIAS_SENSP_DEFAULT_SETTINGS = 0b00000000 //default 0x00
  ,BIAS_SENSN_DEFAULT_SETTINGS = 0b00000000 //default 0x00
  ,LOFF_SENSP_DEFAULT_SETTINGS = 0b00000000 //default 0x00
  ,LOFF_SENSN_DEFAULT_SETTINGS = 0b00000000 //default 0x00
  ,LOFF_FLIP_DEFAULT_SETTINGS  = 0b00000000 //default 0x00

} AdsRegisterFlags_t;


typedef enum
{
  //===========================================================
  // ASD1299 Register Addresses
  //===========================================================
   ID           = 0x00
  ,CONFIG1      = 0x01
  ,CONFIG2      = 0x02
  ,CONFIG3      = 0x03
  ,LOFF         = 0x04
  ,CH1SET       = 0x05
  ,CH2SET       = 0x06
  ,CH3SET       = 0x07
  ,CH4SET       = 0x08
  ,CH5SET       = 0x09
  ,CH6SET       = 0x0A
  ,CH7SET       = 0x0B
  ,CH8SET       = 0x0C
  ,BIAS_SENSP   = 0x0D
  ,BIAS_SENSN   = 0x0E
  ,LOFF_SENSP   = 0x0F
  ,LOFF_SENSN   = 0x10
  ,LOFF_FLIP    = 0x11
  ,LOFF_STATP   = 0x12
  ,LOFF_STATN   = 0x13
  ,GPIO         = 0x14
  ,MISC1        = 0x15
  ,MISC2        = 0x16
  ,CONFIG4      = 0x17 

} AdsRegisters_t;

typedef enum
{
   CH1       = 0
  ,CH2       = 1
  ,CH3       = 2
  ,CH4       = 3
  ,CH5       = 4
  ,CH6       = 5
  ,CH7       = 6
  ,CH8       = 7
} sAdsChannel_t;

//==============================================================================
// Public functions prototypes
//==============================================================================

// BOARD 
void sendChannelData(void); // send the current data with sample number
void startStreaming(void);  // ADD DAISY USE outputType
void stopStreaming(void);   // ADD DAISY USE outputType

// ADS1299  
void initialize_ads(void);
void WREGS(BYTE,BYTE,int);      // write multiple ADS registers
BYTE ADS_getDeviceID(int);         
void printRegisterName(BYTE);   // used for verbosity
void updateChannelData(void);   // retrieve data from ADS
void updateBoardData(void);
void activateChannel(BYTE);                  // enable the selected channel
void deactivateChannel(BYTE);                // disable given channel 1-8(16)
void configureLeadOffDetection(BYTE,BYTE);
void changeChannelLeadOffDetectAll(void);
void changeChannelLeadOffDetect(BYTE);
void configureInternalTestSignal(BYTE,BYTE); 
void changeInputType(BYTE); 
void ADS_writeChannelData(void);
// void ADS_printDeviceID(int);   // 





//==============================================================================
// Macro definitions
//==============================================================================

#define CH_TO_REG 5   // ChX = X-1, X + 5 = ChXRegister
                      // Example: Ch1 = 0, Ch1Register = 0x05

//#define BOARD_ADS	8 	// ADS chip select
//#define DAISY_ADS	3   // ADS Daisy chip select
//#define BOTH_ADS	5   // Slave Select Both ADS chips

//ADS1299 SPI Command Definition Byte Assignments
#define _WAKEUP   0x02 // Wake-up from standby mode
#define _STANDBY  0x04 // Enter Standby mode
#define _RESET    0x06 // Reset the device registers to default
#define _START    0x08 // Start and restart (synchronize) conversions
#define _STOP     0x0A // Stop conversion
#define _RDATAC   0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC   0x11 // Stop Read Data Continuous mode
#define _RDATA    0x12 // Read data by command; supports multiple read back

#define ADS_ID    0x3E // ADS1299 generic ID

//#define OUTPUT_NOTHING  (0) // quiet
//#define OUTPUT_8_CHAN   (1) // not using Daisy module
//#define OUTPUT_16_CHAN  (2) // using Daisy module
//#define ON_BOARD        (8) // slave address for on board ADS
//#define ON_DAISY        (3) // slave address for daisy ADS
//
//// CHANNEL SETTINGS 
//#define POWER_DOWN      (0)
//#define GAIN_SET        (1)
//#define INPUT_TYPE_SET  (2)
//#define BIAS_SET        (3)
//#define SRB2_SET        (4)
//#define SRB1_SET        (5)
//#define YES      	(0x01)
//#define NO      	(0x00)
//
////gainCode choices
//#define ADS_GAIN01 (0b00000000)	// 0x00
//#define ADS_GAIN02 (0b00010000)	// 0x10
//#define ADS_GAIN04 (0b00100000)	// 0x20
//#define ADS_GAIN06 (0b00110000)	// 0x30
//#define ADS_GAIN08 (0b01000000)	// 0x40
//#define ADS_GAIN12 (0b01010000)	// 0x50
//#define ADS_GAIN24 (0b01100000)	// 0x60
//
////inputType choices
//#define ADSINPUT_NORMAL     (0b00000000)
//#define ADSINPUT_SHORTED    (0b00000001)
//#define ADSINPUT_BIAS_MEAS  (0b00000010)
//#define ADSINPUT_MVDD       (0b00000011)
//#define ADSINPUT_TEMP       (0b00000100)
//#define ADSINPUT_TESTSIG    (0b00000101)
//#define ADSINPUT_BIAS_DRP   (0b00000110)
//#define ADSINPUT_BIAL_DRN   (0b00000111)
//
////test signal choices...ADS1299 datasheet page 41
//#define ADSTESTSIG_AMP_1X     (0b00000000)
//#define ADSTESTSIG_AMP_2X     (0b00000100)
//#define ADSTESTSIG_PULSE_SLOW (0b00000000)
//#define ADSTESTSIG_PULSE_FAST (0b00000001)
//#define ADSTESTSIG_DCSIG      (0b00000011)
//#define ADSTESTSIG_NOCHANGE   (0b11111111)
//
////Lead-off signal choices
//#define LOFF_MAG_6NA      (0b00000000)
//#define LOFF_MAG_24NA     (0b00000100)
//#define LOFF_MAG_6UA      (0b00001000)
//#define LOFF_MAG_24UA     (0b00001100)
//#define LOFF_FREQ_DC      (0b00000000)
//#define LOFF_FREQ_7p8HZ   (0b00000001)
//#define LOFF_FREQ_31p2HZ  (0b00000010)
//#define LOFF_FREQ_FS_4    (0b00000011)
//#define PCHAN (0)
//#define NCHAN (1)
//#define OFF   (0)
//#define ON    (1)
//
//// used for channel settings
//#define ACTIVATE_SHORTED  (2)
//#define ACTIVATE          (1)
//#define DEACTIVATE        (0)
//
//// daisy module 
//#define CLK_EN	5
//
////LIS3DH
//#define READ_REG      0x80
//#define READ_MULTI		0x40
//
//
//#define FOOTER 0xC0 
//#define HEADER 0xA0

//==============================================================================
// Variable declarations
//==============================================================================

    INT16 auxData[3];
    
    BYTE lastBoardDataRaw[24];
    BYTE boardChannelDataRaw[24];     // array to hold raw channel data
    BYTE meanBoardDataRaw[24];
    INT32 boardStat;                  // used to hold the status register
    INT32 boardChannelDataInt[8];     // array used when reading channel data as INT32s
    INT32 lastBoardChannelDataInt[8];
    INT32 meanBoardChannelDataInt[8];
    INT32 numChannels;
    BYTE channelSettings[16][6];      // array to hold current channel settings
    BYTE defaultChannelSettings[6];   // default channel settings
    BYTE leadOffSettings[16][2];      // used to control on/off of impedance measure for P and N side of each channel
    
    BOOL useInBias[16];               // used to remember if we were included in Bias before channel power down
    BOOL useSRB2[16];
    BOOL boardUseSRB1;                // used to keep track of if we are using SRB1
    BOOL verbosity;                   // turn on/off Serial verbosity
    BOOL firstDataPacket;
    BOOL isRunning;
    
    BYTE sampleCounter;

    
//=========================================
// Public structure of SPI functions
//=========================================
struct sCochleegAdsSystemCmd
{
   /**
   * @Prototype   static inline void AdsWakeUp (void);
   *
   * @Description This function exits low power stand by mode. Waits at least 
   *              4 tclk cycles 
   *
   * @Example     Ads.SystemCmd.WakeUp();\n
   *              This example exits low power stand by.
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*WakeUp) (void);
  
   /**
   * @Prototype   static inline void AdsStandBy (void);
   *
   * @Description This function enters low power stand by mode. Do not send any
   *              commands other than WakeUp once in stand by mode 
   *
   * @Example     Ads.SystemCmd.StandBy();\n
   *              This example enters low power stand by.
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*StandBy) (void);  

   /**
   * @Prototype   static inline void AdsReset (void);
   *
   * @Description This function resets the digital filter cycle and returns all 
   *              register settings to default values
   *
   * @Example     Ads.SystemCmd.StandBy();\n
   *              This example enters low power stand by.
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*Reset) (void); 

   /**
   * @Prototype   static inline void AdsStart (void);
   *
   * @Description This function starts data conversions. Tie the START pin low 
    *             to control conversions by command. If conversions are in 
    *             progress, this command has no effect.
   *
   * @Example     Ads.SystemCmd.Start();\n
   *              This example starts data conversions.
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*Start) (void); 

   /**
   * @Prototype   static inline void AdsStop (void);
   *
   * @Description This function stops conversions. Tie the START pin low to 
    *             control conversions by command. When the STOP command is sent,
    *             the conversion in progress completes and further conversions 
    *             are stopped. If conversions are already stopped, this command 
    *             has no effect.
   *
   * @Example     Ads.SystemCmd.Stop();\n
   *              This example stops data conversions.
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*Stop) (void);   
};

struct sCochleegAdsDataReadCmd
{
   /**
   * @Prototype   static inline void AdsRdatac (void);
   *
   * @Description This function enables conversion data output on each !DRDY 
    *             without the need to issue subsequent read data opcodes. This 
    *             mode places the conversion data in the output register and may
    *             be shifted out directly. The read data continuous mode is the 
    *             device default mode; the ADS1299 defaults to this mode on 
    *             power-up.
   *
   * @Example     Ads.DataReadCmd.Rdatac();\n
   *              This example enters Read Data Continuous Mode.
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*Rdatac) (void);    

   /**
   * @Prototype   static inline void AdsSdatac (void);
   *
   * @Description This function cancels the Read Data Continuous mode.
   *
   * @Example     Ads.DataReadCmd.Sdatac();\n
   *              This example exits Read Data Continuous Mode.
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*Sdatac) (void);    
};

struct sCochleegAdsRegisterCmd
{

  /**
   * @Prototype   static inline BYTE AdsRreg (AdsRegisters_t adsRegister);
   *
   * @Description This function reads the register value at the address 
   *              specified as a parameter              
   *
   * @Example     config1 = Ads.RegisterCommands.Rreg(CONFIG1);\n
   *              This example reads the CONFIG1 register.
   *
   * @Param       AdsRegisters_t adsRegister\n
   *              Address of ADS1299 register to read
   *
   * @Return      Value of register specified as parameter.
   * 
   * @Registers   List of registers : \n
   *              ID           = 0x00\n
   *              CONFIG1      = 0x01\n
   *              CONFIG2      = 0x02\n
   *              CONFIG3      = 0x03\n
   *              LOFF         = 0x04\n
   *              CH1SET       = 0x05\n
   *              CH2SET       = 0x06\n
   *              CH3SET       = 0x07\n
   *              CH4SET       = 0x08\n
   *              CH5SET       = 0x09\n
   *              CH6SET       = 0x0A\n
   *              CH7SET       = 0x0B\n
   *              CH8SET       = 0x0C\n
   *              BIAS_SENSP   = 0x0D\n
   *              BIAS_SENSN   = 0x0E\n
   *              LOFF_SENSP   = 0x0F\n
   *              LOFF_SENSN   = 0x10\n
   *              LOFF_FLIP    = 0x11\n
   *              LOFF_STATP   = 0x12\n
   *              LOFF_STATN   = 0x13\n
   *              GPIO         = 0x14\n
   *              MISC1        = 0x15\n
   *              MISC2        = 0x16\n
   *              CONFIG4      = 0x17\n
   */
  BYTE (*Rreg) (AdsRegisters_t adsRegister);

  /**
   * @Prototype   static inline void AdsWreg (AdsRegisters_t adsRegister, BYTE registerValue);
   *
   * @Description This function writes the register value specified as a parameter 
   *              at the address specified as a parameter
   * @Example     Ads.RegisterCommands.Wreg(CONFIG1, 0x96);\n
   *              This example writes 0X96 in CONFIG1.
   *
   * @Param       AdsRegisters_t adsRegister\n
   *              Address of ADS1299 register to write\n
   *              BYTE registerValue\n
   *              Value to write in register
   *
   * @Return      None.
   * 
   * @Registers   List of registers : \n
   *              ID           = 0x00\n
   *              CONFIG1      = 0x01\n
   *              CONFIG2      = 0x02\n
   *              CONFIG3      = 0x03\n
   *              LOFF         = 0x04\n
   *              CH1SET       = 0x05\n
   *              CH2SET       = 0x06\n
   *              CH3SET       = 0x07\n
   *              CH4SET       = 0x08\n
   *              CH5SET       = 0x09\n
   *              CH6SET       = 0x0A\n
   *              CH7SET       = 0x0B\n
   *              CH8SET       = 0x0C\n
   *              BIAS_SENSP   = 0x0D\n
   *              BIAS_SENSN   = 0x0E\n
   *              LOFF_SENSP   = 0x0F\n
   *              LOFF_SENSN   = 0x10\n
   *              LOFF_FLIP    = 0x11\n
   *              LOFF_STATP   = 0x12\n
   *              LOFF_STATN   = 0x13\n
   *              GPIO         = 0x14\n
   *              MISC1        = 0x15\n
   *              MISC2        = 0x16\n
   *              CONFIG4      = 0x17\n
   */
  void (*Wreg) (AdsRegisters_t adsRegister, BYTE registerValue);
  
};

struct sCochleegAds
{
   /**
   * @Prototype   static inline void AdsDeactivateChannel(sAdsChannel_t channel);
   *
   * @Description This function launches the ADS1299 init sequence, which
   *              writes the specific settings for config1, 2 and 3
   * @Example     Ads.Initialize();\n
   *              This example initializes cochlEEG's ADS1299
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*Initialize) (void);   
  
  /**
   * @Prototype   static inline void AdsResetSequence(void);
   *
   * @Description This function launches the ADS1299 reset sequence, which
   *              writes the RESET and SDATAC opcodes, waits 100ms and turns off
   *              all the channels.              
   * 
   * @Example     Ads.ResetSequence();\n
   *              This example launches the reset sequence
   *
   * @Param       None.
   *
   * @Return      None.
   */
  void (*ResetSequence) (void);
  
     /**
   * @Prototype   static inline void AdsDeactivateChannel(sAdsChannel_t channel);
   *
   * @Description This function deactivates the channel specified as parameter.
   *              Channel is powered down, disconnected from SRB2, removed from 
   *              bias generation. 
   * @Example     Ads.DeactivateChannel(CH1);\n
   *              This example deactivates channel 1
   *
   * @Param       sAdsChannel_t channel\n
   *              ADS1299 Channel to deactivate\n
   *
   * @Return      None.
   */
  void (*DeactivateChannel) (sAdsChannel_t channel);
  
  /**
   * @Description   Contains general ADS1299 system commands. Write a dot
   *                "." after writing "SystemCmd" to see a description of the 
   *                available system commands.
   */
  struct sCochleegAdsSystemCmd SystemCmd;
  
  /**
   * @Description   Contains general ADS1299 DataRead commands. Write a dot
   *                "." after writing "DataReadCmd" to see a description of the 
   *                available DataRead commands.
   */  
  struct sCochleegAdsDataReadCmd DataReadCmd;
  
   /**
   * @Description   Contains general ADS1299 Register commands. Write a dot
   *                "." after writing "RegisterCmd" to see a description of the 
   *                available Register commands.
   */  
  struct sCochleegAdsRegisterCmd RegisterCmd;
  
   /**
   * @Description   Array used to mirror ADS1299 Register Map (Datashee p.39)
   */
  BYTE registerMap[24];
};

#endif	/* __ADS1299_H__ */

