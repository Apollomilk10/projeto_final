/*!
 * @file timers.h
 * @brief Este modulo contem declaracoes relacionadas com o modulo timers.c
 * @author Wu Shin Ting
 * @date 01/03/2022
 */
 
#ifndef TIMERS_H_
#define TIMERS_H_

#include "stdint.h"

/*!
 * @brief Seta o divisor de frequencia do sinal de barramento
 * @param[in] OUTDIV4 divisor do sinal MCGOUTCLK
 */
void SIM_setaOUTDIV4 (uint8_t OUTDIV4);

/**
 * @brief Configura o temporizador SysTick com um valor de recarga. A base de tempo
 * eh o relogio do nucleo (processador), configurado em 20971520Hz
 * @param[in] periodo valor de recarga
 */
void SysTick_init (uint32_t periodo);

/**
 * @brief Ativa interrupcao do SysTick ("overflow")
 */
void SysTick_ativaInterrupt ();

	
/**
 * @brief Desativa interrupcao do SysTick
 */
void SysTick_desativaInterrupt ();

/**
 * @brief Configura o timer 0 do PIT para que gere pulsos periodicos para um dado valor de recarga.
 * A base de tempo eh o sinal de relogio do barramento (MCGOUTCLK/(OUTDIV1*OUTDIV4)), 
 * ou seja 20971520/(OUTDIV1*OUTDIV2)
 * @param[in] periodo valor de recarga
 */
void PIT_initTimer0(uint32_t periodo);

/**
 * @brief Ativa interrupcao do PIT ("overflow")
 * @param[in] priority
 */
void PIT_ativaIRQ (uint8_t priority);

/**
 * @brief Configura o modulo LPTMR0
 * @param[in] valor contagem maxima
 * 
 * @note https://www.dca.fee.unicamp.br/cursos/EA871/references/ARM/KL25P80M48SF0RM.pdf#page=90
 */
uint8_t LPTimer_init (uint16_t valor);

/**
 * @brief Ativa interrupcao do LPTMR0 (overflow)
 * @param[in] priority
 */
void LPTimer_ativaIRQ (uint8_t priority);

/**
 * @brief Configura o modulo RTC com a fonte de clock LPO
 * 
 * @note Ao inves do prescaler (RTC_TPR) incrementar em 32.768kHz, incrementara em 1kHz  
 */
void RTClpo_init();

/**
 * @brief Ler o valor atual do contador de segundos do RTC
 * @param[out] seconds medida de tempo em segundos
 */
void RTClpo_getTime (uint32_t *seconds);

/**
 * @brief Setar o valor do contador de segundos do RTC
 * @param[in] seconds valor de tempo em segundos
 */
void RTClpo_setTime (uint32_t seconds);

/**
 * @brief Ativa interrupcao de segundos do RTC
 * @param[in] priority
 */
void RTC_ativaSegundoIRQ (uint8_t priority);

/**
 * @brief Ativa interrupcao de alarme do RTC
 * @param[in] priority
 */
void RTC_ativaAlarmIRQ (uint8_t priority);
	
/**
 * @brief Ativa interrupcao de overflow do RTC
 * @param[in] priority
 */
void RTC_ativaOverflowIRQ (uint8_t priority);

#endif /* TIMERS_H_ */
