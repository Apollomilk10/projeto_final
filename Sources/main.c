/**
 * @file main.c
 * @brief Este projeto demonstra o uso de um buffer circular para compatibilizar
 * a velocidade de dois dispositivos. Ao digitar uma das seguintes palavras em maiusculas 		
 * VERDE, VINHO, VIOLETA, VERMELHO, VIRIDIANO, são "ecoadas" a palavra digitada 10x no Terminal 
 * @author Wu Shin-Ting
 * @date 05/03/2022
 *
 */
#include "stdlib.h"
#include "string.h"
#include "buffer_circular.h"
#include "util.h"
#include "timers.h"
#include "UART.h"
#include "ISR.h"

#define BAUD_RATE 9600

/*!
 * @brief Variavel de configuracao de UART2
 */
UARTConfig_type config2 = {
		.bdh_sbns=0,		///< Um stop bit
		.sbr=0b100, 		///< MCGFLLCLK/(baud_rate*osr)=4
		.c1_loops=0,		///< Operacao normal: full-duplex 
		.c1_uartswai=0,		///< clock de UART continua correndo no modo WAIT
		.c1_rsrc=0,			///< nao tem efeito para loops == 0
		.c1_m=0,			///< dados em 8 bits
		.c1_wake=0,			///< wakeup de RX por linha ociosa (IDLE)
		.c1_ilt=0,			///< deteccao do estado ocioso pela contagem apos start bits
		.c1_pe=0,			///< sem checagem de paridade por hardware
		.c1_pt=0,			///< bit de paridade par (sem efeito se pe==0)
		.c2_rwu=0,			///< operacao normal de recepcao do UART0
		.c2_sbk=0,			///< operacao normal de transmissor do UART0
		.s2_rxinv=0,		///< polaridade dos dados de RX nao eh invertida
		.s2_rwuid=0,		///< nao tem efeito para rwu==0
		.s2_brk13=0,		///< nao tem efeito para operacao normal de transmissor
		.s2_lbkde=0,		///< deteccao do caractere break no tamanho menor
		.c3_r8=0,			///< precisa ser lido para completar o MSB dos dados quando m == 1
		.c3_t8=0,			///< precisa ser setado com o MSB dos dados quando m == 1
		.c3_txdir=0,		///< nao tem efeito para loops==0
		.c3_txinv=0,		///< polaridade dos dados de TX nao eh invertida 
		.c4_tdmas=0,		///< selecionar transmissor para transferencia DMA
		.c4_rdmas=0 		///< selecionar receptor para transferencia DMA
};


/*!
 * @brief Função principal
 */
int main (void)
{
	estado_type state_tag;
	
	char string[16];
	
	/*!
	 * Setando um valor inicial para string.
	 */
	string[0] = '\0';
	 
	/*!
	 * Seta o divisor de frequencia do barramento ???
	 */
	SIM_setaOUTDIV4(0b000);
	
	/*!
	 * Configurar UART2 com Rx habilitados: SBR tem que ser aproximado para maior que baud rate setado
	 */	
	config2.sbr = (uint16_t)(20971520./(BAUD_RATE * 16));
	if ((20971520./(1.0 * BAUD_RATE * 16)) > config2.sbr)
		config2.sbr++;
	UART_initH5Bluetooth(&config2);


	/*!
	 * Ativa IRQs
	 */
	UART_ativaIRQH5Bluetooth();
	
	/*!
	 * Inicia Buffer Circular para Rx
	 */
	ISR_inicializaBC();
	
	/*!
	 * Habilita a interrupcao do Rx do UART2
	 */
	UART2_ativaInterruptRxTerminal();

	/*!
	 * Reseta o estado do aplicativo
	 */
	ISR_AtualizaEstado(INICIO);
	state_tag = ISR_LeEstado();
	
	for(;;) {
			if (state_tag == RESULTADO) {
				ISR_extraiString(string);
			}
			
			state_tag = ISR_LeEstado();
	}
	
	return 0;
}

