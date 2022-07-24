/*!
 * @file UART.c
 * @brief Este modulo contem interface dos módulos UARTx.
 * @author Wu Shin Ting
 * @date 05/03/2022
 */

#include "derivative.h"
#include "UART.h"
#include "buffer_circular.h"
#include "util.h"

BufferCirc_type buffer;      	// Buffer circular usado na comunicacao da UART0
uint8_t flag;			  		// controle de refresh do visor
char comando;						//Recebe o valor da UART0
estado_type state_tag, new_state_tag;

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


/**
 * @brief Rotina de servico UART0: controla fluxo serial
 */
void UART0_IRQHandler()
{
	char item;
	if (UART0_S1 & UART0_S1_RDRF_MASK) {//Recepcap
		// So entra no estado RELOGIO
		comando = UART0_D;
		state_tag = UART0_IN;
		flag = 1;
	} 
	if (UART0_S1 & UART0_S1_TDRE_MASK) {//Transmissao
		if (BC_pop (&buffer, &item) == -1)
			UART0_disableTEInterrup();
		else  
			UART0_D = item;
	} 
}

/**
 * @brief Desabilitar a interrupcao do canal transmissor
 */
void UART0_disableTEInterrup (void) {
	UART0_C2 &= ~UART_C2_TIE_MASK;
}

/**
 * @brief Envia para Terminal a mensagem do formato de entrada.
 */
void escreveMsgExplicacao(void) {
	extern BufferCirc_type buffer;
	char msg[]="Consulta: H/h (horario), T/t (temperatura) ou C/c (ultimo valor cronometrado)\n\r";

	uint8_t i=0;
	
	while (msg[i]) 
		BC_push( &buffer, msg[i++]);
	
	// Habilitar interrupcoes
	UART0_enableTEInterrup ();
}


//==================================
// UART0
//==================================
/**
 * @Parametros de configuracao  UART0
 */
/**
 * @brief Inicializar os pinos conectados a UART0 e ao OpenSDA.
 * @return codigo de erro (1=taxa invalida; 2=baud invalido; 3=combinacao dos dois invalida)
 */
uint8_t UART0_init(UART0Config_type *config) {
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;      // habilitar clock gate de UART0

	SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC(0b11); // configurar a fonte de relógio (20.971520MHz)
	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(0x1);	

	// Configurar os pinos de entrada/saida (conectados no FRDMKL25Z)
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;      // habilitar clock gate de PORTA
	PORTA_PCR1 |= PORT_PCR_MUX(0x2);        // configurar com funcao UART0_RX
	PORTA_PCR2 |= PORT_PCR_MUX(0x2);        // configurar com funcao UART0_TX

	// Desabilitar canais RX e TX para configuracoes
	UART0_C2 &= ~(UART0_C2_RE_MASK |           
			UART0_C2_TE_MASK);

	// Configurar stop bits
	UART0_BDH &= ~(UART0_BDH_SBNS_MASK);
	UART0_BDH |= config->bdh_sbns << UART0_BDH_SBNS_SHIFT;

	// Configurar o registrador C1
	UART0_C1 &= ~(UART0_C1_LOOPS_MASK |
			UART0_C1_DOZEEN_MASK |
			UART0_C1_RSRC_MASK |
			UART0_C1_M_MASK |                 
			UART0_C1_WAKE_MASK |
			UART0_C1_ILT_MASK |
			UART0_C1_PE_MASK |			
			UART0_C1_PT_MASK
	);

	UART0_C1 |= (config->c1_loops << UART0_C1_LOOPS_SHIFT |
			config->c1_dozeen << UART0_C1_DOZEEN_SHIFT |
			config->c1_rsrc << UART0_C1_RSRC_SHIFT |
			config->c1_m << UART0_C1_M_SHIFT |                 
			config->c1_wake << UART0_C1_WAKE_SHIFT |
			config->c1_ilt << UART0_C1_ILT_SHIFT |
			config->c1_pe << UART0_C1_PE_SHIFT |			
			config->c1_pt << UART0_C1_PT_SHIFT
	);

	// Configurar o registrador C2
	UART0_C2 &= ~(UART0_C2_RWU_MASK |
			UART0_C2_SBK_MASK);

	UART0_C2 |= (config->c2_rwu << UART0_C2_RWU_SHIFT |
			config->c2_sbk << UART0_C2_SBK_SHIFT
	);

	// Configurar o registrador S2
	UART0_S2 &= ~(UART0_S2_MSBF_MASK |
			UART0_S2_RXINV_MASK |
			UART0_S2_RWUID_MASK |
			UART0_S2_BRK13_MASK |
			UART0_S2_LBKDE_MASK
	);

	UART0_S2 |= (config->s2_msbf << UART0_S2_MSBF_SHIFT |
			config->s2_rxinv << UART0_S2_RXINV_SHIFT |
			config->s2_rwuid << UART0_S2_RWUID_SHIFT |
			config->s2_brk13 << UART0_S2_BRK13_SHIFT |
			config->s2_lbkde << UART0_S2_LBKDE_SHIFT
	);

	// Configurar o registrador C3
	UART0_C3 &= ~(UART0_C3_R8T9_MASK |
			UART0_C3_R9T8_MASK |
			UART0_C3_TXDIR_MASK |
			UART0_C3_TXINV_MASK 
	);

	UART0_C3 |= (config->c3_r8t9 << UART0_C3_R8T9_SHIFT |
			config->c3_r9t8 << UART0_C3_R9T8_SHIFT |
			config->c3_txdir << UART0_C3_TXDIR_SHIFT |
			config->c3_txinv << UART0_C3_TXINV_SHIFT 
	);

	// Configurar o registrador C4
	UART0_C4 &= ~(UART0_C4_MAEN1_MASK |
			UART0_C4_MAEN2_MASK |
			UART0_C4_M10_MASK
	);

	UART0_C4 |= (config->c4_maen1 << UART0_C4_MAEN1_SHIFT |
			config->c4_maen2 << UART0_C4_MAEN2_SHIFT |
			config->c4_m10 << UART0_C4_M10_SHIFT
	);

	// Valores fora do intervalo 4 a 32 implicam em setar 16
	UART0_C4 |= (0b11111 << UART0_C4_OSR_SHIFT);
	UART0_C4 &= ((config->c4_osr << UART0_C4_OSR_SHIFT) | ~UART0_C4_OSR_MASK);

	// Configurar o registrador C5				
	UART0_C5 &= ~(UART0_C5_TDMAE_MASK |
			UART0_C5_RDMAE_MASK |
			UART0_C5_BOTHEDGE_MASK |
			UART0_C5_RESYNCDIS_MASK
	);

	UART0_C5 |= (config->c5_tdmae << UART0_C5_TDMAE_SHIFT |
			config->c5_rdmae << UART0_C5_RDMAE_SHIFT |
			config->c5_bothedge << UART0_C5_BOTHEDGE_SHIFT |
			config->c5_resyncdis << UART0_C5_RESYNCDIS_SHIFT
	);

	// Ajustar amostragem conforme a taxa de superamostragem
	if ((config->c4_osr & 0b11111) > 2 && (config->c4_osr & 0b11111) < 7)
		UART0_C5 |= UART0_C5_BOTHEDGE_MASK;    

	// Mascarar bits invalidos
	config->sbr |= (config->sbr & 0x1FFF);

	// Setar baus rate via SBR
	UART0_BDH &= ~UART0_BDH_SBR(0x1F);                             
	UART0_BDL &= ~UART0_BDL_SBR(0xFF);            
	UART0_BDH |= UART0_BDH_SBR(config->sbr >> 8);                             
	UART0_BDL |= UART0_BDL_SBR(config->sbr & 0x00FF);            

	//Habilitar os canais RX e TX
	UART0_C2 |= (UART0_C2_RE_MASK |         		///< habilita o canal receptor 
			UART0_C2_TE_MASK);	  			///< habilita o canal transmissor  

	return 0;
}

//==================================
// NVIC
//==================================
/**
 * @brief Habilitar a interrupcao IRQ12 (UART0)
 * @param[in] priority prioridade de atendimento
 */
void NVIC_enableUART0(char priority)
{
	NVIC_ISER |= 1 << (28-16) ;      		// Habilitar IRQ12
	NVIC_ICPR |= 1 << (28-16) ;      		// Limpar as pendencias em solicitacao de interrupcoes
	NVIC_IPR3 |= NVIC_IP_PRI_12(priority << 6); ///< seta prioridade 		
}

void PIT_enableModule (void) {	
	PIT_MCR &= ~(PIT_MCR_MDIS_MASK);     		// habilita modulo PIT
}

/**
 * @brief Habilitar a interrupcao do canal receptor
 */
void UART0_enableRIEInterrup (void) {	
	UART0_C2 |= UART_C2_RIE_MASK;	
}

/**
 * @brief Habilitar a interrupcao do canal transmissor
 */
void UART0_enableTEInterrup (void) {
	UART0_C2 |= UART_C2_TIE_MASK;
}
