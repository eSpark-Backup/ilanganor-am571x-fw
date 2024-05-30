/*{{TIDC_Wizard Auto Code Start                                 */
/****************************************************************/
/* don't change anything here until you know what you are doing!*/
/* the plug-in will change all text between the code marks      */
/* without user interaction!                                    */
/*                                                              */
/****************************************************************/

#ifndef DC_CONF_H
#define DC_CONF_H

/* ADC 1 parameter data */
#define    ADC1_TYPE          TLV320AIC14
#define    ADC1_ID            (0)
#define    ADC1_CR1_VALUE     (0x2701)
#define    ADC1_CR2_VALUE     (0x4720)
#define    ADC1_CR3_VALUE     (0x6721)
#define    ADC1_CR4M_VALUE    (0x8790)
#define    ADC1_CR4NP_VALUE   (0x8730)
#define    ADC1_CR5A_VALUE    (0xA72A)
#define    ADC1_CR5B_VALUE    (0xA76A)
#define    ADC1_CR5C_VALUE    (0xA7B8)
#define    ADC1_CR5D_VALUE    (0xA7C0)
#define    ADC1_CR6_VALUE     (0xC700)
#define    ADC1_ADC_GAIN      (42)
#define    ADC1_DAC_GAIN      (42)
#define    ADC1_M_VALUE       (16)
#define    ADC1_NP_VALUE      (48)
#define    AIC14_SERPORT      (0)
#define    NUM_OF_AIC         (1)

/* DSP parameter data      */
#define    TMS320C6416
/* CSL device build option */
//ilan#define    CHIP_6416          (1)
#define    DSP_FREQ           (491)  /* in MHz */

/* McBSP0 parameter data      */
#define    MCBSP0_FPER_VALUE    (15)
#define    MCBSP0_CLKGDV_VALUE  (1)
#define    MCBSP0_FSGM_MODE     (0)
#endif /* DC_CONF_H */

/****************************************************************/
/* END OF DC_CONF.H                                             */
/****************************************************************/
/*TIDC_Wizard Auto Code End}}*/
