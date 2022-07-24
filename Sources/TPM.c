/*!
 * @file TPM.c
 * @brief Este modulo contem interface dos TPMx.
 * @author Wu Shin Ting
 * @date 01/03/2022
 */

#include "derivative.h"
#include "TPM.h"
#include "util.h"

static TPM_MemMapPtr TPM[] = TPM_BASE_PTRS;

void TPM_init (uint8_t x, uint16_t periodo, uint8_t ps) {
	
	SIM_SCGC6 |= GPIO_PIN(24+x); 	///< \li \l habilita clock de TPMx                           	

	/**
	 * Fonte de CLK de TPM: TPMSRC=SIM_SOPT2[25:24]=01 (MCGFLLCLK|MCGPLLCLK/2)
	 * PLLFLLSEL=SIM_SOPT2[16]=0 (MCGFLLCLK) 
	 */
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(0b01);     ///< \li \l seleciona fonte de clock TPM
	SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;  	

	TPM[x]->SC &= ~TPM_SC_CMOD(0x3);		///< desabilita contador

	TPM[x]->SC &= ~(TPM_SC_TOIE_MASK |		///< \li \l desabilita interrupcao "overflow"
				TPM_SC_CPWMS_MASK); 	///< \li \l modo de contagem crescente

	TPM[x]->MOD &= TPM_MOD_MOD(periodo);    	///< \li \l seta contagem maxima
	
	TPM[x]->SC &= ~TPM_SC_PS(0b111);       ///< \li \l seta prescaler 
	TPM[x]->SC |= TPM_SC_PS(ps);         	

	TPM[x]->CNT |= TPM_CNT_COUNT(0x0)  ;   ///< \li \l reseta a contagem com base em LPTPM
	
	TPM[x]->SC |= TPM_SC_CMOD(0x1);		///< \li \l habilita contador 
}


void TPM_initChIC (uint8_t x, uint8_t n, uint8_t mode) {
	/*!
	 * Configura os canais para operar no modo Input Capute mode (0b01, 0b10 e 0b11)
	 */
	TPM_CnSC_REG(TPM[x],n) &= ~(TPM_CnSC_MSB_MASK |  // MSB=TPMx_CnSC[5]=0
			TPM_CnSC_MSA_MASK |         // MSA=TPMx_CnSC[4]=0
			TPM_CnSC_ELSB_MASK |        // ELSB=TPMx_CnSC[3]=0
			TPM_CnSC_ELSA_MASK );       // ELSA=TPMx_CnSC[2]=0

	TPM_CnSC_REG(TPM[x],n) |= (mode << 2);      // rising/falling/both edges
}

void TPM_initChOC (uint8_t x, uint8_t n, uint8_t mode, uint16_t valor) {
	/*!
	 * Configura os canais para operar no modo Output Compare mode (0b01, 0b10, 0b11)  
	 */
	TPM_CnSC_REG(TPM[x],n) &= ~(TPM_CnSC_MSB_MASK |  // MSB=TPM0_C2SC[5]=0
			TPM_CnSC_MSA_MASK |         // MSA=TPM0_C2SC[4]=0
			TPM_CnSC_ELSB_MASK |        // ELSB=TPM0_C2SC[3]=0
			TPM_CnSC_ELSA_MASK );       // ELSA=TPM0_C2SC[2]=0
	/*!
	 * Setar o valor em TPMx_CnV
	 */
	TPM_CnV_REG(TPM[x],n) = TPM_CnV_VAL(valor); 

	TPM_CnSC_REG(TPM[x],n) |= (TPM_CnSC_MSA_MASK |   // MSA=TPM0_C2SC[4]=1
								(mode << 2));      	// toggle/clear/set output
}

void TPM_desativaCh (uint8_t x, uint8_t n) {
	TPM_CnSC_REG(TPM[x],n) &= ~(TPM_CnSC_MSB_MASK |  // MSB=TPM0_C2SC[5]=0
			TPM_CnSC_MSA_MASK |         // MSA=TPM0_C2SC[4]=0
			TPM_CnSC_ELSB_MASK |        // ELSB=TPM0_C2SC[3]=0
			TPM_CnSC_ELSA_MASK );       // ELSA=TPM0_C2SC[2]=0
}

/*LAB 13*/

void TPM_initSwitchIRQA5LedB(uint32_t periodo, uint8_t ps){
	TPM_init (0, periodo, ps);

	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;			// habilita clock de PORTA
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;			// habilita clock de PORTD
	
	PORTA_PCR5 &= ~PORT_PCR_MUX_MASK;			// PTA5 em TPM0_CH2     
	PORTA_PCR5 |= PORT_PCR_MUX(0x3);
	
	
	TPM_initChIC (0, 2, 0b10);
	
	PORTD_PCR1 &= ~PORT_PCR_MUX_MASK;
	PORTD_PCR1 |= PORT_PCR_MUX(0x4);
	TPM_initChOC (0,1,0b01, 0);
	
	TPM_init (0, periodo, ps);

	
}

void TPM_habilitaSwitchIRQA5LedBInterrupt (char priority) {	
    NVIC_ISER |= 1 << (17);              // NVIC_ISER[17]=1 (habilita IRQ17)
    NVIC_ICPR |= 1 << (17);              // NVIC_ICPR[17]=1 (limpa as pendências)
    NVIC_IPR5 |= NVIC_IP_PRI_17(priority << 6);    
    
	/*!
	 * Habilitar o evento de interrupcao
	 */ 
	TPM_CnSC_REG(TPM[0],1) |= TPM_CnSC_CHF_MASK;      	// limpar pendencias 	
	TPM_CnSC_REG(TPM[0],1) |= TPM_CnSC_CHIE_MASK; 	    // habilitar interrupcao do canal 1	

	TPM_CnSC_REG(TPM[0],2) |= TPM_CnSC_CHF_MASK;      	// limpar pendencias 	
	TPM_CnSC_REG(TPM[0],2) |= TPM_CnSC_CHIE_MASK; 	    // habilitar interrupcao do canal 2	
}



void TPM_ativaLedB(){
	TPM_initChOC (0,1,0b01, 0);
}

void TPM_desativaLedbInterrupt() {
	TPM_desativaCh(0,1);
}

void TPM_apagaLedB(){
	TPM_initChOC (0,1,0b11, 0);
}


