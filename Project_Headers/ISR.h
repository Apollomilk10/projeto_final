/*!
 * @file ISR.h
 * @brief Este modulo contem declaracoes relacionadas com o modulo ISR.c
 * @author Wu Shin Ting
 * @date 05/03/2022
 */

#ifndef ISR_H_
#define ISR_H_

typedef enum state_tag{
INICIO,
ENVIA,
RESULTADO,
CRONOMETRO,
RELOGIO,
TERMOMETRO }estado_type;

/*!
 * @brief Le o estado do aplicativo
 */
uint8_t ISR_LeEstado ();


void ADC0_IRQHandler(void);

void UART2_IRQHandler();

/*!
 * @brief Atualiza o estado do aplicativo
 * @param[in] novo estado
 */
void ISR_AtualizaEstado(uint8_t new_state_tag);

/*!
 * @brief Inicializa buffers circulares para o aplicativo
 */
void ISR_inicializaBC();

/*!
 * @brief Extrai do buffer de entrada uma linha de caracteres.
 * @param[out] string contendo a linha terminada com '\0'.
 */
void ISR_extraiString(char *string);


/*!
 * @brief Envia do buffer de saida uma linha de caracteres.
 * @param[out] string contendo a linha terminada com '\0'.
 */
void ISR_enviaString(char *string);

/*!
 * @brief Envia caracteres de controle para realinhamento das linhas no Terminal
 */
void ISR_Realinhamento();

/*!
 * @brief Verificar se o buffer de saida esta vazio
 * @return estado vazio do buffer
 */
uint8_t ISR_BufferSaidaVazio();

//void ISR_BufferVazioE();

//void ISR_BufferVazioS();

/*!
 * @brief Verificar se o buffer de saida esta vazio
 * @return estado vazio do buffer
 */
uint8_t ISR_BufferEntradaVazio();

#endif /* ISR_H_ */
