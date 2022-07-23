/*
 * @file GPIO_latch_lcd.c
 * @brief Este modulo contem interface do LCD via modulo GPIO
 * @author Wu Shin Ting
 * @date 28/02/2022
 */

#include "GPIO_latch_lcd.h"

/*!
 * @def GPIO_PIN
 * @brief Constroi uma mascara com somente bit x em 1  
 */
#define GPIO_PIN(x)  ((1)<<(x)) 

/**
 * @brief Gera um atraso em i multiplos de uma base de tempo
 * @param[in] i multiplos
 */
static void delay_10us (unsigned int multiplos)
{
	int j = multiplos*21;
	
	while (j) j--;
}

void GPIO_ativaConLatchLCD (){
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK ;    	//!< habilita sinal de clock de PORTC				

	/*!
	 * Configura os pinos conectados em Latch e LCD com funcao GPIO
	 */
	PORTC_PCR0 |= PORT_PCR_MUX(0x1);       //!< \li \l D0-D7 dos dados
	PORTC_PCR1 |= PORT_PCR_MUX(0x1);
	PORTC_PCR2 |= PORT_PCR_MUX(0x1);
	PORTC_PCR3 |= PORT_PCR_MUX(0x1);
	PORTC_PCR4 |= PORT_PCR_MUX(0x1);
	PORTC_PCR5 |= PORT_PCR_MUX(0x1);
	PORTC_PCR6 |= PORT_PCR_MUX(0x1);
	PORTC_PCR7 |= PORT_PCR_MUX(0x1);
	
	PORTC_PCR8 |= PORT_PCR_MUX(0x1);       //!< \li \l RS do LCD 
	PORTC_PCR9 |= PORT_PCR_MUX(0x1);       //!< \li \l E do LCD 
	PORTC_PCR10 |= PORT_PCR_MUX(0x1);      //!< \li \l LE do Latch
	
    /*!
     * Configura o sentido do sinal nos pinos PORTC0-10 como saida 
     */
	GPIOC_PDDR |= GPIO_PDDR_PDD(GPIO_PIN(10)| GPIO_PIN(9)|GPIO_PIN(8) |GPIO_PIN(7) | 
			GPIO_PIN(6) | GPIO_PIN(5) | GPIO_PIN(4) | GPIO_PIN(3) | 
			GPIO_PIN(2) | GPIO_PIN(1) | GPIO_PIN(0));  

	/*!
	 * Envia um pulso de LE de largura em unidades de ns para apagar leds vermalhos
	 */ 
	GPIOC_PSOR = GPIO_PIN (10);		
	GPIOC_PCOR = GPIO_PIN (10);	
}

void GPIO_ativaConLCD (){
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK ;    	//!< habilita sinal de clock de PORTC				

	/*!
	 * Configura os pinos conectados em Latch e LCD com funcao GPIO
	 */
	PORTC_PCR0 |= PORT_PCR_MUX(0x1);       //!< \li \l D0-D7 dos dados
	PORTC_PCR1 |= PORT_PCR_MUX(0x1);
	PORTC_PCR2 |= PORT_PCR_MUX(0x1);
	PORTC_PCR3 |= PORT_PCR_MUX(0x1);
	PORTC_PCR4 |= PORT_PCR_MUX(0x1);
	PORTC_PCR5 |= PORT_PCR_MUX(0x1);
	PORTC_PCR6 |= PORT_PCR_MUX(0x1);
	PORTC_PCR7 |= PORT_PCR_MUX(0x1);
	
	PORTC_PCR8 |= PORT_PCR_MUX(0x1);       //!< \li \l RS do LCD 
	PORTC_PCR9 |= PORT_PCR_MUX(0x1);       //!< \li \l E do LCD 
	
    /*!
     * Configura o sentido do sinal nos pinos PORTC0-10 como saida 
     */
	GPIOC_PDDR |= GPIO_PDDR_PDD(GPIO_PIN(9)|GPIO_PIN(8) |GPIO_PIN(7) | 
			GPIO_PIN(6) | GPIO_PIN(5) | GPIO_PIN(4) | GPIO_PIN(3) | 
			GPIO_PIN(2) | GPIO_PIN(1) | GPIO_PIN(0));  

}

void GPIO_escreveByteLatchLCD(char c, uint8_t t)
{
	/*!
	 * Coloca os sinais do byte nos pinos PTC0-PTC7 (byte menos significativo da PORTC)
	 */ 	
	GPIOC_PDOR &= ~0xff; 		
	GPIOC_PDOR |= c; 	
	
	/*!
	 * Envia um pulso de LE de largura em unidades de ns
	 */ 
	GPIOC_PSOR = GPIO_PIN (10);		
	GPIOC_PCOR = GPIO_PIN (10);	
	
	/*!
	 * Envia um pulso de E de largura maior que 450ns
	 */ 
	GPIOC_PSOR = GPIO_PIN (9);		
	delay_10us(1);
	GPIOC_PCOR = GPIO_PIN (9);
	
	/*!
	 * Aguarda pelo processamento 
	 */
	delay_10us (t);
}

void GPIO_escreveByteLatch(char c)
{
	/*!
	 * Coloca os sinais do byte nos pinos PTC0-PTC7 (byte menos significativo da PORTC)
	 */ 	
	GPIOC_PDOR &= ~0xff; 		
	GPIOC_PDOR |= c; 	
	
	/*!
	 * Envia um pulso de LE de largura em unidades de ns
	 */ 
	GPIOC_PSOR = GPIO_PIN (10);		
	GPIOC_PCOR = GPIO_PIN (10);	
}

void GPIO_setRS (lcd_RS_type i) {
	if(i == COMANDO) {
		GPIOC_PCOR = GPIO_PIN(8);     	//!< Seta o LCD no modo de comando
	}
	else if (i == DADO) { 
		GPIOC_PSOR = GPIO_PIN(8);    	//!< Seta o LCD no modo de dados
	}
}

void GPIO_escreveByteLCD(char c, uint32_t t)
{
	/*!
	 * Coloca os sinais do byte nos pinos PTC0-PTC7 (byte menos significativo da PORTC)
	 */ 	
	GPIOC_PDOR &= ~0xff; 		
	GPIOC_PDOR |= c; 	
	
	/*!
	 * Envia um pulso de E de largura maior que 450ns
	 */ 
	GPIOC_PSOR = GPIO_PIN (9);		
	delay_10us(1);
	GPIOC_PCOR = GPIO_PIN (9);
	
	/*!
	 * Aguarda pelo processamento 
	 */
	delay_10us (t);
}

void GPIO_initLCD () {
	delay_10us(3000);               //!< espera por mais de 30ms: 30000us/10 -> 
	
	GPIO_setRS(COMANDO);
	GPIO_escreveByteLCD(0x38, 4);			//!< Function Set: 39us/10 -> 
	GPIO_escreveByteLCD(0x0C, 4);		    //!< Display ON/OFF Control: 39us/10 -> 
	GPIO_escreveByteLCD(0x01, 16);			//!< Display Clear: 1530us/10 ->
	GPIO_escreveByteLCD(0x06, 4);			//!< Entry mode set: 39us/10 -> 
}

void GPIO_escreveBitmapLCD (uint8_t end, char* bitmap)
{
	/*!
	 * Converte end para endereco da memoria CGRAM (end*8) e codifica
	 * o endereco na instrucao "Set CGRAM Address".
	 */
	uint8_t tmp = 0b01000000 | (end*8); 
	
	/*!
	 * Seta end*8 no registrador de endereco de CGRAM por um tempo de processamento 
	 * maior que 39us (39/10 = 4)
	 */
	GPIO_setRS(COMANDO);				
	GPIO_escreveByteLCD(tmp, 4);	

	/*!
	 * Grava os 8 bytes do vetor bitmap a partir do endereco setado. O tempo de escrita
	 * de cada byte eh maior que 43us (43/10 = 5)
	 */
	GPIO_setRS (DADO);
	uint8_t i;
	for (i=0; i< 8; i++) {
		GPIO_escreveByteLCD(bitmap[i], 5);
	}
}

void GPIO_escreveStringLCD (uint8_t end, char* str)
{
	/*!
	 * Codifica o endereco na instrucao "Set DDRAM Address".
	 */
	uint8_t tmp = 0b10000000 | end;
	
	/*!
	 * Seta end no registrador de endereco de DDRAM por um tempo de processamento 
	 * maior que 39us (39/10 = 4)
	 */
	GPIO_setRS(COMANDO);			
	GPIO_escreveByteLCD(tmp, 4);			

	/*!
	 * Grava os caracteres da Tabela de Fontes a partir do endereco setado. 
	 * O tempo de escrita de cada byte eh maior que 43us (43/10 = 5)
	 */
	GPIO_setRS (DADO);
	while (*str != '\0') {
		GPIO_escreveByteLCD(*str, 5);
		str++;
	}
}

void GPIO_setEndDDRAMLCD (uint8_t end)
{
	//Codifica a instrucao do endereco do DDRAM para LCD: 0b1XXXXXX, onde XXXXXXX eh o endereco
	// de DDRAM (do visor)
	uint8_t tmp = 0b10000000 | end;
	
	//Envia a instrucao para setar o end de DDRAM
	GPIO_setRS(COMANDO);			
	GPIO_escreveByteLCD(tmp, 5);			
}

void GPIO_limpaDisplayLCD () {
	GPIO_setRS(COMANDO);
	GPIO_escreveByteLCD(0x01, 16);			//Display Clear: 1530us/10 -> 16
}

