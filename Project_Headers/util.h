/*
 * util.h
 *
 *  Created on: Jun 23, 2022
 *      Author: ea871
 */

#ifndef UTIL_H_
#define UTIL_H_

/*!
 * @def GPIO_PIN
 * @brief Constroi uma mascara com somente bit x em 1  
 */
#define GPIO_PIN(x)  ((1)<<(x)) 
#define TAM_MAX 128
//==================================
//Implementacao da nossa máquina de estados 
//==================================

typedef enum state_tag {
	INICIO,
	RESET,
	LEITURA_UART,
	RESULTADO,
	RELOGIO,				///< operacao normal: relogio e termometro
	AJUSTE_HORA,			///< ajuste de hora
	AJUSTE_HORA_INC,		///< incremento de hora
	AJUSTE_HORA_DEC,		///< decremento de hora
	AJUSTE_MINUTO,			///< ajuste de minuto
	AJUSTE_MINUTO_INC,		///< incremento de minuto
	AJUSTE_MINUTO_DEC,		///< decremento de minuto
	AJUSTE_SEGUNDO,			///< ajuste de segundo
	AJUSTE_SEGUNDO_INC,		///< incremento de segundo
	AJUSTE_SEGUNDO_DEC,		///< decremento de segundo
	CRONOMETRO,     	///< inicio de cronometragem
	ESPERA,			///< fim de cronometragem	
	CONTAGEM,
	LEITURA,
	REALIMENTACAO,
	INTERVALO,
	RESETA_VISOR,
	TERMOMETRO,
	UART0_IN				///< entrada de dados em UART0
} estado_type;

void delay_10us (unsigned int multiplos);

char *ttoa (uint32_t seconds, char *string);

char *itoa (int n, char *string, char b);

void s2dhms (uint32_t segundos, uint32_t *DD, uint32_t *HH, uint32_t *MM, uint32_t *SS);

void dhms2s (uint8_t DD, uint8_t HH, uint8_t MM, uint8_t SS, uint32_t *segundos);


int atoOp(char *string, char *op, float *fvalor, int *ivalor, int *efloat);

void reverse(char *str, int tamanho);

int itos (int x, char str[], int d);

char *ftoa(float m, char* res, int decimal);

void processa_Terminal (char c);	


#endif /* UTIL_H_ */
