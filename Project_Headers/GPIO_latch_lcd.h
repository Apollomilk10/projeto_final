/*
 * @file GPIO_latch_lcd.h
 * @brief Prototipos do modulo GPIO_latch_lcd
 * @author Wu Shin Ting
 * @date 28/02/2022
 */

#ifndef GPIO_LATCH_LCD_H_
#define GPIO_LATCH_LCD_H_

#include "derivative.h"

/**
 * @brief Funcao do byte enviado para LCD
 */
typedef enum lcd_RS_tag {
	COMANDO,  //!< Comando (0) 
	DADO	  //!< Dado (1) 
} lcd_RS_type;

//===========================================================
// Latch e LCD
//===========================================================
/*!
 * @brief Ativa conexao entre mcu com latch e lcd
 */
void GPIO_ativaConLatchLCD ();

/*!
 * @brief Envia um mesmo byte para latch (ativando LE) e LCD (ativando Enable) 
 * @param[in] c byte
 * @param[in] t tempo de processamento em multiplos de 5us
 */
void GPIO_escreveByteLatchLCD(char c, uint8_t t);

/*!
 * @brief Envia um byte para latch
 * @param[in] c byte
 */
void GPIO_escreveByteLatch(char c);

void GPIO_ativaConLCD ();

void GPIO_setRS (lcd_RS_type i);

void GPIO_escreveByteLCD(char c, uint32_t t);

void GPIO_initLCD ();

void GPIO_escreveBitmapLCD (uint8_t end, char* bitmap);

void GPIO_escreveStringLCD (uint8_t end, char* str);

void GPIO_limpaDisplayLCD ();

/*!
 * @brief Seta end no registrador de endereco (cursor) de DDRAM
 * @param[in] end endereco da memoria DDRAM
 * 
 * @note Consulta os enderecos de cada celula das duas linhas na 
 * @note secao 11 em ftp://ftp.dca.fee.unicamp.br/pub/docs/ea079/datasheet/AC162A.pdf
 */
void GPIO_setEndDDRAMLCD (uint8_t end);

#endif /* GPIO_LATCH_LCD_H_ */
