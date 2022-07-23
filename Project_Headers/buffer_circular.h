/*
 * @file buffer_circular.h
 * @brief Arquivo-cabecalho de buffer_circular.c
 * @author Wu Shin-Ting
 * @date 21/06/2021
 */

#ifndef BUFFER_CIRCULAR_H_
#define BUFFER_CIRCULAR_H_

//=====================================
// Estrutura de dados: buffer circular
//=====================================
typedef struct BufferCirc_tag
{
	char * dados;  	 // buffer de dados
	unsigned int tamanho;	// quantidade total de elementos
	unsigned int leitura;       // indice de leitura (tail)  
	unsigned int escrita;       // indice de escrita (head)
} BufferCirc_type;

/**
 * @brief Inicializar um buffer circular
 * @param[in] buffer buffer circular
 * @param[in] tamanho tamanho do buffer circular
 */
void BC_init(BufferCirc_type *buffer, unsigned int tamanho);

void BC_reset(BufferCirc_type *buffer);

void BC_free(BufferCirc_type *buffer);

int BC_push (BufferCirc_type *buffer, char item);

int BC_pop (BufferCirc_type *buffer, char *item);

unsigned int BC_elements (BufferCirc_type *buffer);

uint8_t BC_isFull (BufferCirc_type *buffer);

uint8_t BC_isEmpty (BufferCirc_type *buffer);

#endif /* BUFFER_CIRCULAR_H_ */
