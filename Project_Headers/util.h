/*
 * util.h
 *
 *  Created on: Jun 23, 2022
 *      Author: ea871
 */

#ifndef UTIL_H_
#define UTIL_H_

//==================================
//Implementacao da nossa máquina de estados 
//==================================

typedef enum state_tag {
	BLE,
	INICIO,
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
	PADRAO

} estado_type;

//==================================
//Implementacao da nossas transições dos macro estados 
//==================================

typedef struct transition_BLE {
	char transition1[9];				
	char transition2[9];     	
	char transition3[9];
	char transition4[9];
} Transition_type;

void delay_10us (unsigned int multiplos);

char *ttoa (uint32_t seconds, char *string);

char *itoa (int n, char *string, char b);

void s2dhms (uint32_t segundos, uint32_t *DD, uint32_t *HH, uint32_t *MM, uint32_t *SS);

void dhms2s (uint8_t DD, uint8_t HH, uint8_t MM, uint8_t SS, uint32_t *segundos);

char *ftoa(float valor, char* string);

int atoOp(char *string, char *op, float *fvalor, int *ivalor, int *efloat);



#endif /* UTIL_H_ */
