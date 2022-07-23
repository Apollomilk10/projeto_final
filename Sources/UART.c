/*!
 * @file UART.c
 * @brief Este modulo contem interface dos módulos UARTx.
 * @author Wu Shin Ting
 * @date 05/03/2022
 */

#include "derivative.h"
#include "UART.h"

UART_MemMapPtr UART[] = UART_BASE_PTRS ;

uint8_t UART_configure(uint8_t m, UARTConfig_type *config) {
	uint8_t x = m-1;

	/*!
	 * Ativa sinal de relogio do modulo UART0
	 */
	SIM_SCGC4 |= (SIM_SCGC4_UART1_MASK << x);
	
	SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;  // PLLFLLSEL=SIM_SOPT2[16]=0 (MCGFLLCLK)		
	SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC(0b11); // configurar a fonte de relógio (20.971520MHz)
	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(0x1);	

	/*!
	 * Desabilitar canais RX e TX para configuracoes
	 */ 
	UART[x]->C2 &= ~(UART_C2_RE_MASK |           
			UART_C2_TE_MASK);

	/*!
	 * Configurar stop bits
	 */ 
	UART[x]->BDH &= ~(UART_BDH_SBNS_MASK);
	UART[x]->BDH |= config->bdh_sbns << UART_BDH_SBNS_SHIFT;

	/*!
	 * Configurar o registrador C1
	 */ 
	UART[x]->C1 &= ~(UART_C1_LOOPS_MASK |
			UART_C1_UARTSWAI_MASK |
			UART_C1_RSRC_MASK |
			UART_C1_M_MASK |                 
			UART_C1_WAKE_MASK |
			UART_C1_ILT_MASK |
			UART_C1_PE_MASK |			
			UART_C1_PT_MASK
	);

	UART[x]->C1 |= (config->c1_loops << UART_C1_LOOPS_SHIFT |
			config->c1_uartswai << UART_C1_UARTSWAI_SHIFT |
			config->c1_rsrc << UART_C1_RSRC_SHIFT |
			config->c1_m << UART_C1_M_SHIFT |                 
			config->c1_wake << UART_C1_WAKE_SHIFT |
			config->c1_ilt << UART_C1_ILT_SHIFT |
			config->c1_pe << UART_C1_PE_SHIFT |			
			config->c1_pt << UART_C1_PT_SHIFT
	);

	/*!
	 * Configurar o registrador C2
	 */ 
	UART[x]->C2 &= ~(UART_C2_RWU_MASK |
			UART_C2_SBK_MASK);

	UART[x]->C2 |= (config->c2_rwu << UART_C2_RWU_SHIFT |
			config->c2_sbk << UART_C2_SBK_SHIFT
	);

	/*!
	 * Configurar o registrador S2
	 */ 
	UART[x]->S2 &= ~(UART_S2_RXINV_MASK |
			UART_S2_RWUID_MASK |
			UART_S2_BRK13_MASK |
			UART_S2_LBKDE_MASK
	);

	UART[x]->S2 |= (config->s2_rxinv << UART_S2_RXINV_SHIFT |
			config->s2_rwuid << UART_S2_RWUID_SHIFT |
			config->s2_brk13 << UART_S2_BRK13_SHIFT |
			config->s2_lbkde << UART_S2_LBKDE_SHIFT
	);

	/*!
	 *  Configurar o registrador C3
	 */
	UART[x]->C3 &= ~(UART_C3_R8_MASK |
			UART_C3_T8_MASK |
			UART_C3_TXDIR_MASK |
			UART_C3_TXINV_MASK 
	);

	UART[x]->C3 |= (config->c3_r8 << UART_C3_R8_SHIFT |
			config->c3_t8 << UART_C3_T8_SHIFT |
			config->c3_txdir << UART_C3_TXDIR_SHIFT |
			config->c3_txinv << UART_C3_TXINV_SHIFT 
	);

	/*!
	 * Configurar o registrador C4
	 */ 
	UART[x]->C4 &= ~(UART_C4_TDMAS_MASK |
			UART_C4_RDMAS_MASK
	);

	/*!
	 * Setar baud rate
	 */
	config->sbr |= (config->sbr & 0x1FFF);      // mascarar bits invalidos

	UART[x]->BDH &= ~UART_BDH_SBR(0x1F);                             
	UART[x]->BDL &= ~UART_BDL_SBR(0xFF);            
	UART[x]->BDH |= UART_BDH_SBR(config->sbr >> 8);                             
	UART[x]->BDL |= UART_BDL_SBR(config->sbr & 0x00FF);            

	/*!
	 * Habilitar os canais RX e TX
	 */
	UART[x]->C2 |= (UART_C2_RE_MASK |         		// habilita o canal receptor 
			UART_C2_TE_MASK);	  				// habilita o canal transmissor  
	
	return 0;
}


void UART_initH5Bluetooth (UARTConfig_type *config) {
	/*!
	 * Configura funcao Tx do pino PTE22 para obtenção dos valores do Bluetooth
	 */	
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK ;

	PORTE_PCR23 |= PORT_PCR_MUX(0x4);  // UART2_RX

	UART_configure(2, config);
}

void UART_ativaIRQH5Bluetooth() {
    /*! 
     * Ativa IRQ 14 (UART2) no NVIC: ativar, limpar pendencias e setar prioridade 1
     */
	NVIC_ISER |= 1 << (14) ;      		 ///< Habilitar IRQ14 (UART2)
	NVIC_ICPR |= 1 << (14) ;      		 ///< Limpar as pendencias em solicitacao de interrupcoes
	
	NVIC_IPR3 |= NVIC_IP_PRI_14(1 << 6); ///< 14/4->3 seta prioridade 		
}


void UART2_ativaInterruptRxTerminal() {
	UART2_C2 |= UART_C2_RIE_MASK;	
}
