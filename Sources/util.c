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

void delay_10us (unsigned int multiplos)
{
	int j = multiplos*21;
	
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


char *ftoa(float valor, char* string){
	int inteiro, virgula, flag_inteiro, flag_virgula;
	
	//Ting: Para 2 digitos, precisamos de 3 bytes, um para '\0'
	//char virgula_char[2], inteiro_char[3];
	char virgula_char[3], inteiro_char[3];

	//Ting: o algoritmo eh muito limitado considerando que a aprte inteira so tenha 2 digitos!
	//Vejam a implementacao em 
	//https://www.geeksforgeeks.org/convert-floating-point-number-string/
	
	float virgula_float;

	inteiro = (int)(valor);
	
	virgula_float = (valor-inteiro)*100.;
	
	//Ting: E virgula? Qual eh o valor?
	virgula = (int)virgula_float;
	
	//Ting: Neste projeto, os valores podem ser negativos ...
	//if (virgula_float - virgula > 0.5) {
	if (fabs(virgula_float) - abs(virgula) > 0.5) {
		virgula += 1;
	}
	
	//converte a parte inteira e fracionaria para ASCII
	
	//Ting: A declaracao char * itoa significa que o valor retornado pela funcao itoa
	//eh o endereco do tipo char. Portanto, com a chamada *itoa voces pegariam o conteudo
	//do endereo retornado. Se nao precisarem, podem chamar sem *:
//	*itoa(inteiro, inteiro_char, 10);
//	*itoa(virgula, virgula_char, 10);
	itoa(inteiro, inteiro_char, 10);
	itoa(virgula, virgula_char, 10);
	
	flag_inteiro = strlen(inteiro_char);
	flag_virgula = strlen(virgula_char);
	
	//se o tamanho da parte inteira for 2
	if (flag_inteiro== 2){
		string[0] = ' ';
		string[1] = inteiro_char[0];
		string[2] = inteiro_char[1];
	} 
	//se o tamanho da parte inteira for 1
	else if (flag_inteiro == 1){
		string[0] = '=';
		string[1] = ' ';
		string[2] = ' ';
		string[3] = ' ';
		string[4] = inteiro_char[0];
		string[5] = '\0';
	}
	else{
		string[0] = '=';
		string[1] = ' ';
		string[2] = inteiro_char[0];
		string[3] = inteiro_char[1];
		string[4] = inteiro_char[2];
	}

	if (flag_virgula== 1){
		string[5] = '0';
		string[6] = virgula_char[0];
		string[7] = '\0';
	} else {
		string[5] = virgula_char[0];
		string[6] = virgula_char[1];
		string[7] = '\0';
	}

	
	return string;
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
