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

static BufferCirc_type bufferE, bufferS; //buffer de entrada

estado_type state_tag, new_state_tag;

//UART_MemMapPtr UART2[] = UART_BASE_PTRS;

/**
 * @brief Rotina de servico do IRQ15 (ADC0)
 */
//void ADC0_IRQHandler(void) {
//	if(ADC0_SC1A & ADC_SC1_COCO_MASK)
//	{
//		v = ADC0_RA;
//		if (state_tag == AMOSTRA_TEMP) {
//			valor[1] = v;					//guardar o valor amostrado
//			ISR_AtualizaEstado(TEMPERATURA);
//		}
//	}
//}


void UART2_IRQHandler()
{
	char t, r;
	
	if ((UART2_C2 & UART_C2_TIE_MASK)) {
		/*!
		 * Interrupcao solicitada pelo canal Tx
		 */
		
		BC_pop(&bufferS, &t);
		if  (t == '\0'){
			UART2_D = '\r';
			BC_free(&bufferS);
			UART2_C2 &= ~UART_C2_TIE_MASK;
		} else {
			UART2_D = t;			
		}
	} else if ((UART2_S1 & UART_S1_RDRF_MASK)) {
		/*!
		 * Interrupcao solicitada pelo canal Rx
		 */

		r = UART2_D;
		
		UART2_D = r; // Ecoar informação para leitura no Bluetooth
		
		//Adicionar no buffer circular de entrada
		if (r == '\r') {
			//inserir o terminador e avisar o fim de uma string
			BC_push(&bufferE, '\0');
			UART2_C2 &= ~UART_C2_TIE_MASK;
			state_tag = RESULTADO;
		} else {
			BC_push(&bufferE, r);
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
	BC_init(&bufferS, TAM_MAX);

}


void ISR_enviaString(char *string){

	uint8_t i;
	while(BC_push(&bufferS, string[0])==-1);	
	UART2_C2 |= UART_C2_TIE_MASK;
	i=1;
	while (string[i] != '\0'){
		while(BC_push(&bufferS, string[i])==-1);
		i++;
	}
	while(BC_push(&bufferS, '\n')==-1);
	while(BC_push(&bufferS, '\0')==-1);
	state_tag = INICIO;
}

void ISR_extraiString(char *string) {
	// TO DO:
	//precisamos de strings bem definidas olhar no debug !
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
	
	//state_tag = INICIO;
	state_tag = ENVIA;
	BC_free(&bufferE);
	
}


void ISR_Realinhamento() {
	while (BC_push(&bufferE, '\n')==-1); //newline
	
	while (BC_push(&bufferE, '\r')==-1); //carriage return
	UART2_C2 |= UART_C2_TIE_MASK;
}


uint8_t ISR_BufferSaidaVazio() {
	
	return BC_isEmpty(&bufferS);
}


uint8_t ISR_BufferEntradaVazio(){
	
	return BC_isEmpty(&bufferE);
}
