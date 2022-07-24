/*!
 * @file ISR.c
 * @brief Este modulo contem as rotinas de servico do aplicativo
 * @author Wu Shin Ting
 * @date 05/03/2022
 */

#include "derivative.h"
#include "buffer_circular.h"
#include "ISR.h"
#include "util.h"
#include "string.h"

#define TAM_MAX 128

static BufferCirc_type bufferE; //buffer de entrada

/**** lab cronometro****/
static estado_type state_tag;
static unsigned int tempo_cronometro,multiplos;

static unsigned short t1, t2;

/**
 * @brief Rotina de servico
 */
void FTM0_IRQHandler () {
	static unsigned int counter;
	/*!
	 * Consulta se o bit TPM0_CH2 (Input Capture) esta setado
	 */
	if (TPM0_STATUS & TPM_STATUS_CH2F_MASK) {
		if (ISR_LeEstado()== ESPERA){
			
			t1 = TPM0_C2V;
			counter = 0;
			TPM_ativaLedB (); 
			
			ISR_AtualizaEstado(CONTAGEM);
			tempo_cronometro = 0;


		}
		else if(ISR_LeEstado()== CONTAGEM || ISR_LeEstado()== REALIMENTACAO){
			
			t2 = TPM0_C2V;
			TPM_apagaLedB ();
			
			multiplos = counter;
			counter = 0;
			
			ISR_AtualizaEstado(INTERVALO);		


		}
		/*!
		 * w1c
		 */
		TPM0_C2SC |= TPM_CnSC_CHF_MASK;     	// limpar solicitacao da chave   
	}

	/*!
	 * Consulta se o bit TPM1_CH1 (Output Compare) esta setado
	 */
	if (TPM0_STATUS & TPM_STATUS_CH1F_MASK) {
	
		counter ++;
		
		if((ISR_LeEstado()== CONTAGEM) && (counter/5) > tempo_cronometro){
			tempo_cronometro = counter/5;
			ISR_AtualizaEstado(REALIMENTACAO);
		}
		if((ISR_LeEstado()== LEITURA) && (counter /5) > 5){
			ISR_AtualizaEstado(RESETA_VISOR);
		}
		/*!
		 * w1c
		 */
		TPM0_C1SC |= TPM_CnSC_CHF_MASK;   
}
}



void UART2_IRQHandler()
{

	if (UART2_S1 & UART_S1_RDRF_MASK) {
		/*!
		 * Interrupcao solicitada pelo canal Rx
		 */
		char r;
		
		r = UART2_D;
		BC_push (&bufferE, r);
		state_tag = RESULTADO;
		
		//Ecoar o caractere
		//UART2_D = r;
		//Adicionar no buffer circular de entrada
		//if (r == '\r') {
			//inserir o terminador e avisar o fim de uma string
			//BC_push (&bufferE, '\0');
			//while (!(UART2_S1 & UART_S1_TDRE_MASK));
			//UART2_D = '\n';
			//state_tag = RESULTADO;
		//} else {
			//BC_push (&bufferE, r);
		//}
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
	
	char *cases[] = {'c/','2/','3/','4/'};
	
	uint8_t i = 0; 
	
	BC_pop(&bufferE, &string[i]);
	while (string[i] != '\0') {
		BC_pop(&bufferE, &string[++i]);				
	}
	
	if (strcmp(string, cases[0])==0){
		state_tag = RELOGIO;
		
	} else if (strcmp(string, cases[1])==0){
		state_tag = CRONOMETRO;
		
	} else if (strcmp(string, cases[2])==0){
		state_tag = TERMOMETRO;
		
	}else if (strcmp(string, cases[3])==0){
		state_tag = RESET;
		
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


int ISR_LeTempo_Cronometro(){
	return tempo_cronometro;
}

float ComputaIntervaloTempo(){
	
	float periodo = 0.4;
	
	
	float counter = 0;
	int mod = TPM0_MOD;
	float ft1 = (float) t1;
	float ft2 = (float) t2;
	
	
	if(t1>t2){
		counter = (((mod + ft2 - ft1)/mod)+(multiplos-1))*periodo*0.5;
		return counter;
		
	}else if(t2>=t1){
		counter = (((ft2 - ft1)/mod)+(multiplos))*periodo*0.5;
		return counter;
		
	}
	return 0;
	
}
