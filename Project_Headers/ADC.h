/*!
 * @file ADC.h
 * @brief Este modulo contem declaracoes relacionadas com o modulo ADC.c
 * @author Wu Shin Ting
 * @date 06/03/2022
 */

#ifndef ADC_H_
#define ADC_H_

#include "derivative.h"

#define VREFH 3300			///< tensao de refeencia alta (mV)
#define VREFL 0.0 			///< tensao de referencia baixa(mV)

/**
 * @struct _ADCConfiguration
 */
typedef struct _ADCConfiguration {
	uint8_t sc1_diff;       ///< conversao unipolar (0) ou diferencial (1)
	uint8_t cfg1_adlpc;		///< configuracao de baixa potencia
	uint8_t cfg1_adiv;		///< divisor de frequencia do ADICLK
	uint8_t cfg1_adlsmp; 	///< configuracao do tempo de amostragem
	uint8_t cfg1_mode;		///< resolucao e formato da saida
	uint8_t cfg1_adiclk;	///< fonte de clock de entrada
	uint8_t cfg2_muxsel; 	///< selecao do canal: A (0) e B (1)
	uint8_t cfg2_adacken;	///< habilicao da saida de sinal de clock assincrono
	uint8_t cfg2_adhsc;		///< habilitacao de operacao em alta velocidade
	uint8_t cfg2_adlsts; 	///< selecao de tempo de amostragem longo
	uint8_t sc2_adtrg;		///< selecao de trigger: software (0) e hardware (1)
	uint8_t sc2_acfe; 		///< habilitacao de funcao de comparacao em igualdade
	uint8_t sc2_acfgt;		///< habilitacao de funcao de comparacao em desigualdade
	uint8_t sc2_acren;		///< habilitacao de comparacao de uma faixa
	uint8_t sc2_dmaen; 		///< habilitacao de DMA
	uint8_t sc2_refsel;		///< selecao de tensoes de referencia
	uint8_t sc3_adco;		///< habilitacao de amostragem continua (1) ou unica (0)
	uint8_t sc3_avge; 		///< habilitacao de media de amostras por hardware
	uint8_t sc3_avgs; 		///< selecao da quantidade de amostras por media
} ADCConfig;

/**
 * @brief Calibrar ADC usando a funcao de calibracao
 * 		integrada ao chip segundo o procedimento recomendado pelo fabricante.
 * @note Secao 28.4.6 de KL-25 Sub-Family Reference Manual
 * @param[in] config configuration parameters
 */
void ADC0_calibrate (ADCConfig *config);

/*!
 * @brief Configura o modo de opera&ccedil;&atilde;o do m&oacute;dulo ADC. Para compactar
 * 			os diversos par&acirc;metros de configura&ccedil;&atilde;o numa vari&aacute;vel
 * 			foi definida um novo tipo de dado (struct) ADCConf em adc.h.
 * @note Um exemplo de configura&ccedil;&atilde;o &eacute; dado em 
 * 		 ftp://ftp.dca.fee.unicamp.br/pub/docs/ea871/ARM/KLQRUG.pdf#page=117
 * @param[in] config parametros de configuracao.
 */
void ADC0_configure(ADCConfig *config);

/**
 * @brief Inicializar ADC
 */
void ADC0_init(ADCConfig *config);

/*!
 * @brief Associa pinos fisicos aos canais
 * @note ADC Channel: pg 79 em ftp://ftp.dca.fee.unicamp.br/pub/docs/ea871/ARM/KL25P80M48SF0RM.pdf
 * @note MUX: pg 162 em ftp://ftp.dca.fee.unicamp.br/pub/docs/ea871/ARM/KL25P80M48SF0RM.pdf
 * 
 */
void ADC0_connectPTB1ToChannel ();

/*!
 * @brief Setar trigger por hardware
 */
void ADC0_initADHWT (uint8_t trigger);

/**
 * @brief Selecionar o canal de entrada analogica unipolar
 */
void ADC0_selChannel (uint8_t canal);

/**
 * @brief Ativa interrupcao do ADC0
 * @param[in] priority de interrupcao dos eventos de ADC0
 */
void ADC0_ativaIRQ (uint8_t priority);

#endif /* ADC_H_ */
