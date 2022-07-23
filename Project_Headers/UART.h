/*!
 * @file UART.h
 * @brief Este modulo contem declaracoes relacionadas com o modulo UART.c
 * @author Wu Shin Ting
 * @date 05/03/2022
 */

#ifndef UART_H_
#define UART_H_

typedef struct _UARTConfiguration_tag {
	uint8_t	bdh_sbns;		///< selecionar quantidade de stop bits (0 = 1; 1 = 2)
	uint16_t sbr;		
	uint8_t c1_loops;       ///< operacao em loop (normal = 0)
	uint8_t c1_uartswai;	///< comportamento do clock de UART no modo WAIT (continua correndo = 0) 
	uint8_t c1_rsrc;		///< habilitar a saida do TX em operação de loop
	uint8_t c1_m; 			///< 8-bit (0) ou 9-bit de dados
	uint8_t c1_wake;		///< forma de wakeup do RX (0-idle line; 1-address-mark)
	uint8_t c1_ilt;			///< selecionar a forma de deteccao de "idle line"
	uint8_t c1_pe; 			///< habilitar paridade
	uint8_t c1_pt;			///< tipo de paridade (0-par; 1-impar)
	uint8_t c2_rwu; 		///< setar o RX no estado de standby aguardando pelo wakeup
	uint8_t c2_sbk;			///< habilitar o enfileiramento de caracteres break
	uint8_t s2_rxinv;		///< habilitar a inversao da polaridade dos bits do RX
	uint8_t s2_rwuid;		///< habilitar o set do IDLE bit durante standby do RX
	uint8_t s2_brk13;		///< selecionar o comprimento de caracter break (0=10 bits; 1=13 bits)
	uint8_t s2_lbkde;		///< habilitar deteccao de caractere break longo
	uint8_t c3_r8; 			///< bit 8 (RX)
	uint8_t c3_t8;			///< bit 8 (TX)
	uint8_t c3_txdir; 		///< na configuracao RSRC=1, configurar o sentido de TX (0=entrada; 1=saida)
	uint8_t c3_txinv;		///< habilitar a inversao da polaridade dos bits de TX
	uint8_t c4_tdmas;		///< selecionar transmissor para transferencia DMA
	uint8_t c4_rdmas; 		///< selecionar receptor para transferencia DMA
} UARTConfig_type;

/*!
 * @brief Configurar o modo de operação de UART0
 * @param[in] config parametros do modo de operacao
 */

uint8_t UART_configure(uint8_t m, UARTConfig_type *config);

void UART_initH5Bluetooth(UARTConfig_type *config);

void UART_ativaIRQH5Bluetooth();

void UART2_ativaInterruptRxTerminal();

#endif /* UART_H_ */
