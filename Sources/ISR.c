/*!
 * @file ISR.c
 * @brief Este modulo contem as rotinas de servico do aplicativo
 * @author Wu Shin Ting
 * @date 05/03/2022
 */

#include "derivative.h"
#include "buffer_circular.h"
#include "ISR.h"
#include "string.h"

#define TAM_MAX 128

static BufferCirc_type bufferE; //buffer de entrada

estado_type state_tag, new_state_tag;


void UART2_IRQHandler()
{

	if (UART2_S1 & UART_S1_RDRF_MASK) {
		/*!
		 * Interrupcao solicitada pelo canal Rx
		 */
		char r;

		r = UART2_D;
	
//		if ((state_tag = CRONOMETRO) || (state_tag = RELOGIO) || (state_tag = TERMOMETRO)) {
//			return;
//		} 
		
		//Ecoar o caractere
		UART2_D = r;
		//Adicionar no buffer circular de entrada
		if (r == '\r') {
			//inserir o terminador e avisar o fim de uma string
			BC_push (&bufferE, '\0');
			while (!(UART2_S1 & UART_S1_TDRE_MASK));
			UART2_D = '\n';
			state_tag = RESULTADO;
		} else {
			BC_push (&bufferE, r);
		}
	}
}


uint8_t ISR_LeEstado () {
	return state_tag;
}

void ISR_AtualizaEstado (uint8_t new_state_tag) {

	state_tag = new_state_tag;

}

void ISR_inicializaBC() {
	/*!
	 * Inicializa um buffer circular de entrada
	 */
	BC_init(&bufferE, TAM_MAX);

}

void ISR_extraiString(char *string) {
	
	char *cases[] = {"RELOGIO","CRONOMETRO","TERMOMETRO"};
	
	uint8_t i = 0; 
	
	BC_pop(&bufferE, &string[i]);
	while (string[i] != '\0') {
		BC_pop(&bufferE, &string[++i]);				
	}
	
	if (strcmp(string, cases[0])==0){
		ISR_AtualizaEstado(RELOGIO);
		
	} else if (strcmp(string, cases[1])==0){
		ISR_AtualizaEstado(CRONOMETRO);
		
	} else if (strcmp(string, cases[2])==0){
		ISR_AtualizaEstado(TERMOMETRO);
		
	}
	

}


void ISR_Realinhamento() {
	while (BC_push(&bufferE, '\n')==-1); //newline
	
	while (BC_push(&bufferE, '\r')==-1); //carriage return
	UART2_C2 |= UART_C2_TIE_MASK;
}

void ISR_BufferVazioE(){
	BC_free(&bufferE);
}

uint8_t ISR_BufferEntradaVazio() {
	
	return BC_isEmpty(&bufferE);
}
