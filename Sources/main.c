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
#include "GPIO_latch_lcd.h"
#include "TPM.h"

#define BAUD_RATE 9600

/** VARIAVEIS GLOBAIS **/
char comando;	
BufferCirc_type buffer;      	// Buffer circular usado na comunicacao da UART0
volatile unsigned int cronometro;	//Variavel usada para salvar o valor do cronometro para consulta

UART0Config_type config = {
		.bdh_sbns=0,			// Um stop bit
		.sbr=0b100, 			// MCGFLLCLK/(baud_rate*osr)=4
		.c1_loops=0,			// Operacao normal: full-duplex 
		.c1_dozeen=0,			// UART habilitado no modo WAIT
		.c1_rsrc=0,				// nao tem efeito para loops == 0
		.c1_m=0,				// dados em 8 bits
		.c1_wake=0,				// wakeup de RX por linha ociosa (IDLE)
		.c1_ilt=0,				// deteccao do estado ocioso pela contagem apos start bits
		.c1_pe=0,				// nao ha checagem de paridade por hardware
		.c1_pt=0,				// nao tem efeito para pe==0
		.c2_rwu=0,				// operacao normal de recepcao do UART0
		.c2_sbk=0,				// operacao normal de transmissor do UART0
		.s2_msbf=0,				// envio de LSB para MSB
		.s2_rxinv=0,			// polaridade dos dados de RX nao eh invertida
		.s2_rwuid=0,			// nao tem efeito para rwu==0
		.s2_brk13=0,			// nao tem efeito para operacao normal de transmissor
		.s2_lbkde=0,			// deteccao do caractere break no tamanho menor
		.c3_r8t9=0,				// nao tem efeito para m==0
		.c3_r9t8=0,				// nao tem efeito para m==0
		.c3_txdir=0,			// nao tem efeito para loops==0
		.c3_txinv=0,			// polaridade dos dados de TX nao eh invertida 
		.c4_maen1=0,			// todos os dados recebidos sao transferidos para buffer de dados
		.c4_maen2=0,			
		.c4_m10=0,				// tamanho dos dados em RX e TX é 8 ou 9 bits
		.c4_osr=0b01111,		// superamostragem 16x
		.c5_tdmae=0,			// uso de DMA pelo TX desabilitada
		.c5_rdmae=0,			// uso de DMA pelo RX desabilitado
		.c5_bothedge=0,			// amostragem somente na borda de subida do clock
		.c5_resyncdis=0			// resincronizacao na recepcao habilitada
};

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
	//////////// VARIAVEIS DA MAIN ////////
	int init = 1;
	estado_type state_tag, new_state_tag;
	int atual = 0;
	float conta = 0.0;
	char str[32];
	
	char string[16];	
	string[0] = '\0';
	
					
	
	/**************************************/
	
		
	/*!
	 * Habilitar sinais de clock
	 */
	SIM_setaOUTDIV4(0b0001);

	/*!
	 * Inicilizar o par (botoeira IRQA5,canal do LED B) 
	 */
	TPM_initSwitchIRQA5LedB(32767, 0b111);	// (65535*128)/20971520 = 0.1999
		
	/*!
	 * Habilitar interrupcoes o par (botoeira IRQA5,canal do LED B) 
	 */
	TPM_habilitaSwitchIRQA5LedBInterrupt(1);
	
	/*!
	 * LCD
	 */
	GPIO_ativaConLatchLCD ();
	GPIO_ativaConLCD ();
	GPIO_initLCD ();

	
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
	//ISR_AtualizaEstado(INICIO);
	//state_tag = ISR_LeEstado();
	state_tag = CRONOMETRO;
	new_state_tag = INICIO ;
	
	//float valor;							//ADC -- temperatura
	//char time[] = "00:00:00        ";
	//float color[6][3] = {		
		//	{0.3200, 1.000, 0.6200},         //Sea Green 1
		//	{0.000, 0.500, 0.000},           //Green
		//	{1.000, 1.000, 0.000},           //Yellow
		//	{1.000, 0.4900, 0.000},          //Coral
		//	{1.000, 0.1400, 0.000},          //Orange Red
		//	{1.000, 0.000, 0.000}      //Red
	//};
	//unsigned int current_time = 0;			//Variavel para o polling de tempo
	//flag = 1;								//Atualizar o display ao iniciar

	//Inicializar controle dos Leds R, G e B por PWM.
	//TPM_initLedRGB();
	//Inicializacao do LCD
	//GPIO_initLCD();					// inicializa LCD
	//initLCD();

	//Inicializar interrupcao porta A
	///GPIO_initSwNMIIRQ12 ();
	//GPIO_enableNMIIRQ12Interrupt();	// habilita interrupcao NMI e IRQ12
	//NVIC_enablePORTA(1);			// habilita IRQ31

	//Set de frequencia
	//SIM_selPLLFLL (0); 				// seleciona MCGFLLCLK (20971520Hz)
	
	//Set TPM0
	//TPM0_init();					// inicializa TPM0
	//NVIC_enableTPM0 (1); 			// habilita a IRQ17
	//TPM0_initSwitchIRQ5 ();			// inicializa IRQ5 como TIC

	//SysTick_init(5242880);			// inicializa timeout

	//Pega o clock do barramento de 20.9715MHz e divide por 2
	//SIM_initBusClock (0b001);		// Divisor de frequencia em 2	
	
	//configuracao da UART0
	config.c4_osr = 12;
	config.sbr = (uint16_t)(20971520./(BAUD_RATE * (config.c4_osr+1)));
	if ((20971520./(1.0 * BAUD_RATE * (config.c4_osr+1))) > config.sbr)
		config.sbr++;
	UART0_init(&config);			// configura UART0
	NVIC_enableUART0(2);			// habilitar IRQ12
	BC_init(&buffer, TAM_MAX);		// alocar um buffer circular


	//Inicializacao
	state_tag = RELOGIO;
	escreveMsgExplicacao();

	
	// Habilita modulo PIT
	//PIT_enableModule ();
	UART0_enableRIEInterrup();  		// habilitar RI de UART0
	cronometro = 0;
	for(;;) {
				
			// Verificar o estado do relogio digital
					if (init) {
						state_tag = ISR_LeEstado();
						switch (state_tag) {	//Verificar o conteudo da variavel estado
						case INICIO:
							GPIO_escreveStringLCD(0x00,"PROJETO FINAL871");
							GPIO_escreveStringLCD (0x40," LUCAS & LUCAS");
							break;	
						case LEITURA_UART:
							GPIO_escreveStringLCD(0x00,"                ");
							
							break;
						case RESULTADO:
							ISR_extraiString(string);
							ISR_AtualizaEstado(new_state_tag);
							break;
						case RELOGIO:		//Se for RELOGIO
							GPIO_escreveStringLCD (0x00,"RELOGIO:        ");
							GPIO_escreveStringLCD (0x40,"                ");	
							RTC_getTime(&seconds);					//Atualizar a variavel segundos
							ttoa(seconds, time);					//Converter os segundos para o formato HH:MM:SS
							escreveMensagem(0x45, time);			//Escrever no display o valor no formato HH:MM:SS
							break;
							
						/////////// COMEÇO LAB CRONOMETRO ///////////
						case CRONOMETRO:
							state_tag = REALIMENTACAO;
							GPIO_escreveStringLCD (0x00,"CRONOMETRO:  ");			//Escrever no display o valor no formato HH:MM:SS
							GPIO_escreveStringLCD (0x40,"             ");	
							break;
							
						case ESPERA: //passa
						case CONTAGEM: //passa
						case LEITURA: //passa
						break;
										
						case REALIMENTACAO:					
							atual = ISR_LeTempo_Cronometro(); // Leitura do tempo do cronometro atual ++
											
							itoa(atual,str,10);      // converte segundos em string para o LCD
							GPIO_escreveStringLCD(0x4B,str);  // atualiza o lcd
							ISR_AtualizaEstado(CONTAGEM); // atualiza o estado_tag
						break;
										
						case INTERVALO:
							conta = ComputaIntervaloTempo(); // Leitura do valor cronometrado
											
							ftoa(conta,str,10); //converte pra string
							GPIO_escreveStringLCD(0x4B,str);  //atualiza o lcd	//coloca valor final
											
							GPIO_setEndDDRAMLCD(0x0F);
							GPIO_setRS(DADO);
							GPIO_escreveByteLCD(0x02,5);  // mostra bitmap cronometro no lcd
											
							ISR_AtualizaEstado(LEITURA); // atualiza o estado_tag
						break;
						case RESETA_VISOR:
							GPIO_limpaDisplayLCD(); //Limpa display
							ISR_AtualizaEstado(ESPERA); // atualiza o estado_tag
						break;
						/////////// FIM LAB CRONOMETRO ///////////
						case AJUSTE_HORA:
						case AJUSTE_HORA_INC:
						case AJUSTE_HORA_DEC:
							//ttoa(time_bkp, time);					//Converte o valor do backup
							//GPIO_escreveStringLCD (0x45, time);			//imprimir no display o valor atualizado
							//GPIO_escreveStringLCD (cursor, "");			//Setar o cursor
							break;
						case AJUSTE_MINUTO:
						case AJUSTE_MINUTO_INC:
						case AJUSTE_MINUTO_DEC:
							//ttoa(time_bkp, time);					//Converte o valor do backup
							//GPIO_escreveStringLCD (0x45, time);			//imprimir no display o valor atualizado
							//GPIO_escreveStringLCD (cursor, "");			//Setar o cursor
							break;
						case AJUSTE_SEGUNDO:
						case AJUSTE_SEGUNDO_INC:
						case AJUSTE_SEGUNDO_DEC:
							//ttoa(time_bkp, time);					//Converte o valor do backup
							//GPIO_escreveStringLCD (0x45, time);			//imprimir no display o valor atualizado
							//GPIO_escreveStringLCD (cursor, "");			//Setar o cursor
							break;
						case TERMOMETRO:
							GPIO_escreveStringLCD (0x00,"TERMOMETRO:  ");		
							GPIO_escreveStringLCD (0x4B,"             ");	
							//Apos a verificacao da flag COCO é indicado que se pode converter a temperatura
							//if (cycle_flags) {
								//valor = ((float)(result0A) * (VREFH-VREFL))/MAX;                  

								//temperatura3031 = valor - 600;
								//escala_led((int)temperatura3031);
								//TPM1_valLedR ((unsigned short)(color[contador][0]*COUNTER_OVF), HIGH);
								//TPM2_valLedG ((unsigned short)(color[contador][1]*COUNTER_OVF), HIGH);
								//TPM2_valLedB ((unsigned short)(color[contador][2]*COUNTER_OVF), HIGH);
								//print_temp((int)temperatura3031);
								//escreveMensagem(cursor, "");
								//cycle_flags = 0;
							//}
							break;
						case RESET:
							state_tag = INICIO;
							break;
						case UART0_IN:
							processa_Terminal(comando);			//Processar terminal
							//estado = RELOGIO;				// voltar para o estado normal 
							break;
						}
						init = 0;
					}

					//Polling para mostrar o tempo no modo RELOGIO. Como as operacoes do display sao muito demoradas do ponto de vista do loop infinito
					//compara-se o valor atual do RTC com o valor exibido no display e se esse for diferente autoriza o display a mudar
					//RTC_getTime(&current_time);
					//if(current_time != seconds)
						init = 1;
				}

				return 1;
			}


