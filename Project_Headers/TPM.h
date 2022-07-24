/*!
 * @file TPM.h
 * @brief Este modulo contem declaracoes relacionadas com o modulo TPM.c
 * @author Wu Shin Ting
 * @date 03/03/2022
 */

#ifndef TPM_H_
#define TPM_H_

#include "stdint.h"
#include "util.h"

/**
 * @brief Inicialia modulo TPM
 * @param[in] unidado do modulo
 * @param[in] periodo
 * @param[in] ps pre-scale
 */
void TPM_init (uint8_t x, uint16_t periodo, uint8_t ps);


/*!
 * @brief Configura o canal n do temporizador TPMx em IC. 
 * @param[in] x modulo TPM
 * @param[in] n canal
 * @param[in] mode modo de operacao em codigo binario (borda de subida, descida ou ambas)
 */ 
void TPM_initChIC (uint8_t x, uint8_t n, uint8_t mode);

/*!
 * @brief Ativa/Inicializa o canal n do temporizador TPMx em OC. 
 * @param[in] x modulo TPM
 * @param[in] n canal
 * @param[in] mode modo de operacao em codigo binario (alternado, 0 ou 1)
 * @param[in] valor valor de comparacao
 */ 
void TPM_initChOC (uint8_t x, uint8_t n, uint8_t mode, uint16_t valor);

/**
 * @brief Desativa o canal TPMxCHn
 */
void TPM_desativaCh (uint8_t x, uint8_t n);

/*	LAB 13 */

/**
 * @brief Habilita IRQ do par (IRQA5,LEDb) no NVIC e setar prioridade 
 * @param[in] priority prioridade de atendimento
 */
void TPM_habilitaSwitchIRQA5LedBInterrupt (char priority);

/*!
 * Inicilizar o par (botoeira IRQA5,canal do LED B) 
 */
void TPM_initSwitchIRQA5LedB(uint32_t periodo, uint8_t ps);

/*!
 * Ativa LED B
 */
void TPM_ativaLedB();

/*!
 * Desativa interrupção no LED B
 */
void TPM_desativaLedbInterrupt();

/*!
 * Apaga LED B
 */
void TPM_apagaLedB();

#endif /* TPM_H_ */
