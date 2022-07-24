/*
 * util.c
 *
 *  Created on: Jun 23, 2022
 *      Author: ea871
 */



/*!
 * @file util.c
 * @brief Este modulo contem as funcoes de uso frequente
 * @author Wu Shin Ting
 * @date 27/02/2022
 */

#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "util.h"
estado_type state_tag, new_state_tag;

void delay_10us (unsigned int multiplos)
{
	int j = multiplos*10;
	
	while (j) j--;
}

/**
 * @brief Converter inteiro para string
 * @param[in] n valor inteiro
 * @param[in] string endereco da string
 * @param[in] b base do valor inteiro
 * @return endereco da string
 */
char *itoa (int n, char *string, char b)
{
	char sinal;
	unsigned short i, j, a;
	unsigned int m;
	
	if (n == 0) {
		///< Tratamento do caso especial
		string[0] = '0';
		string[1] = '\0';
		return string;
	}
	
	if (n < 0) sinal = 1;
	else sinal = 0;
	
	/*!
	 * Converter para codigos ASCII, digito por digito, de menos 
	 * significativo para o mais
	 */ 
	i = 0;
	m = abs(n);
	
	while (m != 0) {
		a = m%b;
		string[i+sinal] = (a>9)?(a-10)+'A' : a+'0';
		i++;
		m = m/b;
	}
	string[i+sinal] = '\0';			///< Incluir terminador
	
	///< Reverter a sequencia
	j = strlen(string)-1;
	i = sinal;
			
	while (j > i) {
		char tmp = string[i];
		string[i] = string[j];
		string[j] = tmp;
		i++;
		j--;
	}
	
	///< Adicionar o sinal negativo caso necessario
	if (sinal) string[0] = '-';
	
	return string;
}

char *ttoa (uint32_t seconds, char *string)
{
	uint32_t dd, hh, mm, ss;

	s2dhms (seconds, &dd, &hh, &mm, &ss);
	
	string[2] = string[5] = ':';    // inserir os espacadores
	string[8] = '\0';				// terminador

	if (hh > 23) {
		//!< horario invalido: FF:FF:FF
		string[0] = string[1] = string[3] = string[4] = string[6] = string[7] = 'F';
	}
	
	string[0] = (hh < 10)? '0': (hh/10)+'0';
	string[1] = hh%10+'0';
	
	string[3] = (mm < 10)? '0': (mm/10)+'0';
	string[4] = mm%10+'0';
	
	string[6] = (ss < 10)? '0': (ss/10)+'0';
	string[7] = ss%10+'0';	
	
	return string;
}

void s2dhms (uint32_t segundos, uint32_t *DD, uint32_t *HH, uint32_t *MM, uint32_t *SS)
{
	*DD = segundos/86400;

	uint32_t sec = segundos%86400;
	
	*SS = sec%60;
	
	*MM = (sec/60)%60;
	
	*HH = sec/3600;
	
}

void dhms2s (uint8_t DD, uint8_t HH, uint8_t MM, uint8_t SS, uint32_t *segundos)
{
	*segundos = DD*86400+HH*3600+MM*60+SS;
}


/*!
 * @brief Separar a string extraída do buffer de entrada em 3 tokens, 2 operandos e 1 operador.	
 * @param[in] string mensagem de entrada
 * @param[in] op operando ( +, , *,;,)
 * @param[in] fvalor para resultado float
 * @param[in] ivalor para resultado em inteiro
 */
int atoOp (char *string, char *op, float *fvalor, int *ivalor, int *efloat){
	
	float op1,op2;
	char *sub_string;
	char *oper;
	
	sub_string = strtok(string, ";- ");
	op1 = atof(sub_string);
	
	sub_string = strtok(NULL, ";- ");
	oper = sub_string;

	sub_string = strtok(NULL, ";- ");
	op2 = atof(sub_string);
	
	int op3 = (int)op1;
	int op4 = (int)op2;
	
	//Ponto Flutuante:
	if (*oper == '+'){
		*fvalor = op1+op2;
		*efloat = 1;
	}
	else if (*oper == '-' ){
		*fvalor	= op1-op2;	
		*efloat = 1;
	}
	else if (*oper == '*' ){
		*fvalor = op1*op2;
		*efloat = 1;
	}
	else if (*oper == '/' ){
		*fvalor = op1/op2;
		*efloat = 1;
	}
	else if (*oper == '&' ){
		*ivalor = op3&op4;
		*efloat = 0;
	}
	else if (*oper ==  '|' ){
		*ivalor = op3|op4;
		*efloat = 0;
	}
	else if (*oper == '#' ){
		*ivalor = op3/op4;	
		*efloat = 0;
	}
	else if (*oper == '%' ){
		*ivalor = op3%op4;
		*efloat = 0;
	} else {
		return -1;
	}
	return 0;
}


/**
 * @brief Inverte a string
 * @param[in] str endereço da string
 * @param[in] tamanho -> tamanho da string
 */
void reverse(char *str, int tamanho){
	int i = 0, j = tamanho -1, temp;
	while(i<j){
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}
/**
 * @brief Inverte a string
 * @param[in] str endereço da string
 * @param[in] tamanho -> tamanho da string
 */
int itos (int x, char str[], int d){
	int i = 0;
	
	while(x){
		str[i++] = (x%10) + '0';
		x = x/10;
	}
	
	//Adiciona 0 a string
	while(i < d){
		str[i++] ='0';
	}
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}

/**
 * @brief float para string
 * @param[in] m valor em float
 * @param[in] res endereço de retorno da string
 * @param[in] decimal valor do mod
 */
char *ftoa(float m, char* res, int decimal){
	
	uint8_t j=0;

	if (m < 0.0) {
		j = 1;
		res[0] = '-';
	}
	float n = fabs(m);
	
	//Extrai a parte inteira
	int ipart = (int)n;
	
	//Extrai a parte float
	float fpart = n -(float)ipart;
	
	//Converte parte inteira em string, inserindo '0' qdo a casa inteira eh 0.
	int i;
	
	if (ipart == 0) {
		i = 1;
		res[j] = '0';
	} else {
		i = itos(ipart, res+j, 0);
	}
	
	//checa o decimal e coloca o ponto
	if(decimal != 0){
		res[i+j]='.';
		
		fpart = fpart * pow(10, decimal);
		
		//ajuste de arredondamento!
		if ((fpart - (int)fpart) >= 0.5) {
			itos((int)(fpart+1), res+1+i+j, decimal);
		} else {
			itos((int)fpart, res+1+i+j, decimal);			
		}
	}
	
	return res;
}

/**
 * @brief processamento do caractere recebido do Terminal
 * @param[in] caractere c
 */
void processa_Terminal (char c) {
	switch (c) {
	case 'H':
	case 'h':
		state_tag = RELOGIO;
		//escreveMsgHorario(c);
		break;
	case 'T':
	case 't':
		state_tag = TERMOMETRO;
		//escreveMsgTemperatura(c);
		break;
	case 'C':
	case 'c':
		state_tag = CRONOMETRO;
		//escreveMsgCronometro(c);
		break;	
	case 'R':
	case 'r':
		state_tag = RESET;
		//escreveMsgCronometro(c);
		break;	
	default:
		//escreveMsgRealimentacao(c);
		break;
	}return state_tag;
}
