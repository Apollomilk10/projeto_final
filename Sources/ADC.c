/*!
 * @file ADC.c
 * @brief Este modulo contem interface do módulo ADC.
 * @author Wu Shin Ting
 * @date 06/03/2022
 */

#include "derivative.h"
#include "ADC.h"


void ADC0_calibrate (ADCConfig *config)
{
	uint16_t tmp=0;
	
	/*!
	 * Configurar qtde de entradas analogicas (unipolar/biploar) e desabilitar o canal
	 */
	ADC0_SC1A &= ~(ADC_SC1_DIFF_MASK );

	ADC0_SC1A |= (config->sc1_diff << ADC_SC1_DIFF_SHIFT);
	
	ADC0_SC1A |= ADC_SC1_ADCH(31);
	
	/*!
	 * Configurar frequencia ADCK (<= 4MHz) e resolucao, formato de saida
	 */ 
	ADC0_CFG1 &= ~(ADC_CFG1_ADICLK_MASK |
			ADC_CFG1_MODE_MASK |
			ADC_CFG1_ADLSMP_MASK |
			ADC_CFG1_ADIV_MASK |
			ADC_CFG1_ADLPC_MASK);
			
	ADC0_CFG1 |= (config->cfg1_adiclk << ADC_CFG1_ADICLK_SHIFT |	
			config->cfg1_mode << ADC_CFG1_MODE_SHIFT |
			config->cfg1_adlsmp << ADC_CFG1_ADLSMP_SHIFT |
			0b11 << ADC_CFG1_ADIV_SHIFT |
			config->cfg1_adlpc << ADC_CFG1_ADLPC_SHIFT);

	/*!
	 * Configurar o tempo de conversao e a velocidade de operacao
	 */ 
	ADC0_CFG2 &= ~(ADC_CFG2_MUXSEL_MASK |
			ADC_CFG2_ADACKEN_MASK |
			ADC_CFG2_ADHSC_MASK |
			ADC_CFG2_ADLSTS_MASK);
	
	ADC0_CFG2 |= (config->cfg2_muxsel << ADC_CFG2_MUXSEL_SHIFT |
			config->cfg2_adacken << ADC_CFG2_ADACKEN_SHIFT |
			config->cfg2_adhsc << ADC_CFG2_ADHSC_SHIFT |
			config->cfg2_adlsts << ADC_CFG2_ADLSTS_SHIFT);

	/*!
	 * Configurar fonte de referencia, trigger, funcao de comparacao habilitada e acesso DMA
	 */ 
	ADC0_SC2 &= ~( ADC_SC2_ADTRG_MASK |
			ADC_SC2_ACFE_MASK  |
			ADC_SC2_ACFGT_MASK |
			ADC_SC2_ACREN_MASK | 
			ADC_SC2_DMAEN_MASK |
			ADC_SC2_REFSEL_MASK);
	
	ADC0_SC2 |= (config->sc2_acfe << ADC_SC2_ACFE_SHIFT  |
			0b1 << ADC_SC2_ACFGT_SHIFT |
			0b1 << ADC_SC2_ACREN_SHIFT | 
			config->sc2_dmaen << ADC_SC2_DMAEN_SHIFT |
			0b00 << ADC_SC2_REFSEL_SHIFT);
	
	/*!
	 * Setar valores de comparacao
	 */ 
	ADC0_CV1 = 0x1234u;
	ADC0_CV2 = 0x5678u;

	/*!
	 * Configurar modo de amostrage e media das amostras (32)
	 */ 
	ADC0_SC3 &= ~( ADC_SC3_ADCO_MASK |
			ADC_SC3_AVGE_MASK |
			ADC_SC3_AVGS_MASK);
	
	ADC0_SC3 |= (0b0 << ADC_SC3_ADCO_SHIFT |
			0b1 << ADC_SC3_AVGE_SHIFT |
			0b11 << ADC_SC3_AVGS_SHIFT);
	
	/*!
	 * Calibrar
	 */
	ADC0_SC3 |= (ADC_SC3_CAL_MASK);			///< inicializa a calibracao	
	while (ADC0_SC3 & ADC_SC3_CAL_MASK)  {}	///< aguarda a calibracao

	/*!
	 * Gerar as correcos de erros de ganho (positivo)
	 */ 
	tmp = ADC0_CLP0 + ADC0_CLP1 + ADC0_CLP2 + 
			ADC0_CLP3 + ADC0_CLP4 + ADC0_CLPS;	///< soma PS
	tmp = tmp/2;								///< divide por 2
	tmp |= (uint16_t)(0x8000);					///< setar o bit mais significativo
	ADC0_PG = tmp;								///< plus-side gain
	
	/*!
	 *  Somente util para modos diferenciais 
	 */
	tmp = ADC0_CLM0 + ADC0_CLM1 + ADC0_CLM2 + 
			ADC0_CLM3 + ADC0_CLM4 + ADC0_CLMS;	///< soma PS
	tmp = tmp/2;								///< divide por 2
	tmp |= (uint16_t)(0x8000);					///< setar o bit mais significativo
	ADC0_MG = tmp;								///< minus side gain
	
	/*!
	 * A compensacao do erro de offset eh atribuida em ADC0_OFS automaticamente
	 */ 	
}

void ADC0_configure(ADCConfig *config) {
	
	/*!
	 * Configurar qtde de entradas analogicas (unipolar/biploar)
	 */ 
	ADC0_SC1A &= ~(ADC_SC1_DIFF_MASK );

	ADC0_SC1A |= (config->sc1_diff << ADC_SC1_DIFF_SHIFT);
	
	ADC0_SC1A |= ADC_SC1_ADCH(31);
	
	/*!
	 * Configurar frequencia ADCK e resolucao, formato de saida
	 */ 
	ADC0_CFG1 &= ~(ADC_CFG1_ADICLK_MASK |
			ADC_CFG1_MODE_MASK |
			ADC_CFG1_ADLSMP_MASK |
			ADC_CFG1_ADIV_MASK |
			ADC_CFG1_ADLPC_MASK);
			
	ADC0_CFG1 |= (config->cfg1_adiclk << ADC_CFG1_ADICLK_SHIFT |	
			config->cfg1_mode << ADC_CFG1_MODE_SHIFT |
			config->cfg1_adlsmp << 	ADC_CFG1_ADLSMP_SHIFT |
			config->cfg1_adiv << ADC_CFG1_ADIV_SHIFT |
			config->cfg1_adlpc << ADC_CFG1_ADLPC_SHIFT);

	/*!
	 * Configurar o tempo de conversao e a velocidade de operacao
	 */ 
	ADC0_CFG2 &= ~(ADC_CFG2_MUXSEL_MASK |
			ADC_CFG2_ADACKEN_MASK |
			ADC_CFG2_ADHSC_MASK |
			ADC_CFG2_ADLSTS_MASK);
	
	ADC0_CFG2 |= (config->cfg2_muxsel << ADC_CFG2_MUXSEL_SHIFT |
			config->cfg2_adacken << ADC_CFG2_ADACKEN_SHIFT |
			config->cfg2_adhsc << ADC_CFG2_ADHSC_SHIFT |
			config->cfg2_adlsts << ADC_CFG2_ADLSTS_SHIFT);

	/*!
	 * Configurar fonte de referencia, trigger, funcao de comparacao e acesso DMA
	 */ 
	ADC0_SC2 &= ~( ADC_SC2_ADTRG_MASK |
			ADC_SC2_ACFE_MASK  |
			ADC_SC2_ACFGT_MASK |
			ADC_SC2_ACREN_MASK | 
			ADC_SC2_DMAEN_MASK |
			ADC_SC2_REFSEL_MASK);
	
	ADC0_SC2 |= ( config->sc2_adtrg << ADC_SC2_ADTRG_SHIFT |
			config->sc2_acfe << ADC_SC2_ACFE_SHIFT  |
			config->sc2_acfgt << ADC_SC2_ACFGT_SHIFT |
			config->sc2_acren << ADC_SC2_ACREN_SHIFT | 
			config->sc2_dmaen << ADC_SC2_DMAEN_SHIFT |
			config->sc2_refsel << ADC_SC2_REFSEL_SHIFT);
	
	/*!
	 * Configurar modo de amostrage e media das amostras 
	 */ 
	ADC0_SC3 &= ~( ADC_SC3_ADCO_MASK |
			ADC_SC3_AVGE_MASK |
			ADC_SC3_AVGS_MASK);
	
	ADC0_SC3 |= (config->sc3_adco << ADC_SC3_ADCO_SHIFT |
			config->sc3_avge << ADC_SC3_AVGE_SHIFT |
			config->sc3_avgs << ADC_SC3_AVGS_SHIFT);
}

void ADC0_init(ADCConfig *config) {
	
	//Ativa o sinal de relogio
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
	
	//Calibrar ADC0
	ADC0_calibrate(config);
	
	//Configurar ADC0
	ADC0_configure(config);
}

void ADC0_connectPTB1ToChannel () {
	//Ativa o sinal de clock
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	/*!
	 * ADC Channel: 0b01001 -> ADC0_SE9 -> PTB1 (MUX 0b000)
	 */
	PORTB_PCR1 &= ~PORT_PCR_MUX (0b111); 
	PORTB_PCR1 |= PORT_PCR_ISF_MASK;  
}

void ADC0_initADHWT (uint8_t trigger) {
	/*!
	 * Configurar o trigger por hardware
	 */
	SIM_SOPT7 &= SIM_SOPT7_ADC0TRGSEL(0b111);      ///< resetar o campo
	
	SIM_SOPT7 |= (SIM_SOPT7_ADC0ALTTRGEN_MASK      ///< trigger alternativo de TPM1_CH0 e TPM1_CH1
			| SIM_SOPT7_ADC0TRGSEL(trigger)) ;      
	
	SIM_SOPT7 &= ~SIM_SOPT7_ADC0PRETRGSEL_MASK;	   ///< pre-trigger A

	/*!
	 * Habilitar trigger por hardware
	 */ 
	ADC0_SC2 |= ADC_SC2_ADTRG_MASK;
}

void ADC0_selChannel (uint8_t canal) {
	ADC0_SC1A &= ~ADC_SC1_ADCH (0b11111);
	
	ADC0_SC1A |= ADC_SC1_ADCH (ADC_SC1_ADCH(canal));
}

void ADC0_ativaIRQ (uint8_t priority) {
    /*! 
     * Ativa IRQ 15 (ADC) no NVIC: ativar, limpar pendencias e setar prioridade 
     */
	NVIC_ISER |= 1 << (15);         	// NVIC_ISER[15]=1 (habilita IRQ15)
	NVIC_ICPR |= 1 << (15);         	// NVIC_ICPR[15]=1 (limpa as pendencias)
	NVIC_IPR3 |= NVIC_IP_PRI_15(priority << 6); // 15/4 = 3
	
	ADC0_SC1A |= ADC_SC1_AIEN_MASK;             // ativar interrupcao ADC0
}

