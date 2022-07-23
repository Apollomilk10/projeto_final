/*!
 * @file timers.c
 * @brief Este modulo contem interface dos timers (SysTick, RTC, PIT, LPTMR0) do Kinetis MKL25
 * @author Wu Shin Ting
 * @date 01/03/2022
 */

#include "derivative.h"
#include "timers.h"

//====================================================
// SIM
//====================================================
/*!
 * @brief Seta o divisor de frequencia do sinal de barramento
 * @param[in] OUTDIV4 divisor do sinal MCGOUTCLK
 */
void SIM_setaOUTDIV4 (uint8_t OUTDIV4) {
	/*!
	 * Configurar a frequencia do clock de barramento atraves do divisor da frequencia 
	 * do sinal MCGOUTCLK  (clock de barramento = MCGOUTCLK/(OUTDIV1*OUTDIV4))
	 */
	SIM_CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV4(0b111); //!< \li \l setar OUTDIV4
	SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV4(OUTDIV4); 	
}

//====================================================
// SYSTICK
//====================================================
void SysTick_init (uint32_t periodo) {
	SYST_RVR = SysTick_RVR_RELOAD(periodo);   
	SYST_CVR = SysTick_CVR_CURRENT(0);          //!< \li \l reseta flag e recarga     
	SYST_CSR |= (
				SysTick_CSR_CLKSOURCE_MASK 		//!< \li \l fonte de CLK: nucleo
				| SysTick_CSR_ENABLE_MASK     	//!< \li \l habilita SysTick 
				);
}

void SysTick_ativaInterrupt () {
	SYST_CSR |= SysTick_CSR_TICKINT_MASK;    //!< \li \l ativa interrupcao do SysTick 	
}

void SysTick_desativaInterrupt () {
	SYST_CSR &= ~SysTick_CSR_TICKINT_MASK;    //!< \li \l desativa interrupcao do SysTick 	
}

//====================================================
// PIT
//====================================================
void PIT_initTimer0(uint32_t periodo) {
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;            //!< \li \l ativar o sinal de relogio do PIT

	PIT_TCTRL0 &= ~(PIT_TCTRL_TEN_MASK); 		//!< \li \l desativar o timer 
	PIT_TCTRL0 &= ~PIT_TCTRL_CHN_MASK;   		//!< \li \l timers nao encadeados
	PIT_LDVAL0 = PIT_LDVAL_TSV(periodo);       	//!< \li \l carregar o valor de recarga

	PIT_TCTRL0 |= (PIT_TCTRL_TEN_MASK);  		//!< \li \l ativa timer 0

	PIT_MCR &= ~(PIT_MCR_FRZ_MASK |             //!< \li \l continua contagem no modo Debug
			PIT_MCR_MDIS_MASK );     			//!< \li \l habilita modulo PIT
}

void PIT_ativaIRQ (uint8_t priority) {
    /*! 
     * Ativa IRQ 22 (PIT) no NVIC: ativar, limpar pendencias e setar prioridade 
     */
    NVIC_ISER |= 1 << (22);           	 // NVIC_ISER[22]=1 (habilita IRQ22)
    NVIC_ICPR |= 1 << (22);         	 // NVIC_ICPR[22]=1 (limpa as pendências)
    NVIC_IPR5 |= NVIC_IP_PRI_22(priority << 6); // 22/4 -> 5

    PIT_TCTRL0  |= PIT_TCTRL_TIE_MASK;	  // ativar interrupcao em PIT
    
    return;
}

//====================================================
// LPTMR0
//====================================================
uint8_t LPTimer_init (uint16_t valor)
{
	SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;           //!< \li \l ativar o sinal de relogio do LPTMR0
	
	LPTMR0_CSR &= ~(LPTMR_CSR_TPP_MASK	//!< \li \l CNR incrementa na borda de subida (0) 
			| LPTMR_CSR_TFC_MASK  		//!< \li \l CNR reseta quando CNR = CMR
			| LPTMR_CSR_TMS_MASK  		//!< \li \l modo de contagem por tempo
	);

	/*!
	 * Selecionar a base de tempo dentre MCGIRCLK(0b00)/LPO(0b01)/OSCERCLK(0b11)
	 */ 
	LPTMR0_PSR |= LPTMR_PSR_PCS(0b01);	

	LPTMR0_PSR &= ~LPTMR_PSR_PBYP_MASK;         //!< habilitar divisor (prescaler)
	LPTMR0_PSR |= LPTMR_PSR_PRESCALE(0b0000); 	//!< setar divisor em 2

	LPTMR0_CMR = LPTMR_CMR_COMPARE(valor);  	//!< setar contagem maxima
		
	LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;           //!< habilitar o modulo
	
	return 0;
}

void LPTimer_ativaIRQ (uint8_t priority)
{
    /*!
     * Ativa IRQ 28 (LPTMR) no NVIC: ativar, limpar pendencias e setar prioridade 
     */
    NVIC_ISER |= 1 << (28);         	 // NVIC_ISER[28]=1 (habilita IRQ28)
    NVIC_ICPR |= 1 << (28);         	 // NVIC_ICPR[28]=1 (limpa as pendências)
    NVIC_IPR7 |= NVIC_IP_PRI_28(priority << 6); // 28/4 = 7			

	LPTMR0_CSR |= LPTMR_CSR_TIE_MASK;				// ativar interrupcao em LPTMR0
}

//====================================================
// RTC
//====================================================
void RTClpo_init()
{
	SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;         //!< \li \l ativar o sinal de relogio do RTC           
	
	SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(0b11);  //!< \li \l setar 1kHz LPO como fonte de ERCLK32K

	RTC_CR |= RTC_CR_SWR_MASK;			//!< \li \l resetar os registradores do RTC por software
										//!< \li \l limpar o contador RTC_TSR e flags RTC_SR_TOF e RTC_SR_TIF
	RTC_CR &= ~RTC_CR_SWR_MASK;			//!< \li \l resetar explicitamente SWR 
	
    RTC_TCR = RTC_TCR_CIR(0x00) |		//!< \li \l ajustar o erro de frequencia (ppm) em cada segundo
              RTC_TCR_TCR(0x00);        //!< \li \l 1s = 32.768 ticks (sem ajuste)
    
    RTC_TPR = 0; 			//!< \li \l resetar prescaler (2^15 - 1)
    RTC_TSR = 0;			//!< \li \l resetar o contador para que TIF seja baixada.
    
	RTC_SR |= RTC_SR_TCE_MASK;			//!< \li \l habilita o relogio
}

void RTClpo_getTime (uint32_t *seconds) {
	*seconds = (RTC_TSR*32768)/1000+(RTC_TPR/1000);  //!< reajustar a contagem para segundos
}

/**
 * @brief Setar o valor do contador de segundos do RTC
 * @param[in] seconds valor de tempo em segundos
 */
void RTClpo_setTime (uint32_t seconds) {
	RTC_SR &= ~RTC_SR_TCE_MASK;			//!< desabilita o modulo para atualizar prescaler e contador
	RTC_TPR = (seconds*1000)%32768;     //!< reajustar segundos para a frequencia 1kHz
	RTC_TSR = (seconds*1000)/32768;		
	RTC_SR |= RTC_SR_TCE_MASK;			//!< habilitar o modulo
}

void RTC_ativaSegundoIRQ (uint8_t priority) {
    /*!
     * Ativa IRQ 21 () no NVIC: ativar, limpar pendencias e setar prioridade 
     */
    NVIC_ISER |= 1 << (21);         	 // NVIC_ISER[21]=1 (habilita IRQ21)
    NVIC_ICPR |= 1 << (21);         	 // NVIC_ICPR[21]=1 (limpa as pendências)
    NVIC_IPR5 |= NVIC_IP_PRI_21(priority << 6); // 21/4 = 5			

    RTC_IER |= RTC_IER_TSIE_MASK; 		// ativa interrupcao em RTC
}

void RTC_ativaAlarmIRQ (uint8_t priority) {
    /*!
     * Ativa IRQ 20 () no NVIC: ativar, limpar pendencias e setar prioridade 
     */
    NVIC_ISER |= 1 << (20);         	 // NVIC_ISER[20]=1 (habilita IRQ20)
    NVIC_ICPR |= 1 << (20);         	 // NVIC_ICPR[20]=1 (limpa as pendências)
    NVIC_IPR5 |= NVIC_IP_PRI_20(priority << 6); // 20/4 = 5			

    RTC_IER |= RTC_IER_TAIE_MASK; 		// ativa interrupcao em RTC

}

void RTC_ativaOverflowIRQ (uint8_t priority) {
    /*!
     * Ativa IRQ 20 () no NVIC: ativar, limpar pendencias e setar prioridade 
     */
    NVIC_ISER |= 1 << (20);         	 // NVIC_ISER[20]=1 (habilita IRQ20)
    NVIC_ICPR |= 1 << (20);         	 // NVIC_ICPR[20]=1 (limpa as pendências)
    NVIC_IPR5 |= NVIC_IP_PRI_20(priority << 6); // 20/4 = 5			

    RTC_IER |= RTC_IER_TOIE_MASK; 		// ativa interrupcao em RTC

}
